//
// Created by PC_volt on 29/04/2022.
//

#include "FunctionCallToggler.h"
#include <stdio.h>
#include <windows.h>

#define ASM_NOP 0x90
#define ASM_CALL 0xE8


bool isCallNOPed(int callerAddress)
{
    char callInstruction = *((char *)callerAddress);
        if (callInstruction == ASM_NOP)
    {
        return true;
    }
    return false;
}

//Give an address and it NOPs it
//If the address given already points towards a NOP, then CALL <address> to restore the regular behaviour
void ToggleNOPAt(int callerAddress, int calleeAddress)
{
    DWORD oldProtect;
    if (!VirtualProtect((void *)callerAddress, 5, PAGE_READWRITE, &oldProtect)) {  }

    if (isCallNOPed(callerAddress))
    {
        printf("fixing back\n");
        *((char *)callerAddress) = ASM_CALL; //write the CALL instruction
        *((DWORD *)(callerAddress + 1)) = callerAddress - (calleeAddress + 5); //write the difference between the caller and callee addresses
    }
    else
    {
        //NOP the 5 bytes
        for (int i = 0; i < 5; ++i)
        {
            *((char *)callerAddress + i) = ASM_NOP;
        }
    }

    if (!VirtualProtect((void *)callerAddress, 5, oldProtect, &oldProtect)) {  }
}
