modded class MissionGameplay
{
	protected int m_ZenVehicleSleepWakeKey = -1;
	protected ref array<int> m_ZenSleepInventoryInputs = {
		UAUIMenu, UAUIBack, UAUIRotateInventory, UAUISplit, UAUICombine, UAUIInspectItem,
		UAUIFastEquip, UAUIFastTransferItem, UAUIFastTransferToVicinity, UAUIDragNDrop,
		UAUIPutInHandsFromInventory, UAUIPutInHandsFromVicinity, UAUIExpandCollapseContainer,
		UAUIInventoryContainerUp, UAUIInventoryContainerDown, UAUIInventoryTabLeft, UAUIInventoryTabRight,
		UAUILeftInventory, UAUIRightInventory, UAUIUpInventory, UAUIDownInventory, UAUIQuickbarRadialInventoryOpen
	};

	override void OnKeyPress(int key)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && player.GetZenSleepManager() && player.GetZenSleepManager().IsVehicleSleepRequested())
			m_ZenVehicleSleepWakeKey = key;
		super.OnKeyPress(key);
	}

	protected bool ZenIsVehicleSleepInventoryInput()
	{
		// Use current action bindings, including a rebound inventory key.
		Input input = g_Game.GetInput();
		if (input.LocalPress_ID(UAGear, false))
			return true;
		if (!g_Game.GetUIManager().IsMenuOpen(MENU_INVENTORY))
			return false;
		foreach (int action : m_ZenSleepInventoryInputs)
		{
			if (input.LocalPress_ID(action, false))
				return true;
		}
		return false;
	}

	override void OnUpdate(float timeslice)
	{
		// Resolve raw key events once input actions are updated, before vanilla can close inventory.
		int wakeKey = m_ZenVehicleSleepWakeKey;
		m_ZenVehicleSleepWakeKey = -1;
		bool inventoryInput = wakeKey >= 0 && ZenIsVehicleSleepInventoryInput();
		super.OnUpdate(timeslice);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player || !player.GetZenSleepManager())
			return;

		ZenSleepManager sleep = player.GetZenSleepManager();
		if (!sleep.IsVehicleSleepRequested())
			return;

		if (wakeKey >= 0 && !inventoryInput)
			sleep.RequestVehicleWake("keyboard key=" + wakeKey);
		else if (!sleep.CanSleepInVehicle(true))
			sleep.RequestVehicleWake("vehicle eligibility lost");
	}
}
