#include <Windows.h>
#include <typeinfo>
#include <optional>

#include "Memory.h"
#include "../MeltyLib/src/MeltyLib.h"
#include "Menu.h"
#include "Guard.h"
#include "Position.h"
#include "Framedata.h"
#include "IsInAction.h"
#include "Reversal.h"
#include "FunctionCallToggler.h"

// Original Functions Prototypes
auto oldUpdate = (int(*)(int))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldBattleSceneDraw = (void(*)(int))NULL;
auto oldReset = (void(__stdcall*)(int*))NULL;
auto oldComputeGuardGauge = (void(__fastcall*)(void))NULL;
auto oldDrawInfoBackground = (void(*)(int,int*,int,int,int,int,int,int,int,int,int,int))NULL; // must guess
auto oldDrawHUDText = (void(*)(void))NULL;
auto oldCreateTrainingMenu = (int(*)(void))NULL;

FILE* f = new FILE;

struct GameState
{
    bool frozen = false;
    bool framestep = false;
    int realTimer = 0;
    bool reversalWakeup = false;
};
static GameState GS;

/*
 * Draw Frame Advantage and Gaps
 * Draw Attack Info
 * Draw Boxes
 * */
// void DrawTexture(undefined4 param_1,undefined4 texture,int xPos,int yPos,int yScale,int xOffsetSheet,int yOffsetSheet,undefined4 param_8,undefined4 param_9,
// int hexColor, undefined4 param_11,undefined4 u_alpha)

LPDIRECT3DVERTEXBUFFER9 v_buffer;
#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
void NewDrawHUDText()
{
    oldDrawHUDText();
    MeltyLib::DrawUtils::Vertex vertices[] =
            {

                    {100.f, 200.f, 0.f, 1.f, 0xFFFFFFFF},
                    {500.f, 200.f, 0.f, 1.f, 0xFFFFFFFF},
                    {500.f, 400.f, 0.f, 1.f, 0xFFFFFFFF},
                    {100.f, 400.f, 0.f, 1.f, 0xFFFFFFFF}

                    /*
                    {100, 200, 0, 1, 0xFFFFFFFF},
                    {500, 200, 0, 1, 0xFFFFFFFF},
                    {500, 400, 0, 1, 0xFFFFFFFF},
                    {100, 400, 0, 1, 0xFFFFFFFF}
                     */
            };

     MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(vertices));



    MeltyLib::pd3dDev->CreateVertexBuffer(3*sizeof(MeltyLib::DrawUtils::Vertex),0,CUSTOMFVF,D3DPOOL_MANAGED,&v_buffer,NULL);

    VOID* pVoid;    // the void* we were talking about
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // locks v_buffer, the buffer we made earlier
    memcpy(pVoid, vertices, sizeof(vertices));
    v_buffer->Unlock();

    MeltyLib::pd3dDev->SetFVF(CUSTOMFVF);
    MeltyLib::pd3dDev->SetStreamSource(0, v_buffer, 0, sizeof(MeltyLib::DrawUtils::Vertex));
    MeltyLib::pd3dDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    v_buffer->Release();    // close and release the vertex buffer
}

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

int NewCreateTrainingMenu()
{
    int res = oldCreateTrainingMenu();
    EditTrainingMenuElementsOrder();
    return res;
}

void NewUpdateGame(int arg)
{
    //render_frame();
    int battleMode = *(int*)MeltyLib::ADDR_BATTLEMODE;
    if (battleMode != MeltyLib::PRACTICE_MODE)
    {
        return;
    }

    CheckCharactersCurrentAction(MeltyLib::character1, p1Guard);
    CheckCharactersCurrentAction(MeltyLib::character2, p2Guard);

    auto frameAdvantage = GetFrameAdvantage(MeltyLib::character1, MeltyLib::character2, p1Guard, p2Guard);

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

    if (frameAdvantage)
    {
        printf("P1 is %dF \n\n", frameAdvantage);
    }

    //ReversalOnBlock(chr2, p1Guard, 36); // Why does it work the other way around?
    //DisplaySpecialInput(&chr1, &remember);


    oldUpdate(arg);
}

void ManageToggles()
{
    // RESET POSITIONS
    if (GetAsyncKeyState(0x31) & 1) //key "1"
    {
        ResetPositions();
        // Should regen meter as well
        // Reset Rebeat proratio
    }

    // SAVE POSITIONS
    if (GetAsyncKeyState(0x32) & 1) //key "2"
    {
        SavePositions();
    }

    if (GetAsyncKeyState(VK_F6) & 1)
    {
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERSHADOWS_CALL, MeltyLib::ADDR_DRAW_CHARACTERSHADOWS);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_EFFECTS1_CALL, MeltyLib::ADDR_DRAW_EFFECTS1);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_EFFECTS2_CALL, MeltyLib::ADDR_DRAW_EFFECTS2);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_BATTLEBACKGROUND_CALL, MeltyLib::ADDR_DRAW_BATTLEBACKGROUND);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_HUDTEXT_CALL, MeltyLib::ADDR_DRAW_HUDTEXT);

        //ToggleNOPAt(MeltyLib::ADDR_DRAW_BATTLEBHUD_CALL, MeltyLib::ADDR_DRAW_BATTLEBHUD); //this crashes, have to NOP functions inside
        ToggleNOPAt(MeltyLib::ADDR_DRAW_RESOURCESHUD_CALL, MeltyLib::ADDR_DRAW_RESOURCESHUD);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERRESOURCESTEXT_CALL, MeltyLib::ADDR_DRAW_CHARACTERRESOURCESTEXT);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERRESOURCESTEXT1_CALL, MeltyLib::ADDR_DRAW_CHARACTERRESOURCESTEXT);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERRESOURCES_CALL, MeltyLib::ADDR_DRAW_TEXTURE);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERRESOURCES1_CALL, MeltyLib::ADDR_DRAW_TEXTURE);
        ToggleNOPAt(MeltyLib::ADDR_DRAW_CHARACTERRESOURCES2_CALL, MeltyLib::ADDR_DRAW_TEXTURE);

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
        IncrementGuardGaugeQuality();
    }

    if (GetAsyncKeyState(VK_F8) & 1)
    {
        ToggleExGuard();
    }

    if (GetAsyncKeyState(VK_F9) & 1)
    {
        ToggleFreezeGuard();
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
}

void __fastcall NewBattleSceneUpdate(int arg)
{
    GS.framestep = false;
    ManageToggles();

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

DWORD WINAPI HookThread(HMODULE hModule)
{
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    oldUpdate = (int(*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATEGAME_CALL, (DWORD) NewUpdateGame);
    oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATE_BATTLESCENE_CALL, (DWORD) NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
    oldReset = (void(__stdcall*)(int*)) HookFunctionCall(0x42357D, (DWORD) NewReset); //0x42357D 0x433911
    oldCreateTrainingMenu = (int(*)(void)) HookFunctionCall(MeltyLib::ADDR_CREATE_TRAININGMENU_CALL, (DWORD) NewCreateTrainingMenu);

    //oldDrawInfoBackground = (void(*)(int,int*,int,int,int,int,int,int,int,int,int,int)) HookFunctionCall(MeltyLib::ADDR_DRAW_BATTLEBACKGROUND_CALL, (DWORD) DrawFrameAdvantageAndGap);
    oldDrawHUDText = (void(*)(void)) HookFunctionCall(MeltyLib::ADDR_DRAW_HUDTEXT_CALL, (DWORD) NewDrawHUDText);
    return 0;
}