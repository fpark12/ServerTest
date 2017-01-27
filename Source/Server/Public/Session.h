#pragma once

//#include "Realm.h"
#include "SslContext.h"
//#include "SslSocket.h"
//#include "Socket.h"
//#include "BigNumber.h"
//#include "QueryResult.h"
//#include "QueryCallbackProcessor.h"
#include <asio/ip/tcp.hpp>
#include <asio/ssl.hpp>
#include <google/protobuf/message.h>
#include <memory>
#include "Realm.h"

using asio::ip::tcp;

namespace pb = google::protobuf;

namespace Login
{
	enum EAccountTypes
	{
		PLAYER = 0,
		MODERATOR = 1,
		GAMEMASTER = 2,
		ADMINISTRATOR = 3,
		CONSOLE = 4                                  // must be always last in list, accounts must have less security level always also
	};

	struct LastPlayedCharacterInfo
	{
		Battlenet::RealmHandle RealmId;
		std::string CharacterName;
		uint64 CharacterGUID;
		uint32 LastPlayedTime;
	};

	struct GameAccountInfo
	{
		void LoadResult(SQLOperationResultSet* fields);

		uint32 Id;
		std::string Name;
		std::string DisplayName;
		bool IsBanned;
		bool IsPermanenetlyBanned;
		EAccountTypes SecurityLevel;

		std::unordered_map<uint32 /*realmAddress*/, uint8> CharacterCounts;
		std::unordered_map<std::string /*subRegion*/, LastPlayedCharacterInfo> LastPlayedCharacters;
	};

	struct AccountInfo
	{
		void LoadResult(SQLOperationResultSet result);

		uint32 Id;
		std::string Login;
		bool IsLockedToIP;
		std::string LockCountry;
		std::string LastIP;
		uint32 FailedLogins;
		bool IsBanned;
		bool IsPermanenetlyBanned;

		std::unordered_map<uint32, GameAccountInfo> GameAccounts;
	};

	class Session : public Socket
	{
	private:
		MessageBuffer _headerLengthBuffer;
		MessageBuffer _headerBuffer;
		MessageBuffer _packetBuffer;

		std::unique_ptr<AccountInfo> _accountInfo;
		GameAccountInfo* _gameAccountInfo;          // Points at selected game account (inside _gameAccounts)

		std::string _locale;
		std::string _os;
		uint32 _build;

		std::string _ipCountry;

		std::array<uint8, 32> _clientSecret;

		bool _authed;

		QueryCallbackProcessor _queryProcessor;

		std::unordered_map<uint32, std::function<void(MessageBuffer)>> _responseCallbacks;
		uint32 _requestToken;

		void AsyncWrite(MessageBuffer* packet);

		void AsyncHandshake();

		void CheckIpCallback(SQLOperationResultSet result);

		typedef uint32(Session::*ClientRequestHandler)(std::unordered_map<std::string, Variant const*> const&, game_utilities::v1::ClientResponse*);
		static std::unordered_map<std::string, ClientRequestHandler> const ClientRequestHandlers;

