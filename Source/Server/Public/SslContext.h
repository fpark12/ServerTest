#pragma once
#include <asio/ssl/context.hpp>

class SslContext
{
	DECLARE_SINGLETON(SslContext);
private:
public:
	void Initialize()
	{
		asio::error_code err;
		bool Res = 0;

		std::string CertificateChainFile;
		std::string PrivateKeyFile;
		Res |= GConfig.GetString("LoginService.Initialization.CertificatesFile", CertificateChainFile);
		Res |= GConfig.GetString("LoginService.Initialization.PrivateKeyFile", PrivateKeyFile);


		Context.set_options(asio::ssl::context::no_sslv3, err);
		assert(err.value() == 0);
		Context.use_certificate_chain_file(CertificateChainFile, err);
		assert(err.value() == 0);
		Context.use_private_key_file(PrivateKeyFile, asio::ssl::context::pem, err);
		assert(err.value() == 0);
	}

	asio::ssl::context Context = asio::ssl::context(asio::ssl::context::sslv23);
};

static SslContext& GSslContextInst = Singleton<SslContext>().Instance();
#define GSslContext GSslContextInst
