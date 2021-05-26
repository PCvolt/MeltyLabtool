#include <Windows.h>
#include <typeinfo>
#include <MeltyLib.h>
#include "Memory.h"
#include <iostream>

void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

	VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
}

void MemoryMain()
{
	MeltyLib::CharacterObject *chr1 = (MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
	MeltyLib::CharacterObject *chr2 = (MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;

	while (true)
	{
		if (chr2->wakeupFlag)
		if (chr2->inputEvent != -1)
			printf("%d \n", chr2->inputEvent);

		if (GetAsyncKeyState(VK_ESCAPE) & 1)
		{
			break;
		}
	}
}