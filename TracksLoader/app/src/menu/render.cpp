#include "stdafx.hpp"
#include "menu\menu.hpp"
#include "core\injection\injection.hpp"
#include "core\cleaner\cleaner.hpp"

void menu::render::style(ID3D11Device* device)
{
	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	static const ImWchar icons_ranges[] = { ICON_MIN_CI, ICON_MAX_16_CI, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	static const ImWchar icons_ranges_2[] = { ICON_MIN_CI, ICON_MAX_16_CI, 0 };
	ImFontConfig icons_config_2; icons_config_2.MergeMode = true; icons_config_2.PixelSnapH = true;

	poppins_bg = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fonts::poppins.data(), 24);
	poppins_mn = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fonts::poppins.data(), 15);
	codicon_mn = io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_FILE_NAME_CI, 32, &icons_config_2, icons_ranges_2);
	open_sans_bd = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fonts::open_sans_bd.data(), 30);

	poppins = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fonts::poppins.data(), 17, nullptr, io.Fonts->GetGlyphRangesDefault());
	codicon = io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_FILE_NAME_CI, 50, &icons_config, icons_ranges);

	style.Colors[ImGuiCol_WindowBg] = ImColor(18, 18, 18, 203);
	style.Colors[ImGuiCol_Border] = ImColor(93, 63, 211, 55);

	style.Colors[ImGuiCol_TextDisabled] = ImColor(205, 205, 205, 155);
	style.Colors[ImGuiCol_TextSelectedBg] = ImColor(93, 63, 211, 95);

	style.Colors[ImGuiCol_Button] = ImColor(93, 63, 211, 245);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(90, 0, 180, 245);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(147, 112, 219, 245);

	style.Colors[ImGuiCol_FrameBg] = ImColor(8, 8, 8, 85);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(8, 8, 8, 85);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(8, 8, 8, 85);

	style.Colors[ImGuiCol_ChildBg] = ImColor(110, 0, 180, 95);
	style.Colors[ImGuiCol_CheckMark] = ImColor(93, 63, 211, 245);

	style.WindowPadding = { 0,0 };
	style.WindowBorderSize = 1;
	style.WindowRounding = 8;

	style.FramePadding = { 8, 8 };
	style.FrameRounding = 2;

	style.FrameBorderSize = 1;
	style.ChildRounding = 4;
}

