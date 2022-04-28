//
// Created by PC_volt on 26/04/2022.
//

#include "Guard.h"
#include "Reversal.h"

void ReversalWakeup(MeltyLib::CharacterObject& chr, short attackId)
{
    if (chr.CSO.wakeupFlag == 1)
    {
        chr.CSO.inputEvent = attackId;
    }
}

void ReversalOnBlock(MeltyLib::CharacterObject& chr, Guard& state, short attackId)
{
    if (!state.isBlocking && state.willReversal)
    {
        chr.CSO.inputEvent = attackId;
        state.willReversal = false;
    }
    if (state.isBlocking)
    {
        state.willReversal = true;
    }
}