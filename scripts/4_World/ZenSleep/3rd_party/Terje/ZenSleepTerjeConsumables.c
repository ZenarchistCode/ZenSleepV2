#ifdef TERJE_MEDICINE_MOD
modded class ItemBase
{
	override void ApplyTerjeConsumableEffects(PlayerBase player, float amount)
	{
		if (!ZenSleepCompatibility.OverridesTerje() || !g_Game.IsDedicatedServer() || !player || !player.GetTerjeStats() || !player.GetZenSleepManager() || !player.GetZenSleepManager().HasConfiguredItemEffect(this))
		{
			super.ApplyTerjeConsumableEffects(player, amount);
			return;
		}

		// Keep overdose, skills and all non-sleep medicine effects. Suppress only
		// this item's competing sleep dose; do not erase previously active medicines.
		TerjePlayerStats stats = player.GetTerjeStats();
		bool wasSuppressed = stats.m_ZenSleepSuppressConsumable;
		stats.m_ZenSleepSuppressConsumable = true;
		super.ApplyTerjeConsumableEffects(player, amount);
		stats.m_ZenSleepSuppressConsumable = wasSuppressed;

		// Edibles already apply Zen's normalized portion in PlayerBase.Consume.
		if (!Edible_Base.Cast(this))
			player.GetZenSleepManager().ApplyConfiguredItemEffects(this, amount);
	}
}
#endif
