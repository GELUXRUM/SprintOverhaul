#include "ConfigParser.h"
#include "Hooks.h"
#include "RNJesus.h"

// .ini stuff
CSimpleIniA ini(true, false, false);
ModConfigs iniData;

// globals
//std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
RE::PlayerCharacter* playerRef;
RE::ActorValueInfo* avActionPoints;

void InitPlugin()
{
	// load configs
	iniData.LoadConfigs(ini);

	// display configs and set up necessary hooks/variables if plugin is enabled
	if (iniData.bEnablePlugin) {
		logger::warn("------------------------------------------");
		logger::warn("Loaded configs:");
		logger::warn("bEnableLogging = {}", iniData.bEnableLogging);
		logger::warn("fPenaltyCarryWeight = {}", iniData.fPenaltyCarryWeight);
		logger::warn("fMaxCarryWeightAPDrainMult = {}", iniData.fMaxCarryWeightAPDrainMult);
		logger::warn("fAPDrainScalar = {}", iniData.fAPDrainScalar);
		logger::warn("fEquippedWeightMult = {}", iniData.fEquippedWeightMult);
		logger::warn("fHolsteredWeaponMult = {}", iniData.fHolsteredWeaponMult);
		logger::warn("fHealthScalar = {}", iniData.fHealthScalar);
		logger::warn("bEnableHealthScalarCombatOverride = {}", iniData.bEnableHealthScalarCombatOverride);
		logger::warn("fHealthScalarCombatOverride = {}", iniData.fHealthScalarCombatOverride);
		logger::warn("fOutOfCombatAPDrainMult = {}", iniData.fOutOfCombatAPDrainMult);
		logger::warn("fCombatAPDrainMult = {}", iniData.fCombatAPDrainMult);
		logger::warn("------------------------------------------");

		// install hooks
		auto& trampoline = F4SE::GetTrampoline();
		trampoline.create(14);
		HookLineAndSinker::RegisterHooks(trampoline);

		// get pointers to stuff
		playerRef = RE::PlayerCharacter::GetSingleton();
		avActionPoints = (RE::ActorValueInfo*)RE::TESForm::GetFormByID(0x000002D5);
	} else {
		logger::warn("Plugin is disabled via .ini setting");
	}
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "GLXRM_SprintOverhaul.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("(%#)[%^%l%$] %v"s);

	logger::info("{} v{}", Version::PROJECT, Version::NAME);

	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = "SprintOverhaul";
	a_info->version = 1;

	if (a_f4se->IsEditor()) {
		logger::critical("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < F4SE::RUNTIME_1_10_162) {
		logger::critical("unsupported runtime v{}", ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);

	const F4SE::MessagingInterface* messageInterface = F4SE::GetMessagingInterface();
	messageInterface->RegisterListener([](F4SE::MessagingInterface::Message* msg) -> void {
		if (msg->type == F4SE::MessagingInterface::kGameDataReady) {
			InitPlugin();
		}
	});

	return true;
}
