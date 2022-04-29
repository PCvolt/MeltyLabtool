#include <Windows.h>
#include <typeinfo>
#include <optional>
#include "Memory.h"

#include "../MeltyLib/src/MeltyLib.h"
#include "Guard.h"
#include "Position.h"
#include "Framedata.h"
#include "IsInAction.h"
#include "Reversal.h"

// Original Functions Prototypes
auto oldUpdate = (int(*)(int))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldBattleSceneDraw = (void(*)(int))NULL;
auto oldReset = (void(__stdcall*)(int*))NULL;
auto oldComputeGuardGauge = (void(__fastcall*)(void))NULL;
auto oldTextureDraw = (void(*)(int,int*,int,int,int,int,int,int,int,int,int,int))NULL; // must guess
//auto oldComputeGuardGaugeQuality = (void(__fastcall*)(int*))NULL; //characterSubObj


FILE* f = new FILE;

struct GameState
{
    bool frozen = false;
    bool framestep = false;
    int realTimer = 0;
    bool reversalWakeup = false;
};

static GameState GS;


void DisplaySpecialInput(const MeltyLib::CharacterObject* chr, int* rmb)
{
    if (chr->CSO.inputEvent > 15 && *rmb != chr->CSO.inputEvent)
    {
        printf("%d \n", chr->CSO.inputEvent);
    }
    *rmb = chr->CSO.inputEvent;
}


void __stdcall NewReset(int* dat)
{
    oldReset(dat);
    ResetGuards();
}
/*
void __declspec(naked) NewComputeGuardGauge()
{
	__asm MOV AL, freezeGuard;
	__asm CMP AL, 0;
	__asm JNZ skipCall;
	__asm CALL [oldComputeGuardGauge];
skipCall:
	__asm RET;
	//oldComputeGuardGauge();
}
*/

void NewUpdateGame(int arg)
{
    int battleMode = *(int*)MeltyLib::ADDR_BATTLEMODE;
    if (battleMode != MeltyLib::PRACTICE_MODE)
    {
        return;
    }
    auto frameAdvantage1 = GetFrameAdvantage(MeltyLib::character1, MeltyLib::character2, p1Guard);
    auto frameAdvantage2 = GetFrameAdvantage(MeltyLib::character2, MeltyLib::character1, p2Guard);

    auto gap1 = GetGap(MeltyLib::character1, MeltyLib::character2, p1Guard);
    auto gap2 = GetGap(MeltyLib::character2, MeltyLib::character1, p2Guard);

    if (gap1)
    {
        printf("P1 Gap: %d \n", gap1);
    }
    else if (gap2)
    {
        printf("P2 Gap: %d \n", gap2);
    }

    /*
    if (frameAdvantage1)
    {
        printf("P1 is %dF \n", frameAdvantage1);
    }
    else if (frameAdvantage2)
    {
        printf("P2 is %dF \n", frameAdvantage2);
    }
    */

    //ReversalOnBlock(chr2, p1Guard, 36); // Why does it work the other way around?
    //DisplaySpecialInput(&chr1, &remember);


    oldUpdate(arg);
}

void __fastcall NewBattleSceneUpdate(int arg)
{
    GS.framestep = false;

    // RESET POSITIONS
    if (GetAsyncKeyState(0x31) & 1) //key "1"
    {
        ResetPositions();
        // Should regen meter as well
    }

    // SAVE POSITIONS
    if (GetAsyncKeyState(0x32) & 1) //key "2"
    {
        SavePositions();
    }

    /* Toggles */
    /*
    if (GetAsyncKeyState(VK_F6) & 1)
    {
        GS.reversalWakeup = !GS.reversalWakeup;
        if (GS.reversalWakeup)
            puts("C-Arc 22B");
        else
            puts("Nothing");
    }
    */

    if (GetAsyncKeyState(VK_F7) & 1)
    {
        IncrementGuardGaugeQuality();
    }

    if (GetAsyncKeyState(VK_F8) & 1)
    {
        ToggleExGuard();
    }

    if (GetAsyncKeyState(VK_F9) & 1)
    {
        ToggleFreezeGuard();
        printf("%d\n", p2Guard.frozen);
    }

    if (GetAsyncKeyState(VK_F10) & 1)
    {
        NewReset(reinterpret_cast<int*>(0x774974)); // not necessary anymore
    }

    // FREEZE
    if (GetAsyncKeyState(VK_F11) & 1)
    {
        GS.frozen = !GS.frozen;
    }

    // FRAMESTEP
    if (GetAsyncKeyState(VK_F12) & 1)
    {
        GS.framestep = true;
    }

    //
    if (GS.reversalWakeup == true)
    {
        ReversalWakeup(MeltyLib::character2, 151);
    }

    if (p2Guard.exGuard == true)
    {
        ForceExGuard();
    }

    if (p2Guard.frozen == true)
    {
        MaxGuard(MeltyLib::character1);
        MaxGuard(MeltyLib::character2);
    }

    if (!GS.frozen || GS.framestep == true)
    {
        oldBattleSceneUpdate(arg);
        // freeze drawing function as well?
    }
}


// Hooks the function in the stead of the original function.
// We call the original function in the hooked function so to continue normal behaviour.
inline DWORD HookFunctionCall(DWORD addr, DWORD target)
{
    DWORD oldProtect;

    if (!VirtualProtect((void*)addr, 5, PAGE_READWRITE, &oldProtect))
        return 0;

    DWORD old = (*(DWORD*)(addr + 1)) + (addr + 5);
    *((DWORD*)(addr + 1)) = target - (addr + 5);

    if (!VirtualProtect((void*)addr, 5, oldProtect, &oldProtect))
        return 0;

    return old;
}

inline DWORD HookAfterFunction(DWORD addr, DWORD target)
{
    DWORD oldProtect;
    // ...
}

DWORD WINAPI HookThread(HMODULE hModule)
{
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    oldUpdate = (int(*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATEGAME_CALL, (DWORD) NewUpdateGame);
    oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATE_BATTLESCENE_CALL, (DWORD) NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
    oldReset = (void(__stdcall*)(int*)) HookFunctionCall(0x42357D, (DWORD) NewReset); //0x42357D 0x433911

    //oldComputeGuardGauge = (void(__fastcall*)(void)) HookFunctionCall(0x461948, (DWORD)NewComputeGuardGauge);

    //0x461928 MeltyLib::EXGUARDFLAG_COMPUTE
    //oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char)) HookFunctionCall(0x426838, (DWORD)NewResetCharacter);
    //0x423460 MeltyLib::BATTLESCENE_INIT
    //0x4265EC MeltyLib::BATTLESCENE_SUBMESSAGE_DISPLAY_RESET
    return 0;
}