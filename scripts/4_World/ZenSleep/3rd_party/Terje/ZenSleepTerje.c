#ifdef TERJE_MEDICINE_MOD
// Terje upstream 08208ffe75783207281dbc40ba4f4d6970cb8cb3.
// Zen owns the sleep loop. Reuse Terje's live settings, records, perk accessor and
// health API; no copied settings file, duplicate timers or independent rest pool.
class ZenSleepTerje
{
	static float GetFastSleepMultiplier(PlayerBase player)
	{
		float perk;
		if (player.GetTerjeSkills() && player.GetTerjeSkills().GetPerkValue("immunity", "fsleep", perk))
			return Math.Max(0, 1.0 + perk);

		return 1.0;
	}

	static bool HasStimulant(PlayerBase player)
	{
		float value, time;
		return player.GetTerjeStats() && player.GetTerjeStats().GetSleepingIncrement(value, time) && value > 0 && time > 0;
	}

	static int GetSleepBlock(PlayerBase player)
	{
		if (!player.GetTerjeStats())
			return TerjeMedicineSleepingLevel.TERJESL_NONE;

		if (GetTerjeSettingBool(TerjeSettingsCollection.MEDICINE_SLEEPING_BLOCK_WITH_WOUNDS) && player.HasTerjeSicknesOrInjures())
			return TerjeMedicineSleepingLevel.TERJESL_SICK;

		if (HasStimulant(player))
			return TerjeMedicineSleepingLevel.TERJESL_ENERGED;

		// Use the raw state to avoid recursion through IsSleeping. Zen's start
		// limit must not become a recovery cap after the player falls asleep.
		bool alreadySleeping = player.GetZenSleepManager().GetSleepCondition() == ZenSleepState.ASLEEP && player.GetZenSleepManager().IsInSleepPosition();
		if (!alreadySleeping && player.GetZenSleepManager().GetFatiguePercent() >= GetZenSleepConfig().GeneralSleepConfig.MaxFatiguePercentToAllowSleeping)
			return TerjeMedicineSleepingLevel.TERJESL_ENERGED;

		return TerjeMedicineSleepingLevel.TERJESL_NONE;
	}

	static void TickMedicine(PlayerBase player, float deltaTime)
	{
		TerjePlayerStats stats = player.GetTerjeStats();
		float value, time;
		float change = 0;
		if (stats.GetSleepingIncrement(value, time))
		{
			// Consume the remaining partial second as well; a one-second injection
			// must not disappear when the server's modifier tick is >= one second.
			change += Math.Max(0, value) * Math.Clamp(time, 0, deltaTime);
			time = Math.Max(0, time - deltaTime);
			if (time == 0)
				value = 0;
			stats.SetSleepingIncrement(value, time);
		}
		if (stats.GetSleepingDecrement(value, time))
		{
			change -= Math.Max(0, value) * Math.Clamp(time, 0, deltaTime);
			time = Math.Max(0, time - deltaTime);
			if (time == 0)
				value = 0;
			stats.SetSleepingDecrement(value, time);
		}
		stats.SetSleepingValue(stats.GetSleepingValue() + change);
	}

	static void UpdateState(PlayerBase player, float previousRest)
	{
		TerjePlayerStats stats = player.GetTerjeStats();
		float rest = stats.GetSleepingValue();
		int level = 1;
		if (rest < TerjeMedicineConstants.SLEEPING_LEVEL5) level = 5;
		else if (rest < TerjeMedicineConstants.SLEEPING_LEVEL4) level = 4;
		else if (rest < TerjeMedicineConstants.SLEEPING_LEVEL3) level = 3;
		else if (rest < TerjeMedicineConstants.SLEEPING_LEVEL2) level = 2;

		float difference = rest - previousRest;
		int tendency = 0;
		if (difference < 0)
		{
			if (difference > TerjeMedicineConstants.SLEEPING_TENDENCY_MINUS_STAGE1) tendency = -1;
			else if (difference > TerjeMedicineConstants.SLEEPING_TENDENCY_MINUS_STAGE2) tendency = -2;
			else tendency = -3;
		}
		else if (difference > 0)
		{
			if (difference < TerjeMedicineConstants.SLEEPING_TENDENCY_PLUS_STAGE1) tendency = 1;
			else if (difference < TerjeMedicineConstants.SLEEPING_TENDENCY_PLUS_STAGE2) tendency = 2;
			else tendency = 3;
		}
		stats.SetSleepingLevelAndTendency(level, tendency);

		int state = GetSleepBlock(player);
		if (player.IsUnconscious())
			state = TerjeMedicineSleepingLevel.TERJESL_NONE;
		else if (state == TerjeMedicineSleepingLevel.TERJESL_NONE && player.GetZenSleepManager().IsSleeping())
		{
			state = TerjeMedicineSleepingLevel.TERJESL_COMFORT;
			if (player.GetHeatBufferStage() > 0)
				state = TerjeMedicineSleepingLevel.TERJESL_PERFECT;
		}
		stats.SetSleepingState(state);
	}

