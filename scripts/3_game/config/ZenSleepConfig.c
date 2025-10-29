class ZenSleepConfig
{
	// Config location
	const static string ZEN_MOD_CFG_PROFILES	= "$profile:\\Zenarchist\\";
	const static string ZEN_MOD_CFG_FILE		= "ZenSleepConfig.json";
	
	// Version
	static const string CONFIG_VERSION = "Updated 27th July 2025";
	string ConfigVersion;

	// Non-serialized admin status for clients 
	static bool IS_CLIENT_ADMIN = false;

	// Main config data
	ref ZenSleepGeneralConfig GeneralConfig;
	ref ZenSleepGainConfig GainConfig;
	ref ZenSleepDrainConfig DrainConfig;
	ref ZenSleepModifiers ModifiersConfig;
	ref ZenSleepEffectsServerConfig ServerEffectsConfig;
	ref ZenSleepEffectsClientConfig ClientEffectsConfig;
	ref ZenSleepMessages Messages;
	ref array<string> AdminIDs;
	bool DebugMode; // Turn this on when tweaking & testing important values

	void Load()
	{
		// Load JSON on server only
		if (!GetGame().IsDedicatedServer())
			return;

		SetDefaultValues();

		if (FileExist(ZEN_MOD_CFG_PROFILES + ZEN_MOD_CFG_FILE))
		{
			JsonFileLoader<ZenSleepConfig>.JsonLoadFile(ZEN_MOD_CFG_PROFILES + ZEN_MOD_CFG_FILE, this);

			// If version mismatch, backup old version of json before replacing it
			if (ConfigVersion != CONFIG_VERSION)
			{
				JsonFileLoader<ZenSleepConfig>.JsonSaveFile(ZEN_MOD_CFG_PROFILES + ZEN_MOD_CFG_FILE + "_old", this);
			}
			else
			{
				// Config exists and version matches, stop here. Re-save to add/remove any config changes to structure.
				FixUpdatedVars();
				Save();
				return;
			}
		}

		Save();
	}

	void SetDefaultValues()
	{
		ConfigVersion = CONFIG_VERSION;

		GeneralConfig			= new ZenSleepGeneralConfig();
		DrainConfig				= new ZenSleepDrainConfig();
		GainConfig				= new ZenSleepGainConfig();
		ModifiersConfig			= new ZenSleepModifiers();
		ServerEffectsConfig		= new ZenSleepEffectsServerConfig();
		ClientEffectsConfig		= new ZenSleepEffectsClientConfig();
		Messages				= new ZenSleepMessages();
		DebugMode				= false;
		AdminIDs				= new array<string>;
		AdminIDs.Insert("www.steamidfinder.com");
	}

	void FixUpdatedVars()
	{
		// used to patch any broken config after an update which adds new config
		// (eg. loading an existing json after adding new map<x,x>'s to this class will not init the map properly on next load)
		if (!ModifiersConfig.StaminaRecoveryModifiers || ModifiersConfig.StaminaRecoveryModifiers.Count() == 0)
		{
			ModifiersConfig.SetDefaultStaminaModifiers();
		}
	}

	void Reload()
	{
		delete m_ZenSleepConfig;
		GetZenSleepConfig();
	}

	void Save()
	{
		if (!FileExist("$profile:\\Zenarchist"))
			MakeDirectory("$profile:\\Zenarchist");

		if (!FileExist(ZEN_MOD_CFG_PROFILES))
			MakeDirectory(ZEN_MOD_CFG_PROFILES);

		JsonFileLoader<ZenSleepConfig>.JsonSaveFile(ZEN_MOD_CFG_PROFILES + ZEN_MOD_CFG_FILE, this);
	}

	// For server -> client sync.
	void SetClientEffectsConfig(ZenSleepEffectsClientConfig config)
	{
		ClientEffectsConfig = config;
	}

	bool IsAdmin(string steamID)
	{
		if (GetGame().IsDedicatedServer())
		{
			if (AdminIDs != NULL && AdminIDs.Find(steamID) != -1)
			{
				return true;
			}
		}

		if (GetGame().IsClient())
			return IS_CLIENT_ADMIN;

		return false;
	}
}

