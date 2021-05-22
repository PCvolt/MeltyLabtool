// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <cstdio>
#include <iostream>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		FILE* f = new FILE;
		freopen_s(&f, "CONOUT$", "w", stdout);
		std::cout << "soko" << std::endl;
		break;
	}
	return TRUE;
}
