//! TODO: Needs major refactoring to allow support for future added visual effects if I decide to add more.
class ZenSleepVisualEffectHandlerClient extends ZenSleepEffectHandlerBase
{
	// Our client-side only visual effects
	protected ref PPERequester_ZenSleeping m_SleepingVFX;
	protected ref PPERequester_ZenHallucinate m_HallucinateVFX;
	protected ref PPERequester_ZenBlur m_BlurVFX;

	protected bool m_ClientVfxSetup; // Whether or not the FX classes have been initialized
	protected bool m_IsGoingUncon; // Whether or not the player is about to fall uncon

	protected float m_HallucinateFxValue; // The intensity value of the hallucination visual effect
	protected float m_HallucinateFxTargetValue; // The intensity target value of the hallucination effect
	protected float m_HallucinateFxSecs; // How long to display hallucination PPE effect
	protected float m_TimeSpentRedFlashing; // How long in seconds we've been at red flashing fatigue
	protected float m_TimeSpentRed; // How long in seconds we've been at red fatigue

	override void Init()
	{
		super.Init();

		m_ClientVfxSetup = false;
		m_IsGoingUncon = false;
	}

	void SetFxID(int fxID)
	{
		// Unpack data from the single int
		int arrayID; // sound, ignored here
		int arrayElement; // sound, ignored here
		int visualEffectID;
		int vanillaSoundEventID; // sound, ignored here
		ZenSleepFunctions.ReadPackedIntegerData4(fxID, arrayID, arrayElement, visualEffectID, vanillaSoundEventID);

		// Play any visual effects.
		PlayVisualEffect(visualEffectID);
	}

	void ~ZenSleepVisualEffectHandlerClient()
	{
		if (m_SleepingVFX)
			m_SleepingVFX.Stop();

		if (m_HallucinateVFX)
			m_HallucinateVFX.Stop();

		if (m_BlurVFX)
			m_BlurVFX.Stop();
	}

	override void Update(float deltaTime = 1)
	{
		// Don't update vfx if player is uncon
		if (m_Player.IsUnconscious())
		{
			m_IsGoingUncon = false;
			return;
		}

		if (!m_ClientVfxSetup)
			SetUpClientVFX();

		int fatigueState = m_Player.GetZenSleepManager().GetFatigueStatState() - 1;
		bool redFlashing = fatigueState == EStatLevels.CRITICAL; // 5 = red flashing critical
		bool red = fatigueState == EStatLevels.LOW; // 4 = red status
		bool allowHallucinate = (redFlashing && GetZenSleepConfig().ClientEffectsConfig.AllowHallucinationEffectRed) || (red && GetZenSleepConfig().ClientEffectsConfig.AllowHallucinationEffectRedFlashing);

		if (m_HallucinateFxSecs > 0)
		{
			if (allowHallucinate)
			{
				m_HallucinateFxValue = 0.1;
			}

			m_HallucinateFxSecs -= deltaTime;
		}
		else 
		{
			if (m_HallucinateFxValue > 0)
			{
				m_HallucinateFxValue -= deltaTime * 0.05;

				if (m_HallucinateFxValue < 0)
					m_HallucinateFxValue = 0;
			}
		}

		float blurFX = 0;
		float sleepFX = 0;
		float sleepFX_Speed = 1;

		if (!m_Player.GetZenSleepManager().IsSleeping())
			sleepFX_Speed = 3;

		if (m_Player.GetZenSleepManager().GetSleepCondition() != ZenSleepState.ON_DRUGS)
		{
			if (m_Player.GetZenSleepManager().IsSleeping() && GetZenSleepConfig().ClientEffectsConfig.AllowClosedEyesEffectSleeping)
			{
				sleepFX = 1.0;
			}
		
			if (redFlashing)
			{
				m_TimeSpentRedFlashing += deltaTime;
				float percentTimeSpentRedFlashing = Math.Clamp(m_TimeSpentRedFlashing / ZenSleepConstants.FATIGUE_MAX_FX_TIMER, 0.5, 1.0);

				if (GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRedFlashing > 0)
				{
					blurFX = (GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRedFlashing / 100) * percentTimeSpentRedFlashing;
				}

				if (!m_Player.GetZenSleepManager().IsSleeping() && GetZenSleepConfig().ClientEffectsConfig.AmountToVignetteWhenRedFlashing > 0)
				{
					if (!m_Player.GetZenSleepManager().IsSleeping())
					{
						sleepFX = (GetZenSleepConfig().ClientEffectsConfig.AmountToVignetteWhenRedFlashing / 100) * percentTimeSpentRedFlashing;
						sleepFX_Speed = 0.4;
					}
				}
			}
			else 
			{
				m_TimeSpentRedFlashing = 0;

				if (red)
				{
					m_TimeSpentRed += deltaTime;
					float percentTimeSpentRed = Math.Clamp(m_TimeSpentRed / ZenSleepConstants.FATIGUE_MAX_FX_TIMER, 0.5, 1.0);

					if (!m_Player.GetZenSleepManager().IsSleeping() && GetZenSleepConfig().ClientEffectsConfig.AmountToVignetteWhenRed > 0)
					{
						sleepFX = (GetZenSleepConfig().ClientEffectsConfig.AmountToVignetteWhenRed / 100) * percentTimeSpentRed;
						sleepFX_Speed = 0.25;
					}
				
					if (GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRed > 0)
						blurFX = (GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRed / 100) * percentTimeSpentRed;
				}
				else // Yellow or above
				{
					blurFX = 0;
					m_TimeSpentRed = 0;
				}
			}
		}
		else 
		{
			blurFX = 0;
			m_HallucinateFxValue = 0;
		}

		if (m_IsGoingUncon)
		{
			sleepFX = 1.0;
			sleepFX_Speed = 2.0;
		}

		if (GetZenSleepConfig().ClientEffectsConfig.AllowClosedEyesEffectSleeping || m_IsGoingUncon)
		{
			if (m_SleepingVFX)
			{
				if (!m_SleepingVFX.IsRequesterRunning())
					m_SleepingVFX.Start();

				m_SleepingVFX.SetVisualEffect(sleepFX, sleepFX_Speed);
				m_SleepingVFX.SetRequesterUpdating(true);
			}
		}
		else 
		{
			if (m_SleepingVFX && m_SleepingVFX.IsRequesterRunning())
				m_SleepingVFX.Stop();
		}

		if (allowHallucinate)
		{
			if (m_HallucinateVFX)
			{
				if (!m_HallucinateVFX.IsRequesterRunning())
					m_HallucinateVFX.Start();

				m_HallucinateVFX.SetVisualEffect(m_HallucinateFxValue);
				m_HallucinateVFX.SetRequesterUpdating(true);
			}
		}
		else
		{
			if (m_HallucinateVFX && m_HallucinateVFX.IsRequesterRunning())
				m_HallucinateVFX.Stop();
		}
		
		if (GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRed > 0 || GetZenSleepConfig().ClientEffectsConfig.AmountToBlurWhenRedFlashing > 0)
		{
			if (m_BlurVFX)
			{
				if (!m_BlurVFX.IsRequesterRunning())
					m_BlurVFX.Start();

				m_BlurVFX.SetVisualEffect(blurFX);
				m_BlurVFX.SetRequesterUpdating(true);
			}
			else 
			{
				if (m_BlurVFX && m_BlurVFX.IsRequesterRunning())
					m_BlurVFX.Stop();
			}
		}
		
	}

