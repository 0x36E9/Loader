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

#include <ThemidaSDK.h>
#include <lazy_importer.hpp>

#define SAFE_CALL( fn ) LI_FN( fn ).safe_cached( )

constexpr std::string_view app_version = "1.3.0";