// Deployed
class ZenSleepingBagDeployed_Base extends ItemBase
{
	protected Object m_ZenBagClutterCutter;
	
	void ~ZenSleepingBagDeployed_Base()
    {
        if (m_ZenBagClutterCutter != NULL)
            GetGame().ObjectDelete(m_ZenBagClutterCutter);
    }

	override void DeferredInit()
	{
		super.DeferredInit();

		if (!GetGame().IsDedicatedServer())
			return;

		// Don't use vanilla clutter cutters here. Took me WAY too long to figure out that the game treats the player as "inside building" when standing
		// on a clutter cutter, which triggered sound attenuation and all my inside building sleep boosts whenever you stood on the deployed bag.
		if (!m_ZenBagClutterCutter && !IsHologram())
		{
			m_ZenBagClutterCutter = GetGame().CreateObjectEx("ZenSleepingBag_ClutterCutter", GetPosition(), ECE_PLACE_ON_SURFACE);
			if (m_ZenBagClutterCutter)
			{
				m_ZenBagClutterCutter.SetOrientation(GetOrientation());
				ZenSleepFunctions.AlignToTerrain(m_ZenBagClutterCutter);
			}
		}
	}

	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();

		if (GetZenSleepingBagPackSound() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_DEPLOY_LOOP, GetZenSleepingBagPackSound());
    }

	string GetZenSleepingBagPackSound() 
    {
        return "Zen_SleepingBagPack_SoundSet";
    }

	override bool CanBeRepairedByCrafting()
    {
        return true;
    }

	override bool IsTakeable()
	{
		return false;
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
	
	override bool CanRemoveFromCargo(EntityAI parent)
	{
		return false;
	}
	
	override bool CanRemoveFromHands(EntityAI parent)
	{
		return false;
	}
}

class ZenSleepingBag_Black_Deployed extends ZenSleepingBagDeployed_Base {};
class ZenSleepingBag_Blue_Deployed extends ZenSleepingBagDeployed_Base {};
class ZenSleepingBag_Green_Deployed extends ZenSleepingBagDeployed_Base {};
class ZenSleepingBag_Grey_Deployed extends ZenSleepingBagDeployed_Base {};
class ZenSleepingBag_Red_Deployed extends ZenSleepingBagDeployed_Base {};