	//! TODO: Ideally I should array index the PPE effects and make activating them
	//		  an easy and modular thing to do based on fx ID. But it took me forever to get this first
	//		  effect to work, so I think I'll leave this as-is until I can be bothered adding more fx.
	void PlayVisualEffect(int id)
	{
		int fatigueState = m_Player.GetZenSleepManager().GetFatigueStatState() - 1;
		bool redFlashing = fatigueState == EStatLevels.CRITICAL; // 5 = red flashing critical
		bool red = fatigueState == EStatLevels.LOW; // 4 = red status
		bool canDoHallucination = false;

		if (red == GetZenSleepConfig().ClientEffectsConfig.AllowHallucinationEffectRed)
			canDoHallucination = true;

		if (redFlashing == GetZenSleepConfig().ClientEffectsConfig.AllowHallucinationEffectRedFlashing)
			canDoHallucination = true;

		// Hallucination effect.
		if ((id == ZenSleepConstants.SLEEP_VFX_HALLUCINATION_YAWN || id == ZenSleepConstants.SLEEP_VFX_HALLUCINATION_UNCON) && canDoHallucination)
			TriggerHallucinateFx(id);
	}

	protected void TriggerHallucinateFx(int hallucinationType)
	{
		if (m_HallucinateFxSecs > 0)
			return;

		if (hallucinationType == ZenSleepConstants.SLEEP_VFX_HALLUCINATION_UNCON) // uncon
			m_IsGoingUncon = true;

		m_HallucinateFxSecs = 10;
		m_HallucinateVFX.ResetVisualEffect();
		m_HallucinateVFX.Start();
	}

	protected void SetUpClientVFX()
	{
		m_TimeSpentRedFlashing = 0;

		m_SleepingVFX = PPERequester_ZenSleeping.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_ZEN_SLEEPING));
		m_SleepingVFX.Start();

		m_HallucinateVFX = PPERequester_ZenHallucinate.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_ZEN_HALLUCINATE));
		m_HallucinateVFX.Start();

		m_BlurVFX = PPERequester_ZenBlur.Cast(PPERequesterBank.GetRequester(PPERequesterBank.REQ_ZEN_BLUR));
		m_BlurVFX.Start();

		m_ClientVfxSetup = true;
	}
}