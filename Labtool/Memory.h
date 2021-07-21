#pragma once
#include <Windows.h>
#include <vector>

void WriteToMemory(DWORD addressToWrite, char* valueToWrite, int byteNum);
void LabtoolMain(int arg);
DWORD WINAPI HookThread(HMODULE hModule);
void CloseLabtool();