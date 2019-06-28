// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


static HRESULT(WINAPI* OrigDirectInput8Create)(
	HINSTANCE hinst,
	DWORD dwVersion,
	REFIID riidltf,
	LPVOID* ppvOut,
	LPUNKNOWN punkOuter
	);

extern "C" __declspec(dllexport) HRESULT WINAPI DirectInput8Create(
	HINSTANCE hinst,
	DWORD dwVersion,
	REFIID riidltf,
	LPVOID* ppvOut,
	LPUNKNOWN punkOuter
)
{
	return OrigDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		char lib_path[MAX_PATH];
		GetSystemDirectoryA(lib_path, MAX_PATH);
		strcat_s(lib_path, "\\dinput8.dll");

		auto lib = LoadLibraryA(lib_path);
		if (!lib)
		{
			return FALSE;
		}

		*(void**)& OrigDirectInput8Create = (void*)GetProcAddress(lib, "DirectInput8Create");

		// ur shit
		LPVOID hMain = (LPVOID) GetModuleHandle(NULL);
		PUCHAR instruction = ((UCHAR*)hMain) + 0x2EC912; // hardcoded for now

		if (*instruction == 0x73)
		{
			DWORD oldProtect;
			VirtualProtect((LPVOID)instruction, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			*instruction = 0xeb;
			VirtualProtect((LPVOID)instruction, 1, oldProtect, &oldProtect);
		}

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}