class ZenFatigueMdfr extends ModifierBase
{
	protected static float m_ZenSleepDrainSpeed;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= ZenSleepEnums.MDF_FATIGUE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		#ifdef SERVER
		m_ZenSleepDrainSpeed	= ZenSleepConstants.FATIGUE_DRAIN_BASE_RATE * GetZenSleepConfig().DrainConfig.GlobalDrainMultiplier;
		#else // Should only apply in offline mode, which I never code/test in because I'm retarded and can never get it working: 
		m_ZenSleepDrainSpeed	= ZenSleepConstants.FATIGUE_DRAIN_BASE_RATE;
		#endif
		DisableDeactivateCheck();
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		#ifdef ENFUSION_AI_PROJECT
		// Robots don't sleep
		if (player.IsAI())
			return false;
		#endif

		return true;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player.GetZenSleepManager().IsFatigueDrainPaused())
		{
			return;
		}

		float fatigueBuffDebuff = 1.0;
		float fatigueCurrent = player.GetStatZenFatigue().Get();
		float fatigueSpeed;

		// Sleeping (gain)
		if (player.GetZenSleepManager().IsSleeping())
		{
			float timeAsleepMulti = player.GetZenSleepManager().GetSecondsAsleep() / GetZenSleepConfig().GeneralConfig.SecondsAsleepUntilFullRestApprox;
			float fatigueGain = ZenSleepConstants.FATIGUE_GAIN_BASE_RATE * ZenSleepConstants.SL_FATIGUE_MAX;
			fatigueSpeed = fatigueGain * (timeAsleepMulti * timeAsleepMulti);
			fatigueBuffDebuff = GetZenSleepGainModifier(player) * GetZenSleepConfig().GainConfig.GlobalGainMultiplier;

			// If bed is dry, add temperature buff slowly over time and eventually immunity boost 
			if (player.GetZenSleepManager().HasBed(GameConstants.STATE_DAMP))
			{
				if (GetZenSleepConfig().GainConfig.BedHeatBuffMaxGain > 0 && player.GetStatHeatBuffer().Get() < GetZenSleepConfig().GainConfig.BedHeatBuffMaxGain)
				{
					// 5 secs per heat buff 1x gain (30 = max gain = require ~2.5 minutes asleep to get FULL heat buff if config is set to 30)
					player.GetStatHeatBuffer().Add(0.05 * deltaT);
				}

				if (GetZenSleepConfig().GainConfig.BedHealthMaxGain > 0 && player.GetHealth() < GetZenSleepConfig().GainConfig.BedHealthMaxGain && player.GetZenSleepManager().GetSecondsAsleep() >= GetZenSleepConfig().GainConfig.MinSecsAsleepToTriggerHealthRecovery)
				{
					player.AddHealth("GlobalHealth", "Health", GetZenSleepConfig().GainConfig.BedHealthGainPerSec * deltaT * fatigueBuffDebuff);
				}

				if (GetZenSleepConfig().GainConfig.MinSecsAsleepInBedToTriggerImmunityBoost > 0 && player.GetZenSleepManager().GetSecondsAsleep() >= GetZenSleepConfig().GainConfig.MinSecsAsleepInBedToTriggerImmunityBoost)
				{
					if (!player.GetModifiersManager().IsModifierActive(eModifiers.MDF_IMMUNITYBOOST))
						player.GetModifiersManager().ActivateModifier(eModifiers.MDF_IMMUNITYBOOST);
				}
			}
		}
		else // Awake (drain)
		{
			fatigueSpeed = (GetZenSleepFatigueSpeed(player) + m_ZenSleepDrainSpeed);
			fatigueBuffDebuff = GetZenSleepDrainModifier(player) * GetZenSleepConfig().DrainConfig.GlobalDrainMultiplier * -1;

			// Red flashing
			if (m_Player.GetStatZenFatigue().Get() <= ZenSleepConstants.SL_FATIGUE_CRITICAL && GetZenSleepConfig().DrainConfig.HealthLossPerSecRedFlashing > 0)
			{
				player.AddHealth("GlobalHealth", "Health", Math.AbsFloat(GetZenSleepConfig().DrainConfig.HealthLossPerSecRedFlashing) * deltaT * -1);
			}
		}

		fatigueSpeed *= fatigueBuffDebuff;

		// Apply the fatigue gain/drain based on calculated speed
		player.GetStatZenFatigue().Add(fatigueSpeed * deltaT);
		
		// These value trackings are mainly for debug / cross-class referencing:
		m_Player.GetZenSleepManager().SetFatigueSpeedModifier(fatigueBuffDebuff);
		m_Player.GetZenSleepManager().SetFatigueSpeed(fatigueSpeed);
	}

	// Get fatigue stat drain speed based on player movement activity
	static float GetZenSleepFatigueSpeed(PlayerBase player)
	{
		int movement_speed = player.GetZenSleepManager().GetPlayerMovementState();

		float speed;
		switch (movement_speed)
		{
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
				speed = ZenSleepConstants.FATIGUE_SPEED_ENERGY_WALK * GetZenSleepConfig().DrainConfig.DrainWalkingMultiplier;
			break;
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
				speed = ZenSleepConstants.FATIGUE_SPEED_ENERGY_JOG * GetZenSleepConfig().DrainConfig.DrainJoggingMultiplier;
			break;
			case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
				speed = ZenSleepConstants.FATIGUE_SPEED_ENERGY_SPRINT * GetZenSleepConfig().DrainConfig.DrainSprintingMultiplier;
			break;
			default:
				speed = ZenSleepConstants.FATIGUE_SPEED_ENERGY_WALK * GetZenSleepConfig().DrainConfig.DrainWalkingMultiplier;
			break;
		}

		return speed;
	}

	// Get the fatigue stat recovery speed modifier when sleeping
	static float GetZenSleepGainModifier(PlayerBase player)
	{
		// Do not stack multipliers (eg. two 1.5x multipliers = 2x speed (1 + 0.5 + 0.5), not 2.25x (1.5 x 1.5)
		float modifier = 1;

		// Night boost
		if (GetGame().GetWorld().IsNight())
		{
			modifier = modifier + (GetZenSleepConfig().GainConfig.GainNightMultiplier - 1);
		}

		// Bed boost ONLY if bag is not wet
		if (player.GetZenSleepManager().HasBed())
		{
			modifier = modifier + (GetZenSleepConfig().GainConfig.GainBedObjectUsed - 1);
		}

		// Is player under shelter
		if (player.GetZenSleepManager().IsSheltered())
		{
			modifier = modifier + (GetZenSleepConfig().GainConfig.GainInsideMultiplier - 1);
		}

		float progressToFullHeatBuff = player.GetZenSleepManager().GetPlayerHeatBuffPercent() / 100;

		// Player sleeps "faster" when they're warm (multiplier is controlled by % of progress to full 3+++ heat buff)
		if (progressToFullHeatBuff > 0)
		{
			modifier = modifier + (progressToFullHeatBuff * (GetZenSleepConfig().GainConfig.GainFromHeatBuffMultiplier - 1));
		}

		return modifier;
	}

	// Get any drain multipliers which speed up fatigue stat loss when NOT sleeping
	static float GetZenSleepDrainModifier(PlayerBase player)
	{
		int movement_speed = player.GetZenSleepManager().GetPlayerMovementState();

		// Do not stack multipliers (eg. two 1.5x multipliers = 2x speed (1 + 0.5 + 0.5), not 2.25x (1.5 x 1.5)
		float modifier = 1;

		// Players get tired faster when doing activities during the night time when they should be in bed
		if (GetGame().GetWorld().IsNight())
		{
			modifier = modifier + (GetZenSleepConfig().DrainConfig.DrainNightMultiplier - 1);
		}

		if (movement_speed >= DayZPlayerConstants.MOVEMENTIDX_WALK)
		{
			// Only affect fatigue drain by player weight when they're moving
			float playerWeightPercent = player.GetZenSleepManager().GetPlayerWeightPercent();
			if (playerWeightPercent > 0)
			{
				modifier = modifier + ((playerWeightPercent * GetZenSleepConfig().DrainConfig.DrainWeightMultiplier) / 100);
			}

			// Only affect fatigue drain due to carrying heavy items (eg. barrels, sheet metal, logs) when moving
			ItemBase hands = player.GetItemInHands();
			if (hands != NULL && hands.IsHeavyBehaviour())
			{
				modifier = modifier + (10 * GetZenSleepConfig().DrainConfig.DrainSprintingMultiplier * GetZenSleepConfig().DrainConfig.DrainHeavyItemMultiplier);
			}
		}

		return modifier;
	}
}