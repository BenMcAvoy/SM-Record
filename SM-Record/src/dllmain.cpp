#include "pch.h"
#include "hook.h"

// Defined in `main.cpp`
void Render(State*);
void Update(State*);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	static DisplayHook* dh = DisplayHook::GetInstance();

	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		dh->Render(Render);
		dh->Update(Update);

		dh->Run(hModule);
	}

	if (dwReason == DLL_PROCESS_DETACH)
		dh->Shutdown();

	return TRUE;
}
