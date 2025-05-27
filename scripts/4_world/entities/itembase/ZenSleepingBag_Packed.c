// Packed bag
class ZenSleepingBag_Base extends ItemBase
{
	protected bool m_WasHologrammedZS;

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
    {
        if (GetGame().IsClient())
            return;

        // Place deployed version and carry over health
        string newType = GetType() + "_Deployed";
        ZenSleepingBagDeployed_Base bagDeployed = ZenSleepingBagDeployed_Base.Cast(GetGame().CreateObject(newType, position, false));
        if (!bagDeployed)
        {
            Error("ZenSleep :: SleepingBags - very bad error - couldn't create " + newType);
            return;
        }

        bagDeployed.SetHealth(GetHealth());
        bagDeployed.SetWet(GetWet());
        bagDeployed.SetPosition(position);
        bagDeployed.SetOrientation(orientation);

        if (bagDeployed.GetEconomyProfile().GetLifetime() == 1800) // Server admin has not updated their types.xml
        {
            Error("[ZenSleeping] This mod was updated on 27th May 2025 to separate the sleeping bag so that packed and deployed bags are unique items. Please update the types.xml entry for " + bagDeployed.GetType() + " to have a lifetime greater than 1800 seconds (which is only half an hour).");
            bagDeployed.SetLifetimeMax(172800);
            bagDeployed.SetLifetime(172800);
        }

        // Delete packed version
        GetGame().ObjectDelete(this);
    }

	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		
		if (GetZenSleepingBagUnpackSound() != string.Empty) // Use deploy sound IDs, no need to make a new ID.
			handler.AddSound(SoundConstants.ITEM_DEPLOY, GetZenSleepingBagUnpackSound());
    }

    string GetZenSleepingBagUnpackSound() 
    {
        return "Zen_SleepingBagUnpack_SoundSet";
    }

	void SetHologrammedZS(bool hologram, bool cantDeploy)
    {
        m_WasHologrammedZS = hologram;

        if (m_WasHologrammedZS)
        {
			float opacity = 0.9;
			string color = "0.5,0.5,0.5"; //rgb

			if (cantDeploy)
			{
				opacity = 0.05;
			}

            string textureAlpha = "#(argb,8,8,3)color(" + color + "," + opacity + ",ca)";
			SetObjectTexture(1, textureAlpha);
        }
        else
        {
			SetObjectTexture(1, GetHiddenSelectionsTextures().Get(1));
        }
    }

	override void SetActions()
    {
        super.SetActions();

        AddAction(ActionTogglePlaceObject);
        AddAction(ActionZenUnpackSleepingBag);
        AddAction(ActionWringClothes);
    }

	override bool CanBeRepairedByCrafting()
    {
        return true;
    }

    override bool IsDeployable()
    {
        return true;
    }

	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		// For legacy purposes
		ctx.Write(0);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		int ignoredBagState;
		if (ctx.Read(ignoredBagState))
		{
			// do nothing
		}
		
		return true;
	}
}

class ZenSleepingBag_Black extends ZenSleepingBag_Base {};
class ZenSleepingBag_Blue extends ZenSleepingBag_Base {};
class ZenSleepingBag_Green extends ZenSleepingBag_Base {};
class ZenSleepingBag_Grey extends ZenSleepingBag_Base {};
class ZenSleepingBag_Red extends ZenSleepingBag_Base {};