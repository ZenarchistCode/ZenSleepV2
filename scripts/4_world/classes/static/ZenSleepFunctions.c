class ZenSleepFunctions
{
	//! TODO: Had to use ActionBase code while on a plane with no internet - verify RPC_USER_ACTION_MESSAGE is best option
	protected static ref Param1<string> m_MessageParam = new Param1<string>("");

	//! Debug message - sends a server-side player message to all online players
	static void SendGlobalMessage(string msg)
	{
		SendGlobalMessageEx(msg, "[SERVER] ");
	}

	//! Debug message - sends a server-side player message to all online players
	static void SendGlobalMessageEx(string msg, string prefix = "")
	{
		if (msg == "")
			return; 

		#ifdef SERVER
		array<Man> players = new array<Man>;
		GetGame().GetWorld().GetPlayerList(players);
		for (int x = 0; x < players.Count(); x++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(x));
			if (pb)
			{
				ServerMessage(pb, prefix + msg);
			}
		}
		#endif
	}

	//! Display client message ONLY on client
	static void ClientMessage(string message)
	{
#ifndef SERVER
		if (GetGame().GetPlayer())
		{
			GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", message, ""));
		}
#endif
	}

	//! Send server message to player 
	static void ServerMessage(PlayerBase player, string message)
	{
		if (!player)
			return;

		if (!player.GetIdentity() || !player.IsAlive())
			return;

		m_MessageParam.param1 = message;
		GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, player.GetIdentity());
	}

	//! Print a debug chat message both client-side & server-side
	static void DebugMessage(string message)
	{
		if (!GetZenSleepConfig().DebugMode)
			return;

		ClientMessage("[CLIENT] " + message);
		SendGlobalMessage(message);
	}

	//! Do some fancy ChatGPT stuff to achieve something my dum dum brain could never understand allowing us to share multiple data in 1 integer like a communist for MAXIMUM EFFICIENCY

	// Max value of param1 = 4095, param 2 & 3 = 1023
	static int GetPackedIntegerData3(int param1, int param2, int param3)
	{
		return (param1 << 20) | (param2 << 10) | param3;
	}

	static void ReadPackedIntegerData3(int dataToRead, out int param1, out int param2, out int param3)
	{
		param1 = (dataToRead >> 20) & 0xFFF;
		param2 = (dataToRead >> 10) & 0x3FF;
		param3 = dataToRead & 0x3FF;
	}

	// Max value of each param = 255
	static int GetPackedIntegerData4(int param1, int param2, int param3, int param4)
	{
		return (param1 << 24) | (param2 << 16) | (param3 << 8) | param4;
	}

	static void ReadPackedIntegerData4(int dataToRead, out int param1, out int param2, out int param3, out int param4)
	{
		param1 = (dataToRead >> 24) & 0xFF;
		param2 = (dataToRead >> 16) & 0xFF;
		param3 = (dataToRead >> 8)  & 0xFF;
		param4 = dataToRead & 0xFF;
	}
}