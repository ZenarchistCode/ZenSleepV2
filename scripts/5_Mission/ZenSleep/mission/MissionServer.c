modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		// Pre-load mod config on server startup
		GetZenSleepConfig();
	}

	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		PlayerBase newPlayer = super.OnClientNewEvent(identity, pos, ctx);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return newPlayer;
		}
		#endif

		if (newPlayer)
			newPlayer.GetStatZenFatigue().Set(ZenSleepConstants.SL_FATIGUE_MAX * (Math.Clamp(GetZenSleepConfig().GeneralConfig.FreshSpawnFatiguePercent, 0, 100) / 100));
	
		return newPlayer;
	}
}