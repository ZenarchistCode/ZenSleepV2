class ZenSleepEffectHandlerServer extends ZenSleepEffectHandlerBase
{
	protected float m_DeltaTime;
	protected float m_NextEffectCheckTime;
	protected float m_UnconCheckDeltaTime;
	protected float m_NextUnconCheckTime;

	override void Init()
	{
		m_NextEffectCheckTime = ZenSleepConstants.SLEEP_UPDATE_TICK_SECS;
		m_DeltaTime = 0;
		m_UnconCheckDeltaTime = 0;
		m_NextUnconCheckTime = 60;
	}

	override void Update(float deltaTime = 1)
	{
		m_DeltaTime += deltaTime;

		// Check for uncon chance 
		CheckUnconEffect(deltaTime);

		// Check for yawns / snores every X seconds
		if (m_DeltaTime < m_NextEffectCheckTime)
			return;

		m_NextEffectCheckTime = ZenSleepConstants.SLEEP_UPDATE_TICK_SECS;
		m_DeltaTime = 0;

		if (m_Player.GetZenSleepManager().IsFatigueDrainPaused())
			return; // on drugs, mum would be proud

		if (m_Player.GetZenSleepManager().IsSleeping())
		{
			// Sleeping behaviour 
			int arrayID;
			int arrayElement;
			int visualFX = 0;
			float playChance;
			float minSecs;
			string soundset;

			// Play snores.
			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToSnoreSleeping && GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenSnore > 0)
			{
				if (m_Player.IsMale())
					arrayID = ZenSleepConstants.SLEEP_SOUNDSET_MALE_SNORES;
				else
					arrayID = ZenSleepConstants.SLEEP_SOUNDSET_FEMALE_SNORES;

				arrayElement	= GetRandomSoundset(arrayID);
				soundset		= ZenSleepSounds.GetSoundset(arrayID, arrayElement);
				playChance		= GetGame().ConfigGetFloat("CfgSoundSets " + soundset + " chanceToPlay");
				minSecs			= GetGame().ConfigGetFloat("CfgSoundSets " + soundset + " minSecsAsleepToPlay");
			
				if (m_Player.GetZenSleepManager().GetSecondsAsleep() < minSecs || Math.RandomFloat01() > playChance)
				{
					return;
				}

				m_NextEffectCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenSnore, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenSnore);
				m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(arrayID, arrayElement, visualFX, 0));
			}

			// Stop here if we're sleeping.
			return;
		}

		int fatigueState = m_Player.GetZenSleepManager().GetFatigueStatState() - 1;
		bool redFlashing = fatigueState == EStatLevels.CRITICAL; // 5 = red flashing critical
		bool red = fatigueState == EStatLevels.LOW; // 4 = red status

		if (redFlashing)
		{
			// Red flashing behaviour

			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToYawnRedFlashing)
			{
				SendYawnEffect(1);
			}
		} else
		if (red)
		{
			// Red behaviour

			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToYawnRed)
			{
				SendYawnEffect(2);
			}
		}
	}

	void SendYawnEffect(int status = 0)
	{
		int arrayID;
		int arrayElement;
		int visualFX;

		if (m_Player.IsMale())
			arrayID = ZenSleepConstants.SLEEP_SOUNDSET_MALE_YAWNS;
		else
			arrayID = ZenSleepConstants.SLEEP_SOUNDSET_FEMALE_YAWNS;

		if (status == -1) // Force hallucination (uncon)
		{
			visualFX = ZenSleepConstants.SLEEP_VFX_HALLUCINATION_UNCON;
		} else
		if (status == 1) // Red flashing moon
		{
			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToHallucinateRedFlashing)
				visualFX = ZenSleepConstants.SLEEP_VFX_HALLUCINATION_YAWN;
		}
		else if (status == 2) // Red moon
		{
			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToHallucinateRed)
				visualFX = ZenSleepConstants.SLEEP_VFX_HALLUCINATION_YAWN;
		}

		arrayElement = GetRandomSoundset(arrayID);
		m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(arrayID, arrayElement, visualFX, 0));

		if (status == 1)
			m_NextEffectCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenYawnRedFlashing, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRedFlashing);
		else 
		if (status == 2)
			m_NextEffectCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenYawnRed, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRed);
	}

	protected void CheckUnconEffect(float deltaTime)
	{
		m_UnconCheckDeltaTime += deltaTime;

		if (m_UnconCheckDeltaTime < m_NextUnconCheckTime)
		{
			return;
		}

		m_UnconCheckDeltaTime = 0;
		bool cantUncon = m_Player.IsUnconscious();

		if (!cantUncon && m_Player.GetZenSleepManager().GetSleepCondition() == ZenSleepState.ON_DRUGS)
			cantUncon = true;

		if (!cantUncon && m_Player.m_IsVehicleSeatDriver && !GetZenSleepConfig().ServerEffectsConfig.CanUnconWhileDriving)
			cantUncon = true;

		if (!cantUncon && m_Player.IsSwimming() && !GetZenSleepConfig().ServerEffectsConfig.CanUnconWhileSwimming)
			cantUncon = true;

		// Check if uncon is not supposed to happen. If so, check again in 60 secs
		if (cantUncon)
		{
			m_UnconCheckDeltaTime = 60;
			return;
		}

		int fatigueState = m_Player.GetZenSleepManager().GetFatigueStatState() - 1;
		bool redFlashing = fatigueState == EStatLevels.CRITICAL; // 5 = red flashing critical
		bool red = fatigueState == EStatLevels.LOW; // 4 = red status

		if (redFlashing)
		{
			// Red flashing behaviour

			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToUnconRedFlashing / 100)
			{
				// GO UNCON!
				TriggerPlayerUncon();
			}

			m_NextUnconCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenUnconRedFlashing, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRedFlashing);
		} else
		if (red)
		{
			// Red behaviour

			if (Math.RandomFloat01() < GetZenSleepConfig().ServerEffectsConfig.ChanceToUnconRed / 100)
			{
				// GO UNCON!
				TriggerPlayerUncon();
			}

			m_NextUnconCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenUnconRed, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRed);
		} else 
			m_NextUnconCheckTime = 60;
	}

	void UpdateStateTimers()
	{
		int fatigueState = m_Player.GetZenSleepManager().GetFatigueStatState() - 1;
		bool redFlashing = fatigueState == EStatLevels.CRITICAL; // 5 = red flashing critical
		bool red = fatigueState == EStatLevels.LOW; // 4 = red status

		if (red)
		{
			if (m_NextEffectCheckTime > GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRed)
			{
				m_NextEffectCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenYawnRed, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRed);
			}

			if (m_NextUnconCheckTime > GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRed)
			{
				m_NextUnconCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenUnconRed, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRed);
			}
		}
		else
		if (redFlashing)
		{
			if (m_NextEffectCheckTime > GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRedFlashing)
			{
				m_NextEffectCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenYawnRedFlashing, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenYawnRedFlashing);
			}

			if (m_NextUnconCheckTime > GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRedFlashing)
			{
				m_NextUnconCheckTime = Math.RandomFloatInclusive(GetZenSleepConfig().ServerEffectsConfig.MinimumSecsBetweenUnconRedFlashing, GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBetweenUnconRedFlashing);
			}
		}
	}

	void TriggerPlayerUncon()
	{
		ZenSleepFunctions.DebugMessage("Setting player unconscious in 5 seconds!");
		SendYawnEffect(-1);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetPlayerUncon, 5000, false);
	}

	void SetPlayerUncon()
	{
		m_Player.GiveShock(-100);
		//m_Player.m_UnconsciousEndTime = Math.RandomFloatInclusive(-30, -60); // Doesn't work? Used to. TODO: Find a new way.
	}

	static int GetRandomSoundset(int arrayID)
	{
		int arrayElement;

		switch (arrayID)
		{
			case 1: // male yawns
				arrayElement = Math.RandomIntInclusive(0, ZenSleepSounds.GetMaleYawnSounds().Count() - 1);
				break;
			case 2: // male snores
				arrayElement = Math.RandomIntInclusive(0, ZenSleepSounds.GetMaleSnoreSounds().Count() - 1);
				break;
			case 3: // female yawns
				arrayElement = Math.RandomIntInclusive(0, ZenSleepSounds.GetFemaleYawnSounds().Count() - 1);
				break;
			case 4: // female snores
				arrayElement = Math.RandomIntInclusive(0, ZenSleepSounds.GetFemaleSnoreSounds().Count() - 1);
				break;
		}

		return arrayElement;
	}
}