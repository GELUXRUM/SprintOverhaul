#pragma once

#include "SimpleIni.h"

class ModConfigs
{
public:
	ModConfigs()
	{
		bEnablePlugin					  = false;
		bEnableLogging					  = false;
		fPenaltyCarryWeight				  = 0.5f;
		fMaxCarryWeightAPDrainMult		  = 1.8f;
		fAPDrainScalar					  = 2.0f;
		fEquippedWeightMult				  = 0.8f;
		fHolsteredWeaponMult			  = 0.9f;
		fHealthScalar					  = 1.4f;
		bEnableHealthScalarCombatOverride = true;
		fHealthScalarCombatOverride		  = 1.15f;
		fOutOfCombatAPDrainMult			  = 1.0f;
		fCombatAPDrainMult				  = 1.0f;
	}

	void LoadConfigs(CSimpleIniA &a_ini)
	{
		a_ini.LoadFile("Data\\F4SE\\Plugins\\GLXRM_SprintOverhaul.ini");

		bEnablePlugin					  = BoolValue(a_ini.GetValue("Configs", "bEnablePlugin", "false"));
		bEnableLogging					  = BoolValue(a_ini.GetValue("Configs", "bEnableLogging", "false"));
		fPenaltyCarryWeight				  = std::stof(a_ini.GetValue("Configs", "fPenaltyCarryWeight", "0.6"));
		fMaxCarryWeightAPDrainMult		  = std::stof(a_ini.GetValue("Configs", "fMaxCarryWeightAPDrainMult", "1.5"));
		fAPDrainScalar					  = std::stof(a_ini.GetValue("Configs", "fAPDrainScalar", "1.2"));
		fEquippedWeightMult				  = std::stof(a_ini.GetValue("Configs", "fEquippedWeightMult", "0.8"));
		fHolsteredWeaponMult			  = std::stof(a_ini.GetValue("Configs", "fHolsteredWeaponMult", "0.9"));
		fHealthScalar					  = std::stof(a_ini.GetValue("Configs", "fHealthScalar", "1.4"));
		bEnableHealthScalarCombatOverride = BoolValue(a_ini.GetValue("Configs", "bEnableHealthScalarCombatOverride", "true"));
		fHealthScalarCombatOverride		  = std::stof(a_ini.GetValue("Configs", "fHealthScalarCombatOverride", "0.85"));
		fOutOfCombatAPDrainMult			  = std::stof(a_ini.GetValue("Configs", "fOutOfCombatAPDrainMult", "1.0"));
		fCombatAPDrainMult				  = std::stof(a_ini.GetValue("Configs", "fCombatAPDrainMult", "1.0"));

		a_ini.Reset();
	}

public:
	// General
	bool  bEnablePlugin;
	bool  bEnableLogging;
	float fPenaltyCarryWeight;
	float fMaxCarryWeightAPDrainMult;
	float fAPDrainScalar;
	float fEquippedWeightMult;
	float fHolsteredWeaponMult;
	float fHealthScalar;
	bool  bEnableHealthScalarCombatOverride;
	float fHealthScalarCombatOverride;
	// Global Multipliers
	float fOutOfCombatAPDrainMult;
	float fCombatAPDrainMult;

private:
	bool BoolValue(const char* a_optionValue)
	{
		std::string lowercaseStr;

		size_t length = std::strlen(a_optionValue);

		for (size_t i = 0; i < length; ++i) {
			lowercaseStr += static_cast<char>(std::tolower(static_cast<unsigned char>(a_optionValue[i])));
		}

		if (lowercaseStr == "true") {
			return true;
		} else {
			return false;
		}
	}
};
