class ZenFatigueNotfr extends NotifierBase
{
	private static float DEC_TRESHOLD_LOW;
	private static float DEC_TRESHOLD_MED;
	private static float DEC_TRESHOLD_HIGH;
	private static float INC_TRESHOLD_LOW;
	private static float INC_TRESHOLD_MED;
	private static float INC_TRESHOLD_HIGH;

	protected bool m_ZenSleepDebugAlternate = false;

	void ZenFatigueNotfr(NotifiersManager manager)
	{
		SetUpFatigueThresholds();
	}

	static void SetUpFatigueThresholds()
	{
		#ifdef SERVER
		DEC_TRESHOLD_LOW	= ZenSleepConstants.DEC_TRESHOLD_LOW;
		DEC_TRESHOLD_MED	= (ZenSleepConstants.DEC_TRESHOLD_MED * GetZenSleepConfig().DrainConfig.GlobalDrainMultiplier) * -1;
		DEC_TRESHOLD_HIGH	= (ZenSleepConstants.DEC_TRESHOLD_HIGH * GetZenSleepConfig().DrainConfig.GlobalDrainMultiplier) * -1;
		INC_TRESHOLD_LOW	= ZenSleepConstants.INC_TRESHOLD_LOW;
		INC_TRESHOLD_MED	= ZenSleepConstants.INC_TRESHOLD_MED * (GetZenSleepConfig().GeneralConfig.SecondsAsleepUntilFullRestApprox / ZenSleepConstants.FATIGUE_BASE_SECONDS_TIL_RESTED) * GetZenSleepConfig().GainConfig.GlobalGainMultiplier;
		INC_TRESHOLD_HIGH	= ZenSleepConstants.INC_TRESHOLD_HIGH * (GetZenSleepConfig().GeneralConfig.SecondsAsleepUntilFullRestApprox / ZenSleepConstants.FATIGUE_BASE_SECONDS_TIL_RESTED) * GetZenSleepConfig().GainConfig.GlobalGainMultiplier;
		#endif
	}

	override int GetNotifierType()
	{
		return ZenSleepEnums.NTFKEY_FATIGUE;
	}
	
	// Server-side execution
	override void DisplayTendency(float delta)
	{
		#ifdef CLIENT 
		return;
		#endif

		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		EStatLevels fatigue_state = 1 + m_Player.GetStatLevelZenFatigue();
		
		m_Player.GetZenSleepManager().SetFatigueStatTendency(tendency);
		m_Player.GetZenSleepManager().SetFatigueStatState(fatigue_state);

		m_ZenSleepDebugAlternate = !m_ZenSleepDebugAlternate; // Paces the messages once every 2 update ticks to avoid too much spam

		if (!GetZenSleepConfig().DebugMode || !m_ZenSleepDebugAlternate)
			return;

		float playerTemp = m_Player.GetZenSleepManager().GetPlayerTemperature();
		float playerWet = m_Player.GetZenSleepManager().GetPlayerWetness();
		float playerWeight = m_Player.GetZenSleepManager().GetPlayerWeightPercent();
		float playerHunger = m_Player.GetZenSleepManager().GetPlayerHunger();
		float playerThirst = m_Player.GetZenSleepManager().GetPlayerThirst();
		bool playerSick =  m_Player.GetZenSleepManager().IsPlayerSick();
		bool isNight = m_Player.GetZenSleepManager().IsNightTime();
		bool hasBed = m_Player.GetZenSleepManager().HasBed();

		string debugMsg = "";
		debugMsg = debugMsg + "ftg=" + Trunc(m_Player.GetStatZenFatigue().Get());
		if (m_Player.GetStatZenFatigue().Get() > 0 && delta != 0)
		{
			if (delta > 0)
				debugMsg = debugMsg + " ttf=" + Trunc(((m_Player.GetStatZenFatigue().GetMax() - m_Player.GetStatZenFatigue().Get()) / delta)) + "s";
			else
				debugMsg = debugMsg + " ttz=" + Trunc((m_Player.GetStatZenFatigue().Get() / (delta * -1))) + "s";
		}
		debugMsg = debugMsg + " state=" + m_Player.GetStatLevelZenFatigue();
		debugMsg = debugMsg + " delta=" + Trunc(delta);
		debugMsg = debugMsg + " night=" + isNight;
		debugMsg = debugMsg + " bed=" + hasBed;
		debugMsg = debugMsg + " inside=" + m_Player.GetZenSleepManager().IsSheltered();
		debugMsg = debugMsg + " temp=" + Trunc(playerTemp) + "/b=" + Trunc(m_Player.GetStatHeatBuffer().Get());
		debugMsg = debugMsg + " wet=" + playerWet;
		debugMsg = debugMsg + " weight=" + Trunc(playerWeight) + "%";
		if (m_Player.GetStatLevelZenFatigue() >= GetZenSleepConfig().GeneralConfig.FatigueLevelToNegativeImpactImmunity)
			debugMsg = debugMsg + " imm=" + Trunc(m_Player.GetImmunity());
		debugMsg = debugMsg + " hp=" + m_Player.GetHealth();
		debugMsg = debugMsg + " tsleep=" + m_Player.GetZenSleepManager().GetSecondsAsleep() + "s";
		debugMsg = debugMsg + " multi=" + m_Player.GetZenSleepManager().GetFatigueSpeedModifier() + "x";
		if (m_Player.GetZenSleepManager().GetFatigueDrainPauseSeconds() > 0)
			debugMsg = debugMsg + " paused=" + m_Player.GetZenSleepManager().GetFatigueDrainPauseSeconds() + " secs";

		ZenSleepFunctions.DebugMessage(debugMsg);
	}

	override protected float GetObservedValue()
	{
		return m_Player.GetStatZenFatigue().Get();
	}

	static string Trunc(float v)
	{
		return MiscGameplayFunctions.TruncateToS(v);
	}
}