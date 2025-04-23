#pragma once

#include "pch.h"
#include "state.h"

#ifdef KIERO_USE_MINHOOK
#if KIERO_USE_MINHOOK == 0
#error Kiero: MinHook not included
#endif
#endif

#ifdef KIERO_INCLUDE_D3D11
#if KIERO_INCLUDE_D3D11 == 0
#error Kiero: D3D11 not included
#endif
#endif

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* ResizeBuffers) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

struct D3D11Render {
	Present oPresent = NULL;
	ResizeBuffers oResizeBuffers = NULL;
	WNDPROC oWndProc = NULL;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* mainRenderTargetView = NULL;
	HWND outputWindow = NULL;
	std::function<void(State*)> renderFunction = nullptr;
	bool presentReady = false;
	bool running = true;
};

/// <summary>
/// Configuration for when you run the hook
/// and it begins to render and update
/// </summary>
struct LoopConfig {
	/// <summary>
	/// How often to run the update loop
	/// (this is not synchronized with the game loop)
	/// (this is not synchronized with our/the games render loop)
	/// </summary>
	int updateInterval = 1; // 1ms

	/// <summary>
	/// Shutdown once the loop is broken
	/// </summary>
	bool shutdownAfter = true;
};

class DisplayHook {
public:
	DisplayHook() = default;
	~DisplayHook() = default;

	DisplayHook(const DisplayHook&) = delete;
	DisplayHook& operator=(const DisplayHook&) = delete;

	D3D11Render d3d11Render;
	State state;

	std::function<void(State*)> updateFunction;

	HANDLE hookThread = NULL;

	static DisplayHook* GetInstance();

	void Render(std::function<void(State*)> renderFunction) {
		this->d3d11Render.renderFunction = renderFunction;
	}

	void Update(std::function<void(State*)> updateFunction) {
		this->updateFunction = updateFunction;
	}

	HANDLE Run(HMODULE hModule, LoopConfig loopConfig = LoopConfig());
	void Shutdown(bool fromSelf = false);
};
