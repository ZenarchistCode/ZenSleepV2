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
        m_Text = "#pack_tent";
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
        m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
        m_ConditionItem = new CCINotPresent;
    }

    override bool HasTarget()
    {
        return true;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object targetObject = target.GetObject();
        if (targetObject && targetObject.IsInherited(ZenSleepingBagDeployed_Base))
        {
            ZenSleepingBagDeployed_Base bag;
            if (Class.CastTo(bag, targetObject))
            {
                return true;
            }
        }

        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);

        if (!action_data.m_Target || !action_data.m_Target.GetObject())
            return;

        Object targetObject = action_data.m_Target.GetObject();
        ZenSleepingBagDeployed_Base bag = ZenSleepingBagDeployed_Base.Cast(targetObject);
        if (!bag)
        {
            Error("[ZenSleeping] ActionPackSleepingBag: Failed to cast target object to SleepingBagDeployedBase " + targetObject.GetType());
            return;
        }

        string newType = targetObject.GetType();
        newType.Replace("_Deployed", "");

        ZenSleepingBag_Base bagPacked = ZenSleepingBag_Base.Cast(GetGame().CreateObject(newType, action_data.m_Player.GetPosition(), false));
        if (!bagPacked)
        {
            Error("[ZenSleeping] ActionPackSleepingBag: Failed to create object " + newType);
            return;
        }

        // Drop items in bag - sometimes the bag inventory can't be seen in weird spots, so allow packing it with gear inside but just drop the gear.
        MiscGameplayFunctions.DropAllItemsInInventoryInBounds(bag, vector.Zero);
        bagPacked.SetWet(bag.GetWet());
        bagPacked.SetHealth(bag.GetHealth() - 5); // 250hp = 50 placements if pristine
        GetGame().ObjectDelete(bag);
    }

    override void OnStartAnimationLoop(ActionData action_data)
	{
        super.OnStartAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
            ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());

            if (targetItem)
            {
			    targetItem.StartItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
            }
		}
    }
	
	override void OnEndAnimationLoop(ActionData action_data)
	{
        super.OnEndAnimationLoop(action_data);

        if (GetGame().IsDedicatedServer())
		{
			ItemBase targetItem = ItemBase.Cast(action_data.m_Target.GetObject());
            if (targetItem)
            {
			    targetItem.StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);
            }
		}
    }
}
