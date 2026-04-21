#ifdef JM_COT
modded class JMPlayerInstance
{
	protected float m_ZenSleep;

	override void Update()
	{
		if (g_Game.IsServer() && (g_Game.GetTime() - m_DataLastUpdated) >= 100)
		{
			if (!g_Game.IsMultiplayer())
			{
				Class.CastTo(PlayerObject, g_Game.GetPlayer());
			}

			if (PlayerObject && PlayerObject.GetStatZenFatigue() != null)
			{
				m_ZenSleep = PlayerObject.GetStatZenFatigue().Get();
			}
		}

		super.Update();
	}

	float GetZenSleep()
	{
		return m_ZenSleep;
	}

	override void OnSendHealth(ParamsWriteContext ctx)
	{
		super.OnSendHealth(ctx);

		ctx.Write(m_ZenSleep);
	}

	override void OnRecieveHealth(ParamsReadContext ctx)
	{
		super.OnRecieveHealth(ctx);

		ctx.Read(m_ZenSleep);
	}
}
#endif