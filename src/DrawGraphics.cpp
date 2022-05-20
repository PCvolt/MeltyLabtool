//
// Created by PC_volt on 19/05/2022.
//

#include "DrawGraphics.h"

/*
 * Draw Frame Advantage and Gaps
 * Draw Attack Info
 * Draw Boxes
 * */

void DrawBoxes(MeltyLib::CharacterObject chr)
{
    if (chr.CSO.texture == 0)
    {
        return;
    }

    int cameraX = *(int*)MeltyLib::ADDR_CAMERA_X;
    int cameraY = *(int*)MeltyLib::ADDR_CAMERA_Y;
    float lerpScale = *(float*)MeltyLib::ADDR_CAMERA_SCALE_LERP;

    if (chr.CSO.texture->hurtboxList != 0)
    {
        for (int i = 0; i < chr.CSO.texture->hurtboxCount; ++i)
        {
            // When thrown, the character hurtboxCount is of 21 hurtboxes, which are empty past the first actual Boxes
            if (chr.CSO.texture->hurtboxList[i] == 0)
            {
                i = chr.CSO.texture->hurtboxCount; // instantly stops trying to access non-existent pointers to Boxes
            }
            else
            {
                float x1 = chr.CSO.texture->hurtboxList[i]->x1;
                float y1 = chr.CSO.texture->hurtboxList[i]->y1;
                float x2 = chr.CSO.texture->hurtboxList[i]->x2;
                float y2 = chr.CSO.texture->hurtboxList[i]->y2;

                // First hurtbox is in actuality a collision box
                unsigned long color = 0x8800FF00;
                if (i == 0)
                {
                    color = 0x88FFFFFF; //color to grey
                }
                //color to green, CH state in turquoise
                if (chr.CSO.facingLeftFlag)
                {
                    x1 = -x1;
                    x2 = -x2;
                }

                float finalx1 = lerpScale * (chr.CSO.xPos + x1);
                float finaly1 = lerpScale * (chr.CSO.yPos + y1);
                float finalx2 = lerpScale * (chr.CSO.xPos + x2);
                float finaly2 = lerpScale * (chr.CSO.yPos + y2);

                printf("%f ", finalx1);
                printf("%f ", finaly1);
                printf("%f ", finalx2);
                printf("%f ", finaly2);
                printf("\n");

                MeltyLib::DrawUtils::Vertex box[] = {
                        {finalx1, finaly1, 1, 1, color, 1, 1},
                        {finalx1, finaly2, 1, 1, color, 1, 1},
                        {finalx2, finaly2, 1, 1, color, 1, 1},
                        {finalx2, finaly1, 1, 1, color, 1, 1}
                };

                MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,box, sizeof(MeltyLib::DrawUtils::Vertex));
            }
        }
    }

    if (chr.CSO.texture->hitboxList != 0)
    {
        for (int i = 0; i < chr.CSO.texture->hitboxCount; ++i)
        {
            float x1 = (float)chr.CSO.texture->hitboxList[i]->x1;
            float y1 = (float)chr.CSO.texture->hitboxList[i]->y1;
            float x2 = (float)chr.CSO.texture->hitboxList[i]->x2;
            float y2 = (float)chr.CSO.texture->hitboxList[i]->y2;

            unsigned long color = 0x88FF0000;
            if (chr.CSO.facingLeftFlag)
            {
                x1 = -x1;
                x2 = -x2;
            }

            float finalx1 = lerpScale * (cameraX + chr.CSO.xPos + x1);
            float finaly1 = lerpScale * (cameraY + chr.CSO.yPos + y1);
            float finalx2 = lerpScale * (cameraX + chr.CSO.xPos + x2);
            float finaly2 = lerpScale * (cameraY + chr.CSO.yPos + y2);
            MeltyLib::DrawUtils::Vertex box[] = {
                    {finalx1, finaly1, 1, 1, color, 1, 1},
                    {finalx1, finaly2, 1, 1, color, 1, 1},
                    {finalx2, finaly2, 1, 1, color, 1, 1},
                    {finalx2, finaly1, 1, 1, color, 1, 1}
            };

            MeltyLib::pd3dDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2,box, sizeof(MeltyLib::DrawUtils::Vertex));
        }
    }

}

void DrawMyGraphics()
{
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
typedef HRESULT(__stdcall* PresentFn)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
typedef HRESULT(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

ResetFn Original_Reset = NULL;
PresentFn Original_Present = NULL;
EndSceneFn Original_EndScene = NULL;
int __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice) {
    static bool init = true;

    DrawMyGraphics();

    Original_EndScene(pDevice);
    return 0x8a0e14; //This is not even checked. Soku wink wink.
}
/*
int __stdcall Hooked_Present(IDirect3DDevice9* pDevice, RECT *pSourceRect, RECT *pDestRect, HWND hDestWindowOverride, RGNDATA *pDirtyRegion)
{
    //DrawMyGraphics();

    HRESULT hr = Original_Present(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    if (hr != S_OK)
        return hr;
    return S_OK;
}
*/
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
    //newVTable[17] = (void*)Hooked_Present;
    newVTable[16] = (void*)Hooked_Reset;
    return newVTable;
}

void HookDX(IDirect3DDevice9 *device)
{
    void **oldVTable = *(void***)device;
    void **newVTable = CreateDummyVTable(oldVTable);
    (((void**)device)[0]) = newVTable;
    Original_Reset = (ResetFn)oldVTable[16];
    //Original_Reset = (ResetFn)oldVTable[17];
    Original_EndScene = (EndSceneFn)oldVTable[42];
}