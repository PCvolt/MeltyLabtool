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
	bool reversalWakeup = false;
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


struct Position
{
	int x;
	int y;

	Position(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

struct SavedPositions
{
	Position p1Left = Position(-65536, 0);
	Position p2Left = Position(-55000, 0);

	Position p1Center = Position(-3000, 0);
	Position p2Center = Position(3000, 0);

	Position p1Right = Position(55000, 0);
	Position p2Right= Position(65536, 0);

	Position p1Custom = Position(-16384, 0);
	Position p2Custom = Position(16384, 0);
};

static SavedPositions savedPositions;

//static MeltyLib::CharacterObject& chr1 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_1;
//static MeltyLib::CharacterObject& chr2 = *(MeltyLib::CharacterObject*)MeltyLib::ADDR_CHARACTER_2;

//static MeltyLib::Menu& trainingMenu

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
// Use IsUnderAttack flag to compute frame advantage and gaps?

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
	bool attacking = IsAttacking(MeltyLib::character1);
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


void ResetGuard(MeltyLib::CharacterObject& chr)
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

	chr.CSO.timerGuardQualityRegen = 0;
	chr.CSO.timerGuardQualityRegen = 0;
	chr.CSO.guardGaugeQuality = 0;
}

void ResetGuards()
{
	ResetGuard(MeltyLib::character1);
	ResetGuard(MeltyLib::character2);
}

void ResetPositionsAt(Position p1Pos, Position p2Pos)
{
	MeltyLib::character1.CSO.xPosNext = p1Pos.x;
	MeltyLib::character1.CSO.yPosNext = p1Pos.y;
	MeltyLib::character2.CSO.xPosNext = p2Pos.x;
	MeltyLib::character2.CSO.yPosNext = p2Pos.y;

	ResetGuards();
}


auto oldUpdate = (int(*)(int))NULL;
auto oldBattleSceneUpdate = (void(__fastcall*)(int))NULL;
auto oldReset = (void(__stdcall*)(int*))NULL;
auto oldComputeGuardGauge = (void(__fastcall*)(void))NULL;

//auto oldComputeGuardGaugeQuality = (void(__fastcall*)(int*))NULL; //characterSubObj


void __stdcall NewReset(int* dat)
{
	oldReset(dat);
	ResetGuards();
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

void NewUpdate(int arg)
{
	int battleMode = *(int*)MeltyLib::ADDR_BATTLEMODE;
	if (battleMode != MeltyLib::PRACTICE)
	{
		return;
	}
	auto frameAdvantage1 = GetFrameAdvantage(MeltyLib::character1, MeltyLib::character2, p1BS);
	auto frameAdvantage2 = GetFrameAdvantage(MeltyLib::character2, MeltyLib::character1, p2BS);

	auto gap1 = GetGap(MeltyLib::character1, MeltyLib::character2, p1BS);
	auto gap2 = GetGap(MeltyLib::character2, MeltyLib::character1, p2BS);
	//ReversalOnBlock(chr2, p1BS, 36); // Why does it work the other way around?

	int remember;
	//DisplaySpecialInput(&chr1, &remember);
	if (GS.reversalWakeup == true)
	{
		ReversalWakeup(MeltyLib::character2, 151);
	}

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
		MeltyLib::character2.CSO.ExGuardFlag = 10;
	}

	if (GS.guardFrozen == true)
	{
		MeltyLib::character2.CSO.guardGauge = 8000;
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

	if (GetAsyncKeyState(0x31) & 1) //key "1"
	{
		switch (MeltyLib::character1.CSO.inputDirectionRaw)
		{
		case 4:
			ResetPositionsAt(savedPositions.p1Left, savedPositions.p2Left);
			break;
		case 1:
			ResetPositionsAt(savedPositions.p2Left, savedPositions.p1Left);
			break;
		case 2:
			ResetPositionsAt(savedPositions.p1Center, savedPositions.p2Center);
			break;
		case 3:
			ResetPositionsAt(savedPositions.p2Right, savedPositions.p1Right);
			break;
		case 6:
			ResetPositionsAt(savedPositions.p1Right, savedPositions.p2Right);
			break;
		case 0:
			ResetPositionsAt(savedPositions.p1Custom, savedPositions.p2Custom);
			break;
		default:
			break;
		}
		
		// Should regen meter as well
	}

	if (GetAsyncKeyState(0x32) & 1) //key "2"
	{
		savedPositions.p1Custom.x = MeltyLib::character1.CSO.xPos;
		savedPositions.p1Custom.y = MeltyLib::character1.CSO.yPos;
		savedPositions.p2Custom.x = MeltyLib::character2.CSO.xPos;
		savedPositions.p2Custom.y = MeltyLib::character2.CSO.yPos;
		puts("Custom positions saved");
	}


	/*
	if (GetAsyncKeyState(VK_F6) & 1)
	{
		GS.reversalWakeup = !GS.reversalWakeup;
		if (GS.reversalWakeup)
			puts("C-Arc 22B");
		else
			puts("Nothing");
	}
	*/
	
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

	oldUpdate = (int(*)(int)) HookFunction(MeltyLib::ADDR_UPDATEGAME_CALL, (DWORD)NewUpdate);
	oldBattleSceneUpdate = (void(__fastcall*)(int)) HookFunction(0x4235d1, (DWORD)NewBattleSceneUpdate); //MeltyLib::BATTLESCENE_UPDATE
	oldReset = (void(__stdcall*)(int*)) HookFunction(0x42357D, (DWORD)NewReset); //0x42357D 0x433911

	//oldComputeGuardGauge = (void(__fastcall*)(void)) HookFunction(0x461948, (DWORD)NewComputeGuardGauge);
	
	//0x461928 MeltyLib::EXGUARDFLAG_COMPUTE
	//oldResetCharacter = (void(__fastcall*)(MeltyLib::CharacterObject*, int trash, byte, int, char)) HookFunction(0x426838, (DWORD)NewResetCharacter);
	//0x423460 MeltyLib::BATTLESCENE_INIT
	//0x4265EC MeltyLib::BATTLESCENE_SUBMESSAGE_DISPLAY_RESET
	return 0;
}