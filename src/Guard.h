//
// Created by PC_volt on 26/04/2022.
//

#ifndef MELTYLABTOOL_GUARD_H
#define MELTYLABTOOL_GUARD_H

#include "../MeltyLib/src/MeltyLib.h"

struct Guard
{
    int lastFrame;
    Action lastAction;
    int hitTimer = 0;
    int timer = 0;
    int gapCounter = 0;
    bool isIdling = false;
    bool isBlocking = false;
    bool isAttacking = false;
    bool started = false;
    bool willReversal = false;
};

static Guard p1BS;
static Guard p2BS;

void MaxGuard(MeltyLib::CharacterObject& chr);
void ResetGuard(MeltyLib::CharacterObject& chr);
void ResetGuards();
void ForceExGuard();

#endif //MELTYLABTOOL_GUARD_H
