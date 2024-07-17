#pragma once

#include <windows.h>
#include <iostream>
#include <TlHelp32.h>

#include <psapi.h>
#include <memory>
#include <fstream>
#include <filesystem>

#include <random>
#include <format>
#include <vector>
#include <regex>

#include <dwmapi.h>

#include <d3d11.h>
#include <d3dx11.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "resources.hpp"
#include "spinners/spinners.hpp"

#include "xorstr/xorstr.hpp"
#include "console/console.hpp"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <MinHook.h>

#include <ThemidaSDK.h>
#include <lazy_importer.hpp>

#define SAFE_CALL( fn ) LI_FN( fn ).safe_cached( )
#define ERROR_ASSERT(Error, ...) { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(nullptr, Buffer, "", MB_SYSTEMMODAL | MB_ICONERROR); ExitProcess(0); }
#define INFO_ASSERT(Error, ...)  { char Buffer[1024 * 16]; sprintf_s(Buffer, sizeof Buffer, Error, __VA_ARGS__); MessageBoxA(nullptr, Buffer, "", MB_SYSTEMMODAL | MB_OK); ExitProcess(0); }

constexpr std::string_view app_version = "1.3.0";