//
// Created by PC_volt on 26/04/2022.
//

#ifndef MELTYLABTOOL_ISINACTION_H
#define MELTYLABTOOL_ISINACTION_H

#include "../MeltyLib/src/MeltyLib.h"

bool IsAttacking(const MeltyLib::CharacterObject& chr);
bool IsStunned(const MeltyLib::CharacterObject& chr);
bool IsHit(const MeltyLib::CharacterObject& chr);
bool IsBlocking(const MeltyLib::CharacterObject& chr);
bool IsNotInCommittalAction(int playerIndex);


#endif //MELTYLABTOOL_ISINACTION_H
