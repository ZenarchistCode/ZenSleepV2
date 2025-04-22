modded class NotifiersManager
{
	override void Init()
	{
		super.Init();

		m_Notifiers.Insert(new ZenFatigueNotfr(this));
	}
}

/*
modded class VirtualHud
{
	void VirtualHud(PlayerBase player)
	{
		//! Note: This doesn't work because for some reason the vanilla icon array is hard-coded to have a size limit.
		//		  So when the game syncs tendencies to the client with an extra stat added, the game client and server 
		//		  both crash. Had to use a basic PlayerBase.c RPC workaround instead of using vanilla mechanics to sync.

		//RegisterElement(new TendencyZenFatigue(m_Player));
	}
}
*/