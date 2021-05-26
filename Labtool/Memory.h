#pragma once
#include <Windows.h>
#include <vector>

void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum);
void MemoryMain();