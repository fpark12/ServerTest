#pragma once

#include "Private/Detail/Network/MessageBuffer.h"
#include <atomic>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <asio/ip/tcp.hpp>

using asio::ip::tcp;

#define READ_BLOCK_SIZE 4096
#ifdef ASIO_HAS_IOCP
#define SOCKET_HAS_IOCP
#endif

/**
@class Socket

Base async socket implementation

@tparam T derived class type (CRTP)
@tparam Stream stream type used for operations on socket
Stream must implement the following methods:

void close(boost::system::error_code& error);

void shutdown(boost::asio::socket_base::shutdown_type what, boost::system::error_code& shutdownError);

template<typename MutableBufferSequence, typename ReadHandlerType>
void async_read_some(MutableBufferSequence const& buffers, ReadHandlerType&& handler);

template<typename ConstBufferSequence, typename WriteHandlerType>
void async_write_some(ConstBufferSequence const& buffers, WriteHandlerType&& handler);

template<typename ConstBufferSequence>
std::size_t write_some(ConstBufferSequence const& buffers, boost::system::error_code& error);

template<typename SettableSocketOption>
void set_option(SettableSocketOption const& option, boost::system::error_code& error);

tcp::socket::endpoint_type remote_endpoint() const;
*/

class Socket : public std::enable_shared_from_this<Socket>
{

private:
	tcp::socket SocketImpl;

	asio::ip::address RemoteAddress;
	uint16 RemotePort;

	MessageBuffer ReadBuffer;
	std::queue<MessageBuffer> WriteQueue;

	std::atomic<bool> IsClosed;
	std::atomic<bool> IsClosing;

	bool IsWritingAsync;

	void ReadHandlerInternal(asio::error_code error, size_t transferredBytes)
	{
		if (error)
		{
			CloseSocket();
			return;
		}

		ReadBuffer.WriteCompleted(transferredBytes);
		ReadHandler();
	}

#ifdef SOCKET_HAS_IOCP

	void WriteHandler(asio::error_code error, std::size_t transferedBytes)
	{
		if (!error)
		{
			IsWritingAsync = false;
			WriteQueue.front().ReadCompleted(transferedBytes);
			if (!WriteQueue.front().GetActiveSize())
			{
				WriteQueue.pop();
			}

			if (!WriteQueue.empty())
			{
				AsyncProcessQueue();
			}
			else if (IsClosing)
			{
				CloseSocket();
			}
		}
		else
			CloseSocket();
	}


#else
	void WriteHandlerWrapper(asio::error_code error, std::size_t transferedBytes)
	{
		IsWritingAsync = false;
		HandleQueue();
	}

	bool HandleQueue()
	{
		if (WriteQueue.empty())
		{
			return false;
		}

		MessageBuffer& queuedMessage = WriteQueue.front();

		std::size_t bytesToSend = queuedMessage.GetActiveSize();

		asio::error_code error;
		std::size_t bytesSent = SocketImpl.write_some(asio::buffer(queuedMessage.GetReadPointer(), bytesToSend), error);

		if (error)
		{
			if (error == asio::error::would_block || error == asio::error::try_again)
			{
				return AsyncProcessQueue();
			}

			WriteQueue.pop();

			if (IsClosing && WriteQueue.empty())
			{
				CloseSocket();
			}

			return false;
		}
		else if (bytesSent == 0)
		{
			WriteQueue.pop();
			if (IsClosing && WriteQueue.empty())
			{
				CloseSocket();
			}
			return false;
		}
		else if (bytesSent < bytesToSend) // now n > 0
		{
			queuedMessage.ReadCompleted(bytesSent);
			return AsyncProcessQueue();
		}

		WriteQueue.pop();

		if (IsClosing && WriteQueue.empty())
		{
			CloseSocket();
		}

		return !WriteQueue.empty();
	}

#endif

protected:
	virtual void OnClose() { }

	virtual void ReadHandler() = 0;

