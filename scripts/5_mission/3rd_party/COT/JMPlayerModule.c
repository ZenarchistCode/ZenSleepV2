#ifdef JM_COT
modded class JMPlayerModule
{
	void JMPlayerModule()
	{
		GetPermissionsManager().RegisterPermission("Admin.Player.Set.ZenSleep");
	}

	override private void Exec_Heal(array<string> guids, PlayerIdentity ident, JMPlayerInstance instance = NULL)
	{
		super.Exec_Heal(guids, ident, instance);

		array<JMPlayerInstance> players = GetPermissionsManager().GetPlayers(guids);

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players[i].PlayerObject);
			if (!player)
				continue;

			player.GetStatZenFatigue().Set(player.GetStatZenFatigue().GetMax());
		}
	}

	void SetZenSleep(float fatigue, array<string> guids)
	{
		if (IsMissionHost())
			return;

		// JMPlayerModule RPC has some weird id limit, Expansion overrides it, and I don't want
		// to risk breaking their compatibility so I just use my own RPC method here instead.

		/*ScriptRPC rpc = new ScriptRPC();
		rpc.Write(fatigue);
		rpc.Write(guids);
		rpc.Send(NULL, Zen_JMPlayerModuleRPC.SetZenSleep, true, NULL);*/

		GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenSleepSetStatCOT", new Param2<float, array<string>>(fatigue, guids), true, NULL);
	}
}
#endif