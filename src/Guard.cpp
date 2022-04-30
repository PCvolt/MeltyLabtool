//
// Created by PC_volt on 26/04/2022.
//
#include <string>
#include "stdio.h"
#include "Guard.h"

Guard p1Guard;
Guard p2Guard;

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

void ToggleMessage(bool toggle, const char* msgTrue, const char* msgFalse)
{
    if (toggle)
    {
        puts(msgTrue);
    }
    else
    {
        puts(msgFalse);
    }
}

void ToggleExGuard()
{
    p1Guard.exGuard = !p1Guard.exGuard;
    p2Guard.exGuard = !p2Guard.exGuard;

    ToggleMessage(p2Guard.exGuard, "EX guard!", "Normal guard");
}

void ToggleFreezeGuard()
{
    p1Guard.frozen = !p1Guard.frozen;
    p2Guard.frozen = !p2Guard.frozen;

    ToggleMessage(p2Guard.exGuard, "Guard frozen", "Guard unfrozen");
}

void IncrementGuardGaugeQuality()
{
    p1Guard.guardGaugeQuality = (p1Guard.guardGaugeQuality + 0.5); //In [0;2]
    if (p1Guard.guardGaugeQuality > 2)
        p1Guard.guardGaugeQuality = 0;
    p2Guard.guardGaugeQuality = (p2Guard.guardGaugeQuality + 0.5); //In [0;2]
    if (p2Guard.guardGaugeQuality > 2)
        p2Guard.guardGaugeQuality = 0;

    MeltyLib::character1.CSO.guardGaugeQuality = p1Guard.guardGaugeQuality;
    MeltyLib::character2.CSO.guardGaugeQuality = p2Guard.guardGaugeQuality;
    printf("Gauge quality set at %.1f\n", p1Guard.guardGaugeQuality);
}