modded class PlayerBase
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ####################################### ZEN SLEEP REQUIRED SETUP #######################################
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	protected ref ZenSleepManager m_ZenSleepManager;
	protected ref ZenSleepSoundEffectHandlerClient m_SoundEffectHandlerClient;

	protected int m_ZenSleepFxID;
	protected int m_LastZenSleepFxID;

	override void Init()
	{
		super.Init();

		// Used for triggering sleep sound & visual FX sync.
		RegisterNetSyncVariableInt("m_ZenSleepFxID");
	}

	override void DeferredInit()
	{
		super.DeferredInit();

		// Register these here on delayed Init so playerbase has completed init first.
		m_ZenSleepManager = new ZenSleepManager(this);

		if (GetGame().IsClient() && IsControlledPlayer())
			m_SoundEffectHandlerClient = new ZenSleepSoundEffectHandlerClient(this);
	}

	override void CloseInventoryMenu()
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			super.CloseInventoryMenu();
			return;
		}
		#endif

		if (GetZenSleepManager().IsLyingDown() && GetZenSleepConfig().ClientEffectsConfig.AllowInventoryWhileSleeping)
			return; // Don't close inventory while lie down emote is active.

		super.CloseInventoryMenu();
	}

	ZenSleepManager GetZenSleepManager()
	{
		return m_ZenSleepManager;
	}

	void SetZenSleepFxID(int id)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		m_ZenSleepFxID = id;

		if (!GetGame().IsDedicatedServer())
			return;

		SetSynchDirty();

		// Reset fx ID after sending it to prevent delayed sync for anyone currently outside player's nearby network bubble
		if (id != -1)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ResetZenSleepFxID);	// in case one is queued already
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ResetZenSleepFxID, 100);
		}
	}

	int GetZenSleepFxID()
	{
		return m_ZenSleepFxID;
	}

	void ResetZenSleepFxID()
	{
		SetZenSleepFxID(-1);
	}

	array<AbstractWave>	GetZenSleepActionPerformedSounds() //! TODO? I dont think this is relevant? It stops action sounds. Do we need to do that really for sleep sfx?
	{
		return m_PerformedActionSounds;
	}

	override bool Consume(PlayerConsumeData data)
	{
		bool superConsume = super.Consume(data);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return superConsume;
		}
		#endif

		if (GetGame().IsDedicatedServer())
			GetZenSleepManager().Consume(data);

		return superConsume;
	}

	override void OnUnconsciousStop(int pCurrentCommandID)
	{
		super.OnUnconsciousStop(pCurrentCommandID);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (!GetGame().IsDedicatedServer())
			return;

		if (GetZenSleepConfig().GainConfig.PercentGainedUponUncon > 0 && GetStatLevelZenFatigue() >= EStatLevels.LOW)
		{
			float fatigueProgress = 1.0 - (GetStatZenFatigue().Get() / ZenSleepConstants.SL_FATIGUE_LOW);

			if (fatigueProgress < 0)
				fatigueProgress = 0.1;

			float totalEnergyRestored = GetStatZenFatigue().GetMax() * ((GetZenSleepConfig().GainConfig.PercentGainedUponUncon / 100) * fatigueProgress);
			GetStatZenFatigue().Add(totalEnergyRestored);

			ZenSleepFunctions.DebugMessage("Regained uncon fatigue: " + totalEnergyRestored + " based on " + (fatigueProgress * 100) + "% to zero");
		}
	}

	//! Note: This function only runs on server and on controlled player's client (ie. does not execute on other people's PlayerBase)
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

		if (GetZenSleepManager())
			GetZenSleepManager().OnUpdateTickShared(deltaTime);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		// Trigger ZenSleep effect sync
		if (m_ZenSleepFxID != m_LastZenSleepFxID)
		{
			m_LastZenSleepFxID = m_ZenSleepFxID;

			if (GetZenSleepManager() && GetZenSleepManager().GetVisualEffectHandlerClient())
				GetZenSleepManager().GetVisualEffectHandlerClient().SetFxID(m_ZenSleepFxID);

			if (m_SoundEffectHandlerClient)
			{
				m_SoundEffectHandlerClient.PlaySoundEffect(m_ZenSleepFxID);
			}

			m_ZenSleepFxID = -1;
		}
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);

		//! SLEEPING BAGS
		AddAction(ActionZenPackSleepingBag, InputActionMap);
		AddAction(ActionZenSleepOnBed, InputActionMap);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (damageResult != null)
		{
			GetZenSleepManager().EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ############################### SERVER-SIDE HANDLING OF NEW FATIGUE STAT ###############################
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	PlayerStat<float> GetStatZenFatigue()
	{
		return ZenSleep_FindFloatStatByLabel("ZenSleep");
	}

	// For Namalsk frostbite/other mod compatibility - get stat by label, not index/id, 
	// which can conflict when multiple mods use PCO to register new stats.
	PlayerStat<float> ZenSleep_FindFloatStatByLabel(string label)
	{
		PlayerStatsPCO_Base pco = GetPlayerStats().GetPCO();
		if (!pco) 
			return null;

		array<ref PlayerStatBase> stats = pco.Get();
		if (!stats) 
			return null;

		for (int i = 0; i < stats.Count(); i++)
		{
			PlayerStatBase s = stats[i];
			if (s && s.GetLabel() == label)
				return PlayerStat<float>.Cast(s);
		}

		return NULL;
	}

	EStatLevels GetStatLevelZenFatigue()
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep") || !GetStatZenFatigue())
			return ZenSleepConstants.SL_FATIGUE_HIGH; // To be safe, always return high when not enabled so any debuffs are not applied.
		#endif

		float fatigue = GetStatZenFatigue().Get();
		return GetStatLevel(fatigue, ZenSleepConstants.SL_FATIGUE_CRITICAL, ZenSleepConstants.SL_FATIGUE_LOW, ZenSleepConstants.SL_FATIGUE_NORMAL, ZenSleepConstants.SL_FATIGUE_HIGH);
	}

	float GetStatBordersZenFatigue()
	{
		float fatigue = GetStatZenFatigue().Get();
		float fatigue_max = GetStatZenFatigue().GetMax();
		return GetStatLevelBorders(fatigue, ZenSleepConstants.SL_FATIGUE_CRITICAL, ZenSleepConstants.SL_FATIGUE_LOW, ZenSleepConstants.SL_FATIGUE_NORMAL, ZenSleepConstants.SL_FATIGUE_HIGH, fatigue_max);
	}

	override float GetImmunity()
	{
		float vanillaImmunity = super.GetImmunity();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return vanillaImmunity;
		}
		#endif

		float immunity = vanillaImmunity;

		if (GetStatLevelZenFatigue() >= GetZenSleepConfig().GeneralConfig.FatigueLevelToNegativeImpactImmunity)
		{
			if (GetPlayerStats()) 
			{
				float max_health = GetMaxHealth("GlobalHealth", "Health") + 0.01;//addition to prevent divisioin by zero in case of some messup
				float max_blood = GetMaxHealth("GlobalHealth", "Blood") + 0.01;//addition to prevent divisioin by zero in case of some messup
				float energy_normalized = GetStatEnergy().Get() / PlayerConstants.SL_ENERGY_MAX;
				float water_normalized = GetStatWater().Get() / PlayerConstants.SL_WATER_MAX;
				float health_normalized = GetHealth("GlobalHealth", "Health") / max_health;
				float blood_normalized = GetHealth("GlobalHealth", "Blood") / max_blood;
				float fatigue_normalized = GetStatZenFatigue().Get() / GetStatZenFatigue().GetMax();//added
				immunity = energy_normalized + water_normalized + health_normalized + blood_normalized + fatigue_normalized;
				immunity = immunity / 5;//avg 5 instead of vanilla's 4
				immunity = Math.Clamp(immunity, 0, 1);
			}
		}

		return immunity;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ############################## SERVER-SIDE SYNCING OF FATIGUE ICON STATUS ##############################
	// I tried to tie these stats in with the vanilla stat tracking for VirtualHud.c, but because Bohemia have
	// hard-coded the array size to only allow for vanilla icons I wasn't smart enough to find a way to integrate
	// with their vanilla system to add a new stat icon. This is the best altnerative I could come up with.

	// Client (read)
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		switch (rpc_type)
		{
			// We use 1 integer to sync all values, as they will often require sync at the same time anyway.
			case ZenSleepEnums.RPC_SLEEP_STAT_UPDATE:
			{
				if (GetZenSleepManager())
				{
					int data4;
					ctx.Read(data4);
					GetZenSleepManager().DoClientFromServerSync(data4);
				}
				break;
			}
		}
	}
	
	//####################### END OF SERVER-SIDE SYNCING OF FATIGUE ICON STATUS #######################
	///////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ################################### OPTIONAL EXTRA PERSISTENCE CODE ####################################
	#ifdef ZENSLEEP_PERSISTENCE
	protected float m_PauseDrainEffectSecsZS;
	protected float m_InabilityToSleepEffectSecsZS;

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		if (GetZenSleepManager())
		{
			m_PauseDrainEffectSecsZS = GetZenSleepManager().GetFatigueDrainPauseSeconds();
			m_InabilityToSleepEffectSecsZS = GetZenSleepManager().GetInabilityToSleepEffectSecs();
		}

		ctx.Write(m_PauseDrainEffectSecsZS);
		ctx.Write(m_InabilityToSleepEffectSecsZS);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		super.OnStoreLoad(ctx, version);

		if (!ctx.Read(m_PauseDrainEffectSecsZS))
			return false;

		if (!ctx.Read(m_InabilityToSleepEffectSecsZS))
			return false;

		if (GetZenSleepManager())
			GetZenSleepManager().SetPersistenceVariables(m_PauseDrainEffectSecsZS, m_InabilityToSleepEffectSecsZS);

		return true;
	}
	#endif
}