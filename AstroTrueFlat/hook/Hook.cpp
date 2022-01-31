#include "Hook.h"
#include <Windows.h>

bool AttachHook(Hook* hookData) {
	DWORD MinLen = 10 + 14; // mov rax, jumpBack; jmp qword ptr
	if (hookData->dwLen < MinLen) return false;

	BYTE stub[] = {
		0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, jumpBack
		0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // jmp qword ptr
	};

	DWORD64 jumpBack = (DWORD64)hookData->pSource + hookData->dwLen;
	memcpy(stub + 2, &jumpBack, 8);
	memcpy(stub + 2 + 8 + 6, &hookData->pDest, 8);
	
	DWORD dwOld = 0;
	VirtualProtect(hookData->pSource, hookData->dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	hookData->originalCode = new BYTE[hookData->dwLen];
	memcpy(hookData->originalCode, hookData->pSource, hookData->dwLen);

	memset(hookData->pSource, 0x90, hookData->dwLen);
	memcpy(hookData->pSource, stub, sizeof(stub));

	DWORD dummy = 0;
	VirtualProtect(hookData->pSource, hookData->dwLen, dwOld, &dummy);
	return true;
}

void DetachHook(Hook* hookData) {
	if (!hookData->originalCode || !hookData->pSource) return;

	DWORD dwOld = 0;
	VirtualProtect(hookData->pSource, hookData->dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	memcpy(hookData->pSource, hookData->originalCode, hookData->dwLen);

	DWORD dummy = 0;
	VirtualProtect(hookData->pSource, hookData->dwLen, dwOld, &dummy);
}