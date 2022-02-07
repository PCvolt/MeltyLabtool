#include <Windows.h>
#include <typeinfo>
#include <MeltyLib.h>
#include "Memory.h"
#include <iostream>
#include <optional>

FILE* f = new FILE;

struct GameState
{
	bool frozen = false;
	bool framestep = false;
	int realTimer = 0;
};

static GameState GS;

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

static bool IsStunned(const MeltyLib::CharacterObject& chr)
{
	if (chr.hitstunOnGround > 0)
		return true;

	return false;
}

static bool IsHit(const MeltyLib::CharacterObject& chr)
{
	if (IsStunned(chr))
	{
		if (!(chr.action == Action::ACTION_STANDBLOCK ||
			chr.action == Action::ACTION_CROUCHBLOCK ||
			chr.action == Action::ACTION_AIRBLOCK))
			return true;
	}

	return false;
}

static bool IsBlocking(const MeltyLib::CharacterObject& chr)
{
	if (IsStunned(chr))
	{
		if (chr.action == Action::ACTION_STANDBLOCK ||
			chr.action == Action::ACTION_CROUCHBLOCK ||
			chr.action == Action::ACTION_AIRBLOCK)
			return true;
	}

	return false;
}

static bool IsIdle(const MeltyLib::CharacterObject& chr)
{
	if (!IsStunned(chr))
	{
		if (((chr.action == Action::ACTION_IDLE ||
			(chr.action >= Action::ACTION_WALK && chr.action <= Action::ACTION_TURNAROUND) ||
			chr.action == Action::ACTION_LANDING ||
			(chr.action >= Action::ACTION_j9 && chr.action <= Action::ACTION_dj7))
			|| chr.action == Action::ACTION_STANDBLOCK
			|| chr.action == Action::ACTION_CROUCHBLOCK
			|| chr.action == Action::ACTION_AIRBLOCK))
			return true;

	}
	return false;
}

std::optional<int> GetFrameAdvantage(const MeltyLib::CharacterObject& chr1, const MeltyLib::CharacterObject& chr2, BlockingState& state)
{
	bool attacking = IsAttacking(chr1);
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




auto oldUpdate = (int(*)(int))NULL;
auto oldReset = (void(*)(void))NULL;
//auto oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldFct = (void(__fastcall*)(int*))NULL;

void NewReset()
{
	oldReset();
}

void NewUpdate(int arg)
{
	static MeltyLib::CharacterObject& chr1 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
	static MeltyLib::CharacterObject& chr2 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;
	auto frameAdvantage1 = GetFrameAdvantage(chr1, chr2, p1BS);
	auto frameAdvantage2 = GetFrameAdvantage(chr2, chr1, p2BS);

	auto gap1 = GetGap(chr1, chr2, p1BS);
	auto gap2 = GetGap(chr2, chr1, p2BS);
	//ReversalOnBlock(chr2, p1BS, 36); // Why does it work the other way around?
	
	int remember;
	//DisplaySpecialInput(&chr1, &remember);
	//ReversalWakeup(chr2, 56);
	
	if (gap1) // Looks okay ?
	{
		printf("P1 Gap: %d \n", gap1);
	}
	else if (gap2)
	{
		printf("P2 Gap: %d \n", gap2);
	}

	/*
	if (frameAdvantage1)
	{
		printf("P1 is %dF \n", frameAdvantage1);
	}
	else if (frameAdvantage2)
	{
		printf("P2 is %dF \n", frameAdvantage2);
	}
	*/
	//DisplaySpecialInput(chr1, &rmb);
	
	oldUpdate(arg);
}

/*
void __fastcall NewResetCharacter(MeltyLib::CharacterObject* arg1, int trash, byte arg2, int arg3, char arg4)
{
	oldResetCharacter(arg1, trash, arg2, arg3, arg4);
}
*/

void __fastcall NewFct(int* arg)
{
	oldFct(arg);
}

void __fastcall NewBattleSceneUpdate(int arg)
{
	GS.framestep = false;
	// FREEZE
	if (GetAsyncKeyState(VK_F11) & 1)
	{
		GS.frozen = !GS.frozen;
	}

	// FRAMESTEP or not frozen
	if (GetAsyncKeyState(VK_F12) & 1)
	{
		GS.framestep = true;
		//can't tell if I actually performed a framestep or not
	}

	if (GetAsyncKeyState(VK_F9) & 1)
	{
		//NewResetCharacter();
		int* addr = reinterpret_cast <int*>(0x774974);
		NewFct(addr);
	}

	if (GetAsyncKeyState(VK_F10) & 1)
	{
		NewReset();
	}

	if (!GS.frozen || GS.framestep == true)
	{
		oldBattleSceneUpdate(arg);
	}
}



// Hooks the function in the stead of the original function.
// We call the original function in the hooked function so to continue normal behaviour.
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

inline DWORD HookAfterFunction(DWORD addr, DWORD target)
{
	DWORD oldProtect;

}

DWORD WINAPI HookThread(HMODULE hModule)
{
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);

	oldUpdate = (int(*)(int)) HookFunction(MeltyLib::ADDR_CALL_UPDATE_GAME, (DWORD)NewUpdate);
	oldBattleSceneUpdate = (void (__fastcall*)(int)) HookFunction(0x4235d1, (DWORD)NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
	//oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char)) HookFunction(0x426838, (DWORD)NewResetCharacter);
	
	oldFct = (void(__fastcall*)(int*)) HookFunction(0x0, (DWORD)NewFct);
	oldReset = (void(*)(void)) HookFunction(0x423460, (DWORD)NewReset); //MeltyLib::BATTLESCENE_

	//0x423460 MeltyLib::BATTLESCENE_INIT
	//0x4265EC MeltyLib::BATTLESCENE_SUBMESSAGE_DISPLAY_RESET
	return 0;
}