// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define _USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <optional>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <cmath>
#include <map>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "MinHook.h"
#pragma comment(lib, "MinHook.lib")

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Spoofer.h"
#include "Lazy.h"
#include "Obfuscate.h"
#include "Memory.h"
#include "SDK.h"
#include "CVars.h"
#include "Drawing.h"
#include "Menu.h"
#include "Feature.h"
#include "Engine.h"

#endif //PCH_H
