//
// Created by PC_volt on 26/04/2022.
//
#include "IsInAction.h"

bool IsAttacking(const MeltyLib::CharacterObject& chr)
{
    //get a convenient variable other than listing all the attacks
    if ((chr.CSO.action >= Action::ACTION_5A && chr.CSO.action <= Action::ACTION_jC) || (chr.CSO.action >= Action::ACTION_AIRDASH && chr.CSO.action < Action::HITSTUN_LIGHT_LEANBACK))
        return true;

    return false;
}

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
