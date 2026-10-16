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

	// Use the installed COT version's sync API; older COT uses the health hooks.
	#ifndef COT_ONRECEIVE_PROPER
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
	#else
	override void OnSendStatus(ParamsWriteContext ctx)
	{
		super.OnSendStatus(ctx);

		ctx.Write(m_ZenSleep);
	}

	override bool OnReceiveStatus(ParamsReadContext ctx)
	{
		if (!super.OnReceiveStatus(ctx))
			return false;

		return ctx.Read(m_ZenSleep);
	}
	#endif
}
#endif