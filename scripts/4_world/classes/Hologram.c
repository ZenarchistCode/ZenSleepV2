modded class Hologram
{
    override void SetAnimations()
	{
        super.SetAnimations();

        ZenSleepingBag_Base sleepingBag = ZenSleepingBag_Base.Cast(m_Projection);
        if (sleepingBag)
        {
            sleepingBag.SetZenSleepingBagDeployed(true);
        }
	}

    override void RefreshVisual()
	{
        super.RefreshVisual();

		// Couldn't figure out how to use animations and rvmats like vanilla deployables, so have to do this to apply my hidden selections approach instead
        ZenSleepingBag_Base sleepingBag = ZenSleepingBag_Base.Cast(m_Projection);
        if (sleepingBag)
        {
            bool cantDeploy = IsColliding() || IsFloating();
            sleepingBag.SetHologrammedZS(true, cantDeploy);
        }
	}

    override EntityAI PlaceEntity(EntityAI entity_for_placing)
    {
        //! SLEEPING BAG
        if (m_Parent != NULL && m_Parent.IsInherited(ZenSleepingBag_Base) && m_Projection != NULL)
        {
            // Get bag
            ZenSleepingBag_Base bag = ZenSleepingBag_Base.Cast(m_Parent);
            bag.SetZenSleepingBagDeployed(true);
            return bag;
        }

        return super.PlaceEntity(entity_for_placing);
    }
}
