#include "stdafx.hpp"
#include "utils\utils.hpp"
#include "menu\menu.hpp"
#include "core\memory\memory.hpp"

#pragma optimize("", off)

auto __stdcall WinMain( HINSTANCE, HINSTANCE, LPSTR, int ) -> int
{
	VM_TIGER_RED_START

		utils::process::get_previleges( );

	if ( utils::system::search_drivers( "vgk.sys" ) )
	{
		MessageBoxA( nullptr, E( "vanguard anti-cheat is running under your computer, please close it!" ), E( "error" ), MB_OK );
		return EXIT_FAILURE;
	}

	const std::vector <core::memory_t> strings
	{
		{ E( "lsass.exe" ), { E( "skript.gg" ), E( "api.idandev.xyz" ), E( "GTS CA 1P50" ) } },
		{ E( "Dnscache" ), { E( "skript.gg" ), E( "api.idandev.xyz" ) } }
	};

	const auto window = std::make_unique<menu::window::create_window>( ImVec2( 379, 350 ) );
	if ( !window->run( menu::render::style, menu::render::paint ) )
	{
		window->deatch( );
	}

	//*reinterpret_cast< uint32_t * >( 0 ) = 0XDEAD;

	return EXIT_SUCCESS;

	VM_TIGER_RED_END
}

#pragma optimize("", on)
