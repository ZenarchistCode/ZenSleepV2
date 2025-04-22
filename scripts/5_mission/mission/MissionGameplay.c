modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		Input gameInput = Input.Cast(GetGame().GetInput());

		if (!player || !gameInput)
		{
			return;
		}

		// If a menu is open stop here
		if (GetGame().GetUIManager().GetMenu() != null || GetGame().GetUIManager().IsCursorVisible())
		{
			return;
		}

		if (gameInput.LocalPress("UAZenSleep_RefreshConfig")) // Sends request to server to reload json config if debug mode is enabled and player is admin
		{
			if (GetZenSleepConfig().IS_CLIENT_ADMIN)
			{
				GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenSleepConfigReloadRequest", new Param1<bool>(true), true, NULL);
			}
		}
	}
}