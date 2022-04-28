//
// Created by PC_volt on 26/04/2022.
//

#ifndef MELTYLABTOOL_POSITION_H
#define MELTYLABTOOL_POSITION_H

#include "../MeltyLib/src/MeltyLib.h"

struct Position
{
    int x;
    int y;

    Position(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

struct SavedPositions
{
    Position p1Left = Position(-65536, 0);
    Position p2Left = Position(-55000, 0);

    Position p1Center = Position(-3000, 0);
    Position p2Center = Position(3000, 0);

    Position p1Right = Position(55000, 0);
    Position p2Right= Position(65536, 0);

    Position p1Custom = Position(-16384, 0);
    Position p2Custom = Position(16384, 0);
};

static SavedPositions savedPositions;
void ResetPositionsAt(Position p1Pos, Position p2Pos);

#endif //MELTYLABTOOL_POSITION_H
