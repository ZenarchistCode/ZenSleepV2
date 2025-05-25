modded class IngameHud
{
	protected bool m_HasInitZenSleep = false;
	protected ref ImageWidget m_ZenSleepIcon; 

	override void InitBadgesAndNotifiers()
	{
		super.InitBadgesAndNotifiers();

		RegisterZenSleepIcon();
	}

	protected void RegisterZenSleepIcon()
	{
		// Widget arrays are cleared on respawn so re-set my modded values on creation AND after respawn.
		if (m_ZenSleepIcon)
		{
			m_StatesWidgets.Set(ZenSleepEnums.NTFKEY_FATIGUE, m_ZenSleepIcon);
			m_StatesWidgetNames.Set(ZenSleepEnums.NTFKEY_FATIGUE, "ZenSleep");
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		// Only need to create the icon once, this method also gets re-called when a player respawns.
		if (!m_HasInitZenSleep)
		{
			Widget sleepHud = GetGame().GetWorkspace().CreateWidgets("ZenSleep/data/gui/layout/ZenSleepHud.layout");
			sleepHud.Show(false);

			Widget sleepPanel = sleepHud.FindAnyWidget("ZenSleepPanel");
			Widget notifierWidget = sleepPanel.FindAnyWidget("ZenSleep");
			sleepPanel.RemoveChild(notifierWidget);
		
			Widget defaultBadgesSpacer = m_HudPanelWidget.FindAnyWidget("BadgesSpacer");
			Widget defaultBadgesPanel = m_HudPanelWidget.FindAnyWidget("BadgesPanel");
		
			float widgetOffsetX;
			float widgetWidth;
		
			float x, y, x2;
			m_Notifiers.FindAnyWidget("Thirsty").GetPos(x, y);
			m_Notifiers.FindAnyWidget("Hungry").GetPos(x2, y);
			widgetWidth = x - x2;
			widgetOffsetX = x + widgetWidth;
		
			m_Notifiers.GetSize(x, y);
			m_Notifiers.SetSize(x + widgetWidth, y);
		
			m_Notifiers.AddChild(notifierWidget);
			notifierWidget.SetPos(widgetOffsetX, 0);
			notifierWidget.Show(true);
		
			defaultBadgesSpacer.GetPos(x, y);
			defaultBadgesSpacer.SetPos(x + widgetWidth, y);
		
			defaultBadgesPanel.GetPos(x, y);
			defaultBadgesPanel.SetPos(x + widgetWidth, y);
		
			m_ZenSleepIcon = ImageWidget.Cast(notifierWidget.FindAnyWidget("IconZenSleep")); 

			for (int i = 0; i < 5; i++)
			{
				m_ZenSleepIcon.LoadImageFile(i, "ZenSleep/data/gui/icons/sleeping/icon" + i + ".paa");
			}

			RegisterZenSleepIcon();

			m_HasInitZenSleep = true; // Don't need to load this badge widget again.
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			int tendency	= player.GetZenSleepManager().GetFatigueStatTendencyDisplay();
			int state		= player.GetZenSleepManager().GetFatigueStatState();

			DisplayNotifier(ZenSleepEnums.NTFKEY_FATIGUE, tendency, state);
		}
	}
}