class ZenSleepGeneralConfig
{
	float SecondsAsleepUntilFullRestApprox = ZenSleepConstants.FATIGUE_BASE_SECONDS_TIL_RESTED; // How long it takes from zero energy to full rest (WITHOUT any of the below gain multipliers applied)
	float FreshSpawnFatiguePercent = 70; // % of fatigue a fresh spawn begins with (100% = full rested)
	float MaxFatiguePercentToAllowSleeping = 70; // Maximum % of fatigue to allow sleep (70%+ = full white, 16%+ = white with gap, 6%+ = yellow, 0.02%+ = red, < 0.02% = red flashing)
	int FatigueLevelToNegativeImpactImmunity = 2; // 0 = full white, 1 = white with a gap, 2 = yellow, 3 = red, 4 = red flashing
	bool DisplaySleepMessages = true; // Turn on/off the text messages which get sent to players.
}

class ZenSleepGainConfig
{
	float GlobalGainMultiplier = 1.0; // Speeds up/slows down all fatigue gainers
	float GainNightMultiplier = 1.5; // These multipliers DO NOT stack (ie. at night AND inside = 2x gain, not 2.25x gain. The 1 is subtracted. Max multiplier if all conds are met is 2.5x)
	float GainBedObjectUsed = 1.3; // 30% sleep speed boost
	float GainInsideMultiplier = 1.1; // 10% sleep speed boost
	float GainFromHeatBuffMultiplier = 1.1; // There are 3 stages of heat buffer, 1 = 33% of this value, 2 = 50% of this value, 3 = 100% of this value.
	float MaxRestGain_Illness = 16; // 16% (yellow) max rest
	float MaxRestGain_Temp_DarkBlueFlashing = 0; // Don't allow sleep
	float MaxRestGain_Temp_DarkBlue = 0; // Don't allow sleep
	float MaxRestGain_Temp_LightBlue = 70; // 70% (white with gap)
	float MaxRestGain_Temp_Yellow = 70; // 70% (white with gap) max rest
	float MaxRestGain_Temp_Red = 0; // Don't allow sleep
	float MaxRestGain_Temp_RedFlashing = 0; // Don't allow sleep
	float MaxRestGain_FoodWater_Yellow = 16; // 16% (yellow) Max rest possible with yellow food or water
	float MaxRestGain_FoodWater_Red = 6; // 6% (red) Max rest possible with red food or water
	float MaxRestGain_FoodWater_RedFlashing = 0; // 0% Max rest possible with red flashing food or water
	float MaxRestGain_Wet = 16; // 16% (yellow) Max rest possible when wet
	float MaxRestGain_Time_Daylight = 70; // 70% (white with gap) Max rest possible during daytime
	float MaxRestGain_Time_Night = 100; // 100% (full white) Max rest possible at night time
	float PercentGainedUponUncon = 1; // 1% (approx) Fatigue stat regained if player falls uncon while red or red flashing
	float BedHeatBuffMaxGain = 10; // 10 = heatbuff stage 1, 30 = stage 3.
	float BedHealthMaxGain = 100; // Maximum recoverable health when sleeping in a bed/sleeping bag (set to zero to disable)
	float BedHealthGainPerSec = 0.25; // Health gain per second while sleeping in bed
	float MinSecsAsleepInBedToTriggerImmunityBoost = 120; // Minimum seconds player needs to sleep to receive 5 minute immunity boost (set to zero to disable)
	float MinSecsAsleepToTriggerHealthRecovery = 120; // Minimum seconds player needs to sleep to begin regaining health (if enabled)
}

class ZenSleepDrainConfig
{
	float GlobalDrainMultiplier = 1.0; // Speeds up/slows down all fatigue drainers
	float DrainHeavyItemMultiplier = 1.0; // How much faster/slower to drain while carrying heavy item (eg. barrel)
	float DrainWalkingMultiplier = 1.0; // Walking speed drain multiplier
	float DrainJoggingMultiplier = 1.0; // Jogging speed drain (ie. normal W with no CTRL or shift pressed)
	float DrainSprintingMultiplier = 1.0; // Full-sprint drain
	float DrainWeightMultiplier = 1.0; // How much player weight affects drain (as a % of the stamina bar - the lower the stamina bar, the higher this multiplier is applied)
	float DrainNightMultiplier = 1.5; // How much faster fatigue drains when the world is night time
	float HealthLossPerSecRedFlashing = 0; // No health damage for zero fatigue left by default.
}

