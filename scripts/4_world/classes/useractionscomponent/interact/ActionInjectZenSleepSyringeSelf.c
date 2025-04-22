class ActionInjectZenSleepSyringeSelf: ActionInjectSelf
{
	override void ApplyModifiers(ActionData action_data)
	{
		action_data.m_MainItem.OnApply(action_data.m_Player);
	}
}