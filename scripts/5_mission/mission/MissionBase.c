modded class MissionBase
{
	void MissionBase()
	{
		#ifdef SERVER
		// SERVER RECEIVE RPC
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenSleepConfigReloadRequest", this, SingeplayerExecutionType.Server);
		#else
		// CLIENT RECEIVE RPC
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenSleepClientConfig", this, SingeplayerExecutionType.Client);
		#endif
	}

	//! Server -> client
	void RPC_ReceiveZenSleepClientConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<ref ZenSleepEffectsClientConfig, bool, bool> data;
		if (!ctx.Read(data))
		{
			Error("[ZenModPack] RPC_ReceiveZenSleepClientConfig: sync data read error");
			return;
		}

		GetZenSleepConfig().SetClientEffectsConfig(data.param1);
		GetZenSleepConfig().DebugMode = data.param2;
		GetZenSleepConfig().IS_CLIENT_ADMIN = data.param3;
	}

	//! Client -> server 
	void RPC_ReceiveZenSleepConfigReloadRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<bool> data;
		if (!ctx.Read(data))
		{
			Error("[ZenModPack] RPC_ReceiveZenSleepConfigReloadRequest: sync data read error");
			return;
		}

		GetZenSleepConfig().Reload();

		// Send to players again
		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
			
		foreach (Man man : players)
		{
			PlayerBase player;
			if (Class.CastTo(player, man))
			{
				// Sync client config.
				GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenSleepClientConfig", new Param3<ref ZenSleepEffectsClientConfig, bool, bool>(GetZenSleepConfig().ClientEffectsConfig, GetZenSleepConfig().DebugMode, GetZenSleepConfig().IsAdmin(player.GetIdentity().GetPlainId())), true, player.GetIdentity());
			}
		}

		ZenFatigueNotfr.SetUpFatigueThresholds(); // Reload thresholds
		ZenSleepFunctions.DebugMessage("Successfully reloaded and resync'd config.");
	}
}