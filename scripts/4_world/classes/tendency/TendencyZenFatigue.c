class TendencyZenFatigue extends DisplayElementTendency 
{
	void TendencyZenFatigue(PlayerBase player)
	{
		m_Type	= ZenSleepEnums.DELM_TDCY_FATIGUE;
		m_Key	= ZenSleepEnums.NTFKEY_FATIGUE;
	}
}