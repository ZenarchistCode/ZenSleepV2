// This sleeping bag has 2 states: deployed and packed.
// If it's packed then it can be carried around, put on a backpack etc.
// If it's deployed it can act as storage and be slept on, but can't be moved.
// Packing and deploying the item will hide/show the hidden selections in the same .p3d file.
// This means whatever types.xml settings are applied will apply to both versions of the item,
// so if the server only wants ~30 bags spawning, players finding bags and deploying them
// will not cause them to spawn new bags and cause a loot imbalance (in other words - this works like tents)
// The lifetime of the packed version of the bag can be overridden in the JSON config (set to zero to inherit main bag's lifetime)
class ZenSleepingBag_Base extends ItemBase
{
	static const string SLEEPING_BAG_DEPLOYED_RVMAT	= "ZenSleep\\data\\textures\\sleepingbags\\sleepingbag";
	static const string SLEEPING_BAG_PACKED_RVMAT	= "ZenSleep\\data\\textures\\sleepingbags\\sleepingbag_packed";

	static const int ZENBAG_UNINIT		= -1;
    static const int ZENBAG_PACKED		= 0;
	static const int ZENBAG_DEPLOYED 	= 1;

    protected int m_ZenBagState			= ZENBAG_UNINIT;
	protected int m_ZenBagStateLocal	= ZENBAG_UNINIT;
	protected bool m_WasHologrammedZS;

	protected Object m_ZenBagClutterCutter;

    void ZenSleepingBag_Base()
	{
		RegisterZenBagNetSync();
	}
	
	void ~ZenSleepingBag_Base()
    {
        if (m_ZenBagClutterCutter != NULL)
            GetGame().ObjectDelete(m_ZenBagClutterCutter);
    }

	override void DeferredInit()
	{
		super.DeferredInit();

		if (!GetGame().IsDedicatedServer())
			return;

		if (!IsZenSleepingBagDeployed())
		{
			// Lifetime is zero during OnStoreLoad() so need to check this here.
			if (GetZenSleepConfig().GeneralConfig.PackedBagLifetime > 0 && GetLifetime() > GetZenSleepConfig().GeneralConfig.PackedBagLifetime)
			{
				SetLifetime(GetZenSleepConfig().GeneralConfig.PackedBagLifetime);
			}
		}
		else 
		{
			// Don't use vanilla clutter cutters here. Took me WAY too long to figure out that the game treats the player as "inside building" when standing
			// on a clutter cutter, which triggered sound attenuation and all my inside building sleep boosts whenever you stood on the deployed bag.
			if (!m_ZenBagClutterCutter)
			{
				m_ZenBagClutterCutter = GetGame().CreateObjectEx("ZenSleepingBag_ClutterCutter", GetPosition(), ECE_PLACE_ON_SURFACE);
				if (m_ZenBagClutterCutter)
				{
					m_ZenBagClutterCutter.SetOrientation(GetOrientation());
					ZenSleepFunctions.AlignToTerrain(m_ZenBagClutterCutter);
				}
			}
		}
	}

	override void EEInit()
	{
		super.EEInit();

		// Set to packed when item is first created
		if (m_ZenBagState == ZENBAG_UNINIT)
			SetZenSleepingBagDeployed(false);
	}

	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		
		if (GetZenSleepingBagUnpackSound() != string.Empty) // Use deploy sound IDs, no need to make a new ID.
			handler.AddSound(SoundConstants.ITEM_DEPLOY, GetZenSleepingBagUnpackSound());

