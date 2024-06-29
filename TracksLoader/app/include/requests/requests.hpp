#pragma once

namespace network
{
	class Requests
	{
	public:
		Requests( ) = default;
		~Requests( ) = default;

		cpr::Response query_status( const std::string &hardwareId );
		cpr::Response post_login( const std::string &, const std::string &, const std::string & );
		cpr::Response post_register( const std::string &, const std::string &, const std::string &, const std::string & );
		cpr::Response post_report( const std::string &hardwareId, const std::string &description, const std::string &image, const int type );

	private:
		cpr::Header headers
		{
			{ E( "Content-Type" ), E( "application/json" ) },
			{ E( "User-Agent" ),  E( "Api/2.0.0 (80ed6bd50b1)" ) },
		};

		cpr::SslOptions ssl_options
		{
			cpr::Ssl( cpr::ssl::PinnedPublicKey{ E( "sha256//u1nDHZAb9P0F23AAgIfUWQeNw82fWpuWOwfYpzHFAkI=" ) } )
		};
	};
}

