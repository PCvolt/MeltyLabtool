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


FILE* f = new FILE;

struct GameState
{
    bool frozen = false;
    bool framestep = false;
    int realTimer = 0;

    bool guardFrozen = false;
    int guardGaugeQuality = 0;
    bool exGuard = false;
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

auto oldUpdate = (int(*)(int))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldReset = (void(__stdcall*)(int*))NULL;
auto oldComputeGuardGauge = (void(__fastcall*)(void))NULL;

//auto oldComputeGuardGaugeQuality = (void(__fastcall*)(int*))NULL; //characterSubObj

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
    auto frameAdvantage1 = GetFrameAdvantage(MeltyLib::character1, MeltyLib::character2, p1BS);
    auto frameAdvantage2 = GetFrameAdvantage(MeltyLib::character2, MeltyLib::character1, p2BS);

    auto gap1 = GetGap(MeltyLib::character1, MeltyLib::character2, p1BS);
    auto gap2 = GetGap(MeltyLib::character2, MeltyLib::character1, p2BS);

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

    //ReversalOnBlock(chr2, p1BS, 36); // Why does it work the other way around?
    //DisplaySpecialInput(&chr1, &remember);


    if (GS.reversalWakeup == true)
    {
        ReversalWakeup(MeltyLib::character2, 151);
    }

    if (GS.exGuard == true)
    {
        ForceExGuard();
    }

    if (GS.guardFrozen == true)
    {
        MaxGuard(MeltyLib::character1);
        MaxGuard(MeltyLib::character2);
    }
    oldUpdate(arg);
}

void __fastcall NewBattleSceneUpdate(int arg)
{
    GS.framestep = false;
    // FREEZE
    if (GetAsyncKeyState(VK_F11) & 1)
    {
        GS.frozen = !GS.frozen;
    }

    // FRAMESTEP or not frozen
    if (GetAsyncKeyState(VK_F12) & 1)
    {
        GS.framestep = true;
        //can't tell if I actually performed a framestep or not
    }

    if (GetAsyncKeyState(0x31) & 1) //key "1"
    {
        switch (MeltyLib::character1.CSO.inputDirectionRaw)
        {
            case 4:
                ResetPositionsAt(savedPositions.p1Left, savedPositions.p2Left);
                break;
            case 1:
                ResetPositionsAt(savedPositions.p2Left, savedPositions.p1Left);
                break;
            case 2:
                ResetPositionsAt(savedPositions.p1Center, savedPositions.p2Center);
                break;
            case 3:
                ResetPositionsAt(savedPositions.p2Right, savedPositions.p1Right);
                break;
            case 6:
                ResetPositionsAt(savedPositions.p1Right, savedPositions.p2Right);
                break;
            case 0:
                ResetPositionsAt(savedPositions.p1Custom, savedPositions.p2Custom);
                break;
            default:
                break;
        }

        // Should regen meter as well
    }

    if (GetAsyncKeyState(0x32) & 1) //key "2"
    {
        savedPositions.p1Custom.x = MeltyLib::character1.CSO.xPos;
        savedPositions.p1Custom.y = MeltyLib::character1.CSO.yPos;
        savedPositions.p2Custom.x = MeltyLib::character2.CSO.xPos;
        savedPositions.p2Custom.y = MeltyLib::character2.CSO.yPos;
        puts("Custom positions saved");
    }


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
        GS.guardGaugeQuality = (GS.guardGaugeQuality + 0.5); //In [0;2]
        if (GS.guardGaugeQuality > 2)
            GS.guardGaugeQuality = 0;
        printf("%d\n", GS.guardGaugeQuality);
    }


    if (GetAsyncKeyState(VK_F8) & 1)
    {
        GS.exGuard = !GS.exGuard;
    }

    if (GetAsyncKeyState(VK_F9) & 1)
    {
        GS.guardFrozen = !GS.guardFrozen;
        if (GS.guardFrozen)
            puts("Guard frozen");
        else
            puts("Guard unfrozen");
    }

    if (GetAsyncKeyState(VK_F10) & 1)
    {
        NewReset(reinterpret_cast<int*>(0x774974));
    }

    if (!GS.frozen || GS.framestep == true)
    {
        oldBattleSceneUpdate(arg);
    }
}



// Hooks the function in the stead of the original function.
// We call the original function in the hooked function so to continue normal behaviour.
inline DWORD HookFunction(DWORD addr, DWORD target)
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

}

DWORD WINAPI HookThread(HMODULE hModule)
{
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    oldUpdate = (int(*)(int)) HookFunction(MeltyLib::ADDR_UPDATEGAME_CALL, (DWORD)NewUpdateGame);
    oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunction(0x4235d1, (DWORD)NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
    oldReset = (void(__stdcall*)(int*)) HookFunction(0x42357D, (DWORD)NewReset); //0x42357D 0x433911

    //oldComputeGuardGauge = (void(__fastcall*)(void)) HookFunction(0x461948, (DWORD)NewComputeGuardGauge);

    //0x461928 MeltyLib::EXGUARDFLAG_COMPUTE
    //oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char)) HookFunction(0x426838, (DWORD)NewResetCharacter);
    //0x423460 MeltyLib::BATTLESCENE_INIT
    //0x4265EC MeltyLib::BATTLESCENE_SUBMESSAGE_DISPLAY_RESET
    return 0;
}