#ifdef JM_COT
modded class JMPlayerInstance
{
	protected float m_ZenSleep;

	override void Update()
	{
		if (GetGame().IsServer() && (GetGame().GetTime() - m_DataLastUpdated) >= 100)
		{
			if (!GetGame().IsMultiplayer())
			{
				Class.CastTo(PlayerObject, GetGame().GetPlayer());
			}

			if (PlayerObject)
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

	void OnRecieveHealth(ParamsReadContext ctx)
	{
		super.OnRecieveHealth(ctx);

		ctx.Read(m_ZenSleep);
	}
}
#endif