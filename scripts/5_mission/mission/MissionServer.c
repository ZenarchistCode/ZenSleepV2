modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		// Pre-load mod config on server startup
		GetZenSleepConfig();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) 
	{
		super.InvokeOnConnect(player, identity);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		// Sync client config.
		GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenSleepClientConfig", new Param3<ref ZenSleepEffectsClientConfig, bool, bool>(GetZenSleepConfig().ClientEffectsConfig, GetZenSleepConfig().DebugMode, GetZenSleepConfig().IsAdmin(identity.GetPlainId())), true, identity);
	}

	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		PlayerBase newPlayer = super.OnClientNewEvent(identity, pos, ctx);

		#ifdef ZENMODPACK
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