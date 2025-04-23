// Most of this class is server-side stuff. Manages general sleep-related mechanics.
// I moved it out of PlayerBase.c to avoid clutter in that class as most of this stuff
// doesn't really need to be in there. Inspired by vanilla processes like EmoteManager/BleedingSourcesManager etc.
// It mainly runs on server and on our player client - for other player's sleep sounds, which is the only thing
// which is required to be sync'd to all players, I use ZenSleepSoundEffectHandlerClient.c
class ZenSleepManager
{
	protected PlayerBase m_Player;
	protected ref HumanMovementState m_MovementState;
	protected Object m_Bed;

	protected ref ZenSleepVisualEffectHandlerClient m_VisualEffectHandlerClient;
	protected ref ZenSleepEffectHandlerServer m_EffectHandlerServer;

	protected int m_SleepConditionLastSync;
	protected int m_SleepCondition;
	protected int m_StatFatigueTendency;
	protected int m_StatFatigueTendencyLastSync;
	protected int m_StatFatigueState;
	protected int m_StatFatigueStateLastSync;

	protected float m_PauseDrainEffectSecs;
	protected float m_InabilityToSleepEffectSecs;
	protected float m_DeltaTime;
	protected float m_SleepStartTimestamp;
	protected float m_SleepStartFatigue;	
	protected float m_FatigueSpeedModifier;
	protected float m_FatigueSpeed;

	protected bool m_IsPlayerAI;

	void ZenSleepManager(PlayerBase player)
	{
		m_Player = player;

		Init();
	}

	void Init()
	{
		m_MovementState = new HumanMovementState();
		m_VisualEffectHandlerClient = new ZenSleepVisualEffectHandlerClient(m_Player);
		m_EffectHandlerServer = new ZenSleepEffectHandlerServer(m_Player);
		m_DeltaTime = 0;
		m_SleepCondition = ZenSleepState.AWAKE;
		m_SleepStartTimestamp = -1;
		m_PauseDrainEffectSecs = 0;
		m_InabilityToSleepEffectSecs = 0;
		m_FatigueSpeedModifier = 0;
		m_FatigueSpeed = 0;
		m_SleepStartFatigue = 0;

		#ifdef ENFUSION_AI_PROJECT
		// Robots don't sleep
		m_IsPlayerAI = true;
		#else
		m_IsPlayerAI = false;
		#endif
	}

	PlayerBase GetPlayer()
	{
		return m_Player;
	}

	bool IsAI()
	{
		return m_IsPlayerAI;
	}

	ZenSleepVisualEffectHandlerClient GetVisualEffectHandlerClient()
	{
		return m_VisualEffectHandlerClient;
	}

	ZenSleepEffectHandlerServer GetEffectHandlerServer()
	{
		return m_EffectHandlerServer;
	}

	void SetBed(Object object)
	{
		m_Bed = object;
	}

	Object GetBed()
	{
		return m_Bed;
	}

	// Check if player has used a bed to sleep on, and the bed is not wetter than "excludeDampState"
	bool HasBed(float excludeDampState = GameConstants.STATE_WET)
	{
		ItemBase bed = ItemBase.Cast(m_Bed);

		if (bed && bed.GetWet() >= excludeDampState)
			return false;

		return m_Bed != NULL;
	}

	// Quickest vanilla way to detect being "inside" - will also return true for some tents, vehicles etc
	bool IsSheltered()
	{
		return m_Player.IsSoundInsideBuilding();
	}

	int GetPlayerMovementState()
	{
		m_Player.GetMovementState(m_MovementState);
		return m_MovementState.m_iMovement;
	}

	void SetFatigueStatTendency(int tendency)
	{
		m_StatFatigueTendency = tendency;
	}

	int GetFatigueStatTendency()
	{
		return m_StatFatigueTendency;
	}

	int GetFatigueStatTendencyDisplay()
	{
		float tendency = GetFatigueStatTendency();

		if (tendency > 3) 
		{
			tendency = -(tendency - 3);
		}

		return tendency;
	}

	void SetFatigueStatState(int state)
	{
		m_StatFatigueState = state;
	}

