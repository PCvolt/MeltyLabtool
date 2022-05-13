//
// Created by PC_volt on 26/04/2022.
//
#include "IsInAction.h"

bool IsStunned(const MeltyLib::CharacterObject& chr)
{
    if (chr.CSO.hitstunOnGround != 0)
        return true;

    return false;
}

bool IsHit(const MeltyLib::CharacterObject& chr)
{
    if (IsStunned(chr))
    {
        if (!(chr.CSO.action == Action::ACTION_STANDBLOCK ||
              chr.CSO.action == Action::ACTION_CROUCHBLOCK ||
              chr.CSO.action == Action::ACTION_AIRBLOCK))
            return true;
    }

    return false;
}

bool IsBlocking(const MeltyLib::CharacterObject& chr)
{
    if (IsStunned(chr))
    {
        if (chr.CSO.action == Action::ACTION_STANDBLOCK ||
            chr.CSO.action == Action::ACTION_CROUCHBLOCK ||
            chr.CSO.action == Action::ACTION_AIRBLOCK)
            return true;
    }

    return false;
}

bool IsNotInCommittalAction(int playerIndex)
{
    if (playerIndex == 0)
    {
        if (*(int*) MeltyLib::ADDR_P1_COMMITACTIONTIME > 0) {
            return false;
        }
        return true;
    }
    if (playerIndex == 1)
    {
        if (*(int*) MeltyLib::ADDR_P2_COMMITACTIONTIME > 0) {
            return false;
        }
        return true;
    }
}
