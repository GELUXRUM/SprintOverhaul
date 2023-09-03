#include "Hooks.h"

namespace HookLineAndSinker
{
	typedef float(CalcSprintingActionPointsSig)(void*, float, float, float);
	REL::Relocation<CalcSprintingActionPointsSig> OriginalFunction;

	float HookedCalcSprintingActionPoints(void* a_gameplayFormulas, float a_a2, float a_a3, float a_a4)
	{
		// ooo static variables! so fancy!
		static DrainCalculator valueBank;

		// base AP drain value
		float result = OriginalFunction(a_gameplayFormulas, a_a2, a_a3, a_a4);

		// apply combat-based multipliers
		if (playerRef->IsInCombat()) {
			result *= iniData.fOutOfCombatAPDrainMult;
		} else {
			result *= iniData.fCombatAPDrainMult;
		}

		if (result == 0.0f) {
			return result;
		}

		// update the class with new data and calculate the new AP drain mult
		result *= valueBank.Update();

		return result;
	}

	void RegisterHooks(F4SE::Trampoline& trampoline) {
		REL::Relocation<CalcSprintingActionPointsSig> callLocation{ REL::ID(385539), 0x1C5 };
		OriginalFunction = trampoline.write_call<5>(callLocation.address(), &HookedCalcSprintingActionPoints);
	}
}
