//
// Created by PC_volt on 26/04/2022.
//

#include "Framedata.h"
#include "IsInAction.h"

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& state)
{
    bool attacking = IsAttacking(MeltyLib::character1);
    bool blocking = IsStunned(chr2);
    bool idling1 = IsIdle(chr1);
    bool idling2 = IsIdle(chr2);
    std::optional<int> frameAdvantage;

    if (!idling1 && !idling2 && !state.started)
    {
        state.started = true;
        state.timer = 0;
    }

    if (state.started)
    {
        if (idling1 && idling2)
        {
            state.started = false;
            frameAdvantage = state.timer;
        }
        if (!idling1)
        {
            --state.timer;
        }
        if (!idling2)
        {
            ++state.timer;
        }
    }
    state.isAttacking = attacking;
    state.isBlocking = blocking;

    return frameAdvantage;
}
// Find call of function that is updated only if the game is not paused

std::optional<int> GetGap(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& state)
{
    std::optional<int> gap;

    if (state.isBlocking)
    {
        if (state.gapCounter > 0 && state.gapCounter <= 30)
            gap = state.gapCounter;
        state.gapCounter = 0;
    }
    if (!state.isBlocking)
        state.gapCounter++;
    return gap;
}
