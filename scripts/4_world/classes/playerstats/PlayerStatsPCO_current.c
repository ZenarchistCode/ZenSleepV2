// Registers my new custom fatigue stat alongside vanilla stats. Not sure if this is ideal,
// I tried to copy pure vanilla mechanics for adding this stat but some mechanics 
// don't allow for this due to hard-coded attributes (like Notifiers arrays) so maybe I'm just
// better off simply moving this stat to a regular PlayerBase.c float value. But it's working for 
// now so I'll leave it as-is until I have a good reason to refactor the fatigue stat values tracking.
modded class PlayerStatsPCO_current
{
	override void Init()
	{
		super.Init();

		RegisterStat(ZenSleepEnums.STAT_FATIGUE, new PlayerStat<float>(0, ZenSleepConstants.SL_FATIGUE_MAX, ZenSleepConstants.SL_FATIGUE_MAX, "ZenSleep", EPSstatsFlags.EMPTY));
	}
}