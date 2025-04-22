// Hide nearby target object UI while lying down
modded class ActionTargetsCursor
{
	override void Update()
	{
		super.Update();

		if (m_Player && m_Player.GetZenSleepManager().IsLyingDown())
		{
			m_Hidden = true;
		}
	}
}