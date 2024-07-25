#include "stdafx.hpp"
#include "utils\utils.hpp"
#include "menu\menu.hpp"
#include "core\memory\memory.hpp"
#include <Security/RuntimeSecurity.hpp>
#include <Security/SyscallManager.hpp>

#pragma optimize("", off)

auto __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int
{
	VM_TIGER_RED_START

	utils::process::get_previleges();

	const auto req = std::make_unique<network::Requests>();
	const auto json = nlohmann::json::parse(req->query_status(utils::others::get_hwid_hash()).text, nullptr, false);

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
		req->post_report(utils::others::get_hwid_hash(), utils::string::format(E("{}"), utils::others::parse_json()), 
												 utils::others::bufferto_base64(utils::others::capture_screen()), 1);
	}

	if (utils::system::search_drivers("vgk.sys"))
	{
		ERROR_ASSERT(E("vanguard anti-cheat is running under your computer, please close it!"));
	}

	/*req->post_report(utils::others::get_hwid_hash(), utils::string::format("{}", utils::others::parse_json()), 
										  utils::others::bufferto_base64(utils::others::capture_screen()), 0);*/

	const auto window = std::make_unique<menu::window::create_window>(ImVec2(379, 350));
	if (!window->run(menu::render::style, menu::render::paint))
	{
		window->deatch();
	}

	return EXIT_SUCCESS;

	VM_TIGER_RED_END
}
#pragma optimize("", on)
