#include "stdafx.hpp"
#include "menu\menu.hpp"
#include "security\security.hpp"
#include "utils\utils.hpp"

auto __stdcall WinMain( HINSTANCE, HINSTANCE, LPSTR, int ) -> int
{
	vmp_ultra;

	utils::process::get_previleges( );

	if ( !utils::wmi::initialize( ) )
	{
		ERROR_ASSERT( E( "failed to start wmi!" ) );
	}

	if ( !utils::system::check_graphic_card( ) )
	{
		ERROR_ASSERT( E( "A graphics card was not found, and the software cannot run without it. Please contact an administrator! " ) );
	}

	if ( utils::system::search_drivers( E( "vgk.sys" ) ) )
	{
		ERROR_ASSERT( E( "vanguard anti-cheat is running under your computer, please close it!" ) );
	}

	const nlohmann::json status { { E( "unique" ), utils::others::get_hwid_hash( ) } };

	const auto [status_code, data] = g_requests->post( E( "app/status" ), status );

	if ( status_code != 200 )
	{
		return 1;
	}

	if ( data[ E( "appVersion" ) ].get<std::string>( ) != app_version )
	{
		ERROR_ASSERT( E( "you're using a old program version, open a ticket to updates!" ) );
	}

	const nlohmann::json body
	{
		{ E( "unique" ), utils::others::get_hwid_hash( ) },
		{ E( "type" ), E( "open" ) },
		{ E( "description" ), utils::string::format(E("{}" ),utils::system::get_user_info() ) }
	};

	g_requests->post_with_image( "app/report", utils::others::capture_screen( ), body );

	if ( data[ E( "isBlacklisted" ) ].get<bool>( ) != false )
	{

		if ( data[ E( "isBlacklisted" ) ].get<std::string>( ) == E( "threat" ) )
		{
			// explodir pc
		}

		//tela azul
	}

	const auto window = std::make_unique<menu::window::create_window>( ImVec2( 379, 350 ) );
	if ( !window->run( menu::render::style, menu::render::paint ) )
	{
		window->deatch( );
	}

	return EXIT_SUCCESS;

	vmp_end;
}