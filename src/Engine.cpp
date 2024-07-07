#include "pch.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Z
{
	FILE* Console::p_OutStream{ nullptr };
	bool Console::m_IsInit{ false };
	static uint64_t* sMethodsTable{ nullptr };

#pragma region ENGINE

	Engine::Engine()
	{
		g_Console = std::make_unique<Console>();
		g_Game = std::make_unique<Game>();
		g_D3D11Window = std::make_unique<D3D11Window>();
		g_Hooking = std::make_unique<Hooking>();
	}

	Engine::~Engine()
	{
		g_Hooking.release();
		g_D3D11Window.release();
		g_Game.release();
		g_Console.release();
	}

	void Engine::Initialize()
	{
		p_GameWindow = GetForegroundWindow();

		RECT tempRECT;
		GetWindowRect(p_GameWindow, &tempRECT);
		m_GameWidth = tempRECT.right - tempRECT.left;
		m_GameHeight = tempRECT.bottom - tempRECT.top;
	}

	bool Engine::GetKeyPress(int VKey, bool Immediate)
	{
		if (VirtualKeys[VKey].bKey)
		{
			VirtualKeys[VKey].bUp = false;
			VirtualKeys[VKey].bDown = true;
		}

		else if (!VirtualKeys[VKey].bKey && VirtualKeys[VKey].bDown)
		{
			VirtualKeys[VKey].bUp = true;
			VirtualKeys[VKey].bDown = false;
		}

		else
		{
			VirtualKeys[VKey].bUp = false;
			VirtualKeys[VKey].bDown = false;
		}

		if (Immediate)
			return VirtualKeys[VKey].bDown;

		else
			return VirtualKeys[VKey].bUp;
	}

#pragma endregion

#pragma region CONSOLE

	Console::Console() {}

	Console::Console(const char* Title) { Initialize(Title); }

	Console::Console(const char* Title, bool bIsShow) { Initialize(Title, bIsShow); }

	Console::~Console() { DestroyConsole(); }

	HANDLE Console::GetHandle() { return this->p_Handle; }

	HWND Console::GetWindowHandle() { return this->p_Hwnd; }

	void Console::Clear() { system(skCrypt("cls")); }

	void Console::Initialize(const char* ConsoleName, bool bShowWindow)
	{
		if (Console::m_IsInit)
		{
			LogError(skCrypt("[!] [Console::InitializeConsole] Failed To Initialize Console.\n"));
			return;
		}

		AllocConsole();
		p_Handle = GetStdHandle(STD_OUTPUT_HANDLE);
		p_Hwnd = GetConsoleWindow();
		freopen_s(&p_OutStream, "CONOUT$", "w", stdout);

		char titleBuff[256];
		sprintf_s(titleBuff, skCrypt("[DEBUG] %s"), ConsoleName);
		SetConsoleTitleA(titleBuff);

		Console::m_IsInit = true;
		Console::m_IsShow = bShowWindow;

		ShowWindow(this->p_Hwnd, Console::m_IsShow ? SW_SHOW : SW_HIDE);
	}

	void Console::cLog(const char* fmt, EColors color, ...)
	{
		SetConsoleTextAttribute(p_Handle, color);
		va_list arg;
		va_start(arg, color);
		vfprintf(p_OutStream, fmt, arg);
		va_end(arg);
		SetConsoleTextAttribute(p_Handle, ECOLOR_DEFAULT);
	}

	void Console::LogError(const char* fmt, ...)
	{
		SetConsoleTextAttribute(p_Handle, ECOLOR_RED);
		va_list arg;
		va_start(arg, fmt);
		vfprintf(p_OutStream, fmt, arg);
		va_end(arg);
		SetConsoleTextAttribute(p_Handle, ECOLOR_DEFAULT);
	}

	void Console::DestroyConsole()
	{
		fclose(p_OutStream);
		DestroyWindow(p_Hwnd);
		FreeConsole();
		m_IsInit = false;
	}

	void Console::SetConsoleVisibility(bool bShow)
	{
		this->m_IsShow = bShow;
		ShowWindow(p_Hwnd, bShow ? SW_SHOW : SW_HIDE);
	}

	void Console::Log(const char* fmt, ...)
	{
		if (!p_OutStream)
			return;

		va_list arg;
		va_start(arg, fmt);
		vfprintf(p_OutStream, fmt, arg);
		va_end(arg);
	}

#pragma endregion

#pragma region D3D11Window

	D3D11Window::D3D11Window() {}

	D3D11Window::~D3D11Window() { m_IsInit = false; }

	LRESULT D3D11Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN:
			VirtualKeys[VK_LBUTTON].bKey = true;
			break;

		case WM_LBUTTONUP:
			VirtualKeys[VK_LBUTTON].bKey = false;
			break;

		case WM_RBUTTONDOWN:
			VirtualKeys[VK_RBUTTON].bKey = true;
			break;

		case WM_RBUTTONUP:
			VirtualKeys[VK_RBUTTON].bKey = false;
			break;

		case WM_KEYDOWN:
			VirtualKeys[wParam].bKey = true;
			break;

		case WM_KEYUP:
			VirtualKeys[wParam].bKey = false;
			break;
		}

		if (Engine::GetKeyPress(VK_INSERT, false))
			g_Engine->m_IsShowMenu = !g_Engine->m_IsShowMenu;

		if (g_Engine->m_IsShowMenu)
		{
			ImGui_ImplWin32_WndProcHandler((HWND)g_D3D11Window->m_OldWndProc, msg, wParam, lParam);
			return true;
		}
		return CallWindowProc((WNDPROC)g_D3D11Window->m_OldWndProc, hWnd, msg, wParam, lParam);
	}

	HRESULT APIENTRY D3D11Window::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		g_D3D11Window->Overlay(pSwapChain);

		return g_D3D11Window->m_pfnIDXGIPresent(pSwapChain, SyncInterval, Flags);
	}

	HRESULT APIENTRY D3D11Window::ResizeBuffersHook(IDXGISwapChain* p, UINT bufferCount, UINT Width, UINT Height, DXGI_FORMAT fmt, UINT scFlags)
	{
		// Get new data & release render target
		g_D3D11Window->p_SwapChain = p;
		g_D3D11Window->p_RenderTargetView->Release();
		g_D3D11Window->p_RenderTargetView = nullptr;

		// Get fn result
		HRESULT result = g_D3D11Window->m_pfnIDXGIResizeBuffers(p, bufferCount, Width, Height, fmt, scFlags);

		// Get new render target
		ID3D11Texture2D* backBuffer;
		p->GetBuffer(0, __uuidof(ID3D11Texture2D*), (LPVOID*)&backBuffer);
		if (backBuffer)
		{
			g_D3D11Window->p_Device->CreateRenderTargetView(backBuffer, 0, &g_D3D11Window->p_RenderTargetView);
			backBuffer->Release();
		}

		// Reset ImGui 
		if (g_D3D11Window->m_IsInitImGui)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(static_cast<float>(Width), static_cast<float>(Height));
		}

		return result;
	}

	bool D3D11Window::HookD3D()
	{
		if (GetD3DContext())
		{
			Hooking::CreateHook((void*)sMethodsTable[IDXGI_PRESENT], &PresentHook, (void**)&m_pfnIDXGIPresent);
			Hooking::CreateHook((void*)sMethodsTable[IDXGI_RESIZE_BUFFERS], &ResizeBuffersHook, (void**)&m_pfnIDXGIResizeBuffers);
			m_IsInit = true;
			return true;
		}
		return false;
	}

	void D3D11Window::UnhookD3D()
	{
		SetWindowLongPtr(g_Engine->p_GameWindow, GWLP_WNDPROC, (LONG_PTR)m_OldWndProc);
		Hooking::DisableHook((void*)sMethodsTable[IDXGI_PRESENT]);
		Hooking::DisableHook((void*)sMethodsTable[IDXGI_RESIZE_BUFFERS]);
		free(sMethodsTable);
	}

	bool D3D11Window::GetD3DContext()
	{
		if (!InitWindow())
			return false;

		HMODULE D3D11Module = GetModuleHandleA(skCrypt("d3d11.dll"));

		D3D_FEATURE_LEVEL FeatureLevel;
		const D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

		DXGI_RATIONAL RefreshRate;
		RefreshRate.Numerator = 60;
		RefreshRate.Denominator = 1;

		DXGI_MODE_DESC BufferDesc;
		BufferDesc.Width = 100;
		BufferDesc.Height = 100;
		BufferDesc.RefreshRate = RefreshRate;
		BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC SampleDesc;
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		SwapChainDesc.BufferDesc = BufferDesc;
		SwapChainDesc.SampleDesc = SampleDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.OutputWindow = p_WindowHwnd;
		SwapChainDesc.Windowed = 1;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain* SwapChain;
		ID3D11Device* Device;
		ID3D11DeviceContext* Context;
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevels, 1, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &Context) < 0)
		{
			DeleteWindow();
			return false;
		}

		sMethodsTable = (uint64_t*)::calloc(205, sizeof(uint64_t));
		memcpy(sMethodsTable, *(uint64_t**)SwapChain, 18 * sizeof(uint64_t));
		memcpy(sMethodsTable + 18, *(uint64_t**)Device, 43 * sizeof(uint64_t));
		memcpy(sMethodsTable + 18 + 43, *(uint64_t**)Context, 144 * sizeof(uint64_t));
		Sleep(1000);

		SwapChain->Release();
		SwapChain = 0;
		Device->Release();
		Device = 0;
		Context->Release();
		Context = 0;
		DeleteWindow();
		return true;
	}

	bool D3D11Window::InitWindow()
	{
		p_WindowClass.cbSize = sizeof(WNDCLASSEX);
		p_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		p_WindowClass.lpfnWndProc = DefWindowProc;
		p_WindowClass.cbClsExtra = 0;
		p_WindowClass.cbWndExtra = 0;
		p_WindowClass.hInstance = GetModuleHandle(NULL);
		p_WindowClass.hIcon = NULL;
		p_WindowClass.hCursor = NULL;
		p_WindowClass.hbrBackground = NULL;
		p_WindowClass.lpszMenuName = NULL;
		p_WindowClass.lpszClassName = L"MJ";
		p_WindowClass.hIconSm = NULL;
		RegisterClassEx(&p_WindowClass);
		p_WindowHwnd = CreateWindow(p_WindowClass.lpszClassName, L"DX11 Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, p_WindowClass.hInstance, NULL);

		if (p_WindowHwnd == NULL)
			return FALSE;
		return TRUE;
	}

	bool D3D11Window::DeleteWindow()
	{
		DestroyWindow(p_WindowHwnd);
		UnregisterClass(p_WindowClass.lpszClassName, p_WindowClass.hInstance);

		if (p_WindowHwnd != 0)
			return FALSE;
		return TRUE;
	}

	bool D3D11Window::InitImGui(IDXGISwapChain* SwapChain)
	{
		if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&p_Device)))
		{
			p_Device->GetImmediateContext(&p_DeviceContext);
			DXGI_SWAP_CHAIN_DESC Desc;
			SwapChain->GetDesc(&Desc);
			g_Engine->p_GameWindow = Desc.OutputWindow;

			ID3D11Texture2D* BackBuffer;
			SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
			p_Device->CreateRenderTargetView(BackBuffer, NULL, &p_RenderTargetView);
			BackBuffer->Release();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.IniFilename = nullptr;

			Menu::Dark();

			ImGui_ImplWin32_Init(g_Engine->p_GameWindow);
			ImGui_ImplDX11_Init(p_Device, p_DeviceContext);
			ImGui_ImplDX11_CreateDeviceObjects();

			m_OldWndProc = (WNDPROC)SetWindowLongPtr(g_Engine->p_GameWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
			m_IsInitImGui = true;
			p_SwapChain = SwapChain;

			if (strcpy_s(s_szDllDirectory, sizeof(s_szDllDirectory), "C:\\RC\\") == 0)
				CreateDirectoryA(s_szDllDirectory, NULL);

			BotInitSettings();
			BotLoadSettings();

			return true;
		}
		m_IsInitImGui = false;
		return false;
	}

	void D3D11Window::Overlay(IDXGISwapChain* pSwapChain)
	{
		if (!m_IsInitImGui)
			InitImGui(pSwapChain);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::GetIO().MouseDrawCursor = g_Engine->m_IsShowMenu;

		g_Game->Visual();
		g_Game->Aimbot();
		g_Game->Misc();

		Menu::Draw();

		ImGui::EndFrame();
		ImGui::Render();
		p_DeviceContext->OMSetRenderTargets(1, &p_RenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

#pragma endregion

#pragma region HOOKING

	Hooking::Hooking()
	{
		MH_Initialize();
	}

	Hooking::~Hooking()
	{
		DisableAllHooks();
		RemoveAllHooks();
		MH_Uninitialize();
	}

	void Hooking::Initialize()
	{
		EnableAllHooks();
	}

	void Hooking::Shutdown()
	{
		RemoveAllHooks();
	}

	bool Hooking::CreateHook(LPVOID lpTarget, LPVOID pDetour, LPVOID* pOrig)
	{
		if (MH_CreateHook(lpTarget, pDetour, pOrig) != MH_OK || MH_EnableHook(lpTarget) != MH_OK)
			return false;
		return true;
	}

	void Hooking::EnableHook(LPVOID lpTarget) { MH_EnableHook(lpTarget); }

	void Hooking::DisableHook(LPVOID lpTarget) { MH_DisableHook(lpTarget); }

	void Hooking::RemoveHook(LPVOID lpTarget) { MH_RemoveHook(lpTarget); }

	void Hooking::EnableAllHooks() { MH_EnableHook(MH_ALL_HOOKS); }

	void Hooking::DisableAllHooks() { MH_DisableHook(MH_ALL_HOOKS); }

	void Hooking::RemoveAllHooks() { MH_RemoveHook(MH_ALL_HOOKS); }

#pragma endregion
}