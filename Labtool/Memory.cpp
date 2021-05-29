#include <Windows.h>
#include <typeinfo>
#include <MeltyLib.h>
#include "Memory.h"
#include <iostream>
#include <optional>



void ReversalWakeup(MeltyLib::CharacterObject &chr, short attackId)
{
	if (chr.wakeupFlag == 1)
	{
		chr.inputEvent = attackId;
	}
}

void ReversalOnBlock(MeltyLib::CharacterObject &chr, short attackId)
{
	//remember
	if (chr.hitstunOnGround == 0 /*&& u_hitstunFlags2 == 0 0x1B0*/) //doesn't differentiate on block and on hit
	{
		chr.inputEvent = attackId;
	}
}

static bool IsAttacking(const MeltyLib::CharacterObject &chr)
{
	//get a convenient variable other than listing all the attacks
	if (chr.action >= Action::ACTION_5A && chr.action <= Action::ACTION_jC)
		return true;

	return false;
}

static bool IsBlocking(const MeltyLib::CharacterObject &chr)
{
	if (chr.action == Action::ACTION_STANDBLOCK ||
		chr.action == Action::ACTION_CROUCHBLOCK ||
		chr.action == Action::ACTION_AIRBLOCK)
		return true;
	return false;
}

static bool IsIdle(const MeltyLib::CharacterObject &chr)
{
	if (chr.action == Action::ACTION_IDLE ||
		(chr.action >= Action::ACTION_WALK && chr.action <= Action::ACTION_TURNAROUND) ||
		chr.action == Action::ACTION_LANDING ||
		(chr.action >= Action::ACTION_j9 && chr.action > Action::ACTION_dj7))
		return true;
	return false;
}

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject &chr1, const MeltyLib::CharacterObject &chr2, BlockingState &state)
{
	bool attacking= IsAttacking(chr1);
	bool blocking = IsBlocking(chr2);
	std::optional<int> frameAdvantage;

	if (blocking && attacking)
	{
		state.started = true;
		state.timer = 0;
	}
	if (!attacking && !blocking && state.started)
	{
		state.started = false;
		frameAdvantage = state.wasBlocking ? state.timer : state.timer - 1;
	}
	if (!attacking || !blocking)
	{
		++state.timer;
	}
	state.wasAttacking = attacking;
	state.wasBlocking = blocking;

	return frameAdvantage;
}

std::optional<int> GetGap(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, BlockingState& state)
{
	std::optional<int> gap;

	if (state.wasBlocking)
	{
		if (state.gapCounter >= 0 && state.gapCounter <= 30)
			gap = state.gapCounter;
		state.gapCounter = -1;
	}
	if (!state.wasBlocking)
		state.gapCounter++;
	return gap;
}
//hitstun and blockstun are the same
void DisplaySpecialInput(const MeltyLib::CharacterObject* chr, int *rmb)
{
	if (chr->inputEvent > 15 && *rmb != chr->inputEvent)
	{
		printf("%d \n", chr->inputEvent);
	}
	*rmb = chr->inputEvent;
}

struct BlockingState
{
	int lastFrame;
	Action lastAction;
	unsigned hitTimer = 0;
	unsigned timer = 0;
	int gapCounter = -1;
	bool wasBlocking = false;
	bool wasAttacking = false;
	bool started = false;
};

static BlockingState p1BS;
static BlockingState p2BS;
void MemoryMain()
{
	MeltyLib::CharacterObject &chr1 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
	MeltyLib::CharacterObject &chr2 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;

	while (true)
	{	
		auto frameAdvantage1 = GetFrameAdvantage(chr1, chr2, p1BS);
		auto frameAdvantage2 = GetFrameAdvantage(chr2, chr1, p2BS);

		ReversalWakeup(chr2, 2);

		if (frameAdvantage1)
		{

			printf("%d \n", frameAdvantage1);
		}
		if (frameAdvantage2)
		{

			printf("%d \n", frameAdvantage2);
		}
		//DisplaySpecialInput(chr1, &rmb);

		if (GetAsyncKeyState(VK_ESCAPE) & 1)
		{
			break;
		}
	}
}