	int GetFatigueStatState()
	{
		return m_StatFatigueState;
	}

	void SetSleepCondition(int condition, string notification = "")
	{
		m_SleepCondition = condition;

		if (m_SleepCondition == ZenSleepState.AWAKE)
		{
			m_SleepStartTimestamp = -1;

			if (GetGame().IsClient())
				StopAudioAttenuation();
		}

		if (notification != "" && GetZenSleepConfig().GeneralConfig.DisplaySleepMessages)
			ZenSleepFunctions.ServerMessage(m_Player, notification);
	}

	int GetSleepCondition()
	{
		return m_SleepCondition;
	}

	bool RequiresServerToClientSync()
	{
		if (m_StatFatigueTendency != m_StatFatigueTendencyLastSync)
			return true;

		if (m_StatFatigueState != m_StatFatigueStateLastSync)
			return true;

		if (m_SleepCondition != m_SleepConditionLastSync)
			return true;

		return false;
	}

	protected void DoServerToClientSync()
	{
		if (!RequiresServerToClientSync())
			return;

		// Check if server effects manager needs a timer update 
		if (m_StatFatigueStateLastSync != m_StatFatigueState)
		{
			GetEffectHandlerServer().UpdateStateTimers();
		}

		// Pack all 4 relevant fatigue stats into a single integer
		int packedInt = ZenSleepFunctions.GetPackedIntegerData3(m_StatFatigueTendency, m_StatFatigueState, m_SleepCondition);

		// Send RPC data
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(packedInt);
		rpc.Send(m_Player, ZenSleepEnums.RPC_SLEEP_STAT_UPDATE, false, m_Player.GetIdentity());

		// Sync latest values
		m_StatFatigueTendencyLastSync = m_StatFatigueTendency;
		m_StatFatigueStateLastSync = m_StatFatigueState;
		m_SleepConditionLastSync = m_SleepCondition;
	}

	void DoClientFromServerSync(int data)
	{
		// Read all 4 relevant fatigue stat info from single integer
		ZenSleepFunctions.ReadPackedIntegerData3(data, m_StatFatigueTendency, m_StatFatigueState, m_SleepCondition);

		if (m_SleepCondition != m_SleepConditionLastSync)
		{
			m_SleepConditionLastSync = m_SleepCondition;

			if (m_SleepCondition == ZenSleepState.AWAKE)
				OnStopSleep();
		}
	}

	void OnStartSleep()
	{
		m_SleepStartTimestamp = GetGame().GetTime();
		m_SleepStartFatigue = m_Player.GetStatZenFatigue().Get();

		if (GetGame().IsClient())
		{
			if (GetZenSleepConfig().ClientEffectsConfig.DelaySecsUntilAudioAttenuation > 0)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ApplyAudioAttenuation, GetZenSleepConfig().ClientEffectsConfig.DelaySecsUntilAudioAttenuation * 1000);
			}

