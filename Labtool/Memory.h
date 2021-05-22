#pragma once
#include <Windows.h>

void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum);
void ReadFromMemory(DWORD addressToRead, float value);