	static void TickMedicalBenefits(PlayerBase player, float deltaTime)
	{
		if (!player.GetZenSleepManager().IsSleeping() || player.IsUnconscious() || GetSleepBlock(player) != TerjeMedicineSleepingLevel.TERJESL_NONE)
			return;

		// Non-bed healing has no Zen equivalent. In a dry bed, Zen's rate/cap/delay win.
		float healthGain = Math.Max(0, GetTerjeSettingFloat(TerjeSettingsCollection.MEDICINE_SLEEPING_HEALTH_INC)) * deltaTime;
		if (player.GetZenSleepManager().HasBed(GameConstants.STATE_DAMP))
		{
			healthGain = Math.Max(0, GetZenSleepConfig().GainConfig.BedHealthGainPerSec * ZenFatigueMdfr.GetZenSleepGainModifier(player) * GetZenSleepConfig().GainConfig.GlobalGainMultiplier) * deltaTime;
			if (player.GetZenSleepManager().GetSecondsAsleep() < GetZenSleepConfig().GainConfig.MinSecsAsleepToTriggerHealthRecovery)
				healthGain = 0;
			else
			{
				healthGain = Math.Min(healthGain, GetZenSleepConfig().GainConfig.BedHealthMaxGain - player.GetTerjeHealth().GetHealth());
				healthGain = Math.Max(0, healthGain);
			}
		}
		player.GetTerjeHealth().AddHealth(healthGain);
		player.GetTerjeHealth().AddBlood(Math.Max(0, GetTerjeSettingFloat(TerjeSettingsCollection.MEDICINE_SLEEPING_BLOOD_INC)) * deltaTime);
		TerjePlayerStats stats = player.GetTerjeStats();
		stats.SetMindValue(stats.GetMindValue() + Math.Max(0, GetTerjeSettingFloat(TerjeSettingsCollection.MEDICINE_SLEEPING_MIND_INC)) * deltaTime);
	}

	static void TickExhaustion(PlayerBase player, float deltaTime)
	{
		if (!player.GetAllowDamage())
			return;

		// Zen's configurable chance/timing controls sleep-induced unconsciousness.
		// Apply its health-loss rate once, through Terje's health API.
		if (!player.GetZenSleepManager().IsSleeping() && !player.GetZenSleepManager().IsFatigueDrainPaused() && player.GetStatZenFatigue().Get() <= ZenSleepConstants.SL_FATIGUE_CRITICAL)
		{
			float healthLoss = Math.Max(0, GetZenSleepConfig().DrainConfig.HealthLossPerSecRedFlashing);
			player.GetTerjeHealth().DecreaseHealth(healthLoss * deltaTime, TerjeDamageSource.SLEEPING);
		}

		float rest = player.GetTerjeStats().GetSleepingValue();
		if (rest < TerjeMedicineConstants.SLEEPING_CRITICAL)
		{
			player.GetTerjeHealth().DecreaseBlood(Math.Max(0, GetTerjeSettingFloat(TerjeSettingsCollection.MEDICINE_SLEEPING_BLOOD_DEC)) * deltaTime, TerjeDamageSource.SLEEPING);
			TerjePlayerStats stats = player.GetTerjeStats();
			if (player.IsAlive() && stats)
				stats.SetMindValue(stats.GetMindValue() - Math.Max(0, GetTerjeSettingFloat(TerjeSettingsCollection.MEDICINE_SLEEPING_MIND_DEC)) * deltaTime);
		}
	}
}

modded class TerjePlayerModifierSleeping
{
	override void OnServerFixedTick(PlayerBase player, float deltaTime)
	{
		// Never run both rest loops. Other Terje modifiers remain untouched.
		if (!ZenSleepCompatibility.OverridesTerje())
			super.OnServerFixedTick(player, deltaTime);
	}
}

modded class EmoteManager
{
	override bool IsPlayerSleeping()
	{
		if (ZenSleepCompatibility.OverridesTerje())
			return m_Player.GetZenSleepManager() && m_Player.GetZenSleepManager().IsSleeping();

		return super.IsPlayerSleeping();
	}
}
#endif
