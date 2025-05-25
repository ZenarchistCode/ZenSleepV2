modded class MenuPlayerManager
{
	SliderWidget m_SliderZenSleep;
	ButtonWidget m_BtnApplyZenSleep;

	//! Hijack the OnCreate function to load my custom layout file if enabled in the server config.
	//  Allows setting ZenSleep Fatigue as a player stat.
	override void OnCreateZenSleep(Widget RootW)
	{
		super.OnCreateZenSleep(RootW);
		
		M_SUB_WIDGET  = CreateWidgets("ZenSleep/data/gui/layout/ZenSleepVPPAT.layout");
		M_SUB_WIDGET.SetHandler(this);
		m_TitlePanel  = Widget.Cast( M_SUB_WIDGET.FindAnyWidget( "Header") );
		m_closeButton = ButtonWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "BtnClose") );
		
		m_GridPlayerInfo   	   = GridSpacerWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "GridPlayerInfo") );
		m_GridPlayerList 	   = GridSpacerWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "GridPlayerList") );
		m_PlayerList 	   	   = ScrollWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "PlayerList") );
		m_PlayerInfoScroll 	   = ScrollWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "PlayerInfoScroll") );
		m_SearchInputBox 	   = EditBoxWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "SearchInputBox") );
		m_txtPlayerCount 	   = TextWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "txtPlayerCount") );
		m_txtPlayerCountSelected = TextWidget.Cast(M_SUB_WIDGET.FindAnyWidget("txtPlayerCountSelected"));
		
		m_SelectAllPlayers 	   = CheckBoxWidget.Cast( M_SUB_WIDGET.FindAnyWidget( "ChkSelectAllPlayers") );
		m_BtnRefreshPlayerList = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "BtnRefreshPlayerList"));
		m_btnFnAddPlayersToGrp = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "btnFnAddPlayersToGrp"));
		GetVPPUIManager().HookConfirmationDialog(m_btnFnAddPlayersToGrp, M_SUB_WIDGET,this,"FinishPlayerSelect", DIAGTYPE.DIAG_YESNO, "#VSTR_TOOLTIP_TITLE_NOTICE", "#VSTR_TOOLTIP_WRN_ADDPLAYERTOGRP");
		
		//Stats Sliders
		m_SliderHealth  = SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderHealth"));
		m_SliderBlood	= SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderBlood"));
		m_SliderShock	= SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderShock"));
		m_SliderWater	= SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderWater"));
		m_SliderEnergy	= SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderEnergy"));
		m_SliderZenSleep	= SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderZenFatigue"));
		m_SliderTemperature = SliderWidget.Cast(M_SUB_WIDGET.FindAnyWidget("SliderTemperature"));
		
		m_BtnApplyHealth  = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyHealth"));
		m_BtnApplyBlood   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyBlood"));
		m_BtnApplyShock   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyShock"));
		m_BtnApplyWater   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyWater"));
		m_BtnApplyEnergy  = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyEnergy"));
		m_BtnApplyTemperature  = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyTemperature"));
		m_BtnApplyZenSleep  = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("BtnApplyZenFatigue"));
		//-------------
		
		//Action Buttons
		m_ActionMakeVomit 	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionMakeVomit"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionMakeVomit, M_SUB_WIDGET, this, "VomiteDiagResult", DIAGTYPE.DIAG_OK_CANCEL_INPUT, "#VSTR_TOOLTIP_TITLE_VOMIT", "#VSTR_TOOLTIP_VOMIT");

		m_ActionHeal  		   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionHeal"));
		m_ActionKickPlayer     = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionKickPlayer"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionKickPlayer, M_SUB_WIDGET,this,"KickPlayer", DIAGTYPE.DIAG_OK_CANCEL_INPUT, "#VSTR_TOOLTIP_TITLE_KICK", "#VSTR_TOOLTIP_KICK_REASON", true);
		
		m_ActionKill	   	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionKill"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionKill, M_SUB_WIDGET,this,"KillSelectedPlayers", DIAGTYPE.DIAG_YESNO, "#VSTR_TOOLTIP_TITLE_KILL_PLAYERS", "#VSTR_TOOLTIP_KILL_PLAYERS");
		
		m_ActionSendMessage    = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionSendMessage"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionSendMessage, M_SUB_WIDGET,this,"SendMessageToPlayer", DIAGTYPE.DIAG_OK_CANCEL_INPUT, "#VSTR_TOOLTIP_TITLE_SEND_MSG", "#VSTR_TOOLTIP_SEND_MSG", true);
		
		m_ActionBanPlayer  	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionBanPlayer"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionBanPlayer, M_SUB_WIDGET,this,"BanPlayer", DIAGTYPE.DIAG_YESNO, "#VSTR_TOOLTIP_TITLE_BAN_PLAYER", "#VSTR_TOOLTIP_BAN_PLAYER");
		
		m_ActionReturnPlayer  	= ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget("ActionReturnPlayer"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionReturnPlayer, M_SUB_WIDGET,this,"ReturnPlayer", DIAGTYPE.DIAG_YESNO, "#VSTR_TOOLTIP_TITLE_RET_PLAYER", "#VSTR_TOOLTIP_RET_PLAYER");
		
		m_ActionTpToMe  	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionTpToMe"));
		m_ActionTpMeTo  	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionTpMeTo"));
		m_ActionSpectate  	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionSpectate"));
		
		m_ActionGiveGodmode    = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionGiveGodmode"));
		m_ActionUnlimitedAmmo  = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionUnlimitedAmmo"));
		m_ActionInvisible  	   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionInvisible"));
		m_ActionFreezePlayer   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionFreezePlayer"));

		m_ActionScalePlayer   = ButtonWidget.Cast(M_SUB_WIDGET.FindAnyWidget( "ActionScalePlayer"));
		GetVPPUIManager().HookConfirmationDialog(m_ActionScalePlayer, M_SUB_WIDGET, this, "PlayerScaleDiag", DIAGTYPE.DIAG_OK_CANCEL_INPUT, "Set Scale", "Insert value to change to, between 0.01 and 100.0 (Some specific values will result in the player to be frozen and uncontrollable. To avoid, use rounded numbers)");
		//--------------
		
		//init first "page"
		ResetPages();
		UpdateEntries();
		m_Init = true;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (m_BtnApplyZenSleep)
		{
			m_BtnApplyZenSleep.Enable(GetSelectedPlayers().Count() >= 1);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (!m_BtnApplyZenSleep)
			return false;

		switch (w)
		{
			case m_BtnApplyZenSleep:
			GetVPPUIManager().DisplayNotification(string.Format("Applying new ZenSleep Fatigue level on" + " (%1) player(s)",GetSelectedPlayers().Count().ToString()));
			UpdateStat("ZenSleep");
			break;
		}

		return false;
	}

	override private void UpdateStat(string statType)
	{
		if (m_BtnApplyZenSleep)
		{
			array<ref VPPPlayerEntry> selectedPlayers = GetSelectedPlayers();
			if (selectedPlayers == null || selectedPlayers.Count() < 1) return;
		
			foreach (VPPPlayerEntry entry : selectedPlayers)
			{
				float stateNewValue;
				switch (statType)
				{
					case "ZenSleep":
					stateNewValue = m_SliderZenSleep.GetCurrent();
					GetRPCManager().VSendRPC("RPC_PlayerManager", "SetPlayerStats", new Param3<float,string,string>(stateNewValue,entry.GetID(),statType), true); //stat level, player id, stat type
					return;
				}
			}			
		}

		super.UpdateStat(statType);
	}

	override void HandlePlayerStats( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target )
	{
		if (!m_BtnApplyZenSleep)
		{
			super.HandlePlayerStats(type, ctx, sender, target);
			return;
		}

		Param1<ref PlayerStatsData> data;
		if(!ctx.Read(data)) return;
		
		PlayerStatsData statsData = data.param1;
		if( type == CallType.Client )
		{
			m_PlayerStats.Insert(new VPPPlayerStats(m_GridPlayerInfo, statsData.GetMap()));
			
			array<ref VPPPlayerEntry> selectedPlayers = GetSelectedPlayers();
			if (selectedPlayers.Count() == 1)
			{
				foreach(VPPPlayerStats stats : m_PlayerStats)
				{
					if (stats.GetID() == selectedPlayers.Get(0).GetID())
					{
						m_SliderBlood.SetCurrent(stats.GetStatValue("Blood").ToFloat());
						m_SliderHealth.SetCurrent(stats.GetStatValue("Health").ToFloat());
						m_SliderShock.SetCurrent(stats.GetStatValue("Shock").ToFloat());
						m_SliderWater.SetCurrent(stats.GetStatValue("Water").ToFloat());
						m_SliderEnergy.SetCurrent(stats.GetStatValue("Energy").ToFloat());
						m_SliderZenSleep.SetCurrent(stats.GetStatValue("ZenSleep").ToFloat());
					}
				}
			}
		}
	}
}