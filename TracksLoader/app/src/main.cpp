#include "stdafx.hpp"
#include "utils\utils.hpp"
#include "menu\menu.hpp"
#include "security\security.hpp"

auto __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int
{
	vmp_ultra;

	utils::process::get_previleges();

	const auto req = std::make_unique<network::Requests>();
	const auto json = nlohmann::json::parse(req->query_status(utils::others::get_hwid_hash()).text, nullptr, false);
	const auto report = std::make_unique<security::runtime_security>( );

	if (json[E("error")].get<bool>() != false)
	{
		ERROR_ASSERT(E("API error!"));
	}

	if (json[E("appVersion")].get<std::string>() != app_version)
	{
		ERROR_ASSERT(E("you're using a old program version, open a ticket to updates!"));
	}

	if (json[E("isBlacklisted")].get<bool>() != false)
	{
		report->security_callback( E( "Blacklist Open Bypass" ), 1 );
	}

	if (utils::system::search_drivers("vgk.sys"))
	{
		ERROR_ASSERT(E("vanguard anti-cheat is running under your computer, please close it!"));
	}

	if (!utils::system::check_graphic_card( ) )
	{
		ERROR_ASSERT( E("A graphics card was not found, and the software cannot run without it. Please contact an administrator! ") )
	}

	report->security_callback( "" , 0 );

	const auto window = std::make_unique<menu::window::create_window>(ImVec2(379, 350));
	if (!window->run(menu::render::style, menu::render::paint))
	{
		window->deatch();
	}

	return EXIT_SUCCESS;

	vmp_end;
}