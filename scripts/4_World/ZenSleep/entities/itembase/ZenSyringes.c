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
		super.OnApply(player);

		if (!player)
			return;

		#ifdef TERJE_MEDICINE_MOD
		if (player.GetTerjeStats())
		{
			ApplyTerjeConsumableEffects(player, 1);
			// The compatibility item hook applies Zen's configured dose, retaining other Terje effects.
			if (!ZenSleepCompatibility.OverridesTerje())
				player.GetTerjeStats().SetSleepingValue(0);
			return;
		}
		#endif

		player.GetZenSleepManager().OnModifierActivate(GetType());
		player.GetZenSleepManager().OnModifierDeactivate(GetType());
	}

	#ifdef TERJE_MEDICINE_MOD
	override int TerjeOverrideDescriptionByConsumableEffects()
	{
		return 2;
	}
	#endif
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
		super.OnApply(player);

		if (!player)
			return;

		#ifdef TERJE_MEDICINE_MOD
		if (player.GetTerjeStats())
		{
			ApplyTerjeConsumableEffects(player, 1);
			if (!ZenSleepCompatibility.OverridesTerje())
				player.GetTerjeStats().AddSleepingIncrement(46, 300);
			return;
		}
		#endif

		player.GetZenSleepManager().OnModifierActivate(GetType());
		player.GetZenSleepManager().OnModifierDeactivate(GetType());
	}

	#ifdef TERJE_MEDICINE_MOD
	override int TerjeOverrideDescriptionByConsumableEffects()
	{
		return 2;
	}
	#endif
}
