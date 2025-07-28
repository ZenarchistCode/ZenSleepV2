#ifdef JM_COT
modded class JMPlayerForm
{
	UIActionSlider m_ZenSleep;
	bool m_ZenSleepUpdated;

	override private Widget InitActionWidgetsStats(Widget actionsParent)
	{
		Widget parent = super.InitActionWidgetsStats(actionsParent);

		if (!m_ZenSleep && m_Health)
		{
			Widget parentWidgie = m_Health.GetLayoutRoot().GetParent();
			m_ZenSleep = UIActionManager.CreateSlider(parentWidgie, "ZenSleep:", 0, ZenSleepConstants.SL_FATIGUE_MAX, this, "Click_SetZenSleep");
			m_ZenSleep.SetSliderWidth(0.5);
		}

		return parent;
	}

	override void RefreshStats(bool force = false)
	{
		super.RefreshStats(force);

		if (!m_SelectedInstance)
			return;
		
		if (GetGame().IsClient() && m_SelectedInstance.GetDataLastUpdatedTime() < m_LastChangeTime)
			return;

		if (force)
		{
			m_ZenSleepUpdated = false;
		}

		if (m_ZenSleep && !m_ZenSleepUpdated)
			m_ZenSleep.SetCurrent(m_SelectedInstance.GetZenSleep());

		if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_HIGH)
		{
			m_ZenSleep.SetColor(Colors.COLOR_PRISTINE);
			m_ZenSleep.SetAlpha(1.0);
		}
		else if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_NORMAL)
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 220, 220));
		}
		else if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_LOW)
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 220, 0));
		}
		else
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 0, 0));
		}
	}

	void Click_SetZenSleep(UIEvent eid, UIActionBase action)
	{
		if (eid != UIEvent.CHANGE)
			return;

		UpdateLastChangeTime();

		if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_HIGH)
		{
			m_ZenSleep.SetColor(Colors.COLOR_PRISTINE);
			m_ZenSleep.SetAlpha(1.0);
		}
		else if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_NORMAL)
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 220, 220));
		}
		else if (m_ZenSleep.GetCurrent() >= ZenSleepConstants.SL_FATIGUE_LOW)
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 220, 0));
		}
		else
		{
			m_ZenSleep.SetColor(ARGB(255, 220, 0, 0));
		}

		m_ZenSleepUpdated = true;
	}

	override void Click_ApplyStats(UIEvent eid, UIActionBase action)
	{
		super.Click_ApplyStats(eid, action);

		if (eid != UIEvent.CLICK)
			return;

		if (m_ZenSleepUpdated)
		{
			m_ZenSleepUpdated = false;

			if (m_ZenSleep)
			{
				m_Module.SetZenSleep(m_ZenSleep.GetCurrent(), JM_GetSelected().GetPlayersOrSelf());
			}
		}
	}
}
#endif