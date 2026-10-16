// Keep the existing PCO slot and serialized float. With the override enabled it is
// a scaled view of Terje's persisted rest record, so medicines/admin tools cannot
// update a second, disconnected fatigue bar. Client HUD values use Zen's own RPC.
class ZenSleepFatigueStat : PlayerStat<float>
{
	#ifdef TERJE_MEDICINE_MOD
	protected TerjePlayerStats GetZenTerjeStats()
	{
		if (!g_Game.IsDedicatedServer() || !ZenSleepCompatibility.OverridesTerje())
			return null;

		PlayerBase player = PlayerBase.Cast(m_Player);
		if (player)
			return player.GetTerjeStats();

		return null;
	}

	override float Get()
	{
		TerjePlayerStats stats = GetZenTerjeStats();
		if (stats)
			return stats.GetSleepingValue() * GetMax() / TerjeMedicineConstants.SLEEPING_MAX_VALUE;

		return super.Get();
	}

	override void Set(float value, string system = "")
	{
		super.Set(value, system);
		TerjePlayerStats stats = GetZenTerjeStats();
		if (stats)
			stats.SetSleepingValue(m_Value * TerjeMedicineConstants.SLEEPING_MAX_VALUE / GetMax());
	}

	override void Add(float value, string system = "")
	{
		Set(Get() + value, system);
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		m_Value = Get();
		super.OnStoreSave(ctx);
	}

	override void OnAfterStoreLoad()
	{
		// Terje loads its own record later in the player load chain. Do not replace it
		// with the PCO cache (or a fresh-spawn default) during this callback.
		if (!ZenSleepCompatibility.OverridesTerje())
			super.OnAfterStoreLoad();
	}
	#endif
}

#ifdef TERJE_MEDICINE_MOD
modded class TerjePlayerStats
{
	// Scoped to applying one configured item; never persisted or used for other medical effects.
	bool m_ZenSleepSuppressConsumable;

	override void SetSleepingValue(float value)
	{
		if (!ZenSleepCompatibility.OverridesTerje())
		{
			super.SetSleepingValue(value);
			return;
		}

		SetFloatValue(m_TerjeMed_SleepingValue, Math.Clamp(value, 0, TerjeMedicineConstants.SLEEPING_MAX_VALUE));
	}

	override void AddSleepingIncrement(float value, float time)
	{
		if (ZenSleepCompatibility.OverridesTerje() && m_ZenSleepSuppressConsumable)
			return;

		if (!ZenSleepCompatibility.OverridesTerje())
		{
			super.AddSleepingIncrement(value, time);
			return;
		}

		float currentValue, currentTime;
		GetSleepingIncrement(currentValue, currentTime);
		if (value > currentValue)
			SetSleepingIncrement(value, time);
		else if (value == currentValue)
			SetSleepingIncrement(currentValue, currentTime + time);
	}

	override void AddSleepingDecrement(float value, float time)
	{
		if (ZenSleepCompatibility.OverridesTerje() && m_ZenSleepSuppressConsumable)
			return;

		if (!ZenSleepCompatibility.OverridesTerje())
		{
			super.AddSleepingDecrement(value, time);
			return;
		}

		float currentValue, currentTime;
		GetSleepingDecrement(currentValue, currentTime);
		if (value > currentValue)
			SetSleepingDecrement(value, time);
		else if (value == currentValue)
			SetSleepingDecrement(currentValue, currentTime + time);
	}
}

#endif
