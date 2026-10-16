class ZenSleepVehicleActionData : ActionData
{
	int m_ZenSleepRequestID;
	bool m_ZenSleepActionStarted;
}

class ZenSleepVehicleActionReceiveData : ActionReciveData
{
	int m_ZenSleepRequestID;
}

// Instant and unanimated: the player remains seated in the vehicle command.
class ActionZenSleepInVehicle : ActionBase
{
	void ActionZenSleepInVehicle()
	{
		m_Text = "#STR_ZenSleep_VehicleSleepText";
	}

	override typename GetInputType() { return ZenSleepVehicleActionInput; }
	override bool HasTarget() { return false; }
	override bool UseMainItem() { return false; }
	override bool IsInstant() { return true; }
	override bool CanBeUsedInVehicle() { return true; }

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTNone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ZenSleepManager sleep = player.GetZenSleepManager();
		// Environmental restrictions are checked on execution so the normal HUD
		// denial message is sent instead of silently hiding an otherwise valid action.
		return sleep && !sleep.IsVehicleSleepRequested() && sleep.CanSleepInVehicle(g_Game.IsServer());
	}

	override ActionData CreateActionData()
	{
		return new ZenSleepVehicleActionData();
	}

	override bool Post_SetupAction(ActionData action_data)
	{
		if (!super.Post_SetupAction(action_data))
		{
			action_data.m_Player.GetZenSleepManager().DebugVehicleSleep("Action blocked: native action setup failed");
			return false;
		}

		if (!g_Game.IsDedicatedServer())
		{
			ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
			data.m_ZenSleepRequestID = action_data.m_Player.GetZenSleepManager().PrepareVehicleSleepRequest();
		}
		return true;
	}

	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
		ctx.Write(data.m_ZenSleepRequestID);
	}

	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		ZenSleepVehicleActionReceiveData data = new ZenSleepVehicleActionReceiveData();
		action_recive_data = data;
		if (!super.ReadFromContext(ctx, action_recive_data))
			return false;
		return ctx.Read(data.m_ZenSleepRequestID) && data.m_ZenSleepRequestID > 0;
	}

	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		ZenSleepVehicleActionReceiveData received = ZenSleepVehicleActionReceiveData.Cast(action_recive_data);
		ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
		data.m_ZenSleepRequestID = received.m_ZenSleepRequestID;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
		action_data.m_Player.GetZenSleepManager().HandleVehicleSleepRequest(true, data.m_ZenSleepRequestID);
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
		data.m_ZenSleepActionStarted = true;
		action_data.m_Player.GetZenSleepManager().DebugVehicleSleep("Native action acknowledged by server; request=" + data.m_ZenSleepRequestID);
	}

	override void ActionCleanup(ActionData action_data)
	{
		super.ActionCleanup(action_data);
		ZenSleepVehicleActionData data = ZenSleepVehicleActionData.Cast(action_data);
		if (g_Game.IsClient() && !data.m_ZenSleepActionStarted)
			action_data.m_Player.GetZenSleepManager().CancelVehicleSleepRequest(data.m_ZenSleepRequestID);
	}
}
