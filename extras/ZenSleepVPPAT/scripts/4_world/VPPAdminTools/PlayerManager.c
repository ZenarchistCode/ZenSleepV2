/*
	Override some VPPAdminTools code to enable compatibility with my sleep mod fatigue stat. 
	This is all copy & paste code with just some additions.
*/

modded class PlayerManager
{
	override void SetPlayerStats(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Server)
		{
			Param3<float,string,string> data; //stat level, player id, stat type
			if(!ctx.Read(data)) 
				return;

			if (!GetPermissionManager().VerifyPermission(sender.GetPlainId(),"PlayerManager:SetPlayerStats") || !GetPermissionManager().VerifyPermission(sender.GetPlainId(),"PlayerManager:SetPlayerStats",data.param2)) return;
			
			PlayerBase targetPlayer = GetPermissionManager().GetPlayerBaseByID(data.param2);
			if (targetPlayer == null) 
				return;
			
			string targetPlayerName = targetPlayer.VPlayerGetName();
			string targetPlayerId   = targetPlayer.VPlayerGetSteamId();

			switch(data.param3)
			{
				case "Blood":
				targetPlayer.SetHealth( "","Blood", data.param1 );
				break;
				
				case "Health":
				targetPlayer.SetHealth( data.param1 );
				break;
				
				case "Shock":
				targetPlayer.SetHealth( "","Shock", data.param1 );
				break;
				
				case "Water":
				targetPlayer.GetStatWater().Set(data.param1);
				break;
				
				case "Energy":
				targetPlayer.GetStatEnergy().Set(data.param1);
				break;

				case "ZenSleep":
				targetPlayer.GetStatZenFatigue().Set(data.param1);
				break;
			}

			GetSimpleLogger().Log(string.Format("\"%1\" (steamid=%2) just updated a health stat on (steamid=%3)", sender.GetName(), sender.GetPlainId(), data.param2));
			GetWebHooksManager().PostData(AdminActivityMessage, new AdminActivityMessage(sender.GetPlainId(), sender.GetName(), "[PlayerManager] Set Player stats on target: " + targetPlayerId + " Name: " + targetPlayerName));
		}
	}

	override void GetPlayerStats(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Server)
		{
			Param1<string> data;
			if(!ctx.Read(data)) 
				return;

			if (!GetPermissionManager().VerifyPermission(sender.GetPlainId(), "MenuPlayerManager", "", false)) 
				return;
			
			string id = data.param1;
			PlayerBase playerMan = GetPermissionManager().GetPlayerBaseByID(id);

			if (playerMan != null && sender != null)
			{
				string inHandsName = "";
				
				if(playerMan.GetHumanInventory() != null && playerMan.GetHumanInventory().GetEntityInHands() != null)
					inHandsName = playerMan.GetHumanInventory().GetEntityInHands().GetDisplayName();
			
				if(inHandsName == "")
					inHandsName = "None";

				map<string,string> dataMap = new map<string,string>;
				dataMap.Insert("Guid",playerMan.VPlayerGetHashedId());
				dataMap.Insert("Name",playerMan.VPlayerGetName());
				dataMap.Insert("Blood",playerMan.GetHealth("GlobalHealth", "Blood").ToString());
				dataMap.Insert("Health",playerMan.GetHealth("GlobalHealth", "").ToString());
				dataMap.Insert("Shock",playerMan.GetHealth("GlobalHealth", "Shock").ToString());
				dataMap.Insert("Water",PlayerBase.Cast(playerMan).GetStatWater().Get().ToString());
				dataMap.Insert("Energy",PlayerBase.Cast(playerMan).GetStatEnergy().Get().ToString());
				dataMap.Insert("ZenSleep",PlayerBase.Cast(playerMan).GetStatZenFatigue().Get().ToString());
				dataMap.Insert("Weapon",inHandsName);
				dataMap.Insert("SteamID",playerMan.VPlayerGetSteamId());
				dataMap.Insert("UserGroup",GetPermissionManager().GetPlayerUserGrpNameByID(id));
	
				PlayerStatsData stats = new PlayerStatsData(dataMap);
				GetRPCManager().VSendRPC( "RPC_MenuPlayerManager", "HandlePlayerStats", new Param1<ref PlayerStatsData>(stats), true, sender);
				GetWebHooksManager().PostData(AdminActivityMessage, new AdminActivityMessage(sender.GetPlainId(), sender.GetName(), "[PlayerManager] GetPlayerStats Target ID: " + id + " Name: " + playerMan.VPlayerGetSteamId()));
			}
		}
	}

	override void GetPlayerStatsGroup(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Server)
		{
			Param1<ref array<string>> data;
			if(!ctx.Read(data)) 
				return;

			array<string> UIDS = data.param1;
			int pcount = UIDS.Count();

			if (sender != null)
			{
				if (!GetPermissionManager().VerifyPermission(sender.GetPlainId(), "MenuPlayerManager", "", false)) 
					return;
				
				foreach(string id : UIDS)
				{
					PlayerBase playerMan = GetPermissionManager().GetPlayerBaseByID(id);
					if (playerMan != null && sender != null)
					{
						string inHandsName = "";
				
						if(playerMan.GetHumanInventory() != null && playerMan.GetHumanInventory().GetEntityInHands() != null)
							inHandsName = playerMan.GetHumanInventory().GetEntityInHands().GetDisplayName();
						
						if(inHandsName == "")
							inHandsName = "None";
						
						map<string,string> dataMap = new map<string,string>;
						dataMap.Insert("Guid",playerMan.VPlayerGetHashedId());
						dataMap.Insert("Name",playerMan.VPlayerGetName());
						dataMap.Insert("Blood",playerMan.GetHealth("GlobalHealth", "Blood").ToString());
						dataMap.Insert("Health",playerMan.GetHealth("GlobalHealth", "").ToString());
						dataMap.Insert("Shock",playerMan.GetHealth("GlobalHealth", "Shock").ToString());
						dataMap.Insert("Water",PlayerBase.Cast(playerMan).GetStatWater().Get().ToString());
						dataMap.Insert("Energy",PlayerBase.Cast(playerMan).GetStatEnergy().Get().ToString());
						dataMap.Insert("ZenSleep",PlayerBase.Cast(playerMan).GetStatZenFatigue().Get().ToString());
						dataMap.Insert("Weapon",inHandsName);
						dataMap.Insert("SteamID",playerMan.VPlayerGetSteamId());
						dataMap.Insert("UserGroup",GetPermissionManager().GetPlayerUserGrpNameByID(id));
						
						PlayerStatsData stats = new PlayerStatsData(dataMap);
						GetRPCManager().VSendRPC( "RPC_MenuPlayerManager", "HandlePlayerStats", new Param1<ref PlayerStatsData>(stats), true, sender);
					}
				}

				if (pcount > 0)
					GetWebHooksManager().PostData(AdminActivityMessage, new AdminActivityMessage(sender.GetPlainId(), sender.GetName(), "[PlayerManager] Requested Stats for player(s): " + pcount));
			}
		}
	}	
}