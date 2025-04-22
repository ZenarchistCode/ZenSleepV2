class ZenSleep_Anesthetic extends Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionInjectZenSleepSyringeTarget);
		AddAction(ActionInjectZenSleepSyringeSelf);
	}
	
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;

		player.GetZenSleepManager().OnModifierActivate(GetType());
		player.GetZenSleepManager().OnModifierDeactivate(GetType());
	}
}

class ZenSleep_Amphetamine extends Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionInjectZenSleepSyringeTarget);
		AddAction(ActionInjectZenSleepSyringeSelf);
	}
	
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;

		player.GetZenSleepManager().OnModifierActivate(GetType());
		player.GetZenSleepManager().OnModifierDeactivate(GetType());
	}
}