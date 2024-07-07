#pragma once

#define VEC4CPY(i,v)	(v.x = i.x, v.y = i.y, v.z = i.z, v.w = i.w)
#define MAX_VIRTUALKEYS 0x100

namespace Z
{
	struct VirtualKey
	{
		bool bKey, bDown, bUp;
	};
	inline VirtualKey VirtualKeys[MAX_VIRTUALKEYS];

	class Engine
	{
	public:
		HWND p_GameWindow{ 0 };
		int m_GameWidth{ 0 };
		int	m_GameHeight{ 0 };

		bool m_IsShowMenu{ false };

	public:
		void Initialize();

	public:
		static bool GetKeyPress(int VKey, bool Immediate);

		Engine();
		~Engine();
	};
	inline std::unique_ptr<Engine> g_Engine;

	class Console
	{
	public:
		enum EColors : int
		{
			ECOLOR_DEFAULT,
			ECOLOR_DARK_BLUE,
			ECOLOR_DARK_GREEN,
			ECOLOR_DARK_TEAL,
			ECOLOR_DARK_RED,
			ECOLOR_DARK_PINK,
			ECOLOR_DARK_YELLOW,
			ECOLOR_DARK_WHITE,
			ECOLOR_DARK_GRAY,
			ECOLOR_BLUE,
			ECOLOR_GREEN,
			ECOLOR_TEAL,
			ECOLOR_RED,
			ECOLOR_PINK,
			ECOLOR_YELLOW,
			ECOLOR_WHITE,
		};

	public:
		static FILE* p_OutStream;
		static bool m_IsInit;
		bool m_IsShow{ false };
		bool m_IsVerbose{ false };

	public:
		void Initialize(const char* ConsoleName, bool bShowWindow = true);
		void cLog(const char* fmt, const EColors Color = ECOLOR_DEFAULT, ...);
		void LogError(const char* fmt, ...);
		void DestroyConsole();
		void SetConsoleVisibility(bool bShow);
		HANDLE GetHandle();
		HWND GetWindowHandle();

	public:
		static void Log(const char* fmt, ...);
		static void Clear();

	public:
		explicit Console();
		Console(const char* title);
		Console(const char* title, bool bShow);
		~Console();

	private:
		HANDLE p_Handle{};
		HWND p_Hwnd{};
	};
	inline std::unique_ptr<Console> g_Console;

	class D3D11Window
	{
	public:
		enum DXGI : int
		{
			IDXGI_PRESENT = 8,
			IDXGI_DRAW_INDEXED = 12,
			IDXGI_RESIZE_BUFFERS = 13,
		};

	public:
		char s_szDllDirectory[MAX_PATH]{};
		bool m_IsInit{ false };
		bool m_IsInitImGui{ false };
		WNDPROC	m_OldWndProc{};

	public:
		bool GetD3DContext();
		bool HookD3D();
		void UnhookD3D();
		bool InitWindow();
		bool DeleteWindow();
		bool InitImGui(IDXGISwapChain* SwapChain);
		void Overlay(IDXGISwapChain* p_SwapChain);

	public:
		explicit D3D11Window();
		~D3D11Window() noexcept;

	private:
		typedef HRESULT(WINAPI* IDXGIPresent)(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
		typedef HRESULT(WINAPI* IDXGIResizeBuffers)(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

		IDXGIPresent m_pfnIDXGIPresent = 0;
		IDXGIResizeBuffers m_pfnIDXGIResizeBuffers = 0;
		WNDCLASSEX p_WindowClass{};
		HWND p_WindowHwnd{};
		ID3D11Device* p_Device{};
		ID3D11DeviceContext* p_DeviceContext{};
		ID3D11RenderTargetView* p_RenderTargetView{};
		IDXGISwapChain* p_SwapChain{};

	private:
		static LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static HRESULT APIENTRY PresentHook(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
		static HRESULT WINAPI ResizeBuffersHook(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	};
	inline std::unique_ptr<D3D11Window> g_D3D11Window;

	class Hooking
	{
	public:
		void Initialize();
		void Shutdown();
		static bool CreateHook(LPVOID Target, LPVOID Detour, LPVOID* Orig);
		static void	EnableHook(LPVOID Target);
		static void	EnableAllHooks();
		static void	DisableHook(LPVOID Target);
		static void	RemoveHook(LPVOID Target);
		static void	DisableAllHooks();
		static void	RemoveAllHooks();

	public:
		explicit Hooking();
		~Hooking() noexcept;
	};
	inline std::unique_ptr<Hooking> g_Hooking;
}