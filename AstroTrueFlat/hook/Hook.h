#pragma once
#include <windows.h>
struct Hook {
	void* pSource;
	void* pDest;
	int dwLen;
	BYTE* originalCode;
};

bool AttachHook(Hook* hookData);
void DetachHook(Hook* hookData);