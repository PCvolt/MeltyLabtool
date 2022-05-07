//
// Created by PC_volt on 29/04/2022.
//

#include <windows.h>
#include <stdio.h>
#include "FunctionCallToggler.h"

#define ASM_NOP 0x90
#define ASM_CALL 0xE8


bool isCallNOPed(int callerAddress)
{
    if (*((byte *)callerAddress) == ASM_NOP)
    {
        return true;
    }
    return false;
}

void SetCALLToNOP(int callerAddress)
{
    for (int i = 0; i < 5; ++i)
    {
        *((byte *)callerAddress + i) = ASM_NOP; //NOP the 5 bytes
    }
}

void SetNOPToCALL(int callerAddress, int calleeAddress)
{
    *((byte *)callerAddress) = ASM_CALL; //write the CALL instruction
    *((DWORD *)(callerAddress + 1)) = calleeAddress - (callerAddress + 5); //write the difference between the caller and callee addresses
}

//Give an address and it NOPs it
//If the address given already points towards a NOP, then CALL <address> to restore the regular behaviour
void ToggleNOPAt(int callerAddress, int calleeAddress)
{
    DWORD oldProtect;
    if (!VirtualProtect((void *)callerAddress, 5, PAGE_READWRITE, &oldProtect)) {  }

    if (isCallNOPed(callerAddress))
    {
        SetNOPToCALL(callerAddress, calleeAddress);
    }
    else
    {
        SetCALLToNOP(callerAddress);
    }

    if (!VirtualProtect((void *)callerAddress, 5, oldProtect, &oldProtect)) {  }
}
