// AstroTrueFlat.cpp : Defines the entry point for the application.
//

#include "AstroTrueFlat.h"
#include "Gui.h"
#include "hook/Hook.h"
#include <windows.h>
#include <iostream>

extern "C" {
	float originalNormalX;
	float originalNormalZ;
	float modifiedNormalX;
	float modifiedNormalZ;
	bool applyNewNormal;
	void patch_deformtool_move();
}

Hook deformToolInteractionHook;
Hook deformToolMoveHook;

void PrepareDebug() {
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	std::cout << "Debug initialized" << std::endl;
}

void Init() {
	DWORD_PTR baseAddress = (DWORD_PTR)GetModuleHandleW(nullptr);
	if (baseAddress == 0) {
		std::cerr << "Failed to get base module handle!" << std::endl;
		return;
	}

	DWORD_PTR moveInject = baseAddress + 0xfaf0b8; // TODO: dynamic detection
	deformToolMoveHook.pSource = (void*)moveInject;
	deformToolMoveHook.pDest = (void*)patch_deformtool_move;
	deformToolMoveHook.dwLen = 36;

	if (!AttachHook(&deformToolMoveHook)) {
		std::cerr << "Failed to attach deform tool move hook!" << std::endl;
		return;
	}

	Gui::UpdatePointers(&originalNormalX, &originalNormalZ, &modifiedNormalX, &modifiedNormalZ, &applyNewNormal);
	Gui::InitGui();
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID reserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
#ifdef _DEBUG
		PrepareDebug();
#endif
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Init, nullptr, 0, 0);
	} else if (dwReason == DLL_PROCESS_DETACH) {
		Gui::DestroyGui();
		DetachHook(&deformToolMoveHook);
	}

	return true;
}