bool menu::render::paint(HWND hwnd, ImVec2 size)
{
	static auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPos({ 0.f, 0.f });
	ImGui::SetNextWindowSize(size);

	static bool window_state = true;
	ImGui::Begin(E("###main_painel"), &window_state, flags);
	{
		const auto draw = ImGui::GetWindowDrawList();
		static auto req_ = std::make_unique<network::Requests>();

		particles::create_particles(ImColor(93, 63, 211, 25));

		static std::string username{}, password{}, license{};

		static bool skript_tab{ false }, gosth_tab{ false }, unitheft_tab{ false }, tzx_tab{ false };

		static bool stream_mode{ true };
		SetWindowDisplayAffinity(hwnd, stream_mode ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);

		switch (render::state)
		{
		case render::t_tabs::LLOGIN:
		{
			ImGui::PushFont(poppins);
			{
				ImGui::SetCursorPos({ size.x - 25, 10 });
				ImGui::Text(E("X"));
				if (ImGui::IsItemClicked(0))
				{
					window_state = false;
				}
			}
			ImGui::PopFont();

			ImGui::AddFontText(codicon, { 162, 105 }, ICON_CI_GLOBE, ImGui::GetColorU32(ImGuiCol_Button), 18);

			ImGui::PushFont(poppins);
			{
				ImGui::PushItemWidth(193);
				{
					ImGui::SetCursorPos({ 95, 160 });
					ImGui::InputTextWithHint(E("###username_label"), E("Username"), &username, ImGuiInputTextFlags_NoHorizontalScroll);
					ImGui::AddFontText(codicon, { 262, 181 }, ICON_CI_INFO, ImGui::GetColorU32(ImGuiCol_TextDisabled), 6.f);

					ImGui::SetCursorPos({ 95, 203 });
					ImGui::InputTextWithHint(E("###password_label"), E("Password"), &password, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoHorizontalScroll);
					ImGui::AddFontText(codicon, { 264, 223 }, ICON_CI_LOCK, ImGui::GetColorU32(ImGuiCol_TextDisabled), 5.f);
				}
				ImGui::PopItemWidth();

				ImGui::SetCursorPos({ 95, 253 });

				

					if (ImGui::CustomButton(E("Continue"), { 193, 27 }))
					{
						const auto json = nlohmann::json::parse(req_->post_login(username, password, utils::others::get_hwid_hash()).text, nullptr, false);

						if (json[E("error")].get<bool>() == false)
						{
							user_data.days = json[E("daysRemain")].get<int>();
							user_data.subscriptions = json[E("subscriptions")].get<std::vector<std::string>>();

							if (!user_data.subscriptions.empty())
							{
								set_cheat_type(user_data.subscriptions[0]);

								for (const auto& subscription : user_data.subscriptions)
								{
									set_cheat_type(subscription);
								}
								skript_tab = (cheat_type == cheat_t::ESKRIPT);
								gosth_tab = (cheat_type == cheat_t::EGOSTH);
								unitheft_tab = (cheat_type == cheat_t::EUNITHEFT);
								tzx_tab = (cheat_type == cheat_t::ETZX);
							}

							menu::render::state = t_tabs::LLOADINGLOGIN;
						}
						else
						{
							MessageBoxA(nullptr, json[E("message")].get<std::string>().c_str(), nullptr, MB_OK | MB_ICONERROR);
						}

					}

				

			}
			ImGui::PopFont();

			ImGui::PushFont(poppins_mn);
			{
				static ImColor current_user = { 205, 205, 205, 155 };

				ImGui::SetCursorPos({ 118, size.y - 60 });
				ImGui::TextColored(current_user, E("You dont have a account?"));
				if (ImGui::IsItemClicked(0))
				{
					render::state = render::t_tabs::LREGISTER;
				}

				ImGui::IsItemHovered(0) ? current_user = { 255, 255, 255, 155 } : current_user = { 205, 205, 205, 155 };
			}
			ImGui::PopFont();

		}
		break;

		case render::t_tabs::LLOADINGLOGIN:
		{
			ImGui::AddFontText(poppins_bg, { 157, 143 }, E("Loading"), ImGui::GetColorU32(ImGuiCol_TextDisabled), 24);

			ImGui::SetCursorPos({ 157, 183 });
			ImSpinner::SpinnerLemniscate(E("###loading_spinner"), 30, 7, ImColor(93, 63, 211, 120), 4.4f);

			static int count{ 0 };
			count >= 300 ? static_cast<int>(render::state = render::t_tabs::LINJECTOR) : count += 1;
		}
		break;

		case render::t_tabs::LREGISTER:
		{
			ImGui::PushFont(poppins);
			{
				ImGui::SetCursorPos({ size.x - 25, 10 });
				ImGui::Text(E("X"));
				if (ImGui::IsItemClicked(0))
				{
					window_state = false;
				}
			}
			ImGui::PopFont();

			ImGui::AddFontText(codicon, { 162, 105 }, ICON_CI_GLOBE, ImGui::GetColorU32(ImGuiCol_Button), 18);

			ImGui::PushFont(poppins);
			{
				ImGui::PushItemWidth(193);
				{
					ImGui::SetCursorPos({ 95, 145 });
					ImGui::InputTextWithHint(E("###username_label"), E("Username"), &username, ImGuiInputTextFlags_NoHorizontalScroll);
					ImGui::AddFontText(codicon, { 262, 166 }, ICON_CI_INFO, ImGui::GetColorU32(ImGuiCol_TextDisabled), 6.f);

					ImGui::SetCursorPos({ 95, 188 });
					ImGui::InputTextWithHint(E("###password_label"), E("Password"), &password, ImGuiInputTextFlags_NoHorizontalScroll);
					ImGui::AddFontText(codicon, { 264, 208 }, ICON_CI_LOCK, ImGui::GetColorU32(ImGuiCol_TextDisabled), 5.f);

					ImGui::SetCursorPos({ 95, 228 });
					ImGui::InputTextWithHint(E("###license_label"), E("License"), &license, ImGuiInputTextFlags_NoHorizontalScroll);
					ImGui::AddFontText(codicon, { 264, 248 }, ICON_CI_KEY, ImGui::GetColorU32(ImGuiCol_TextDisabled), 5.f);

				}
				ImGui::PopItemWidth();

				ImGui::SetCursorPos({ 95, 275 });

				

					if (ImGui::CustomButton(E("Register"), { 193, 27 }))
					{

						const auto json = nlohmann::json::parse(req_->post_register(username, password, license, utils::others::get_hwid_hash()).text, nullptr, false);
						if (json[E("error")].get<bool>() == false)
						{
							render::state = render::t_tabs::LLOGIN;
						}
						else
						{
							MessageBoxA(nullptr, json[E("message")].get<std::string>().c_str(), nullptr, MB_OK | MB_ICONERROR);
						}

					}

				
			}
			ImGui::PopFont();

			ImGui::PushFont(poppins_mn);
			{
				static ImColor current_user = { 205, 205, 205, 155 };

				ImGui::SetCursorPos({ 125, size.y - 40 });
				ImGui::TextColored(current_user, E("I'm already a customer!"));
				if (ImGui::IsItemClicked(0))
				{
					render::state = render::t_tabs::LLOGIN;
				}

				ImGui::IsItemHovered(0) ? current_user = { 255, 255, 255, 155 } : current_user = { 205, 205, 205, 155 };
			}
			ImGui::PopFont();
		}
		break;

		case t_tabs::LINJECTOR:
		{
			ImGui::PushFont(poppins);
			{
				ImGui::SetCursorPos({ size.x - 25, 10 });
				ImGui::Text(E("X"));
				if (ImGui::IsItemClicked(0))
				{
					window_state = false;
				}
			}
			ImGui::PopFont();

			ImGui::SetCursorPos({ size.x - 75, 9 });
			ImGui::ToggleButton(E("###stream_mode"), &stream_mode);

			ImGui::PushFont(poppins);
			{
				if (!stream_mode)
				{
					ImGui::SetCursorPos({ 110, 300 });
					ImGui::TextColored(static_cast<ImColor>(ImGui::GetColorU32(ImGuiCol_TextDisabled)), E("Stream-Mode is Disabled!"));
				}
			}
			ImGui::PopFont();

			ImGui::SetCursorPos({ 50, 40 });
			ImGui::TextColored(ImColor(200, 200, 200), E("Tracks | Advantages"));

			draw->AddLine({ 50, 68 }, { 210, 68 }, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);

			static float size{ 129 };
			static float size_btn{ 0 };
			draw->AddRectFilled({ 69, 100 }, { 310, size }, ImGui::GetColorU32(ImGuiCol_FrameBg), 4.0f);

			ImGui::PushFont(poppins_mn);
			{
				ImGui::SetCursorPos({ 79, 105 });

				if (menu::render::cheat_type == cheat_t::ESKRIPT)
				{
					ImGui::TextColored(ImColor(200, 200, 200), E("Skript.gg | release"));
				}
				else if (menu::render::cheat_type == cheat_t::EGOSTH)
				{
					ImGui::TextColored(ImColor(200, 200, 200), E("Gosth.gg | release"));
				}
				else if (menu::render::cheat_type == cheat_t::EUNITHEFT)
				{
					ImGui::TextColored(ImColor(200, 200, 200), E("Unitheft | release"));
				}
				else if (menu::render::cheat_type == cheat_t::ETZX)
				{
					ImGui::TextColored(ImColor(200, 200, 200), E("TZX | release"));
				}
				else
				{
					ImGui::TextColored(ImColor(200, 200, 200), E("Unknown"));
				}

				ImGui::AddFontText(codicon, { 284, 117 }, ICON_CI_ARROW_DOWN, ImGui::GetColorU32(ImGuiCol_TextDisabled), 5.f);

				if (ImGui::GetMousePos().x >= 69 && ImGui::GetMousePos().x <= 310 && ImGui::GetMousePos().y >= 100
					&& ImGui::GetMousePos().y <= size)
				{
					if (size <= 160)
					{
						size += 2;
					}

					if (size_btn <= 24)
					{
						size_btn += 2;
					}
				}

				if (size_btn >= 1 && size >= 100)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 3 });
					{
						ImGui::SetCursorPos({ 79, 125 });
						if (ImGui::Button(E("Inject"), { 100, size_btn }))
						{
							if (menu::render::cheat_type == cheat_t::ESKRIPT)
							{
								std::thread( core::initialize_skript ).detach( );
							}
							else if (menu::render::cheat_type == cheat_t::EGOSTH)
							{
								std::thread( core::initialize_gosth ).detach( );
							}
							else if (menu::render::cheat_type == cheat_t::EUNITHEFT)
							{
								std::thread([&] {MessageBoxA(nullptr, E("Unitheft"), nullptr, MB_OK); }).detach();

							}
							else if (menu::render::cheat_type == cheat_t::ETZX)
							{
								std::thread([&] {MessageBoxA(nullptr, E("TZX"), nullptr, MB_OK); }).detach();

							}

							menu::render::state = t_tabs::LLOADINGINJECT;
						}
					}
					ImGui::PopStyleVar();
				}

				if (size >= 129 && !(ImGui::GetMousePos().x >= 69 && ImGui::GetMousePos().x <= 310 && ImGui::GetMousePos().y >= 100
					&& ImGui::GetMousePos().y <= size))
				{
					size -= 2;
					if (size_btn != 0)
					{
						size_btn -= 2;
					}
				}

				draw->AddRectFilled({ 69, 180 }, { 310, 210 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 198 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CLOCK);

					ImGui::SetCursorPos({ 95, 186 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Expires"));

					ImGui::SetCursorPos({ 246, 186 });

					ImGui::TextColored(ImColor(130, 130, 130), user_data.days > 600 ? E("Lifetime") : (std::to_string(user_data.days) + E(" days")).c_str());
				}

				draw->AddRectFilled({ 69, 215 }, { 310, 245 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 233 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CODE);

					ImGui::SetCursorPos({ 95, 221 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Version"));

					ImGui::SetCursorPos({ 248, 221 });
					ImGui::TextColored(ImColor(130, 130, 130), app_version.data());
				}

				draw->AddRectFilled({ 69, 250 }, { 310, 280 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 267 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CALENDAR);

					ImGui::SetCursorPos({ 95, 256 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Last Update"));

					ImGui::SetCursorPos({ 234, 256 });
					ImGui::TextColored(ImColor(130, 130, 130), __DATE__);
				}
			}
			ImGui::PopFont();

			if (user_data.subscriptions.size() > 1)
			{
				ImGui::SetCursorPos({ 164, 322 });

				if (std::find(user_data.subscriptions.begin(), user_data.subscriptions.end(), E("Skript")) != user_data.subscriptions.end())
				{
					if (ImGui::checkbox_c(E("###skript_label"), &skript_tab))
					{
						if (skript_tab)
						{
							unitheft_tab = false;
							gosth_tab = false;
							tzx_tab = false;
							menu::render::cheat_type = cheat_t::ESKRIPT;
						}
					}
				}

				ImGui::SetCursorPos({ 184, 322 });
				if (std::find(user_data.subscriptions.begin(), user_data.subscriptions.end(), E("UniTheft")) != user_data.subscriptions.end())
				{
					if (ImGui::checkbox_c(E("###unitheft_label"), &unitheft_tab))
					{
						if (unitheft_tab)
						{
							skript_tab = false;
							gosth_tab = false;
							tzx_tab = false;
							menu::render::cheat_type = cheat_t::EUNITHEFT;
						}
					}
				}

				ImGui::SetCursorPos({ 201, 322 });
				if (std::find(user_data.subscriptions.begin(), user_data.subscriptions.end(), E("Gosth")) != user_data.subscriptions.end())
				{
					if (ImGui::checkbox_c(E("###gostht_label"), &gosth_tab))
					{
						if (gosth_tab)
						{
							skript_tab = false;
							unitheft_tab = false;
							tzx_tab = false;
							menu::render::cheat_type = cheat_t::EGOSTH;
						}
					}
				}

				ImGui::SetCursorPos({ 219, 322 });
				if (std::find(user_data.subscriptions.begin(), user_data.subscriptions.end(), E("Tzx")) != user_data.subscriptions.end())
				{
					if (ImGui::checkbox_c(E("###tzx_label"), &tzx_tab))
					{
						if (tzx_tab)
						{
							skript_tab = false;
							unitheft_tab = false;
							gosth_tab = false;
							menu::render::cheat_type = cheat_t::ETZX;
						}
					}
				}
			}

			break;
		}


		case render::t_tabs::LLOADINGINJECT:
		{
			ImGui::AddFontText(poppins_bg, { 157, 143 }, E("Loading"), ImGui::GetColorU32(ImGuiCol_TextDisabled), 24);

			ImGui::SetCursorPos({ 157, 183 });
			ImSpinner::SpinnerLemniscate(E("###loading_spinner"), 30, 7, ImGui::GetColorU32(ImGuiCol_Button), 4.4f);

			static int count{ 0 };
			count >= 300 ? static_cast<int>(render::state = render::t_tabs::LCLEANER) : count += 1;
		}
		break;

		case render::t_tabs::LCLEANER:
		{
			ImGui::PushFont(poppins);
			{
				ImGui::SetCursorPos({ size.x - 25, 10 });
				ImGui::Text(E("X"));
				if (ImGui::IsItemClicked(0))
				{
					window_state = false;
				}
			}
			ImGui::PopFont();

			ImGui::SetCursorPos({ size.x - 75, 9 });
			ImGui::ToggleButton(E("###stream_mode"), &stream_mode);

			ImGui::SetCursorPos({ 50, 40 });
			ImGui::TextColored(ImColor(200, 200, 200), E("Tracks | Advantages"));

			draw->AddLine({ 50, 68 }, { 210, 68 }, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);

			ImGui::PushFont(poppins);
			{
				ImGui::PushFont(poppins_mn);
				{
					ImGui::SetCursorPos({ 115, 100 });
					ImGui::Text(E("To perform the log cleanup,\npress the destruct button to \n  initiate the removal of all\n           logs generated."));

					draw->AddLine({ 125, 170 }, { 268, 170 }, ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
				}

				draw->AddRectFilled({ 69, 180 }, { 310, 210 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 198 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CLOCK);

					ImGui::SetCursorPos({ 95, 186 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Expires"));

					ImGui::SetCursorPos({ 246, 186 });
					ImGui::TextColored(ImColor(130, 130, 130), render::user_data.days > 600 ? E("Lifetime") : (std::to_string(render::user_data.days) + E(" days")).data());
				}

				draw->AddRectFilled({ 69, 215 }, { 310, 245 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 233 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CODE);

					ImGui::SetCursorPos({ 95, 221 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Version"));

					ImGui::SetCursorPos({ 248, 221 });
					ImGui::TextColored(ImColor(130, 130, 130), app_version.data());
				}

				draw->AddRectFilled({ 69, 250 }, { 310, 280 }, ImGui::GetColorU32(ImGuiCol_FrameBg), 5.0f);
				{
					draw->AddText(codicon, 5.0f, { 75, 267 }, ImGui::GetColorU32(ImGuiCol_ChildBg), ICON_CI_CALENDAR);

					ImGui::SetCursorPos({ 95, 256 });
					ImGui::TextColored(ImColor(200, 200, 200), E("Last Update"));

					ImGui::SetCursorPos({ 234, 256 });
					ImGui::TextColored(ImColor(130, 130, 130), E(__DATE__));
				}
				ImGui::PopFont();

				ImGui::SetCursorPos({ 72, 300 });
				if (ImGui::CustomButton(E("Destruct"), { 238, 26 }))
				{
					if ( menu::render::cheat_type == cheat_t::ESKRIPT )
					{
						std::thread( core::cleaner::initialize_skript ).detach( );
					}
					else if ( menu::render::cheat_type == cheat_t::EGOSTH )
					{
						std::thread( core::cleaner::initialize_gosth ).detach( );
					}
					else if ( menu::render::cheat_type == cheat_t::EUNITHEFT )
					{
						std::thread( [ & ]
						{
							MessageBoxA( nullptr, E( "Unitheft" ), nullptr, MB_OK );
						} ).detach( );

					}
					else if ( menu::render::cheat_type == cheat_t::ETZX )
					{
						std::thread( [ & ]
						{
							MessageBoxA( nullptr, E( "TZX" ), nullptr, MB_OK );
						} ).detach( );

					}

					render::state = render::t_tabs::LLOADINGDESTRUCT;
				}
			}
			ImGui::PopFont();
		}
		break;

		case render::t_tabs::LLOADINGDESTRUCT:
		{
			ImGui::AddFontText(poppins_bg, { 157, 143 }, E("Loading"), ImGui::GetColorU32(ImGuiCol_TextDisabled), 24);

			ImGui::SetCursorPos({ 157, 183 });
			ImSpinner::SpinnerLemniscate(E("###loading_spinner"), 30, 7, ImGui::GetColorU32(ImGuiCol_Button), 4.4f);

			static int count{ 0 };
			count >= 300 ? static_cast<int>(render::state = render::t_tabs::LFINALIZING) : count += 1;
		}
		break;

		case render::t_tabs::LFINALIZING:
		{
			ImGui::PushFont(open_sans_bd);
			{
				ImGui::SetCursorPos({ 90, 153 });
				ImGui::TextColored(ImColor(255, 255, 255), E("       Chill lil bro\nEverything is Gucci."));
			}
			ImGui::PopFont();

		}
		break;
		}
	}
	ImGui::End();

	return window_state;
}