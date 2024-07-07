#include "pch.h"

using namespace Z;

void Initialize()
{
	g_Engine = std::make_unique<Engine>();
	g_Engine->Initialize();
	g_Console->Initialize(skCrypt("VAMPIRE: THE MASQUERADE"), false);
	g_Game->Initialize();
	g_D3D11Window->HookD3D();
	g_Hooking->Initialize();
}