class ZenSleepEffectsServerConfig
{
	float MinimumSecsBetweenYawnRedFlashing = 30; // Min seconds between triggering a yawn when red flashing fatigue
	float MaximumSecsBetweenYawnRedFlashing = 300; // Max seconds between triggering a yawn when red flashing fatigue
	float MinimumSecsBetweenYawnRed = 180; // Min seconds between triggering a yawn when red fatigue
	float MaximumSecsBetweenYawnRed = 300; // Max seconds between triggering a yawn when red fatigue
	float MinimumSecsBetweenSnore = 10; // Minimum seconds between triggering a snore sleep sound
	float MaximumSecsBetweenSnore = 20; // Maximum seconds between triggering a snore sleep sound
	float MinimumSecsBetweenUnconRedFlashing = 180; // Minimum seconds between possibly falling uncon when red flashing fatigue (affected by chance below)
	float MaximumSecsBetweenUnconRedFlashing = 360; // Maximum seconds between possibly falling uncon when red flashing fatigue (affected by chance below)
	float MinimumSecsBetweenUnconRed = 600; // Minimum seconds between possibly falling uncon when red fatigue (affected by chance below)
	float MaximumSecsBetweenUnconRed = 900; // Maximum seconds between possibly falling uncon when red fatigue (affected by chance below)
	float MaximumSecsBuzzedByCombat = 0; // Maximum seconds damage reduces your ability to sleep (ie. 90% total health damage = 90% of this value, does not pause fatigue drain and resets on logout)
	float ChanceToSnoreSleeping = 90; // 90% chance to play snore sound when sleeping
	float ChanceToYawnRedFlashing = 90; // 90% chance to yawn when red flashing fatigue
	float ChanceToYawnRed = 25; // 25% chance to yawn when red fatigue
	float ChanceToHallucinateRedFlashing = 100; // 100% chance to trigger hallucination visual effect when yawning while red flashing fatigue
	float ChanceToHallucinateRed = 75; // 75% chance to trigger hallucination visual effect when yawning while red fatigue
	float ChanceToUnconRedFlashing = 50; // 50% to fall uncon chance every ~3-6 minutes (based on above settings)
	float ChanceToUnconRed = 1; // 1% chance every ~10-15 minutes (based on above settings)
	bool CanUnconWhileDriving = true; // Turn on/off the ability to fall asleep while driving (based on above settings)
	bool CanUnconWhileSwimming = false; // Turn on/off the ability to fall asleep while swimming
}

// These variables are sync'ed to the client and shared by the server
class ZenSleepEffectsClientConfig
{
	float AmountToBlurWhenRedFlashing = 2; // Amount to blur the screen when red flashing fatigue (2% = very subtle)
	float AmountToBlurWhenRed = 1; // Amount to blur the screen when red flashing fatigue (1% = extremely subtle)
	float AmountToVignetteWhenRedFlashing = 50; // Amount to apply blackened corners of screen when red flashing fatigue (takes ~5 minutes at red flashing to reach this value)
	float AmountToVignetteWhenRed = 25; // Amount to apply blackened corners of screen when red fatigue (takes ~5 minutes at red to reach this value)
	float DelaySecsUntilAudioAttenuation = 60; // How long to wait after lying down to sleep before engaging sleep attenuation (set to zero to disable)
	bool AllowInventoryWhileSleeping = true; // Allow player to access their inventory while sleeping (can't drop or use any items in hands)
	bool AllowClosedEyesEffectSleeping = true; // Enables the "closing eyes" effect
	bool AllowHallucinationEffectRedFlashing = true; // Enables the chroma hallucination effect when yawning while red flashing
	bool AllowHallucinationEffectRed = true; // Enables the chroma hallucination effect when yawning while red fatigue
	int CharacterMovementSpeedRedFlashing = 1; // Applies a character movement limitation when red flashing fatigue (0 = no effect, 1 = eInjuryHandlerLevels.WORN, 2 = DAMAGED, 3 = RUINED)
	int CharacterMovementSpeedRed = 0; // Applies movement speed limitation when red fatigue (if above zero)
	ref array<string> BedObjects; // List of bed objects the player can sleep on and receive benefits (heat buff/health gain/increased rest speed etc)

