class ActionZenSleepOnBedCB : ActionInteractBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUse;
	}

    /*
    // Not sure I like this, feels janky. Let the player decide how to face the bag if they're into RP and shit.
	override void InitActionComponent()
    {
        super.InitActionComponent();

        if (m_ActionData.m_Target && m_ActionData.m_Target.GetObject())
        {
            // Set player orientation to match bag - don't teleport their position to center of bag as it can be exploited for raids etc.
            vector sleepOrientation = m_ActionData.m_Target.GetObject().GetOrientation() + "-90 0 0";
            m_ActionData.m_Player.SetOrientation(sleepOrientation);
        }
    }
    */
}

class ActionZenSleepOnBed : ActionInteractBase
{
    void ActionZenSleepOnBed()
    {
		m_CallbackClass = ActionZenSleepOnBedCB;
        m_CommandUID = -1;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text = "#STR_USRACT_ID_EMOTE_LYINGDOWN";
    }

    override bool IsInstant()
	{
		return true;
	}

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (player.GetItemInHands() || !target)
            return false;

        Object targetObject = target.GetObject();
        if (!targetObject)
            return false;

        return player.GetZenSleepManager().CanSleepOnBedObject(targetObject);
    }

    override void Start(ActionData action_data)
	{
		super.Start(action_data);

        if (GetGame().IsClient())
        {
            if (action_data.m_Player.GetEmoteManager())
            {
                action_data.m_Player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_LYINGDOWN);
            }
        }

        if (GetGame().IsDedicatedServer())
        {
            if (action_data.m_Player.GetZenSleepManager())
            {
                action_data.m_Player.GetZenSleepManager().SetBed(action_data.m_Target.GetObject());
            }
        }
	}
}