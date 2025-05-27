class ActionZenUnpackSleepingBag : ActionDeployObject
{
	override bool CanBeUsedWithBrokenLegs()
	{
		return true;
	}

	override void OnStartAnimationLoop(ActionData action_data)
	{
        super.OnStartAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
			if (action_data.m_MainItem)
			{
				action_data.m_MainItem.StartItemSoundServer(SoundConstants.ITEM_DEPLOY);
			}
		}
    }
	
	override void OnEndAnimationLoop(ActionData action_data)
	{
        super.OnEndAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
			if (action_data.m_MainItem)
			{
				action_data.m_MainItem.StopItemSoundServer(SoundConstants.ITEM_DEPLOY);
			}
		}
    }
}