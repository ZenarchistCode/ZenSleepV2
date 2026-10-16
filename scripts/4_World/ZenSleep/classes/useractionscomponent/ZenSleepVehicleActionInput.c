class ZenSleepVehicleActionInput : DefaultActionInput
{
	void ZenSleepVehicleActionInput(PlayerBase player)
	{
		// Resolve the input registered by ZenSleep's CfgMods declaration.
		m_input = null;
		if (GetUApi().GetInputByName("UAZenSleepVehicle"))
			SetInput("UAZenSleepVehicle");
		else
			Print("[ZenSleep] Missing UAZenSleepVehicle input. Check that the loaded CfgMods declares ZenSleep/data/inputs.xml.");
		m_InputType = ActionInputType.AIT_SINGLE;
		m_Priority = 100;
	}

	override UAInput GetUAInput()
	{
		if (!m_input)
			return null;
		return super.GetUAInput();
	}

	override void Update()
	{
		if (!m_input)
		{
			m_Active = false;
			m_JustActivate = false;
			return;
		}

		super.Update();
		ZenSleepManager sleep = m_Player.GetZenSleepManager();
		// Trace only an actual key press; action availability is queried every frame.
		if (GetZenSleepConfig().DebugMode && GetUAInput().LocalPress() && sleep)
		{
			sleep.DebugVehicleSleep("Sleep hotkey pressed");
			if (sleep.IsVehicleSleepInputBlocked() || sleep.IsVehicleSleepRequested())
				sleep.DebugVehicleSleep("Hotkey is waking/cancelling vehicle sleep; release is consumed.");
			else if (sleep.CanSleepInVehicle(true))
				sleep.DebugVehicleSleep("Seat checks passed; waiting for native action execution.");
		}
		if (sleep && sleep.IsVehicleSleepInputBlocked())
		{
			// Consume the release of a key that just woke the player.
			m_Active = false;
			m_JustActivate = false;
			if (GetUAInput().LocalValue() == 0)
				sleep.ReleaseVehicleSleepInput();
		}
	}

	override void UpdatePossibleActions(PlayerBase player, ActionTarget target, ItemBase item, int action_condition_mask)
	{
		m_SelectAction = null;
		m_MainItem = null;
		m_Target = null;
		if (!m_input || !player || !player.IsInVehicle())
			return;

		array<ActionBase_Basic> actions;
		player.GetActions(this.Type(), actions);
		if (!actions)
			return;

		foreach (ActionBase_Basic candidate : actions)
		{
			ActionBase action = ActionBase.Cast(candidate);
			if (action && action.Can(player, null, null, action_condition_mask))
			{
				m_SelectAction = action;
				return;
			}
		}
	}

	override ActionBase GetAction()
	{
		return m_SelectAction;
	}
}
