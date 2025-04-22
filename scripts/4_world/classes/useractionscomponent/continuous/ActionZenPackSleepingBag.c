class ActionZenPackSleepingBagCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(5);
    }
}

class ActionZenPackSleepingBag : ActionContinuousBase
{
    void ActionZenPackSleepingBag()
    {
        m_CallbackClass = ActionZenPackSleepingBagCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
        m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
        m_ConditionItem = new CCINotPresent;
    }

    override string GetText()
    {
        return "#pack_tent";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object targetObject = target.GetObject();
        if (targetObject && targetObject.IsInherited(ZenSleepingBag_Base))
        {
            ZenSleepingBag_Base bag;
            if (Class.CastTo(bag, targetObject))
            {
                return bag.IsZenSleepingBagDeployed();
            }
        }

        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        ZenSleepingBag_Base bag = ZenSleepingBag_Base.Cast(action_data.m_Target.GetObject());
        if (bag != NULL)
        {
            bag.SetZenSleepingBagDeployed(false);
            bag.SetHealth(bag.GetHealth() - 5); // 250hp = 50 placements from pristine
        }
    }

    override void OnStartAnimationLoop(ActionData action_data)
	{
        super.OnStartAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
            ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());
            if (targetItem)
			    targetItem.StartItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
		}
    }
	
	override void OnEndAnimationLoop(ActionData action_data)
	{
        super.OnEndAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
			ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());
            if (targetItem)
			    targetItem.StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
		}
    }
}
