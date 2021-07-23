#include <Windows.h>
#include <typeinfo>
#include <MeltyLib.h>
#include "Memory.h"
#include <iostream>
#include <optional>

FILE* f = new FILE;

struct BlockingState
{
	int lastFrame;
	Action lastAction;
	int hitTimer = 0;
	int timer = 0;
	int gapCounter = 0;
	bool isIdling = false;
	bool isBlocking = false;
	bool isAttacking = false;
	bool started = false;
	bool willReversal = false;
};


static BlockingState p1BS;
static BlockingState p2BS;


void ReversalWakeup(MeltyLib::CharacterObject& chr, short attackId)
{
	if (chr.wakeupFlag == 1)
	{
		chr.inputEvent = attackId;
	}
}

void ReversalOnBlock(MeltyLib::CharacterObject& chr, BlockingState& state, short attackId)
{
	if (!state.isBlocking && state.willReversal)
	{
		chr.inputEvent = attackId;
		state.willReversal = false;
	}
	if (state.isBlocking)
	{
		state.willReversal = true;
	}
}

static bool IsAttacking(const MeltyLib::CharacterObject& chr)
{
	//get a convenient variable other than listing all the attacks
	if ((chr.action >= Action::ACTION_5A && chr.action <= Action::ACTION_jC) || (chr.action >= Action::ACTION_AIRDASH && chr.action < Action::HITSTUN_LIGHT_LEANBACK))
		return true;

	return false;
}

static bool IsBlocking(const MeltyLib::CharacterObject& chr)
{
	if (chr.hitstunOnGround > 0)
		return true;

	return false;
}

static bool IsHit(const MeltyLib::CharacterObject& chr)
{
	if (chr.hitstunOnGround > 0 && !(chr.action == Action::ACTION_STANDBLOCK ||
		chr.action == Action::ACTION_CROUCHBLOCK ||
		chr.action == Action::ACTION_AIRBLOCK))
		return true;

	return false;
}

static bool IsIdle(const MeltyLib::CharacterObject& chr)
{
	if (chr.action == Action::ACTION_IDLE ||
		(chr.action >= Action::ACTION_WALK && chr.action <= Action::ACTION_TURNAROUND) ||
		chr.action == Action::ACTION_LANDING ||
		(chr.action >= Action::ACTION_j9 && chr.action > Action::ACTION_dj7))
		return true;
	return false;
}

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, BlockingState& state)
{
	bool attacking = IsAttacking(chr1);
	bool blocking = IsBlocking(chr2);
	bool idling = IsIdle(chr2);
	std::optional<int> frameAdvantage;

	if (blocking && attacking && !state.started)
	{
		state.started = true;
		state.timer = 0;
	}
	if (state.started)
	{
		if (!attacking && !blocking)
		{
			state.started = false;
			frameAdvantage = state.timer;
		}
		else if (!attacking && blocking)
		{
			++state.timer;
		}
		else if (attacking && !blocking)
		{
			--state.timer;
		}
	}
	state.isAttacking = attacking;
	state.isBlocking = blocking;
	state.isIdling = idling;

	return frameAdvantage;
}
//lol it glitches in pause since it's not synced to the round timer function

std::optional<int> GetGap(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, BlockingState& state)
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

void DisplaySpecialInput(const MeltyLib::CharacterObject* chr, int* rmb)
{
	if (chr->inputEvent > 15 && *rmb != chr->inputEvent)
	{
		printf("%d \n", chr->inputEvent);
	}
	*rmb = chr->inputEvent;
}


int (*oldUpdate)(int) = NULL;
void LabtoolMain(int arg)
{
	static MeltyLib::CharacterObject& chr1 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
	static MeltyLib::CharacterObject& chr2 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;
	auto frameAdvantage1 = GetFrameAdvantage(chr1, chr2, p1BS);
	auto frameAdvantage2 = GetFrameAdvantage(chr2, chr1, p2BS);

	auto gap1 = GetGap(chr1, chr2, p1BS);
	auto gap2 = GetGap(chr2, chr1, p2BS);
	//ReversalOnBlock(chr2, p1BS, 3); // Why does it work the other way around?


	if (gap1) // Looks okay ?
	{
		printf("P1 Gap: %d \n", gap1);
	}
	else if (gap2)
	{
		printf("P2 Gap: %d \n", gap2);
	}

	if (frameAdvantage1)
	{
		printf("P1 is %dF \n", frameAdvantage1);
	}
	else if (frameAdvantage2)
	{
		printf("P2 is %dF \n", frameAdvantage2);
	}

	//DisplaySpecialInput(chr1, &rmb);
	/*
	if (GetAsyncKeyState(VK_ESCAPE) & 1)
	{
		fclose(f);
		FreeConsole();
	}
	*/
	oldUpdate(arg);
}

inline DWORD HookFunction(DWORD addr, DWORD target)
{
	DWORD oldProtect;

	if (!VirtualProtect((void*)addr, 5, PAGE_READWRITE, &oldProtect))
		return 0;

	DWORD old = (*(DWORD*)(addr + 1)) + (addr + 5);
	*((DWORD*)(addr + 1)) = target - (addr + 5);

	if (!VirtualProtect((void*)addr, 5, oldProtect, &oldProtect))
		return 0;

	return old;
}

DWORD WINAPI HookThread(HMODULE hModule)
{
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);

	oldUpdate = (int(*)(int)) HookFunction(MeltyLib::ADDR_CALL_UPDATE_GAME, (DWORD)LabtoolMain);

	return 0;
}