	bool AsyncProcessQueue()
	{
		if (IsWritingAsync)
			return false;

		IsWritingAsync = true;
#ifdef SOCKET_HAS_IOCP
		MessageBuffer& buffer = WriteQueue.front();
		SocketImpl.async_write_some(asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()), std::bind(&Socket::WriteHandler,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#else
		SocketImpl.async_write_some(asio::null_buffers(), std::bind(&Socket::WriteHandlerWrapper,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

		return false;
	}

public:
	Socket(tcp::socket&& SocketImpl) :
		SocketImpl(std::move(SocketImpl)),
		RemoteAddress(SocketImpl.remote_endpoint().address()),
		RemotePort(SocketImpl.remote_endpoint().port()),
		ReadBuffer(), IsClosed(false), IsClosing(false), IsWritingAsync(false)
	{
		ReadBuffer.Resize(READ_BLOCK_SIZE);
	}

	virtual ~Socket()
	{
		IsClosed = true;
		SocketImpl.close(asio::error_code());
	}

	virtual void Start() = 0;

	virtual bool Update()
	{
		if (IsClosed)
			return false;

#ifndef SOCKET_HAS_IOCP
		if (IsWritingAsync || (WriteQueue.empty() && !IsClosing))
			return true;

		for (; HandleQueue();)
			;
#endif

		return true;
	}

	asio::ip::address GetRemoteIpAddress() const
	{
		return RemoteAddress;
	}

	uint16 GetRemotePort() const
	{
		return RemotePort;
	}


	void AsyncRead()
	{
		if (!IsOpen())
			return;

		ReadBuffer.Normalize();
		ReadBuffer.EnsureFreeSpace();
		SocketImpl.async_read_some(asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
			std::bind(&Socket::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void AsyncReadWithCallback(void (Socket::*callback)(asio::error_code, std::size_t))
	{
		if (!IsOpen())
			return;

		ReadBuffer.Normalize();
		ReadBuffer.EnsureFreeSpace();
		SocketImpl.async_read_some(asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
			std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}


	void QueuePacket(MessageBuffer&& buffer)
	{
		WriteQueue.push(std::move(buffer));

#ifdef SOCKET_HAS_IOCP
		AsyncProcessQueue();
#endif
	}

	bool IsOpen() const { return !IsClosed && !IsClosing; }

	void CloseSocket()
	{
		if (IsClosed.exchange(true))
			return;

		asio::error_code shutdownError;
		SocketImpl.shutdown(asio::socket_base::shutdown_send, shutdownError);
		if (shutdownError)
		{
			//TC_LOG_DEBUG("network", "Socket::CloseSocket: %s errored when shutting down socket: %i (%s)", GetRemoteIpAddress().to_string().c_str(),
			//	shutdownError.value(), shutdownError.message().c_str());

		}

		OnClose();
	}

	// Marks the socket for closing after write buffer becomes empty
	void DelayedCloseSocket() { IsClosing = true; }

	MessageBuffer& GetReadBuffer() { return ReadBuffer; }




};

/*
template<class T, class Stream = tcp::socket>
class Socket : public std::enable_shared_from_this<T>
{
public:
	explicit Socket(tcp::socket&& socket) : SocketImpl(std::move(socket)), _remoteAddress(SocketImpl.remote_endpoint().address()),
		_remotePort(SocketImpl.remote_endpoint().port()), ReadBuffer(), IsClosed(false), IsClosing(false), IsWritingAsync(false)
	{
		ReadBuffer.Resize(READ_BLOCK_SIZE);
	}

	virtual ~Socket()
	{
		IsClosed = true;
		boost::system::error_code error;
		SocketImpl.close(error);
	}

	virtual void Start() = 0;

	virtual bool Update()
	{
		if (IsClosed)
			return false;

#ifndef TC_SOCKET_USE_IOCP
		if (IsWritingAsync || (WriteQueue.empty() && !IsClosing))
			return true;

		for (; HandleQueue();)
			;
#endif

		return true;
	}

	boost::asio::ip::address GetRemoteIpAddress() const
	{
		return _remoteAddress;
	}

	uint16 GetRemotePort() const
	{
		return _remotePort;
	}

	void AsyncRead()
	{
		if (!IsOpen())
			return;

		ReadBuffer.Normalize();
		ReadBuffer.EnsureFreeSpace();
		SocketImpl.async_read_some(boost::asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
			std::bind(&Socket<T, Stream>::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void AsyncReadWithCallback(void (T::*callback)(boost::system::error_code, std::size_t))
	{
		if (!IsOpen())
			return;

		ReadBuffer.Normalize();
		ReadBuffer.EnsureFreeSpace();
		SocketImpl.async_read_some(boost::asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
			std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}

	void QueuePacket(MessageBuffer&& buffer)
	{
		WriteQueue.push(std::move(buffer));

#ifdef TC_SOCKET_USE_IOCP
		AsyncProcessQueue();
#endif
	}

	bool IsOpen() const { return !IsClosed && !IsClosing; }

	void CloseSocket()
	{
		if (IsClosed.exchange(true))
			return;

		boost::system::error_code shutdownError;
		SocketImpl.shutdown(boost::asio::socket_base::shutdown_send, shutdownError);
		if (shutdownError)
			TC_LOG_DEBUG("network", "Socket::CloseSocket: %s errored when shutting down socket: %i (%s)", GetRemoteIpAddress().to_string().c_str(),
				shutdownError.value(), shutdownError.message().c_str());

		OnClose();
	}

	/// Marks the socket for closing after write buffer becomes empty
	void DelayedCloseSocket() { IsClosing = true; }

	MessageBuffer& GetReadBuffer() { return ReadBuffer; }

protected:
	virtual void OnClose() { }

	virtual void ReadHandler() = 0;

	bool AsyncProcessQueue()
	{
		if (IsWritingAsync)
			return false;

		IsWritingAsync = true;

#ifdef TC_SOCKET_USE_IOCP
		MessageBuffer& buffer = WriteQueue.front();
		SocketImpl.async_write_some(boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()), std::bind(&Socket<T, Stream>::WriteHandler,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#else
		SocketImpl.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T, Stream>::WriteHandlerWrapper,
			this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

		return false;
	}

	void SetNoDelay(bool enable)
	{
		boost::system::error_code err;
		SocketImpl.set_option(tcp::no_delay(enable), err);
		if (err)
			TC_LOG_DEBUG("network", "Socket::SetNoDelay: failed to set_option(boost::asio::ip::tcp::no_delay) for %s - %d (%s)",
				GetRemoteIpAddress().to_string().c_str(), err.value(), err.message().c_str());
	}

	Stream& underlying_stream()
	{
		return SocketImpl;
	}

private:
	void ReadHandlerInternal(boost::system::error_code error, size_t transferredBytes)
	{
		if (error)
		{
			CloseSocket();
			return;
		}

		ReadBuffer.WriteCompleted(transferredBytes);
		ReadHandler();
	}

#ifdef TC_SOCKET_USE_IOCP

	void WriteHandler(boost::system::error_code error, std::size_t transferedBytes)
	{
		if (!error)
		{
			IsWritingAsync = false;
			WriteQueue.front().ReadCompleted(transferedBytes);
			if (!WriteQueue.front().GetActiveSize())
				WriteQueue.pop();

			if (!WriteQueue.empty())
				AsyncProcessQueue();
			else if (IsClosing)
				CloseSocket();
		}
		else
			CloseSocket();
	}

#else

	void WriteHandlerWrapper(boost::system::error_code error, std::size_t transferedBytes)
	{
		IsWritingAsync = false;
		HandleQueue();
	}

	bool HandleQueue()
	{
		if (WriteQueue.empty())
			return false;

		MessageBuffer& queuedMessage = WriteQueue.front();

		std::size_t bytesToSend = queuedMessage.GetActiveSize();

		boost::system::error_code error;
		std::size_t bytesSent = SocketImpl.write_some(boost::asio::buffer(queuedMessage.GetReadPointer(), bytesToSend), error);

		if (error)
		{
			if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
				return AsyncProcessQueue();

			WriteQueue.pop();
			if (IsClosing && WriteQueue.empty())
				CloseSocket();
			return false;
		}
		else if (bytesSent == 0)
		{
			WriteQueue.pop();
			if (IsClosing && WriteQueue.empty())
				CloseSocket();
			return false;
		}
		else if (bytesSent < bytesToSend) // now n > 0
		{
			queuedMessage.ReadCompleted(bytesSent);
			return AsyncProcessQueue();
		}

		WriteQueue.pop();
		if (IsClosing && WriteQueue.empty())
			CloseSocket();
		return !WriteQueue.empty();
	}

#endif

	Stream SocketImpl;

	boost::asio::ip::address _remoteAddress;
	uint16 _remotePort;

	MessageBuffer ReadBuffer;
	std::queue<MessageBuffer> WriteQueue;

	std::atomic<bool> IsClosed;
	std::atomic<bool> IsClosing;

	bool IsWritingAsync;
};
*/