//
// Created by PC_volt on 15/05/2021.
//

#include <string>
#include <Windows.h>
#include <TlHelp32.h>

DWORD FindProcessId(LPCSTR processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &procEntry))
        {
            do {
                if (!lstrcmpi(procEntry.szExeFile, processName))
                {
                    CloseHandle(hSnapshot);
                    return procEntry.th32ProcessID;
                }
            } while (Process32Next(hSnapshot, &procEntry));
        }
    }

    return 0;
}

bool InjectDLL(DWORD processId, char* dllPath)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);

    if (hProcess && hProcess != INVALID_HANDLE_VALUE)
    {
        LPVOID LoadPath = VirtualAllocEx(hProcess, 0, strlen(dllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (LoadPath)
        {
            WriteProcessMemory(hProcess, LoadPath, dllPath, strlen(dllPath) + 1, 0);
        }
        HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, LoadPath, 0, 0);

        if (hThread)
        {
            CloseHandle(hThread);
        }

        if (hProcess)
        {
            CloseHandle(hProcess);
        }
        return true;
    }
    return false;
}

int main()
{
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::wstring wStr(path);
    std::string str = std::string(wStr.begin(), wStr.end());

    str = str.substr(0, str.find_last_of("\\/"));
    std::string dllPath = str + "\\Labtool.dll";

    InjectDLL(FindProcessId((LPCSTR)"MBAA.exe"), (char*)dllPath.c_str());
}