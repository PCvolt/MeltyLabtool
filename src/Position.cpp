//
// Created by PC_volt on 26/04/2022.
//

#include "Position.h"
#include "Guard.h"

void ResetPositionsAt(Position p1Pos, Position p2Pos)
{
    MeltyLib::character1.CSO.xPosNext = p1Pos.x;
    MeltyLib::character1.CSO.yPosNext = p1Pos.y;
    MeltyLib::character2.CSO.xPosNext = p2Pos.x;
    MeltyLib::character2.CSO.yPosNext = p2Pos.y;

    ResetGuards();
}