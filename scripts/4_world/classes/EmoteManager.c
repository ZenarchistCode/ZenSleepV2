modded class EmoteManager
{
	bool IsLyingDownZS()
	{
		return m_Callback != NULL && m_Callback.m_callbackID == DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN && m_bEmoteIsPlaying;
	}

	// Triggered on both client & server when player uses an emote
	override bool PlayEmote(int id)
	{
		bool playEmote = super.PlayEmote(id);

		// If the player has used the lie down emote, prepare for sleep (GetSimulationTimeStamp() >= 300 allows using sleeping emote upon login without triggering sleep fx - used for compatibility with my Immersive Login mod)
		if (playEmote && id == EmoteConstants.ID_EMOTE_LYINGDOWN && m_Player.GetSimulationTimeStamp() >= 300)
		{
			m_Player.GetZenSleepManager().OnStartSleep();

			if (GetZenSleepConfig().ClientEffectsConfig.AllowInventoryWhileSleeping) // Allow inventory access if permitted by server config
			{
				m_Player.SetInventorySoftLock(false);
			}
		}

		return playEmote;
	}

	override void OnEmoteEnd()
	{
		int lastGesture = m_CurrentGestureID;
		super.OnEmoteEnd();

		if (lastGesture == EmoteConstants.ID_EMOTE_LYINGDOWN)
		{
			m_Player.GetZenSleepManager().OnStopSleep();
		}
	}
}