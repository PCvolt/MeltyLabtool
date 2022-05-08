//
// Created by PC_volt on 26/04/2022.
//

#include "Framedata.h"
#include "IsInAction.h"

void CheckCharactersCurrentAction(const MeltyLib::CharacterObject& chr, Guard& state)
{
    state.isAttacking = IsAttacking(chr);
    state.isStunned = IsStunned(chr);
    state.isIdling = IsNotInCommittalAction(chr.CSO.player);
}

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& p1State, Guard& p2State)
{
    std::optional<int> frameAdvantage;

    if ((p1State.isStunned || p2State.isStunned) && (!p1State.isIdling && !p2State.isIdling))
    {
        p1State.time = 0;
        p1State.started = true;
    }

    if (p1State.started)
    {
        if (!p1State.isStunned && !p2State.isStunned && p1State.isIdling && p2State.isIdling)
        {
            p1State.started = false;
            frameAdvantage = p1State.time;
        }

        if (p1State.isIdling && (p2State.isStunned || !p2State.isIdling))
        {
            ++p1State.time;
        }
        if (p2State.isIdling && (p1State.isStunned || !p1State.isIdling))
        {
            --p1State.time;
        }
    }

    return frameAdvantage;
}
// Find call of function that is updated only if the game is not paused

std::optional<int> GetGap(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& state)
{
    std::optional<int> gap;

    if (state.isStunned)
    {
        if (state.gapCounter > 0 && state.gapCounter <= 30)
            gap = state.gapCounter;
        state.gapCounter = 0;
    }
    if (!state.isStunned)
        state.gapCounter++;
    return gap;
}