			//LockCameraLimits();
		}

		// Fail-safe check for a nearby bed within 1 meter if player forgot to use F to lie down on the object itself
		if (GetGame().IsDedicatedServer())
		{
			if (!m_Bed)
			{
				array<Object> nearest_objects = new array<Object>;
				GetGame().GetObjectsAtPosition3D(m_Player.GetPosition(), 1.0, nearest_objects, NULL);

				for (int i = 0; i < nearest_objects.Count(); i++)
				{
					Object object = nearest_objects.Get(i);
					
					if (CanSleepOnBedObject(object))
					{
						SetBed(object);
						break;
					}
				}
			}
		}
	}

	bool CanSleepOnBedObject(Object object)
	{
		foreach (string s : GetZenSleepConfig().ClientEffectsConfig.BedObjects)
		{
			if (object.IsKindOf(s))
			{
				ZenSleepingBag_Base bagBase = ZenSleepingBag_Base.Cast(object);
				if (bagBase && (!bagBase.IsZenSleepingBagDeployed() || vector.Distance(bagBase.GetPosition(), m_Player.GetPosition()) > 0.5))
					return false;

				TentBase tent = TentBase.Cast(object);
				if (tent && tent.GetState() != tent.PITCHED)
					return false;

				return true;
			}
		}

		return false;
	}

	void OnStopSleep()
	{
		if (GetGame().IsClient())
		{
			StopAudioAttenuation();

			//UnlockCameraLimits();
		}

		if (GetGame().IsDedicatedServer())
		{
			SetBed(NULL);
		}

		m_SleepStartTimestamp = -1;
	}

	void ApplyAudioAttenuation()
	{
		if (m_Player.GetMasterAttenuation() == "")
			m_Player.SetMasterAttenuation("ZenSleepAttenuation");
	}

	void StopAudioAttenuation()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ApplyAudioAttenuation);

		if (m_Player.GetMasterAttenuation() == "ZenSleepAttenuation") // check master atten hasn't been overridden (eg. flashbang'd/burlapped)
			m_Player.SetMasterAttenuation("");
	}

	void LockCameraLimits()
	{
		//! TODO: Come back to this, can't seem to get this to work after half an hour of screwing around.
		// I wanted to lock the camera view so you can't go all fuckin Chucky 360 head spins while asleep
		m_Player.SetLookLimits(30, -30, -45, 45);
		m_Player.SetAimLimits(30, -30, -45, 45);
	}

	void UnlockCameraLimits()
	{
		m_Player.SetLookLimits(-85, 85, -160, 160);
		m_Player.SetAimLimits(-85, 85, -180, 180);
	}

	void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		float damagePct = damageResult.GetDamage("", "Health") / 100;

		// Acts like drug effect but without any benefits - if player takes damage, stop them from sleeping for a little while due to adrenaline
		if (damagePct > 0 && m_InabilityToSleepEffectSecs < GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBuzzedByCombat)
			m_InabilityToSleepEffectSecs += GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBuzzedByCombat * damagePct;
	}

	void OnUpdateTickShared(float deltaTime)
	{
		if (IsAI())
			return;

		#ifdef SERVER 
		OnUpdateTickServer(deltaTime);
		#else
		if (m_Player.IsControlledPlayer())
			OnUpdateTickClient(deltaTime);
		#endif
	}

	// Server-side player checks
	protected void OnUpdateTickServer(float deltaTime)
	{
		DoServerToClientSync();

		// Check for drug effect before anything else.
		if (m_PauseDrainEffectSecs > 0)
		{
			m_PauseDrainEffectSecs -= deltaTime;

			if (m_PauseDrainEffectSecs <= 0)
			{
				SetSleepCondition(ZenSleepState.AWAKE);
			}
		}

		if (m_InabilityToSleepEffectSecs > 0)
			m_InabilityToSleepEffectSecs -= deltaTime;

		// If player is incapacitated, no need to update anything right now.
		if (!m_Player.IsAlive() || !m_Player.GetEmoteManager() || m_Player.IsUnconscious()) 
			return;

		// Handle server-side sound & visual syncs in separate class
		GetEffectHandlerServer().Update(deltaTime);

		// Throttle server checks which don't need to be checked every tick
		m_DeltaTime += deltaTime;
		if (m_DeltaTime < ZenSleepConstants.SLEEP_UPDATE_TICK_SECS)
			return;
		m_DeltaTime = 0;

		// Check if we're lying down.
		if (!IsLyingDown() && GetSleepCondition() != ZenSleepState.ON_DRUGS)
		{
			SetSleepCondition(ZenSleepState.AWAKE);
			return;
		}

		// Lying down, in sleep state
		if (IsSleeping())
		{
			// Check for wake-up conditions
			if (CheckWakeUpConditions())
				SetSleepCondition(ZenSleepState.AWAKE);

			// Check for valid bed 
			if (GetBed() && vector.Distance(GetBed().GetPosition(), GetPlayer().GetPosition()) > 2)
				SetBed(NULL);
		}
		else // Lying down, but not yet in sleep state
		{
			if (GetSecondsAsleep() >= 5)
			{
				if (!CheckWakeUpConditions())
				{
					SetSleepCondition(ZenSleepState.ASLEEP);
				}
				else 
				{
					OnStopSleep();
				}
			}
		}
	}

	// Client-side player checks
	protected void OnUpdateTickClient(float deltaTime)
	{
		// Handle client-side vfx/sfx events in separate class
		if (GetVisualEffectHandlerClient())
			GetVisualEffectHandlerClient().Update(deltaTime);
	}

	float GetFatiguePercent()
	{
		return (m_Player.GetStatZenFatigue().Get() / m_Player.GetStatZenFatigue().GetMax()) * 100;
	}

	float GetPlayerTemperature()
	{
		return m_Player.GetStatHeatComfort().Get();
	}

	float GetPlayerHeatBuffPercent()
	{
		return (m_Player.GetStatHeatBuffer().Get() / m_Player.GetStatHeatBuffer().GetMax()) * 100;
	}

	float GetPlayerWetness()
	{
		return m_Player.GetStatWet().Get();
	}

	float GetPlayerWeightPercent()
	{
		float weight = m_Player.GetWeightEx();
		float staminaImpact = Math.Max((CfgGameplayHandler.GetStaminaMax() - (((weight - CfgGameplayHandler.GetStaminaWeightLimitThreshold()) / GameConstants.STAMINA_KG_TO_GRAMS) * CfgGameplayHandler.GetStaminaKgToStaminaPercentPenalty())), CfgGameplayHandler.GetStaminaMinCap());
		return 100 - Math.Clamp(staminaImpact, 0, 100);
	}

	float GetPlayerHunger()
	{
		return m_Player.GetStatEnergy().Get();
	}

	float GetPlayerThirst()
	{
		return m_Player.GetStatWater().Get();
	}

	bool IsPlayerSick()
	{
		return m_Player.HasDisease() && !DoesPlayerOnlyHaveKuru();
	}

	// Check if player's only disease is kuru. I don't want to prevent cannibals from sleeping, that would be inhumane.
	bool DoesPlayerOnlyHaveKuru()
	{
		return m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_BRAIN) && m_Player.m_DiseaseCount == 1;
	}

	bool IsNightTime()
	{
		return GetGame().GetWorld().IsNight();
	}

	void SendVanillaSleepSoundset(string soundset)
	{
		int soundsetArrayID = ZenSleepSounds.GetUniVanillaSounds().Find(soundset);

		if (soundsetArrayID != -1)
		{
			m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(5, soundsetArrayID, 0, 0));
		}
	}

	void SendVanillaSleepSoundEventID(int id)
	{
		if (id > 0 && id < EPlayerSoundEventID.ENUM_COUNT)
			m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(0, 0, 0, id));
	}

	// This method will check all the possible ways we can be woken up.
	// It will select whichever wake-up call is triggered by the lowest fatigue level.
	bool CheckWakeUpConditions()
	{
		string restMessage = "";

		// Get relevant stats to check against fatigue thresholds
		float playerFatigue = GetFatiguePercent();
		float playerTemp = GetPlayerTemperature();
		float playerWet = GetPlayerWetness();
		float playerHunger = GetPlayerHunger();
		float playerThirst = GetPlayerThirst();
		bool playerSick =  IsPlayerSick();
		bool isNight = IsNightTime();
		bool tooEnergized = playerFatigue >= m_Player.GetStatZenFatigue().GetMax() - 1 || (isNight && playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Time_Night);

		// On drugs (ie. someone dosed us while sleeping the cheeky bastards)
		if (IsFatigueDrainPaused() || m_InabilityToSleepEffectSecs > 0)
		{
			restMessage = "#STR_ZenSleepMsg_TooBuzzed.";
			SendVanillaSleepSoundEventID(EPlayerSoundEventID.SYMPTOM_GASP);
		}

		// Too energized
		if (restMessage == "" && tooEnergized)
		{
			restMessage = "#STR_ZenSleepMsg_TooAwake.";
			SendVanillaSleepSoundEventID(EPlayerSoundEventID.RELEASE_BREATH);
		}

		if (restMessage == "")
		{
			if (!isNight)
			{
				if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Time_Daylight && m_Player.m_UndergroundPresence != EUndergroundPresence.FULL)
				{
					restMessage = "#STR_ZenSleepMsg_TooSunny.";
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.RELEASE_BREATH);
				}
			}
		}

		// Sick
		if (restMessage == "" && playerSick)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Illness)
			{
				restMessage = "#STR_ZenSleepMsg_TooSick.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.SYMPTOM_COUGH);
			}
		}

		// Wet 
		if (restMessage == "" && (playerWet > 0))
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Wet)
			{
				restMessage = "#STR_ZenSleepMsg_TooWet.";
				SendVanillaSleepSoundset("WringClothes_SoundSet");
			}
		}
		
		// Dark blue flashing cold
		if (restMessage == "" && playerTemp <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_DarkBlueFlashing)
			{
				restMessage = "#STR_ZenSleepMsg_TooCold.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.RATTLING_TEETH);
			}
		}

		// Having a bed overrides these checks
		if (GetZenSleepConfig().GainConfig.GainBedObjectUsed <= 0 || !HasBed()) // Allow bed to override
		{
			// Dark blue cold
			if (restMessage == "" && playerTemp <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL)
			{
				if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_DarkBlue)
				{
					restMessage = "#STR_ZenSleepMsg_TooCold.";
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.RATTLING_TEETH);
				}
			}

			// Light blue cold
			if (restMessage == "" && playerTemp <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING)
			{
				if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_LightBlue)
				{
					restMessage = "#STR_ZenSleepMsg_TooCold.";
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.FREEZING);
				}
			}
		}

		// Red flashing temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_RedFlashing)
			{
				restMessage = "#STR_ZenSleepMsg_TooHot.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Red temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_Red)
			{
				restMessage = "#STR_ZenSleepMsg_TooHot.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Yellow temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_Yellow)
			{
				restMessage = "#STR_ZenSleepMsg_TooHot.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Red flashing food
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_RedFlashing)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet"); // why is this the only one not a sound event ID bohemia?? Geez looeyz
			}
		}

		// Red flashing water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_RedFlashing)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// Red food
		if (restMessage == "" && playerHunger <= PlayerConstants.SL_ENERGY_LOW)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Red)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet");
			}
		}

		// Red water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_LOW)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Red)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// Yellow food
		if (restMessage == "" && playerHunger <= PlayerConstants.SL_ENERGY_NORMAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Yellow)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet");
			}
		}

		// Yellow water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_NORMAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Yellow)
			{
				restMessage = "#STR_ZenSleepMsg_TooUncomfortable.";
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// If we have a rest message, it's time to wake up. Otherwise we keep on sleeping.
		if (restMessage != "")
		{
			int sleepState = ZenSleepState.AWAKE;
			if (IsFatigueDrainPaused())
				sleepState = ZenSleepState.ON_DRUGS;

			SetSleepCondition(sleepState, "#STR_ZenSleepMsg_CantSleep, " + restMessage);
			return true;
		}

		return false;
	}

	bool IsLyingDown()
	{
		// Check emote manager first
		if (m_Player.GetEmoteManager() != NULL && m_Player.GetEmoteManager().IsLyingDownZS())
			return true;

		// Then check action manager
		ActionManagerBase amb = m_Player.GetActionManager();
		if (!amb)
			return false;

		if (!amb.GetRunningAction())
			return false;

		AnimatedActionBase aab = AnimatedActionBase.Cast(amb.GetRunningAction());
		if (!aab)
			return false;

		if (aab.IsPlayerLyingDownZS(m_Player))
			return true;

		return false;
	}

	bool IsSleeping()
	{
		return IsLyingDown() && m_SleepCondition == ZenSleepState.ASLEEP;
	}

	float GetSleepStartFatigue()
	{
		return m_SleepStartFatigue;
	}

	float GetSecondsAsleep()
	{
		if (m_SleepStartTimestamp == -1)
			return 0;

		return (GetGame().GetTime() - m_SleepStartTimestamp) / 1000;
	}

	float GetSleepStartTimestamp()
	{
		return m_SleepStartTimestamp;
	}

	void Consume(PlayerConsumeData data)
	{
		if (IsAI() || !data || !data.m_Source)
			return;

		Edible_Base edibleItem = Edible_Base.Cast(data.m_Source);
		if (!edibleItem || !edibleItem.CanBeConsumed())
			return;

		float quantity = data.m_Amount / data.m_Source.GetQuantityMax();

		int i;
		string key;

		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Count(); i++)
		{
			key = GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.GetKey(i);
			if (edibleItem.IsKindOf(GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.GetKey(i)))
			{
				OnModifierActivate(GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.GetKey(i), quantity);
			}
		}

		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.Count(); i++)
		{
			key = GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.GetKey(i);
			if (edibleItem.IsKindOf(GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.GetKey(i)))
			{
				OnModifierDeactivate(GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.GetKey(i), quantity);
			}
		}
	}

	void OnModifierActivate(string className, float quantity = 1.0)
	{
		if (IsAI())
			return;

		if (className != "BloodRegenMdfr") // This spams the debug messages which is annoying.
			ZenSleepFunctions.DebugMessage("MODIFIER ACTIVATE: " + className + " x " + quantity);

		float gainAmount;
		if (GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Find(className, gainAmount))
		{
			m_Player.GetStatZenFatigue().Add(m_Player.GetStatZenFatigue().GetMax() * (gainAmount / 100) * quantity);
		}

		float pauseSecs;
		if (GetZenSleepConfig().ModifiersConfig.ModifierPausesSeconds.Find(className, pauseSecs))
		{
			if (m_PauseDrainEffectSecs < 0)
				m_PauseDrainEffectSecs = 0;

			m_PauseDrainEffectSecs += pauseSecs * quantity;

			if (m_PauseDrainEffectSecs > 0)
				SetSleepCondition(ZenSleepState.ON_DRUGS);
		}
	}

	void OnModifierDeactivate(string className, float quantity = 1.0)
	{
		if (IsAI())
			return;

		if (className != "BloodRegenMdfr") // This spams the debug messages which is annoying.
			ZenSleepFunctions.DebugMessage("MODIFIER DEACTIVATE: " + className + " x " + quantity);

		float drainAmount;
		if (GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.Find(className, drainAmount))
		{
			float drainFatigue = m_Player.GetStatZenFatigue().GetMax() * (drainAmount / 100) * quantity;
			m_Player.GetStatZenFatigue().Add(drainFatigue);

			// Reduce drugs effect proportionately (eg. if player injects a 100% sleep drain, reset the "buzz" state completely)
			if (m_PauseDrainEffectSecs > 0 && drainAmount < 0)
			{
				m_PauseDrainEffectSecs -= Math.AbsFloat(m_PauseDrainEffectSecs * (drainAmount / 100) * quantity);
				if (m_PauseDrainEffectSecs < 0)
					m_PauseDrainEffectSecs = 0;
			}
		}
	}

	float GetFatigueDrainPauseSeconds()
	{
		return m_PauseDrainEffectSecs;
	}

	bool IsFatigueDrainPaused()
	{
		return GetFatigueDrainPauseSeconds() > 0;
	}

	float GetInabilityToSleepEffectSecs()
	{
		return m_InabilityToSleepEffectSecs;
	}

	void SetInabilityToSleepEffectSecs(float secs)
	{
		m_InabilityToSleepEffectSecs = secs;
	}

	void SetFatigueSpeedModifier(float modifier)
	{
		m_FatigueSpeedModifier = modifier;
	}

	float GetFatigueSpeedModifier()
	{
		return m_FatigueSpeedModifier;
	}

	void SetFatigueSpeed(float speed)
	{
		m_FatigueSpeed = speed;
	}

	float GetFatigueSpeed()
	{
		return m_FatigueSpeed;
	}

	#ifdef ZENSLEEP_PERSISTENCE
	void SetPersistenceVariables(float drainEffectSecs, float inabilitySleepSecs)
	{
		m_PauseDrainEffectSecs = drainEffectSecs;
		m_InabilityToSleepEffectSecs = inabilitySleepSecs;
	}
	#endif
}