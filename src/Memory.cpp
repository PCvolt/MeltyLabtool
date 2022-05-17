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
auto oldDrawHUDText = (void(__stdcall*)(void))NULL;
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

// void DrawTexture(undefined4 param_1,undefined4 texture,int xPos,int yPos,int yScale,int xOffsetSheet,int yOffsetSheet,undefined4 param_8,undefined4 param_9,
// int hexColor, undefined4 param_11,undefined4 u_alpha)
void __stdcall DrawSomething(int arg1,int *arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8,int arg9,int arg10,int arg11,int arg12)
{
    //oldDrawInfoBackground(arg1, arg2, arg3, arg4 + 50, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
    oldDrawInfoBackground(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
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



/*
 * Draw Frame Advantage and Gaps
 * Draw Attack Info
 * Draw Boxes
 * */

struct point_vertex{
    float x, y, z, rhw;  // The transformed(screen space) position for the vertex.
    DWORD colour;        // The vertex colour.
};

static point_vertex fan2[]={ //A coloured fan

        {325,300,1,1,0xFFFFFFFF},
        {250,175,1,1,0xFFFF0000},{300,165,1,1,0xFF7F7F00},{325,155,1,1,0xFF00FF00},
        {375,165,1,1,0xFF007F7F},{400,185,1,1,0xFF0000FF}
};

void DrawBoxes(MeltyLib::CharacterObject chr)
{
    if (chr.CSO.texture == 0)
    {
        return;
    }

    if (chr.CSO.texture->hurtboxList != 0)
    {
        for (int i = 0; i < chr.CSO.texture->hurtboxCount; ++i)
        {
            float x1 = chr.CSO.texture->hurtboxList[i].x1;
            float y1 = chr.CSO.texture->hurtboxList[i].y1;
            float x2 = chr.CSO.texture->hurtboxList[i].x2;
            float y2 = chr.CSO.texture->hurtboxList[i].y2;

            // First hurtbox is in actuality a collision box
            unsigned long color = 0x8800FF00;
            if (i == 0)
            {
                color = 0x88FFFFFF; //color to grey
            }
            //color to green, CH state in turquoise

            MeltyLib::DrawUtils::Vertex box[] = {
                    {x1, y1, 1, 1, color, 1, 1},
                    {x1, y2, 1, 1, color, 1, 1},
                    {x2, y2, 1, 1, color, 1, 1},
                    {x2, y1, 1, 1, color, 1, 1}
            };

            MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,box, sizeof(MeltyLib::DrawUtils::Vertex));
        }
    }

    if (chr.CSO.texture->hitboxList != 0)
    {
        //printf("%d\n", chr.CSO.texture->hitboxCount);
        for (int i = 0; i < chr.CSO.texture->hitboxCount; ++i)
        {

            float x1 = chr.CSO.texture->hitboxList[i].x1;
            float y1 = chr.CSO.texture->hitboxList[i].y1;
            float x2 = chr.CSO.texture->hitboxList[i].x2;
            float y2 = chr.CSO.texture->hitboxList[i].y2;

            unsigned long color = 0x88FF0000;
            MeltyLib::DrawUtils::Vertex box[] = {
                    {x1, y1, 1, 1, color, 1, 1},
                    {x1, y2, 1, 1, color, 1, 1},
                    {x2, y2, 1, 1, color, 1, 1},
                    {x2, y1, 1, 1, color, 1, 1}
            };

            MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,box, sizeof(MeltyLib::DrawUtils::Vertex));
        }
    }

}

void DrawMyGraphics()
{
    //MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 4,fan2, sizeof(point_vertex));//VertexStreamZeroStride

    if (MeltyLib::character1.isInitialized == 1)
        DrawBoxes(MeltyLib::character1);
    if (MeltyLib::character2.isInitialized == 1)
        DrawBoxes(MeltyLib::character2);
    if (MeltyLib::character3.isInitialized == 1)
        DrawBoxes(MeltyLib::character3);
    if (MeltyLib::character4.isInitialized == 1)
        DrawBoxes(MeltyLib::character4);

}


typedef HRESULT(__stdcall* EndSceneFn)(IDirect3DDevice9*);
typedef HRESULT(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

ResetFn Original_Reset = NULL;
EndSceneFn Original_EndScene = NULL;
int __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice) {
    static bool init = true;

    DrawMyGraphics();

    Original_EndScene(pDevice);
    return 0x8a0e14; //This is not even checked. Soku wink wink.
}

int __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* params) {
    static bool init = true;

    HRESULT hr = Original_Reset(pDevice, params);
    if (hr != S_OK)
        return hr;

    return S_OK;
}

void **CreateDummyVTable(void **oldVTable)
{
    void ** newVTable = (void **)malloc(175 * sizeof(void*));
    memcpy(newVTable, oldVTable, 175 * sizeof(void*)),
    newVTable[42] = (void*)Hooked_EndScene;
    newVTable[16] = (void*)Hooked_Reset;
    return newVTable;
}

void HookDX(IDirect3DDevice9 *device)
{
    void **oldVTable = *(void***)device;
    void **newVTable = CreateDummyVTable(oldVTable);
    (((void**)device)[0]) = newVTable;
    Original_Reset = (ResetFn)oldVTable[16];
    Original_EndScene = (EndSceneFn)oldVTable[42];
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

    HookDX(MeltyLib::pd3dDev);
/*
    oldUpdate = (int(*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATEGAME_CALL, (DWORD) NewUpdateGame);
    oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunctionCall(MeltyLib::ADDR_UPDATE_BATTLESCENE_CALL, (DWORD) NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
    oldReset = (void(__stdcall*)(int*)) HookFunctionCall(0x42357D, (DWORD) NewReset); //0x42357D 0x433911
    oldCreateTrainingMenu = (int(*)(void)) HookFunctionCall(MeltyLib::ADDR_CREATE_TRAININGMENU_CALL, (DWORD) NewCreateTrainingMenu);
*/
    //oldDrawInfoBackground = (void(*)(int,int*,int,int,int,int,int,int,int,int,int,int)) HookFunctionCall(0x4da97d, (DWORD) DrawSomething);
    //oldDrawHUDText = (void(__stdcall*)(void)) HookFunctionCall(MeltyLib::ADDR_DRAW_HUDTEXT_CALL, (DWORD) DrawBoxes);
    return 0;
}