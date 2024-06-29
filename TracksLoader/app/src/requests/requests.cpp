#include "stdafx.hpp"
#include "requests\requests.hpp"
#include "utils\utils.hpp"

#pragma optimize("", off)

cpr::Response network::Requests::query_status( const std::string &hardwareId )
{
	VM_TIGER_RED_START

	nlohmann::json body {};
	body[ E( "hardwareId" ) ] = hardwareId;

	const auto response = cpr::Post( cpr::Url { E( "api.idandev.xyz/status" ) }, cpr::Body { body.dump( ) }, headers, ssl_options );

	log_dbg( E( "Response: {}" ), response.text.c_str( ) );

	return response;

	VM_TIGER_RED_END
}

cpr::Response network::Requests::post_report( const std::string &hardwareId, const std::string &description, const std::string &image, const int type )
{
	VM_TIGER_RED_START

	nlohmann::json body {};
	body[ E( "hardwareId" ) ] = hardwareId;
	body[ E( "image" ) ] = image;
	body[ E( "type" ) ] = utils::string::format( "{}", type );
	body[ E( "description" ) ] = description;

	const auto response = cpr::Post( cpr::Url { E( "api.idandev.xyz/report" ) }, cpr::Body { body.dump( ) }, headers, ssl_options );

	log_dbg( E( "Response: {}" ), response.text.c_str( ) );

	return response;

	VM_TIGER_RED_END
}

cpr::Response network::Requests::post_login( const std::string &username, const std::string &password, const std::string &hardwareId )
{
	VM_TIGER_RED_START

	nlohmann::json body {};
	body[ E( "username" ) ] = username;
	body[ E( "password" ) ] = password;
	body[ E( "hardwareId" ) ] = hardwareId;

	const auto response = cpr::Post(
		cpr::Url { E( "api.idandev.xyz/login" ) },
		cpr::Body { body.dump( ) },
		headers,
		ssl_options
	);

	log_dbg( "response: {}", response.text.c_str( ) );
	return response;

	VM_TIGER_RED_END
}

cpr::Response network::Requests::post_register( const std::string &username, const std::string &password, const std::string &productKey, const std::string &hardwareId )
{
	VM_TIGER_RED_START

	nlohmann::json body {};
	body[ E( "username" ) ] = username;
	body[ E( "password" ) ] = password;
	body[ E( "productKey" ) ] = productKey;
	body[ E( "hardwareId" ) ] = hardwareId;

	const auto response = cpr::Post(
		cpr::Url { E( "api.idandev.xyz/register" ) },
		cpr::Body { body.dump( ) },
		headers,
		ssl_options
	);

	log_dbg( "response: {}", response.text.c_str( ) );
	return response;

	VM_TIGER_RED_END
}

#pragma optimize("", on)