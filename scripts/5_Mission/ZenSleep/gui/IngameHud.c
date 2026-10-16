modded class IngameHud
{
	protected bool m_HasInitZenSleep = false;
	protected ref ImageWidget m_ZenSleepIcon; 
	protected int m_ZenSleepNotifierKey = ZenSleepEnums.NTFKEY_FATIGUE;
	protected string m_ZenSleepWidgetName = "ZenSleep";

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
			m_StatesWidgets.Set(m_ZenSleepNotifierKey, m_ZenSleepIcon);
			m_StatesWidgetNames.Set(m_ZenSleepNotifierKey, m_ZenSleepWidgetName);
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		if (!ZenSleepCompatibility.IsZenSleepEnabled())
			return;

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (!m_HasInitZenSleep)
		{
			AddZenSleepWidget();
		}
		
		UpdateZenSleepWidget();
	}

	// Only need to create the icon once, this method also gets re-called when a player respawns.
	void AddZenSleepWidget()
	{
		#ifdef TERJE_MEDICINE_MOD
		if (ZenSleepCompatibility.OverridesTerje())
		{
			UseTerjeSleepWidget();
			return;
		}
		#endif

		#ifdef TERJE_CORE_MOD
		// Terje allocates notifier IDs/positions dynamically. Use its allocator so
		// the Zen moon never overlaps Mind/Radiation or replaces their widget ID.
		m_ZenSleepNotifierKey = RegisterTerjeNotifierWidget("ZenSleep/data/gui/icons/sleeping/icon{ID}.paa", "ZenSleep");
		m_ZenSleepIcon = m_StatesWidgets.Get(m_ZenSleepNotifierKey);
		m_HasInitZenSleep = true;
		return;
		#endif

		Widget sleepHud = g_Game.GetWorkspace().CreateWidgets("ZenSleep/data/gui/layout/ZenSleepHud.layout");
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

	void UpdateZenSleepWidget()
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player || !player.GetZenSleepManager())
			return;

		int tendency	= player.GetZenSleepManager().GetFatigueStatTendencyDisplay();
		int state		= player.GetZenSleepManager().GetFatigueStatState();
		DisplayNotifier(m_ZenSleepNotifierKey, tendency, state);
	}

	#ifdef TERJE_MEDICINE_MOD
	protected void UseTerjeSleepWidget()
	{
		Widget notifier = m_HudPanelWidget.FindAnyWidget("TerjeSleeping");
		bool needsSlot = !notifier;

		// Reuse Terje's slot, ID and arrows instead of hiding it and appending another.
		// Calling super avoids re-entering our registration override below.
		m_ZenSleepNotifierKey = super.RegisterTerjeNotifierWidget("ZenSleep/data/gui/icons/sleeping/icon{ID}.paa", "TerjeSleeping");
		m_ZenSleepIcon = m_StatesWidgets.Get(m_ZenSleepNotifierKey);
		if (!m_ZenSleepIcon)
			return;

		m_ZenSleepWidgetName = "TerjeSleeping";
		notifier = m_HudPanelWidget.FindAnyWidget(m_ZenSleepWidgetName);

		// When native sleeping is disabled, Mind may already exist when Zen's config
		// arrives. Insert the new slot next to Thirsty, where Terje normally puts sleep.
		if (needsSlot)
			PositionTerjeSleepWidget(notifier);

		// Terje's allocator does not reload textures when it finds an existing widget.
		for (int i = 0; i < 5; i++)
			m_ZenSleepIcon.LoadImageFile(i, "ZenSleep/data/gui/icons/sleeping/icon" + i + ".paa");

		notifier.Show(true);
		RegisterZenSleepIcon();
		TERJE_NOTIFIER_SLEEPING = -1; // Only Zen updates the shared widget's level/arrows.
		m_HasInitZenSleep = true;
	}

	protected void PositionTerjeSleepWidget(Widget notifier)
	{
		Widget notifiers = m_HudPanelWidget.FindAnyWidget("NotifiersPanel");
		Widget thirsty = notifiers.FindAnyWidget("Thirsty");
		Widget hungry = notifiers.FindAnyWidget("Hungry");
		float x, y, hungryX, hungryY;
		thirsty.GetPos(x, y);
		hungry.GetPos(hungryX, hungryY);
		float spacing = x - hungryX;
		float sleepX = x + spacing;

		// The allocator has already expanded the row and moved the badge separator.
		// Move later indicators one slot along without expanding anything a second time.
		Widget child = notifiers.GetChildren();
		while (child)
		{
			if (child != notifier)
			{
				float childX, childY;
				child.GetPos(childX, childY);
				if (childX >= sleepX)
					child.SetPos(childX + spacing, childY);
			}
			child = child.GetSibling();
		}
		notifier.SetPos(sleepX, y);
	}

	override protected int RegisterTerjeNotifierWidget(string iconPath, string name)
	{
		if (name == "TerjeSleeping" && ZenSleepCompatibility.OverridesTerje())
		{
			UseTerjeSleepWidget();
			return -1;
		}

		return super.RegisterTerjeNotifierWidget(iconPath, name);
	}
	#endif
}
