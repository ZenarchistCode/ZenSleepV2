// Some mods use the lie-down emote as an action ID. This triggers differently to the emote manager method.
// So this little injection ensures that those mods remain compatible with this mod (eg. Boomlay's things beds)
modded class AnimatedActionBase
{
	bool IsPlayerLyingDownZS(PlayerBase player)
	{
		return GetActionCommand(player) == DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN;
	}

	// Reset sleep vars when Lie Down emote action is detected
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		if (IsPlayerLyingDownZS(action_data.m_Player))
		{
			action_data.m_Player.GetZenSleepManager().OnStartSleep();

			Object targetObj = action_data.m_Target.GetObject();

			// Let's just assume any modded items which allow sleeping on them provide warmth/comfort benefits.
			if (targetObj != NULL)
			{
				action_data.m_Player.GetZenSleepManager().SetBed(targetObj);
			}
		}
	}
}