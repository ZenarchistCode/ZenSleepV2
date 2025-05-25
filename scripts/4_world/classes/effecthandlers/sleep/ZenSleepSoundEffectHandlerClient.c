class ZenSleepSoundEffectHandlerClient extends ZenSleepEffectHandlerBase
{
	// Shared playerbase sound effects
	protected EffectSound m_Sound;

	void ~ZenSleepSoundEffectHandlerClient()
	{
		if (m_Sound)
			m_Sound.Stop();
	}

	void PlaySoundEffect(int fxID)
	{
		if (fxID == -1)
			return;

		// Unpack soundset array ID and array element from the single int
		int arrayID;
		int arrayElement;
		int visualEffectID; // visual, ignored here
		int vanillaSoundEventID;
		ZenSleepFunctions.ReadPackedIntegerData4(fxID, arrayID, arrayElement, visualEffectID, vanillaSoundEventID);

		if (vanillaSoundEventID > 0)
		{
			if (vanillaSoundEventID < 0 || vanillaSoundEventID > (EPlayerSoundEventID.ENUM_COUNT - 1))
			{
				Error("EPlayerSoundEventID out of bounds: " + vanillaSoundEventID);
				return;
			}

			m_Player.PlaySoundEventEx(vanillaSoundEventID, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
		}

		if (arrayID == 0)
			return;

		string soundSet = ZenSleepSounds.GetSoundset(arrayID, arrayElement);
		m_Sound = SEffectManager.PlaySoundOnObject(soundSet, m_Player);
		
		if (m_Sound)
		{
			m_Sound.SetAutodestroy(true);
		}
		else
		{
			Error("[ZenSleep] Missing soundset for sound ID array=" + arrayID + " element=" + arrayElement + " soundSet=" + soundSet);
		}
	}
}