#ifdef TERJE_MEDICINE_MOD
modded class ActionUseTerjeSleepingBag
{
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		if (ZenSleepCompatibility.OverridesTerje() && action_data.m_Player.GetZenSleepManager())
			action_data.m_Player.GetZenSleepManager().SetBed(action_data.m_Target.GetObject());
	}
}
#endif