	void ZenSleepEffectsClientConfig()
	{
		BedObjects = new array<string>;
		BedObjects.Insert("MediumTent");
		BedObjects.Insert("LargeTent");
		BedObjects.Insert("ZenSleepingBagDeployed_Base");
		BedObjects.Insert("ZenSleepingBagStatic_Base");
	}
}

//! Be aware that modifiers here will NOT reactivate on reconnection. So if it's an important modifier effect, only apply it to the deactivation config.
//  Same with the fatigue pause effect - to avoid requiring servers to wipe to add/remove this mod I don't save any of these values to PlayerBase on disconnect.
class ZenSleepModifiers
{
	ref map<string, float> ModifierDeactivatedPercent;
	ref map<string, float> ModifierActivatedPercent;
	ref map<string, float> ModifierPausesSeconds;
	ref map<string, float> StaminaRecoveryModifiers;

	void ZenSleepModifiers()
	{
		ModifierDeactivatedPercent = new map<string, float>;
		ModifierDeactivatedPercent.Set("EpinephrineMdfr", -1); // 1% drain on energy
		ModifierDeactivatedPercent.Set("MorphineMdfr", -1); // 1% drain on energy
		ModifierDeactivatedPercent.Set("PainKillersMdfr", -0.5); // 0.5% drain on energy
		ModifierDeactivatedPercent.Set("ContaminationStage3Mdfr", -100); // 100% drain on energy
		ModifierDeactivatedPercent.Set("ZenSleep_Anesthetic", -100); // 100% drain on energy
		
		ModifierActivatedPercent = new map<string, float>;
		ModifierActivatedPercent.Set("SodaCan_ColorBase", 1); // 1% gain on energy
		ModifierActivatedPercent.Set("ZenSleep_Amphetamine", 100); // 100% gain on energy

		ModifierPausesSeconds = new map<string, float>;
		ModifierPausesSeconds.Set("EpinephrineMdfr", 300); // Epi pauses sleep drain for 5 minutes
		ModifierPausesSeconds.Set("ZenSleep_Amphetamine", 900); // Speed pauses sleep drain for 15 minutes
	}

	void SetDefaultStaminaModifiers()
	{
		StaminaRecoveryModifiers = new map<string, float>;
		StaminaRecoveryModifiers.Set("Yellow", 0.8);
		StaminaRecoveryModifiers.Set("Red", 0.6);
		StaminaRecoveryModifiers.Set("RedFlashing", 0.4);
	}
}

// Server-side message config 
class ZenSleepMessages
{
	string CantSleep = "#STR_ZenSleepMsg_CantSleep,";
	string TooBuzzed = "#STR_ZenSleepMsg_TooBuzzed.";
	string TooAwake = "#STR_ZenSleepMsg_TooAwake.";
	string TooSunny = "#STR_ZenSleepMsg_TooSunny.";
	string TooSick = "#STR_ZenSleepMsg_TooSick.";
	string TooWet = "#STR_ZenSleepMsg_TooWet.";
	string TooCold = "#STR_ZenSleepMsg_TooCold.";
	string TooHot = "#STR_ZenSleepMsg_TooHot.";
	string TooUncomfortable = "#STR_ZenSleepMsg_TooUncomfortable.";
	string TooHungryThirsty = "#STR_ZenSleepMsg_TooHungryThirsty.";
}

ref ZenSleepConfig m_ZenSleepConfig;

static ZenSleepConfig GetZenSleepConfig()
{
	if (!m_ZenSleepConfig)
	{
		m_ZenSleepConfig = new ZenSleepConfig;
		m_ZenSleepConfig.Load();
	}

	return m_ZenSleepConfig;
}