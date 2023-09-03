#pragma once
#include "ConfigParser.h"

extern ModConfigs iniData;
extern RE::PlayerCharacter* playerRef;
extern RE::ActorValueInfo* avActionPoints;

namespace HookLineAndSinker
{
	float HookedCalcSprintingActionPoints(void*, float, float, float);

	void RegisterHooks(F4SE::Trampoline& trampoline);
}

class DrainCalculator
{
public:
	float Update()
	{
		UpdateValues();
		WriteToLog();
		return CalcActionPointDrain();
	}

public:
	// weight variables
	float adjustedEquippedWeight;
	float unequippedWeight;
	float adjustedWeight;
	float excessWeight;
	float penaltyWeight;
	// AP variables
	float maxAP;
	float currentAP;
	float apUsedPercent;
	float apDrainScalar;
	// health variables
	float healthPercent;
	float healthScalar;

private:
	// updates the class members
	inline void UpdateValues()
	{
		// some of these could be checked less frequently; but that's future me's problem
		// e.g. the weight ones could be probably updated via events, but I doubt it has a
		// significant enough overhead to warrant all that extra effort
		adjustedEquippedWeight = playerRef->equippedWeight * iniData.fEquippedWeightMult;
		unequippedWeight = playerRef->GetWeightInContainer() - playerRef->equippedWeight;
		adjustedWeight = adjustedEquippedWeight + unequippedWeight;
		// penaltyWeight
		if (iniData.fPenaltyCarryWeight <= 1.0f && iniData.fPenaltyCarryWeight >= 0.0f) {
			penaltyWeight = playerRef->GetActorValue(*RE::ActorValue::GetSingleton()->carryWeight) * iniData.fPenaltyCarryWeight;
		} else if (iniData.fPenaltyCarryWeight < 0.0f) {
			penaltyWeight = 69420.69f;
		}
		excessWeight = adjustedWeight - penaltyWeight;

		maxAP = playerRef->GetBaseActorValue(*avActionPoints);
		currentAP = playerRef->GetActorValue(*avActionPoints);
		// Action Point drain scalar
		if (iniData.fAPDrainScalar > 0.0f) {
			apUsedPercent = 1 - (currentAP / maxAP);
			apDrainScalar = (1 + ((iniData.fAPDrainScalar - 1) * apUsedPercent));
		} else {
			apDrainScalar = 0.0f;
		}

		healthPercent = playerRef->GetHealthPercent();
		if (healthPercent == -1.0f) {
			healthPercent = 1.0f;
		}

		// at max health scalar is 1
		// as it gets lower, the value moves from 1 to whatever is set on fHealthScalar
		if (iniData.bEnableHealthScalarCombatOverride) {
			// mmm fancy ternary operators
			healthScalar = 1.0f + ((1.0f - healthPercent) * ((playerRef->IsInCombat() ? iniData.fHealthScalarCombatOverride : iniData.fHealthScalar) - 1.0f));
		} else {
			healthScalar = 1.0f + ((1.0f - healthPercent) * (iniData.fHealthScalar - 1.0f));
		}
	}

	inline float CalcActionPointDrain()
	{
		float result = apDrainScalar;

		// check if there's any excess weight that we should penalise with extra AP drain
		if (excessWeight > 0.0f) {
			// calculate how far away this is from max carry weight as %
			result *= 1 + (iniData.fMaxCarryWeightAPDrainMult * (excessWeight / playerRef->GetActorValue(*RE::ActorValue::GetSingleton()->carryWeight)));
		}

		result *= healthScalar;

		// apply the holstered weapon AP drain mult
		if (playerRef->GetWeaponMagicDrawn() == false) {
			result *= iniData.fHolsteredWeaponMult;
		}

		return result;
	}

	inline void WriteToLog()
	{
		// TODO: make a better logger
		if (iniData.bEnableLogging) {
			logger::warn("adjustedEquippedWeight = {}", adjustedEquippedWeight);
			logger::warn("unequippedWeight = {}", unequippedWeight);
			logger::warn("adjustedWeight = {}", adjustedWeight);
			logger::warn("excessWeight = {}", excessWeight);
			logger::warn("penaltyWeight = {}", penaltyWeight);
			logger::warn("maxAP = {}", maxAP);
			logger::warn("currentAP = {}", currentAP);
			logger::warn("apUsedPercent = {}", apUsedPercent);
			logger::warn("apDrainScalar = {}", apDrainScalar);
			logger::warn("healthPercent = {}", healthPercent);
			logger::warn("healthScalar = {}", healthScalar);
		}
	}
};
