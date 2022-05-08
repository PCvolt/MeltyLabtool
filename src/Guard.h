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
    int time = 0;
    int gapCounter = 0;
    bool isIdling = false;
    bool isStunned = false;
    bool isAttacking = false;
    bool started = false;
    bool willReversal = false;

    bool frozen = false;
    bool exGuard = false;
    float guardGaugeQuality = 0;
};

extern Guard p1Guard;
extern Guard p2Guard;

void MaxGuard(MeltyLib::CharacterObject& chr);
void ResetGuard(MeltyLib::CharacterObject& chr);
void ResetGuards();
void ForceExGuard();
void ToggleExGuard();
void ToggleFreezeGuard();
void IncrementGuardGaugeQuality();

#endif //MELTYLABTOOL_GUARD_H