		uint32 GetRealmListTicket(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 GetLastCharPlayed(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 GetRealmList(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 JoinRealm(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);



	public:
		struct LastPlayedCharacterInfo
		{
			Battlenet::RealmHandle RealmId;
			std::string CharacterName;
			uint64 CharacterGUID;
			uint32 LastPlayedTime;
		};





		explicit Session(tcp::socket&& socket);
		~Session();

		void Start() override;
		bool Update() override;

		uint32 GetAccountId() const { return _accountInfo->Id; }
		uint32 GetGameAccountId() const { return _gameAccountInfo->Id; }

		void SendResponse(uint32 token, pb::Message const* response);
		void SendResponse(uint32 token, uint32 status);

		void SendRequest(uint32 serviceHash, uint32 methodId, pb::Message const* request, std::function<void(MessageBuffer)> callback)
		{
			_responseCallbacks[_requestToken] = std::move(callback);
			SendRequest(serviceHash, methodId, request);
		}

		void SendRequest(uint32 serviceHash, uint32 methodId, pb::Message const* request);

		uint32 HandleLogon(authentication::v1::LogonRequest const* logonRequest);
		uint32 HandleVerifyWebCredentials(authentication::v1::VerifyWebCredentialsRequest const* verifyWebCredentialsRequest);
		uint32 HandleGetAccountState(account::v1::GetAccountStateRequest const* request, account::v1::GetAccountStateResponse* response);
		uint32 HandleGetGameAccountState(account::v1::GetGameAccountStateRequest const* request, account::v1::GetGameAccountStateResponse* response);
		uint32 HandleProcessClientRequest(game_utilities::v1::ClientRequest const* request, game_utilities::v1::ClientResponse* response);
		uint32 HandleGetAllValuesForAttribute(game_utilities::v1::GetAllValuesForAttributeRequest const* request, game_utilities::v1::GetAllValuesForAttributeResponse* response);

		std::string GetClientInfo() const;

	protected:
		void HandshakeHandler(asio::error_code const& error);
		void ReadHandler() override;
		bool ReadHeaderLengthHandler();
		bool ReadHeaderHandler();
		bool ReadDataHandler();


	};
}

/*
namespace bgs
{
	namespace protocol
	{
		class Variant;

		namespace account
		{
			namespace v1
			{
				class GetAccountStateRequest;
				class GetAccountStateResponse;
				class GetGameAccountStateRequest;
				class GetGameAccountStateResponse;
			}
		}

		namespace authentication
		{
			namespace v1
			{
				class LogonRequest;
				class VerifyWebCredentialsRequest;
			}
		}

		namespace game_utilities
		{
			namespace v1
			{
				class ClientRequest;
				class ClientResponse;
				class GetAllValuesForAttributeRequest;
				class GetAllValuesForAttributeResponse;
			}
		}
	}
}

using namespace bgs::protocol;

namespace Battlenet
{
	class Session : public Socket<Session, SslSocket<SslContext>>
	{
		typedef Socket<Session, SslSocket<SslContext>> BattlenetSocket;

	public:
		struct LastPlayedCharacterInfo
		{
			Battlenet::RealmHandle RealmId;
			std::string CharacterName;
			uint64 CharacterGUID;
			uint32 LastPlayedTime;
		};

		struct GameAccountInfo
		{
			void LoadResult(Field* fields);

			uint32 Id;
			std::string Name;
			std::string DisplayName;
			bool IsBanned;
			bool IsPermanenetlyBanned;
			AccountTypes SecurityLevel;

			std::unordered_map<uint32 realmAddress, uint8> CharacterCounts;
			std::unordered_map<std::string subRegion, LastPlayedCharacterInfo> LastPlayedCharacters;
		};

		struct AccountInfo
		{
			void LoadResult(SQLOperationResultSet result);

			uint32 Id;
			std::string Login;
			bool IsLockedToIP;
			std::string LockCountry;
			std::string LastIP;
			uint32 FailedLogins;
			bool IsBanned;
			bool IsPermanenetlyBanned;

			std::unordered_map<uint32, GameAccountInfo> GameAccounts;
		};

		explicit Session(tcp::socket&& socket);
		~Session();

		void Start() override;
		bool Update() override;

		uint32 GetAccountId() const { return _accountInfo->Id; }
		uint32 GetGameAccountId() const { return _gameAccountInfo->Id; }

		void SendResponse(uint32 token, pb::Message const* response);
		void SendResponse(uint32 token, uint32 status);

		void SendRequest(uint32 serviceHash, uint32 methodId, pb::Message const* request, std::function<void(MessageBuffer)> callback)
		{
			_responseCallbacks[_requestToken] = std::move(callback);
			SendRequest(serviceHash, methodId, request);
		}

		void SendRequest(uint32 serviceHash, uint32 methodId, pb::Message const* request);

		uint32 HandleLogon(authentication::v1::LogonRequest const* logonRequest);
		uint32 HandleVerifyWebCredentials(authentication::v1::VerifyWebCredentialsRequest const* verifyWebCredentialsRequest);
		uint32 HandleGetAccountState(account::v1::GetAccountStateRequest const* request, account::v1::GetAccountStateResponse* response);
		uint32 HandleGetGameAccountState(account::v1::GetGameAccountStateRequest const* request, account::v1::GetGameAccountStateResponse* response);
		uint32 HandleProcessClientRequest(game_utilities::v1::ClientRequest const* request, game_utilities::v1::ClientResponse* response);
		uint32 HandleGetAllValuesForAttribute(game_utilities::v1::GetAllValuesForAttributeRequest const* request, game_utilities::v1::GetAllValuesForAttributeResponse* response);

		std::string GetClientInfo() const;

	protected:
		void HandshakeHandler(boost::system::error_code const& error);
		void ReadHandler() override;
		bool ReadHeaderLengthHandler();
		bool ReadHeaderHandler();
		bool ReadDataHandler();

	private:
		void AsyncWrite(MessageBuffer* packet);

		void AsyncHandshake();

		void CheckIpCallback(SQLOperationResultSet result);

		typedef uint32(Session::*ClientRequestHandler)(std::unordered_map<std::string, Variant const*> const&, game_utilities::v1::ClientResponse*);
		static std::unordered_map<std::string, ClientRequestHandler> const ClientRequestHandlers;

		uint32 GetRealmListTicket(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 GetLastCharPlayed(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 GetRealmList(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);
		uint32 JoinRealm(std::unordered_map<std::string, Variant const*> const& params, game_utilities::v1::ClientResponse* response);

		MessageBuffer _headerLengthBuffer;
		MessageBuffer _headerBuffer;
		MessageBuffer _packetBuffer;

		std::unique_ptr<AccountInfo> _accountInfo;
		GameAccountInfo* _gameAccountInfo;          // Points at selected game account (inside _gameAccounts)

		std::string _locale;
		std::string _os;
		uint32 _build;

		std::string _ipCountry;

		std::array<uint8, 32> _clientSecret;

		bool _authed;

		QueryCallbackProcessor _queryProcessor;

		std::unordered_map<uint32, std::function<void(MessageBuffer)>> _responseCallbacks;
		uint32 _requestToken;
	};
}
*/