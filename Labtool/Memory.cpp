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

	bool guardFrozen = false;
	int guardGaugeQuality = 0;
	bool exGuard = false;
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
	if (chr.CSO.wakeupFlag == 1)
	{
		chr.CSO.inputEvent = attackId;
	}
}

void ReversalOnBlock(MeltyLib::CharacterObject& chr, BlockingState& state, short attackId)
{
	if (!state.isBlocking && state.willReversal)
	{
		chr.CSO.inputEvent = attackId;
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
	if ((chr.CSO.action >= Action::ACTION_5A && chr.CSO.action <= Action::ACTION_jC) || (chr.CSO.action >= Action::ACTION_AIRDASH && chr.CSO.action < Action::HITSTUN_LIGHT_LEANBACK))
		return true;

	return false;
}

static bool IsStunned(const MeltyLib::CharacterObject& chr)
{
	if (chr.CSO.hitstunOnGround > 0)
		return true;

	return false;
}

static bool IsHit(const MeltyLib::CharacterObject& chr)
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

static bool IsBlocking(const MeltyLib::CharacterObject& chr)
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

static bool IsIdle(const MeltyLib::CharacterObject& chr)
{
	if (!IsStunned(chr))
	{
		if (((chr.CSO.action == Action::ACTION_IDLE ||
			(chr.CSO.action >= Action::ACTION_WALK && chr.CSO.action <= Action::ACTION_TURNAROUND) ||
			chr.CSO.action == Action::ACTION_LANDING ||
			(chr.CSO.action >= Action::ACTION_j9 && chr.CSO.action <= Action::ACTION_dj7))
			|| chr.CSO.action == Action::ACTION_STANDBLOCK
			|| chr.CSO.action == Action::ACTION_CROUCHBLOCK
			|| chr.CSO.action == Action::ACTION_AIRBLOCK))
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
	if (chr->CSO.inputEvent > 15 && *rmb != chr->CSO.inputEvent)
	{
		printf("%d \n", chr->CSO.inputEvent);
	}
	*rmb = chr->CSO.inputEvent;
}




auto oldUpdate = (int(*)(int))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldReset = (void(__stdcall*)(int*))NULL;
auto oldComputeGuardGauge = (void(__fastcall*)(void))NULL;

//auto oldComputeGuardGaugeQuality = (void(__fastcall*)(int*))NULL; //characterSubObj


static MeltyLib::CharacterObject& chr1 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
static MeltyLib::CharacterObject& chr2 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;

void __stdcall NewReset(int* dat)
{
	oldReset(dat);
	chr1.CSO.guardGauge = 8000;
	chr2.CSO.guardGauge = 8000;
}
/*
void __declspec(naked) NewComputeGuardGauge()
{
	__asm MOV AL, freezeGuard;
	__asm CMP AL, 0;
	__asm JNZ skipCall;
	__asm CALL [oldComputeGuardGauge];
skipCall:
	__asm RET;
	//oldComputeGuardGauge();
}
*/
/*
void NewComputeGuardGaugeQuality()
{

}
*/

void NewUpdate(int arg)
{
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

	if (GS.exGuard == true)
	{
		chr2.CSO.u_ExGuardFlag = 10;
	}

	if (GS.guardFrozen == true)
	{
		chr2.CSO.guardGauge = 8000;
	}
	oldUpdate(arg);
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

	
	if (GetAsyncKeyState(VK_F7) & 1)
	{
		GS.guardGaugeQuality = (GS.guardGaugeQuality + 0.5); //In [0;2]
		if (GS.guardGaugeQuality > 2)
			GS.guardGaugeQuality = 0;
		printf("%d\n", GS.guardGaugeQuality);
	}
	

	if (GetAsyncKeyState(VK_F8) & 1)
	{
		GS.exGuard = !GS.exGuard;
	}

	if (GetAsyncKeyState(VK_F9) & 1)
	{
		GS.guardFrozen = !GS.guardFrozen;
		if (GS.guardFrozen)
			puts("Guard frozen");
		else
			puts("Guard unfrozen");
	}

	if (GetAsyncKeyState(VK_F10) & 1)
	{
		NewReset(reinterpret_cast<int*>(0x774974));
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
	oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunction(0x4235d1, (DWORD)NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
	oldReset = (void(__stdcall*)(int*)) HookFunction(0x42357D, (DWORD)NewReset); //0x42357D 0x433911

	//oldComputeGuardGauge = (void(__fastcall*)(void)) HookFunction(0x461948, (DWORD)NewComputeGuardGauge);
	
	//0x461928 MeltyLib::EXGUARDFLAG_COMPUTE
	//oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char)) HookFunction(0x426838, (DWORD)NewResetCharacter);
	//0x423460 MeltyLib::BATTLESCENE_INIT
	//0x4265EC MeltyLib::BATTLESCENE_SUBMESSAGE_DISPLAY_RESET
	return 0;
}