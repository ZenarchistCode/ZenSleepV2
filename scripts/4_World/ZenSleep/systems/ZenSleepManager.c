
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
	protected bool m_VehicleSleepActive;
	protected bool m_VehicleSleepLastSync;
	protected bool m_VehicleSleepRequested;
	protected bool m_VehicleSleepInputBlocked;
	protected int m_VehicleSleepRequestID;
	protected Transport m_SleepVehicle;
	protected int m_SleepVehicleSeat;
	protected float m_VehicleSleepHealth;
	protected float m_VehicleSleepBlood;
	protected float m_VehicleSleepShock;

	void ZenSleepManager(PlayerBase player)
	{
		m_Player = player;

		Init();
	}

	void Init()
	{
		#ifdef SERVER
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif
		m_EffectHandlerServer = new ZenSleepEffectHandlerServer(m_Player);
		#else
		m_VisualEffectHandlerClient = new ZenSleepVisualEffectHandlerClient(m_Player);
		#endif

		m_MovementState = new HumanMovementState();
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
		m_IsPlayerAI = m_Player.IsAI();
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

	// Check if player has used a bed to sleep on, and the bed is not wetter than "excludeDampState" or more damaged than "excludeDamageState"
	bool HasBed(float excludeDampState = GameConstants.STATE_WET, int excludeDamageState = GameConstants.STATE_BADLY_DAMAGED)
	{
		ItemBase bedItem = ItemBase.Cast(GetBed());

		if (bedItem)
		{
			if (bedItem.GetWet() >= excludeDampState)
			{
				return false;
			}
			
			if (bedItem.GetHealthLevel() >= excludeDamageState)
			{
				return false;
			}
		}

		return GetBed() != NULL;
	}

	// Quickest vanilla way to detect being "inside" - will also return true for some tents, vehicles etc. Can be a bit hit and miss but hey, it's DayZ, what isn't?
	bool IsSheltered()
	{
		return m_Player && m_Player.IsSoundInsideBuilding();
	}

	int GetPlayerMovementState()
	{
		if (!m_Player)
			return -1;

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

	// 1 = full white, 2 = gapped, 3 = yellow, 4 = red, 5 = red flashing
	int GetFatigueStatState()
	{
		return m_StatFatigueState;
	}

	void SetSleepCondition(int condition, string notification = "")
	{
		if (m_VehicleSleepActive && condition == ZenSleepState.ASLEEP && m_SleepCondition != condition)
			DebugVehicleSleep("Now asleep; normal recovery and sleep effects enabled.");

		bool stopVehicleSleep = m_VehicleSleepActive && condition != ZenSleepState.ASLEEP;
		if (stopVehicleSleep)
		{
			DebugVehicleSleep("Vehicle sleep ended; new state=" + condition);
			m_VehicleSleepActive = false;
			m_SleepVehicle = null;
			OnStopSleep();
		}

		m_SleepCondition = condition;

		if (m_SleepCondition == ZenSleepState.AWAKE)
		{
			m_SleepStartTimestamp = -1;

			if (g_Game.IsClient())
				StopAudioAttenuation();
		}

		if (stopVehicleSleep && g_Game.IsServer())
			DoServerToClientSync(true);

		if (notification != "" && GetZenSleepConfig().GeneralSleepConfig.DisplaySleepMessages)
			ZenSleepFunctions.ServerMessage(m_Player, notification);
	}

	int GetSleepCondition()
	{
		return m_SleepCondition;
	}

	bool RequiresServerToClientSync()
	{
		if (m_VehicleSleepActive != m_VehicleSleepLastSync)
			return true;

		if (m_StatFatigueTendency != m_StatFatigueTendencyLastSync)
			return true;

		if (m_StatFatigueState != m_StatFatigueStateLastSync)
			return true;

		if (m_SleepCondition != m_SleepConditionLastSync)
			return true;

		return false;
	}

	protected void DoServerToClientSync(bool force = false)
	{
		if (!m_Player || !m_Player.GetIdentity())
			return;

		if (!force && !RequiresServerToClientSync())
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
		rpc.Write(m_VehicleSleepActive);
		rpc.Write(m_VehicleSleepRequestID);
		rpc.Send(m_Player, ZenSleepEnums.RPC_SLEEP_STAT_UPDATE, true, m_Player.GetIdentity());

		// Sync latest values
		m_VehicleSleepLastSync = m_VehicleSleepActive;
		m_StatFatigueTendencyLastSync = m_StatFatigueTendency;
		m_StatFatigueStateLastSync = m_StatFatigueState;
		m_SleepConditionLastSync = m_SleepCondition;
	}

	void DoClientFromServerSync(int data, bool vehicleSleep = false, int requestID = 0)
	{
		// A wake request may overtake an earlier start acknowledgement on the round trip.
		if (requestID < m_VehicleSleepRequestID)
			return;

		if (m_VehicleSleepRequested || m_VehicleSleepActive != vehicleSleep)
			DebugVehicleSleep("Server state received: vehicleSleep=" + vehicleSleep + ", request=" + requestID);

		bool wasVehicleSleeping = m_VehicleSleepActive;
		bool wasVehicleRequested = IsVehicleSleepRequested();
		m_VehicleSleepRequestID = requestID;
		m_VehicleSleepRequested = false;
		m_VehicleSleepActive = vehicleSleep;

		// Read all 4 relevant fatigue stat info from single integer
		ZenSleepFunctions.ReadPackedIntegerData3(data, m_StatFatigueTendency, m_StatFatigueState, m_SleepCondition);

		if (m_SleepCondition != m_SleepConditionLastSync)
		{
			m_SleepConditionLastSync = m_SleepCondition;

			if (m_SleepCondition == ZenSleepState.AWAKE)
				OnStopSleep();
		}

		if (vehicleSleep && !wasVehicleSleeping)
			OnStartSleep();
		else if (!vehicleSleep && wasVehicleRequested)
		{
			OnStopSleep();
			ClearVehicleSleepEffects();
		}
	}

	void OnStartSleep()
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled() || !m_Player || !m_Player.GetStatZenFatigue())
			return;

		// This is a start limit, not a cap on recovery once already asleep.
		if (ZenSleepCompatibility.OverridesTerje() && g_Game.IsDedicatedServer() && GetFatiguePercent() >= GetZenSleepConfig().GeneralSleepConfig.MaxFatiguePercentToAllowSleeping)
		{
			SetSleepCondition(ZenSleepState.AWAKE, GetZenSleepConfig().Messages.CantSleep + " " + GetZenSleepConfig().Messages.TooAwake);
			return;
		}

		m_SleepStartTimestamp = g_Game.GetTime();
		m_SleepStartFatigue = m_Player.GetStatZenFatigue().Get();

		if (g_Game.IsClient())
		{
			if (GetZenSleepConfig().ClientEffectsConfig && GetZenSleepConfig().ClientEffectsConfig.DelaySecsUntilAudioAttenuation > 0)
			{
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ApplyAudioAttenuation, GetZenSleepConfig().ClientEffectsConfig.DelaySecsUntilAudioAttenuation * 1000);
			}

			//LockCameraLimits();
		}

		// Fail-safe check for a nearby bed within 1 meter if player forgot to use F to lie down on the object itself
		if (g_Game.IsDedicatedServer())
		{
			if (!m_VehicleSleepActive && !GetBed())
			{
				array<Object> nearest_objects = new array<Object>;
				g_Game.GetObjectsAtPosition3D(m_Player.GetPosition(), 1.0, nearest_objects, NULL);

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
			#ifdef TERJE_MEDICINE_MOD
			// Reject illness/stimulant and Zen environmental blocks as soon as sleep is requested.
			if (ZenSleepCompatibility.OverridesTerje())
				CheckWakeUpConditions();
			#endif
		}
	}

	bool CanSleepOnBedObject(Object object)
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled() || !object)
			return false;

		#ifdef TERJE_MEDICINE_MOD
		TerjeSleepingBag terjeBag = TerjeSleepingBag.Cast(object);
		if (terjeBag)
			return terjeBag.CanBePacked() && !terjeBag.IsRuined();
		#endif

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
			return false;
		#endif

		if (!m_Player || !GetZenSleepConfig().ClientEffectsConfig)
			return false;

		foreach (string s : GetZenSleepConfig().ClientEffectsConfig.BedObjects)
		{
			if (object.IsKindOf(s))
			{
				ZenSleepingBagDeployed_Base bag = ZenSleepingBagDeployed_Base.Cast(object);
				if (bag && (bag.IsRuined() || vector.Distance(bag.GetPosition(), m_Player.GetPosition()) > 0.5))
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
		if (g_Game.IsClient())
		{
			StopAudioAttenuation();

			//UnlockCameraLimits();
		}

		if (g_Game.IsDedicatedServer())
		{
			SetBed(NULL);
		}

		m_SleepStartTimestamp = -1;
	}

	void ApplyAudioAttenuation()
	{
		if (ZenSleepCompatibility.IsZenSleepEnabled() && m_Player && m_Player.GetMasterAttenuation() == "")
			m_Player.SetMasterAttenuation("ZenSleepAttenuation");
	}

	void StopAudioAttenuation()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ApplyAudioAttenuation);

		if (m_Player && m_Player.GetMasterAttenuation() == "ZenSleepAttenuation") // check master atten hasn't been overridden (eg. flashbang'd/burlapped)
			m_Player.SetMasterAttenuation("");
	}

	void LockCameraLimits()
	{
		if (!m_Player)
			return;

		//! TODO: Come back to this, can't seem to get this to work after half an hour of screwing around.
		// I wanted to lock the camera view so you can't go all fuckin Chucky 360 head spins while asleep
		m_Player.SetLookLimits(30, -30, -45, 45);
		m_Player.SetAimLimits(30, -30, -45, 45);
	}

	void UnlockCameraLimits()
	{
		if (!m_Player)
			return;

		m_Player.SetLookLimits(-85, 85, -160, 160);
		m_Player.SetAimLimits(-85, 85, -180, 180);
	}

	void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		float damagePct = damageResult.GetDamage("", "Health") / 100;

		// Acts like drug effect but without any benefits - if player takes damage, stop them from sleeping for a little while due to adrenaline
		if (damagePct > 0 && GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBuzzedByCombat > 0 && m_InabilityToSleepEffectSecs < GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBuzzedByCombat)
		{
			m_InabilityToSleepEffectSecs += GetZenSleepConfig().ServerEffectsConfig.MaximumSecsBuzzedByCombat * damagePct;
		}
	}

	void OnUpdateTickShared(float deltaTime)
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled() || IsAI())
		{
			return;
		}

		#ifdef SERVER 
		OnUpdateTickServer(deltaTime);
		#else
		if (m_Player && m_Player.IsControlledPlayer())
		{
			OnUpdateTickClient(deltaTime);
		}
		#endif
	}

	// Server-side player checks
	protected void OnUpdateTickServer(float deltaTime)
	{
		// Vehicle interruptions are checked once per frame by PlayerBase.CommandHandler.
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
		{
			m_InabilityToSleepEffectSecs -= deltaTime;
		}

		// If player is incapacitated, no need to update anything right now.
		if (!m_Player || !m_Player.IsAlive() || !m_Player.GetEmoteManager() || m_Player.IsUnconscious()) 
			return;

		// Handle server-side sound & visual syncs in separate class
		GetEffectHandlerServer().Update(deltaTime);

		// Throttle server checks which don't need to be checked every tick
		m_DeltaTime += deltaTime;
		if (m_DeltaTime < ZenSleepConstants.SLEEP_UPDATE_TICK_SECS)
			return;
		m_DeltaTime = 0;

		// Check if we're lying down.
		if (!IsInSleepPosition() && GetSleepCondition() != ZenSleepState.ON_DRUGS)
		{
			SetSleepCondition(ZenSleepState.AWAKE);
			return;
		}

		// Lying down, in sleep state
		if (GetSleepCondition() == ZenSleepState.ASLEEP)
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
		if (!m_Player)
			return 0;

		return (m_Player.GetStatZenFatigue().Get() / m_Player.GetStatZenFatigue().GetMax()) * 100;
	}

	float GetPlayerTemperature()
	{
		if (!m_Player)
			return 0;

		return m_Player.GetStatHeatComfort().Get();
	}

	float GetPlayerHeatBuffPercent()
	{
		if (!m_Player)
			return 0;

		return (m_Player.GetStatHeatBuffer().Get() / m_Player.GetStatHeatBuffer().GetMax()) * 100;
	}

	float GetPlayerWetness()
	{
		if (!m_Player)
			return 0;

		return m_Player.GetStatWet().Get();
	}

	float GetPlayerWeightPercent()
	{
		if (!m_Player)
			return 0;

		float weight = m_Player.GetWeightEx();
		float staminaImpact = Math.Max((CfgGameplayHandler.GetStaminaMax() - (((weight - CfgGameplayHandler.GetStaminaWeightLimitThreshold()) / GameConstants.STAMINA_KG_TO_GRAMS) * CfgGameplayHandler.GetStaminaKgToStaminaPercentPenalty())), CfgGameplayHandler.GetStaminaMinCap());
		return 100 - Math.Clamp(staminaImpact, 0, 100);
	}

	float GetPlayerHunger()
	{
		if (!m_Player)
			return 0;

		return m_Player.GetStatEnergy().Get();
	}

	float GetPlayerThirst()
	{
		if (!m_Player)
			return 0;

		return m_Player.GetStatWater().Get();
	}

	bool IsPlayerSick()
	{
		if (!m_Player)
			return 0;

		return m_Player.HasDisease() && !DoesPlayerOnlyHaveKuru();
	}

	// Check if player's only disease is kuru. I don't want to prevent cannibals from sleeping, that would be inhumane.
	bool DoesPlayerOnlyHaveKuru()
	{
		if (!m_Player)
			return false;

		return m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_BRAIN) && m_Player.m_DiseaseCount == 1;
	}

	bool IsNightTime()
	{
		return g_Game.GetWorld().IsNight();
	}

	void SendVanillaSleepSoundset(string soundset)
	{
		if (!m_Player)
			return;

		int soundsetArrayID = ZenSleepSounds.GetUniVanillaSounds().Find(soundset);

		if (soundsetArrayID != -1)
		{
			m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(5, soundsetArrayID, 0, 0));
		}
	}

	void SendVanillaSleepSoundEventID(int id)
	{
		if (!m_Player)
			return;

		if (id < 0 || id > (EPlayerSoundEventID.ENUM_COUNT - 1))
		{
			Error("[ZenSleepManager] Send vanilla sleep sound enum ID out of bounds = " + id);
			return;
		}

		m_Player.SetZenSleepFxID(ZenSleepFunctions.GetPackedIntegerData4(0, 0, 0, id));
	}

	// This method will check all the possible ways we can be woken up.
	// It will select whichever wake-up call is triggered by the lowest fatigue level.
	bool CheckWakeUpConditions(bool debugVehicle = false)
	{
		if (!m_Player)
			return false;

		#ifdef TERJE_MEDICINE_MOD
		if (ZenSleepCompatibility.OverridesTerje())
		{
			int terjeBlock = ZenSleepTerje.GetSleepBlock(m_Player);
			if (terjeBlock != TerjeMedicineSleepingLevel.TERJESL_NONE)
			{
				string terjeMessage = GetZenSleepConfig().Messages.TooBuzzed;
				if (terjeBlock == TerjeMedicineSleepingLevel.TERJESL_SICK)
					terjeMessage = GetZenSleepConfig().Messages.TooSick;
				else if (!ZenSleepTerje.HasStimulant(m_Player))
					terjeMessage = GetZenSleepConfig().Messages.TooAwake;
				if (debugVehicle || m_VehicleSleepActive)
					DebugVehicleSleep("Action blocked by sleep rules: " + terjeMessage);
				SetSleepCondition(ZenSleepState.AWAKE, GetZenSleepConfig().Messages.CantSleep + " " + terjeMessage);
				return true;
			}
		}
		#endif

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
		if (ZenSleepCompatibility.OverridesTerje())
			tooEnergized = playerFatigue >= 100 || (isNight && playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Time_Night);

		// On drugs (ie. someone dosed us while sleeping the cheeky bastards)
		if (IsFatigueDrainPaused() || m_InabilityToSleepEffectSecs > 0)
		{
			restMessage = GetZenSleepConfig().Messages.TooBuzzed;
			SendVanillaSleepSoundEventID(EPlayerSoundEventID.SYMPTOM_GASP);
			Print("[ZenSleep] Player " + m_Player.GetIdentity().GetId() + " too buzzed - drainPaused=" + IsFatigueDrainPaused() + " inabilityToSleepSecs=" + m_InabilityToSleepEffectSecs);
		}

		// Too energized
		if (restMessage == "" && tooEnergized)
		{
			restMessage = GetZenSleepConfig().Messages.TooAwake;
			SendVanillaSleepSoundEventID(EPlayerSoundEventID.RELEASE_BREATH);
		}

		if (restMessage == "")
		{
			if (!isNight)
			{
				if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Time_Daylight && m_Player.m_UndergroundPresence != EUndergroundPresence.FULL)
				{
					restMessage = GetZenSleepConfig().Messages.TooSunny;
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.RELEASE_BREATH);
				}
			}
		}

		// Sick
		if (restMessage == "" && playerSick)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Illness)
			{
				restMessage = GetZenSleepConfig().Messages.TooSick;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.SYMPTOM_COUGH);
			}
		}

		// Wet 
		if (restMessage == "" && (playerWet > 0))
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Wet)
			{
				restMessage = GetZenSleepConfig().Messages.TooWet;
				SendVanillaSleepSoundset("WringClothes_SoundSet");
			}
		}
		
		// Dark blue flashing cold
		if (restMessage == "" && playerTemp <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_DarkBlueFlashing)
			{
				restMessage = GetZenSleepConfig().Messages.TooCold;
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
					restMessage = GetZenSleepConfig().Messages.TooCold;
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.RATTLING_TEETH);
				}
			}

			// Light blue cold
			if (restMessage == "" && playerTemp <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING)
			{
				if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_LightBlue)
				{
					restMessage = GetZenSleepConfig().Messages.TooCold;
					SendVanillaSleepSoundEventID(EPlayerSoundEventID.FREEZING);
				}
			}
		}

		// Red flashing temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_RedFlashing)
			{
				restMessage = GetZenSleepConfig().Messages.TooHot;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Red temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_Red)
			{
				restMessage = GetZenSleepConfig().Messages.TooHot;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Yellow temperature
		if (restMessage == "" && playerTemp >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_Temp_Yellow)
			{
				restMessage = GetZenSleepConfig().Messages.TooHot;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.HOT);
			}
		}

		// Red flashing food
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_RedFlashing)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet"); // why is this the only one not a sound event ID bohemia?? Geez looeyz
			}
		}

		// Red flashing water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_CRITICAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_RedFlashing)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// Red food
		if (restMessage == "" && playerHunger <= PlayerConstants.SL_ENERGY_LOW)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Red)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet");
			}
		}

		// Red water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_LOW)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Red)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// Yellow food
		if (restMessage == "" && playerHunger <= PlayerConstants.SL_ENERGY_NORMAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Yellow)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundset("hungry_uni_Voice_Char_SoundSet");
			}
		}

		// Yellow water 
		if (restMessage == "" && playerThirst <= PlayerConstants.SL_WATER_NORMAL)
		{
			if (playerFatigue >= GetZenSleepConfig().GainConfig.MaxRestGain_FoodWater_Yellow)
			{
				restMessage = GetZenSleepConfig().Messages.TooHungryThirsty;
				SendVanillaSleepSoundEventID(EPlayerSoundEventID.THIRST);
			}
		}

		// If we have a rest message, it's time to wake up. Otherwise we keep on sleeping.
		if (restMessage != "")
		{
			if (debugVehicle || m_VehicleSleepActive)
				DebugVehicleSleep("Action blocked by sleep rules: " + restMessage);
			int sleepState = ZenSleepState.AWAKE;
			if (IsFatigueDrainPaused())
				sleepState = ZenSleepState.ON_DRUGS;

			SetSleepCondition(sleepState, GetZenSleepConfig().Messages.CantSleep + " " + restMessage);
			return true;
		}

		return false;
	}

	// Vehicle rest shares the normal sleep loop without pretending to play a lie-down emote.
	bool IsInSleepPosition()
	{
		return IsLyingDown() || (m_VehicleSleepActive && CanSleepInVehicle());
	}

	bool IsVehicleSleepActive()
	{
		return m_VehicleSleepActive;
	}

	bool IsVehicleSleepRequested()
	{
		return m_VehicleSleepRequested || m_VehicleSleepActive;
	}

	// Event-only diagnostics: no player-list broadcast and no dependency on ZenCore's debug toggle.
	void DebugVehicleSleep(string message)
	{
		if (!GetZenSleepConfig().DebugMode)
			return;

		string prefix = "[ZenSleep vehicle][CLIENT] ";
		if (g_Game.IsServer())
			prefix = "[ZenSleep vehicle][SERVER] ";
		Print(prefix + message);
		if (g_Game.IsServer())
			ZenSleepFunctions.ServerMessage(m_Player, prefix + message);
		else if (m_Player && m_Player.IsControlledPlayer())
			ZenFunctions.ZenClientMessage(prefix + message);
	}

	protected bool VehicleSleepBlocked(bool explain, string reason)
	{
		if (explain)
			DebugVehicleSleep("Action blocked: " + reason);
		return false;
	}

	bool CanSleepInVehicle(bool explain = false)
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled())
			return VehicleSleepBlocked(explain, "ZenSleep disabled by ZenModPack selector or Terje override setting");
		if (!GetZenSleepConfig().GeneralSleepConfig.AllowVehicleSleep)
			return VehicleSleepBlocked(explain, "GeneralSleepConfig.AllowVehicleSleep is disabled");
		if (IsAI())
			return VehicleSleepBlocked(explain, "AI player");
		if (!m_Player || !m_Player.IsAlive() || m_Player.IsPlayerDisconnected())
			return VehicleSleepBlocked(explain, "player missing, dead or disconnected");
		if (m_Player.IsUnconscious())
			return VehicleSleepBlocked(explain, "player unconscious");

		HumanCommandVehicle command = m_Player.GetCommand_Vehicle();
		if (!command)
			return VehicleSleepBlocked(explain, "player is not in a vehicle seat command");
		if (command.IsGettingIn() || command.IsGettingOut() || command.IsSwitchSeat())
			return VehicleSleepBlocked(explain, "entering, leaving or switching vehicle seats");

		Transport transport = command.GetTransport();
		if (!transport || transport.CrewMemberIndex(m_Player) < 0)
			return VehicleSleepBlocked(explain, "vehicle or crew seat unavailable");

		if (IsVehicleSleepDriver())
		{
			// Ignore normal parked-car physics jitter; use the same 1 km/h limit for starting and waking.
			float speedKmh = GetVelocity(transport).Length() * 3.6;
			if (speedKmh > 1.0)
			{
				if (explain && GetZenSleepConfig().DebugMode)
					DebugVehicleSleep("Action blocked: car speed=" + speedKmh + " km/h (driver sleep limit: 1 km/h)");
				return false;
			}
		}

		return true;
	}

	bool IsVehicleSleepDriver()
	{
		HumanCommandVehicle command = m_Player.GetCommand_Vehicle();
		if (!command || !command.GetTransport())
			return false;

		return command.GetTransport().CrewMemberIndex(m_Player) == 0 || command.GetVehicleSeat() == DayZPlayerConstants.VEHICLESEAT_DRIVER;
	}

	// The native action carries this ID to the server; no start RPC bypasses it.
	int PrepareVehicleSleepRequest()
	{
		m_VehicleSleepRequestID++;
		m_VehicleSleepRequested = true;
		// Immediate local feedback, without starting fatigue recovery before server approval/delay.
		if (GetVisualEffectHandlerClient())
			GetVisualEffectHandlerClient().Update(0);
		DebugVehicleSleep("Native action prepared; sending start request=" + m_VehicleSleepRequestID);
		return m_VehicleSleepRequestID;
	}

	bool IsVehicleSleepInputBlocked()
	{
		return m_VehicleSleepInputBlocked;
	}

	void ReleaseVehicleSleepInput()
	{
		m_VehicleSleepInputBlocked = false;
	}

	void CancelVehicleSleepRequest(int requestID)
	{
		if (requestID == m_VehicleSleepRequestID && IsVehicleSleepRequested())
			RequestVehicleWake("native action cancelled before client acknowledgement");
	}

	void RequestVehicleWake(string reason = "wake requested")
	{
		if (!g_Game.IsClient() || !m_Player || !m_Player.IsControlledPlayer() || !IsVehicleSleepRequested())
			return;

		DebugVehicleSleep("Wake requested: " + reason + ", previous request=" + m_VehicleSleepRequestID);
		m_VehicleSleepRequestID++;
		m_VehicleSleepRequested = false;
		m_VehicleSleepInputBlocked = true;
		m_VehicleSleepActive = false;
		OnStopSleep();
		SetSleepCondition(ZenSleepState.AWAKE);
		ClearVehicleSleepEffects();

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(m_VehicleSleepRequestID);
		rpc.Send(m_Player, ZenSleepEnums.RPC_VEHICLE_SLEEP_WAKE, true);
	}

	void HandleVehicleSleepRequest(bool start, int requestID)
	{
		if (!g_Game.IsServer())
			return;
		if (requestID <= m_VehicleSleepRequestID)
		{
			DebugVehicleSleep("Request ignored: stale/cancelled request=" + requestID + ", latest=" + m_VehicleSleepRequestID);
			return;
		}

		DebugVehicleSleep("Request received: start=" + start + ", request=" + requestID);
		m_VehicleSleepRequestID = requestID;
		if (!start && m_VehicleSleepActive)
		{
			// SetSleepCondition sends the wake acknowledgement; avoid a duplicate RPC.
			StopVehicleSleep("client wake request");
			return;
		}
		if (start && m_VehicleSleepActive)
			DebugVehicleSleep("Action blocked: already sleeping in vehicle");
		if (start && !m_VehicleSleepActive && CanSleepInVehicle(true))
		{
			if (!m_Player.GetStatZenFatigue())
				DebugVehicleSleep("Action blocked: Zen fatigue stat is unavailable");
			else
			{
				SetBed(null);
				if (GetFatiguePercent() >= GetZenSleepConfig().GeneralSleepConfig.MaxFatiguePercentToAllowSleeping)
				{
					DebugVehicleSleep("Action blocked: too awake (fatigue=" + GetFatiguePercent() + ", sleep limit=" + GetZenSleepConfig().GeneralSleepConfig.MaxFatiguePercentToAllowSleeping + ")");
					SetSleepCondition(ZenSleepState.AWAKE, GetZenSleepConfig().Messages.CantSleep + " " + GetZenSleepConfig().Messages.TooAwake);
				}
				else if (!CheckWakeUpConditions(true))
				{
					m_VehicleSleepActive = true;
					m_SleepVehicle = m_Player.GetCommand_Vehicle().GetTransport();
					m_SleepVehicleSeat = m_SleepVehicle.CrewMemberIndex(m_Player);
					m_VehicleSleepHealth = m_Player.GetHealth("", "Health");
					m_VehicleSleepBlood = m_Player.GetHealth("", "Blood");
					m_VehicleSleepShock = m_Player.GetHealth("", "Shock");
					OnStartSleep();
					if (m_VehicleSleepActive)
						DebugVehicleSleep("Sleep accepted; waiting for the normal 5-second sleep delay.");
				}
			}
		}

		// Also acknowledge rejected starts and cancelled requests.
		DoServerToClientSync(true);
	}

	void StopVehicleSleep(string reason = "sleep condition changed")
	{
		if (!m_VehicleSleepActive)
			return;

		DebugVehicleSleep("Wake forced: " + reason);
		int condition = ZenSleepState.AWAKE;
		if (IsFatigueDrainPaused())
			condition = ZenSleepState.ON_DRUGS;
		SetSleepCondition(condition);
	}

	void InterruptVehicleSleep(string reason = "sleep interrupted")
	{
		if (g_Game.IsServer())
			StopVehicleSleep(reason);
		else
			RequestVehicleWake(reason);
	}

	void UpdateVehicleSleepServer()
	{
		if (!g_Game.IsServer() || !m_VehicleSleepActive)
			return;

		if (!CanSleepInVehicle(true))
		{
			StopVehicleSleep("vehicle eligibility lost");
			return;
		}

		Transport transport = m_Player.GetCommand_Vehicle().GetTransport();
		float health = m_Player.GetHealth("", "Health");
		float blood = m_Player.GetHealth("", "Blood");
		float shock = m_Player.GetHealth("", "Shock");
		// Crashes and other scripts can subtract stats directly without calling EEHitBy.
		if (transport != m_SleepVehicle || transport.CrewMemberIndex(m_Player) != m_SleepVehicleSeat || health < m_VehicleSleepHealth || blood < m_VehicleSleepBlood || shock < m_VehicleSleepShock)
			StopVehicleSleep("vehicle/seat changed or health, blood or shock decreased");

		m_VehicleSleepHealth = health;
		m_VehicleSleepBlood = blood;
		m_VehicleSleepShock = shock;
	}

	void ClearVehicleSleepEffects()
	{
		if (GetVisualEffectHandlerClient())
			GetVisualEffectHandlerClient().ClearVehicleSleepEffects();
	}

	bool IsLyingDown()
	{
		// Check emote manager first
		if (!m_Player || (m_Player.GetEmoteManager() != NULL && m_Player.GetEmoteManager().IsLyingDownZS()))
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
		bool sleeping = IsInSleepPosition() && m_SleepCondition == ZenSleepState.ASLEEP;
		#ifdef TERJE_MEDICINE_MOD
		// Terje's medical code must never observe valid sleep while its current
		// illness rules block it, even between Zen's scheduled wake-up checks.
		if (sleeping && ZenSleepCompatibility.OverridesTerje() && g_Game.IsDedicatedServer())
			return !m_Player.IsUnconscious() && ZenSleepTerje.GetSleepBlock(m_Player) == TerjeMedicineSleepingLevel.TERJESL_NONE;
		#endif
		return sleeping;
	}

	float GetSleepStartFatigue()
	{
		return m_SleepStartFatigue;
	}

	float GetSecondsAsleep()
	{
		if (m_SleepStartTimestamp == -1)
			return 0;

		return (g_Game.GetTime() - m_SleepStartTimestamp) / 1000;
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

		ApplyConfiguredItemEffects(edibleItem, quantity);
	}

	bool HasConfiguredItemEffect(ItemBase item)
	{
		if (!item)
			return false;

		int i;
		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Count(); i++)
			if (item.IsKindOf(GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.GetKey(i)))
				return true;
		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.Count(); i++)
			if (item.IsKindOf(GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.GetKey(i)))
				return true;
		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierPausesSeconds.Count(); i++)
			if (item.IsKindOf(GetZenSleepConfig().ModifiersConfig.ModifierPausesSeconds.GetKey(i)))
				return true;

		return false;
	}

	void ApplyConfiguredItemEffects(ItemBase item, float quantity)
	{
		int i;
		string key;
		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Count(); i++)
		{
			key = GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.GetKey(i);
			if (item.IsKindOf(key))
				OnModifierActivate(key, quantity);
		}

		// A pause-only JSON entry is also an explicit override, including zero.
		if (ZenSleepCompatibility.OverridesTerje())
		{
			for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierPausesSeconds.Count(); i++)
			{
				key = GetZenSleepConfig().ModifiersConfig.ModifierPausesSeconds.GetKey(i);
				if (!GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Contains(key) && item.IsKindOf(key))
					OnModifierActivate(key, quantity);
			}
		}

		for (i = 0; i < GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.Count(); i++)
		{
			key = GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.GetKey(i);
			if (item.IsKindOf(key))
				OnModifierDeactivate(key, quantity);
		}
	}

	void OnModifierActivate(string className, float quantity = 1.0, bool skipSleepGain = false)
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled() || !m_Player || IsAI() || !m_Player.GetStatZenFatigue())
			return;

		if (skipSleepGain && ZenSleepCompatibility.OverridesTerje())
			return;

		#ifdef ZenSleepDebug
		if (className != "BloodRegenMdfr") // This spams the debug messages which is annoying.
			ZenSleepFunctions.DebugMessage("MODIFIER ACTIVATE: " + className + " x " + quantity);
		#endif

		float gainAmount;
		if (!skipSleepGain && GetZenSleepConfig().ModifiersConfig.ModifierActivatedPercent.Find(className, gainAmount))
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

	void OnModifierDeactivate(string className, float quantity = 1.0, bool skipSleepGain = false)
	{
		if (!ZenSleepCompatibility.IsZenSleepEnabled() || !m_Player || IsAI())
			return;

		if (skipSleepGain && ZenSleepCompatibility.OverridesTerje())
			return;

		#ifdef ZenSleepDebug
		if (className != "BloodRegenMdfr") // This spams the debug messages which is annoying.
			ZenSleepFunctions.DebugMessage("MODIFIER DEACTIVATE: " + className + " x " + quantity);
		#endif

		float drainAmount;
		if (GetZenSleepConfig().ModifiersConfig.ModifierDeactivatedPercent.Find(className, drainAmount))
		{
			if (m_Player.GetStatZenFatigue() == null)
				return;
				
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

	void SetPersistenceVariables(float drainEffectSecs, float inabilitySleepSecs)
	{
		m_PauseDrainEffectSecs = drainEffectSecs;
		m_InabilityToSleepEffectSecs = inabilitySleepSecs;
	}
}
