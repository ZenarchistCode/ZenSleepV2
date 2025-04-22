#ifdef ZENSLEEP_VPPADMINTOOLS
#ifdef VPPADMINTOOLS

modded class HealPlayerChatModule : ChatCommand
{
    override void ExecuteCommand(PlayerBase caller, array<Man> targets, array<string> args)
    {
        super.ExecuteCommand(caller, targets, args);

        if (caller == null) 
            return;

        foreach (Man target : targets)
        {
            PlayerBase playerTarget = PlayerBase.Cast(target);
            if (playerTarget != null)
            {
				playerTarget.GetStatZenFatigue().Set(playerTarget.GetStatZenFatigue().GetMax());
            }
        }
    }
}

#endif
#endif