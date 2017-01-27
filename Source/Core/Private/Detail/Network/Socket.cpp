#include "Private/Detail/Network/Socket.h"

MessageBuffer& Socket::GetReadBuffer()
{
	return ReadBuffer;
}

void Socket::DelayedCloseSocket()
{
	IsClosing = true;
}

void Socket::CloseSocket()
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

bool Socket::IsOpen() const
{
	return !IsClosed && !IsClosing;
}

void Socket::QueuePacket(MessageBuffer&& buffer)
{
	WriteQueue.push(std::move(buffer));

#ifdef SOCKET_HAS_IOCP
	AsyncProcessQueue();
#endif
}

void Socket::AsyncReadWithCallback(void (Socket::*callback)(asio::error_code, std::size_t))
{
	if (!IsOpen())
		return;

	ReadBuffer.Normalize();
	ReadBuffer.EnsureFreeSpace();
	SocketImpl.async_read_some(asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
		std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Socket::AsyncRead()
{
	if (!IsOpen())
		return;

	ReadBuffer.Normalize();
	ReadBuffer.EnsureFreeSpace();
	SocketImpl.async_read_some(asio::buffer(ReadBuffer.GetWritePointer(), ReadBuffer.GetRemainingSpace()),
		std::bind(&Socket::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

uint16 Socket::GetRemotePort() const
{
	return RemotePort;
}

asio::ip::address Socket::GetRemoteIpAddress() const
{
	return RemoteAddress;
}

bool Socket::Update()
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

Socket::~Socket()
{
	IsClosed = true;
	SocketImpl.close(asio::error_code());
}

Socket::Socket(tcp::socket&& SocketImpl) :
	SocketImpl(std::move(SocketImpl)),
	RemoteAddress(SocketImpl.remote_endpoint().address()),
	RemotePort(SocketImpl.remote_endpoint().port()),
	ReadBuffer(), IsClosed(false), IsClosing(false), IsWritingAsync(false)
{
	ReadBuffer.Resize(READ_BLOCK_SIZE);
}

bool Socket::AsyncProcessQueue()
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

void Socket::OnClose()
{

}
#ifdef SOCKET_HAS_IOCP

void Socket::WriteHandler(asio::error_code error, std::size_t transferedBytes)
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

bool Socket::HandleQueue()
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

void Socket::WriteHandlerWrapper(asio::error_code error, std::size_t transferedBytes)
{
	IsWritingAsync = false;
	HandleQueue();
}

#endif

void Socket::ReadHandlerInternal(asio::error_code error, size_t transferredBytes)
{
	if (error)
	{
		CloseSocket();
		return;
	}

	ReadBuffer.WriteCompleted(transferredBytes);
	ReadHandler();
}

