//
// Created by PC_volt on 26/04/2022.
//

#ifndef MELTYLABTOOL_FRAMEDATA_H
#define MELTYLABTOOL_FRAMEDATA_H

#include <optional>
#include "Guard.h"
#include "../MeltyLib/src/MeltyLib.h"

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& state);
std::optional<int> GetGap(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, Guard& state);

#endif //MELTYLABTOOL_FRAMEDATA_H
