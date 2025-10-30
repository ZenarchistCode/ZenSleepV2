modded class EmoteManager
{
	bool IsLyingDownZS()
	{
		return m_Callback != NULL && m_Callback.m_callbackID == DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN && m_bEmoteIsPlaying;
	}

	bool ShouldAllowInvWhileSleepingZS()
    {
        #ifdef ZENMODPACK
        if (!ZenModEnabled("ZenSleep")) 
			return false;
        #endif

        return GetZenSleepConfig().ClientEffectsConfig.AllowInventoryWhileSleeping && IsLyingDownZS();
    }

	// Triggered on both client & server when player uses an emote
	override bool PlayEmote(int id)
	{
		bool playEmote = super.PlayEmote(id);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return playEmote;
		}
		#endif

		if (playEmote && id == EmoteConstants.ID_EMOTE_LYINGDOWN)
		{
			// If the player has used the lie down emote, prepare for sleep 
			// GetSimulationTimeStamp() >= 300ms allows using sleeping emote upon login without triggering sleep fx.
			// used for compatibility with my Immersive Login mod
			if (m_Player.GetSimulationTimeStamp() >= 300)
			{
				m_Player.GetZenSleepManager().OnStartSleep();
			}
		}

		return playEmote;
	}

	override void OnEmoteEnd()
	{
		int lastGesture = m_CurrentGestureID;
		super.OnEmoteEnd();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (lastGesture == EmoteConstants.ID_EMOTE_LYINGDOWN)
		{
			m_Player.GetZenSleepManager().OnStopSleep();
		}
	}
}