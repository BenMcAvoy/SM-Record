#include "pch.h"

#include "hook.h"

#define CHECK_HRES(x) if (x != S_OK) { CERROR("HRESULT failed ({})", __LINE__); return dh->d3d11Render.oPresent(pSwapChain, SyncInterval, Flags); }
#define CHECK_KIERO(x) if (x != kiero::Status::Success) { CERROR("kiero failed with reason {} line {}", (int)x, __LINE__); return 1; }

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static DisplayHook* dh = nullptr;
static LRESULT CALLBACK hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) || dh->state.showMenu)
		return true;

	return CallWindowProc(dh->d3d11Render.oWndProc, hWnd, msg, wParam, lParam);
}

static HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	// Cleanup our old (hooked) resources
	if (dh->d3d11Render.mainRenderTargetView) {
		dh->d3d11Render.mainRenderTargetView->Release();
		dh->d3d11Render.mainRenderTargetView = NULL;
	}

	if (dh->d3d11Render.pContext) {
		dh->d3d11Render.pContext->Release();
		dh->d3d11Render.pContext = NULL;
	}

	if (dh->d3d11Render.pDevice) {
		dh->d3d11Render.pDevice->Release();
		dh->d3d11Render.pDevice = NULL;
	}

	// Cleanup ImGui resources
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Inform the present function that we need to reinitialize
	dh->d3d11Render.presentReady = false;

	// Restore the original WndProc
	SetWindowLongPtr(GetForegroundWindow(), GWLP_WNDPROC, (LONG_PTR)dh->d3d11Render.oWndProc);

	// Call the original function
	return dh->d3d11Render.oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!dh->d3d11Render.running)
		return dh->d3d11Render.oPresent(pSwapChain, SyncInterval, Flags);

	if (!dh->d3d11Render.presentReady) {
		CHECK_HRES(pSwapChain->GetDevice(__uuidof(dh->d3d11Render.pDevice), (void**)&dh->d3d11Render.pDevice));
		dh->d3d11Render.pDevice->GetImmediateContext(&dh->d3d11Render.pContext);

		DXGI_SWAP_CHAIN_DESC sd;
		CHECK_HRES(pSwapChain->GetDesc(&sd))
			ID3D11Texture2D* pBackBuffer = NULL;
		CHECK_HRES(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
		if (pBackBuffer == 0) return dh->d3d11Render.oPresent(pSwapChain, SyncInterval, Flags);
		CHECK_HRES(dh->d3d11Render.pDevice->CreateRenderTargetView(pBackBuffer, NULL, &dh->d3d11Render.mainRenderTargetView));
		pBackBuffer->Release();

		dh->d3d11Render.oWndProc = (WNDPROC)SetWindowLongPtr(sd.OutputWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 8.0f;
		style.FrameRounding = 4.0f;
		style.GrabRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.ScrollbarRounding = 4.0f;
		style.TabRounding = 4.0f;
		style.WindowBorderSize = 2.0f;
		style.FrameBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.GrabMinSize = 8.0f;
		style.GrabRounding = 4.0f;

		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;
		io.Fonts->AddFontFromMemoryCompressedTTF(JetBrainsMonoNF_compressed_data, JetBrainsMonoNF_compressed_size, 18.0f, &config);
		io.Fonts->Build();

		ImGui_ImplWin32_Init(sd.OutputWindow);
		ImGui_ImplDX11_Init(dh->d3d11Render.pDevice, dh->d3d11Render.pContext);

		dh->d3d11Render.outputWindow = sd.OutputWindow;
		dh->d3d11Render.presentReady = true;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (dh->d3d11Render.renderFunction)
		dh->d3d11Render.renderFunction(&dh->state);

	ImGui::EndFrame();
	ImGui::Render();

	dh->d3d11Render.pContext->OMSetRenderTargets(1, &dh->d3d11Render.mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return dh->d3d11Render.oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI RunThread(std::pair<HMODULE, LoopConfig>* pair) {
	auto [hModule, loopConfig] = *pair;
	delete pair;

	auto dh = DisplayHook::GetInstance();

	CHECK_KIERO(kiero::init(kiero::RenderType::D3D11Render));
	CHECK_KIERO(kiero::bind(8, (void**)&dh->d3d11Render.oPresent, hkPresent));
	CHECK_KIERO(kiero::bind(13, (void**)&dh->d3d11Render.oResizeBuffers, hkResizeBuffers));

	CINFO("Hooked Present and ResizeBuffers");

	while (!GetAsyncKeyState(VK_END)) {
		if (dh->updateFunction)
			dh->updateFunction(&dh->state);

		if (GetAsyncKeyState(VK_INSERT) & 1)
			dh->state.showMenu = !dh->state.showMenu;

		static auto milliseconds = std::chrono::milliseconds(loopConfig.updateInterval);
		std::this_thread::sleep_for(milliseconds);
	}

	if (dh->hookThread) {
		CloseHandle(dh->hookThread);
		dh->hookThread = NULL;
	}

	if (loopConfig.shutdownAfter)
		dh->Shutdown();

	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

HANDLE DisplayHook::Run(HMODULE hModule, LoopConfig loopConfig) {
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RunThread, new std::pair<HMODULE, LoopConfig>(hModule, loopConfig), 0, NULL);
	this->hookThread = hThread;
	return hThread;
}

void DisplayHook::Shutdown(bool fromSelf) {
	if (!this->d3d11Render.running)
		return;

	this->d3d11Render.running = false;

	// Cleanup ImGui resources
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Restore the original WndProc
	SetWindowLongPtr(this->d3d11Render.outputWindow, GWLP_WNDPROC, (LONG_PTR)this->d3d11Render.oWndProc);

	// Cleanup our old (hooked) resources
	if (this->d3d11Render.mainRenderTargetView) {
		this->d3d11Render.mainRenderTargetView->Release();
		this->d3d11Render.mainRenderTargetView = NULL;
	}

	if (this->d3d11Render.pContext) {
		this->d3d11Render.pContext->Release();
		this->d3d11Render.pContext = NULL;
	}

	if (this->d3d11Render.pDevice) {
		this->d3d11Render.pDevice->Release();
		this->d3d11Render.pDevice = NULL;
	}

	kiero::unbind(8);
	kiero::unbind(13);
	kiero::shutdown();

	CINFO("Shutdown complete");

	// Terminate the thread
	if (this->hookThread) {
		TerminateThread(this->hookThread, 0);
		CloseHandle(this->hookThread);
		this->hookThread = NULL;
	}
}

DisplayHook* DisplayHook::GetInstance() {
	static DisplayHook instance;
	dh = &instance;
	return &instance;
}
