// Hide nearby target object UI while lying down
modded class ActionTargetsCursor
{
	override void Update(bool fullUpdate = true)
	{
		super.Update(fullUpdate);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (m_Player && m_Player.GetZenSleepManager() && m_Player.GetZenSleepManager().IsLyingDown())
		{
			m_Hidden = true;
		}
	}
}