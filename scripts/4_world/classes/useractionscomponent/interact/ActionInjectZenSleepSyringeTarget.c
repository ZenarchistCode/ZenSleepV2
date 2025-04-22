class ActionInjectZenSleepSyringeTarget: ActionInjectTarget
{
	override void ApplyModifiers(ActionData action_data)
	{
		action_data.m_MainItem.OnApply(PlayerBase.Cast(action_data.m_Target.GetObject()));
	}
}