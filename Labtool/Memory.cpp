#include <Windows.h>

void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum)
{
    unsigned long OldProtection;
    VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

    memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

    VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
}

void ReadFromMemory(DWORD addressToRead, float value)
{
    value = *(float*)addressToRead;
}