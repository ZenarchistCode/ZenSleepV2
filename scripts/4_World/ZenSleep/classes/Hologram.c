modded class Hologram
{
    override string ProjectionBasedOnParent()
    {
        //! SLEEPING BAGS 
        if (m_Parent.IsKindOf("ZenSleepingBag_Base"))
        {
            string newType = m_Parent.GetType();
            return newType + "_Deployed";
        }

        return super.ProjectionBasedOnParent();
    }

    override void RefreshVisual()
	{
        super.RefreshVisual();

        // Hacky but whatever.
		ZenSleepingBag_Base sleepingBag = ZenSleepingBag_Base.Cast(m_Projection);
        if (sleepingBag)
        {
            bool cantDeploy = IsColliding() || IsFloating();
            sleepingBag.SetHologrammedZS(true, cantDeploy);
        }
	}

    override EntityAI PlaceEntity(EntityAI entity_for_placing)
    {
        if (m_Parent != NULL && m_Parent.IsInherited(ZenSleepingBag_Base) && m_Projection != NULL)
        {
            // Don't let vanilla do this, it's handled internally instead by ZenSleepingBag_Base.c
            return NULL;
        }

        return super.PlaceEntity(entity_for_placing);
    }
}
