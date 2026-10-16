// Override player movement speed based on config
modded class InjuryAnimationHandler
{
	#ifdef TERJE_MEDICINE_MOD
	override private eInjuryHandlerLevels CalculateSleepingState()
	{
		// Suppress only Terje's sleep limp; its injury/disease movement effects remain.
		if (ZenSleepCompatibility.OverridesTerje())
			return eInjuryHandlerLevels.PRISTINE;

		return super.CalculateSleepingState();
	}
	#endif

	override eInjuryHandlerLevels GetInjuryLevel(float health)
	{
		eInjuryHandlerLevels vanillaResult = super.GetInjuryLevel(health);

		if (!ZenSleepCompatibility.IsZenSleepEnabled())
			return vanillaResult;

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return vanillaResult;
		}
		#endif
		
		if (!GetZenSleepConfig().ClientEffectsConfig)
			return vanillaResult;

		if (m_Player.GetStatLevelZenFatigue() >= EStatLevels.LOW && m_Player.GetZenSleepManager().GetSleepCondition() != ZenSleepState.ON_DRUGS)
		{
			eInjuryHandlerLevels vanillaOverrideResult = 0;

			if (m_ForceInjuryAnimMask)
			{
				vanillaOverrideResult = GetOverrideLevel(vanillaResult);
			}

			eInjuryHandlerLevels fatigueResult = 0;

			if (m_Player.GetStatLevelZenFatigue() == EStatLevels.CRITICAL)
			{
				if (GetZenSleepConfig().ClientEffectsConfig.CharacterMovementSpeedRedFlashing > 0)
					fatigueResult = GetZenSleepConfig().ClientEffectsConfig.CharacterMovementSpeedRedFlashing;
			} else 
			if (m_Player.GetStatLevelZenFatigue() == EStatLevels.LOW)
			{
				if (GetZenSleepConfig().ClientEffectsConfig.CharacterMovementSpeedRed > 0)
					fatigueResult = GetZenSleepConfig().ClientEffectsConfig.CharacterMovementSpeedRed;
			}

			// Only override vanilla movement speed if ours is worse
			if (fatigueResult > vanillaResult && fatigueResult > vanillaOverrideResult)
				vanillaResult = fatigueResult;
		}

		return vanillaResult;
	}
}
