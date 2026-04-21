modded class MissionBase
{
	void MissionBase()
	{
		// CommunityOnlineTools compatibility
		#ifdef JM_COT
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenSleepSetStatCOT", this, SingeplayerExecutionType.Server);
		#endif
	}

	#ifdef JM_COT
	//! Client -> server 
	void RPC_ReceiveZenSleepSetStatCOT(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		JMPlayerInstance instance;
		if (!GetPermissionsManager().HasPermission("Admin.Player.Set.ZenSleep", sender, instance))
			return;

		Param2<float, array<string>> data;
		if (!ctx.Read(data))
		{
			Error("[ZenModPack] RPC_ReceiveZenSleepSetStatCOT: sync data read error");
			return;
		}

		array<Man> players = new array<Man>();
		g_Game.GetPlayers(players);
			
		foreach (Man man : players)
		{
			PlayerBase player;
			if (Class.CastTo(player, man))
			{
				if (player.GetIdentity() && player.GetStatZenFatigue() && data.param2.Find(player.GetIdentity().GetId()) != -1)
				{
					player.GetStatZenFatigue().Set(data.param1);
				}
			}
		}
	}
	#endif
}