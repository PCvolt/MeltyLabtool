//
// Created by PC_volt on 26/04/2022.
//

#include "Guard.h"

void MaxGuard(MeltyLib::CharacterObject& chr)
{
    switch (chr.CSO.moon)
    {
        case 0:
            chr.CSO.guardGauge = 8000;
            break;
        case 1:
            chr.CSO.guardGauge = 7000;
            break;
        case 2:
            chr.CSO.guardGauge = 10500;
            break;
        default:
            break;
    }
}

void ResetGuard(MeltyLib::CharacterObject& chr)
{
    MaxGuard(chr);

    chr.CSO.timerGuardQualityRegen = 0;
    chr.CSO.timerGuardQualityRegen = 0;
    chr.CSO.guardGaugeQuality = 0;
}

void ResetGuards()
{
    ResetGuard(MeltyLib::character1);
    ResetGuard(MeltyLib::character2);
}

void ForceExGuard()
{
    MeltyLib::character1.CSO.ExGuardFlag = 10;
    MeltyLib::character2.CSO.ExGuardFlag = 10;
}