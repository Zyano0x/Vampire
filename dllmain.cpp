#include "pch.h"

extern void Initialize();

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	LI_FN(DisableThreadLibraryCalls)(hModule);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		HANDLE pHandleNewTimer = NULL;
		HANDLE pCurrentProcess = NULL;
		DWORD lpflOldProtect = NULL;

		pCurrentProcess = LI_FN(GetCurrentProcess)();
		if (LI_FN(VirtualProtectEx)(pCurrentProcess, hModule, 0x40, PAGE_READWRITE, &lpflOldProtect))
		{
			for (size_t i = 0; i < 4; i++)
			{
				*((DWORD*)hModule + i) = 0;
			}
		}

		if ((LI_FN(VirtualProtectEx)(pCurrentProcess, hModule, 0x108, PAGE_READWRITE, &lpflOldProtect)))
			memset(hModule, 0, 0x108);

		LI_FN(CreateTimerQueueTimer).safe()((PHANDLE)&pHandleNewTimer, 0, (WAITORTIMERCALLBACK)Initialize, 0, 900, 0, 0);
		return TRUE;
	}
	return FALSE;
}