		if (GetZenSleepingBagPackSound() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_DEPLOY_LOOP, GetZenSleepingBagPackSound());
    }

    string GetZenSleepingBagUnpackSound() 
    {
        return "Zen_SleepingBagUnpack_SoundSet";
    }

	string GetZenSleepingBagPackSound() 
    {
        return "Zen_SleepingBagPack_SoundSet";
    }

	protected void RegisterZenBagNetSync()
	{
		RegisterNetSyncVariableInt("m_ZenBagState");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
						
		if (m_ZenBagState != m_ZenBagStateLocal)
		{
			if (m_ZenBagState == ZENBAG_DEPLOYED)
				SetZenSleepingBagDeployed(true);
			else
				SetZenSleepingBagDeployed(false);
			
			m_ZenBagStateLocal = m_ZenBagState;
		}
	}

	override int GetViewIndex()
	{
		if (MemoryPointExists("invView2"))
		{
			if (IsZenSleepingBagDeployed())
			{
				return 1;
			}
			else
			{
				return 0;
			}	
		}

		return 0;
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

	int GetZenSleepingBagState()
	{
		return m_ZenBagState;
	}

	bool IsZenSleepingBagDeployed()
	{
		return m_ZenBagState == ZENBAG_DEPLOYED;
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);

		if (newLevel >= GameConstants.STATE_RUINED)
		{
			SetObjectMaterial(0, SLEEPING_BAG_PACKED_RVMAT + "_destruct.rvmat");
			SetObjectMaterial(1, SLEEPING_BAG_DEPLOYED_RVMAT + "_destruct.rvmat");
		} else
		if (newLevel >= GameConstants.STATE_DAMAGED)
		{
			SetObjectMaterial(0, SLEEPING_BAG_PACKED_RVMAT + "_damage.rvmat");
			SetObjectMaterial(1, SLEEPING_BAG_DEPLOYED_RVMAT + "_damage.rvmat");
		} else
		{
			SetObjectMaterial(0, SLEEPING_BAG_PACKED_RVMAT + ".rvmat");
			SetObjectMaterial(1, SLEEPING_BAG_DEPLOYED_RVMAT + ".rvmat");
		}
	}

	void SetZenSleepingBagDeployed(bool isDeployed)
	{
		if (isDeployed)
		{
			m_ZenBagState = ZENBAG_DEPLOYED;

			// Hide packed state and show deployed state
			SetSimpleHiddenSelectionState(0, 0);
			SetSimpleHiddenSelectionState(1, 1);
		}
		else 
		{
			m_ZenBagState = ZENBAG_PACKED;

			// Hide deployed state and show packed state
			SetSimpleHiddenSelectionState(0, 1);
			SetSimpleHiddenSelectionState(1, 0);

			if (GetGame().IsDedicatedServer())
			{
				// Sometimes it's impossible to see the cargo if placed in a weird spot, so allow packing with cargo inside and just drop the items.
				if (GetNumberOfItems() > 0)
				{
					MiscGameplayFunctions.DropAllItemsInInventoryInBounds(this, vector.Zero);
				}

				if (m_ZenBagClutterCutter != NULL)
				{
					GetGame().ObjectDelete(m_ZenBagClutterCutter);
				}

				if (GetZenSleepConfig().GeneralConfig.PackedBagLifetime > 0 && GetLifetime() > GetZenSleepConfig().GeneralConfig.PackedBagLifetime)
				{
					SetLifetime(GetZenSleepConfig().GeneralConfig.PackedBagLifetime);
				}
			}
		}

		SetSynchDirty();

		if (GetGame().IsClient())
			GetOnViewIndexChanged().Invoke();
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		if (new_owner != NULL)
		{
			// Double-make sure it's packed if it goes into any form of cargo
			if (IsZenSleepingBagDeployed())
				SetZenSleepingBagDeployed(false);
		}
	}

	override bool CanBeRepairedByCrafting()
    {
        return true;
    }

    override bool IsDeployable()
    {
        return true;
    }

	override bool IsTakeable()
	{
		if (!super.IsTakeable())	
		{
			return false;
		}
		
		return !IsZenSleepingBagDeployed();
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}
				
		return !IsZenSleepingBagDeployed();
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}
				
		return !IsZenSleepingBagDeployed();
	}
	
	override bool CanRemoveFromCargo(EntityAI parent)
	{
		if (!super.CanRemoveFromCargo(parent))		
		{
			return false;
		}
				
		return !IsZenSleepingBagDeployed();
	}
	
	override bool CanRemoveFromHands(EntityAI parent)
	{
		if (!super.CanRemoveFromHands(parent))
		{
			return false;
		}
		
		return !IsZenSleepingBagDeployed();
	}

	override bool CanDisplayCargo()
	{
		if (!super.CanDisplayCargo())
		{
			return false;
		}

		return IsZenSleepingBagDeployed();
	}

	override bool IsBuilding()
    {
        return false;
    }

	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		ctx.Write(m_ZenBagState);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (ctx.Read(m_ZenBagState))
		{
			if (m_ZenBagState == ZENBAG_DEPLOYED)
			{
				SetZenSleepingBagDeployed(true);
			}
			else
			{
				SetZenSleepingBagDeployed(false);
			}
		}
		
		return true;
	}
}

class ZenSleepingBag_Black extends ZenSleepingBag_Base {};
class ZenSleepingBag_Blue extends ZenSleepingBag_Base {};
class ZenSleepingBag_Green extends ZenSleepingBag_Base {};
class ZenSleepingBag_Grey extends ZenSleepingBag_Base {};
class ZenSleepingBag_Red extends ZenSleepingBag_Base {};

class ZenSleepingBagStatic_Base extends BuildingSuper
{
    void ZenSleepingBagStatic_Base()
    {
        SetAllowDamage(false);

		// Always show static bags as deployed
		SetSimpleHiddenSelectionState(0, 0);
		SetSimpleHiddenSelectionState(1, 1);
    }

    override bool IsBuilding()
    {
        return false;
    }
}

class ZenSleepingBagStatic_Black extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Blue extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Grey extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Red extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Green extends ZenSleepingBagStatic_Base {};