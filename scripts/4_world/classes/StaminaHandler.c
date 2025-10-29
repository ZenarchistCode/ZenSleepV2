modded class StaminaHandler 
{
    override void Init()
	{
        super.Init();

		//----------------- recovery --------------------
		m_RegisteredRecoveryModifiers.Insert(ZenSleepEnums.ZENSLEEP_STAMINA_MDFR, 1);
	}
}