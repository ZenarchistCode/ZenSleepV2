// Registers my new custom fatigue stat alongside vanilla stats. Not sure if this is ideal,
// I tried to copy pure vanilla mechanics for adding this stat but some mechanics 
// don't allow for this due to hard-coded attributes (like Notifiers arrays) so maybe I'm just
// better off simply moving this stat to a regular PlayerBase.c float value. But it's working for 
// now so I'll leave it as-is until I have a good reason to refactor the fatigue stat values tracking.
modded class PlayerStatsPCO_current
{
	protected int m_ZenFatigueIndex = -1;

	int GetZenStatFatigueIndex()
	{
		return m_ZenFatigueIndex;
	}

	override void Init()
	{
		super.Init();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
			return;
		#endif

		// For Namalsk compatibility. Registering multiple modded stats can create conflicting stat indexes, so just append the last registered stat.
		m_ZenFatigueIndex = m_PlayerStats.Count();
		RegisterStat(m_ZenFatigueIndex, new PlayerStat<float>(0, ZenSleepConstants.SL_FATIGUE_MAX, ZenSleepConstants.SL_FATIGUE_MAX, "ZenSleep", EPSstatsFlags.EMPTY));
	}
}

modded class PlayerStatsPCO_Base
{
	override void RegisterStat(int id, PlayerStatBase stat)
	{
		super.RegisterStat(id, stat);
		
		#ifdef SERVER
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
			return;
		#endif

		if (GetZenSleepConfig().DebugMode)
		{
			Print("[ZenSleepDebug] Registered stat: " + stat.GetLabel() + " with id " + id);
		}
		#endif
	}
}