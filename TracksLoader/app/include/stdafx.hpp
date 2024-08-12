#pragma once

#include <iostream>
#include <windows.h>
#include <filesystem>

#include <TlHelp32.h>
#include <fstream>
#include <memory>
#include <psapi.h>

#include <format>
#include <random>
#include <regex>
#include <vector>
#include <unordered_set>


#include <dwmapi.h>
#include <wbemcli.h>
#include <comutil.h>

#include <d3d11.h>
#include <d3dx11.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "resources.hpp"
#include "spinners\spinners.hpp"

#include "console\console.hpp"
#include "xorstr\xorstr.hpp"

#include <curl/curl.h>
#include <nlohmann\json.hpp>

#include <lazy_importer.hpp>
#include <VMProtectSDK.h>

#define SAFE_CALL( fn ) LI_FN( fn ).safe_cached( )
#define vmp_ultra VMProtectBeginUltra( __FUNCTION__ )
#define vmp_mutation VMProtectBeginMutation( __FUNCTION__ )
#define vmp_virt VMProtectBeginVirtualization(__FUNCTION__)
#define vmp_end VMProtectEnd( )
#define ERROR_ASSERT(Error, ...) { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(nullptr, Buffer, "", MB_SYSTEMMODAL | MB_ICONERROR); ExitProcess(0); }
#define INFO_ASSERT(Error, ...)  { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(nullptr, Buffer, "", MB_SYSTEMMODAL | MB_OK); ExitProcess(0); }

constexpr std::string_view app_version = "1.4.0";