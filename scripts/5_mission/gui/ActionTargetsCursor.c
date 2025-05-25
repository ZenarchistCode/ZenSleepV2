// Hide nearby target object UI while lying down
modded class ActionTargetsCursor
{
	override void Update()
	{
		super.Update();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (m_Player && m_Player.GetZenSleepManager().IsLyingDown())
		{
			m_Hidden = true;
		}
	}
}