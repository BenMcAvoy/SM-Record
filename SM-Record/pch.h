#pragma once

// Windows Header Files
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

// ImGui
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Vendor/resource
#include <carbon/tools.h>
#include <kiero/kiero.h>
#include <resources/font.h>

// STD/STL
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <optional>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <algorithm>
