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

void ResetPositions()
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
}

void SavePositions()
{
    savedPositions.p1Custom.x = MeltyLib::character1.CSO.xPos;
    savedPositions.p1Custom.y = MeltyLib::character1.CSO.yPos;
    savedPositions.p2Custom.x = MeltyLib::character2.CSO.xPos;
    savedPositions.p2Custom.y = MeltyLib::character2.CSO.yPos;
}