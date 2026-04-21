//! Credit to the Syberia Project dev team for most of this PPE codebase.
class PPERequester_ZenSleeping : PPERequester_GameplayBase
{
	private float m_SleepingOffset = 0;
	private float m_SleepingCurrent = 0;
	private float m_SleepingValue = 0;
	private float m_SleepingFxSpeed = 1.0;
	
	void SetVisualEffect(float eyesValue, float fxIntensity)
	{
		m_SleepingValue = eyesValue;
		m_SleepingFxSpeed = fxIntensity;
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop();

		m_SleepingOffset = 0;
		m_SleepingCurrent = 0;
		m_SleepingValue = 0;
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,0,PPEGlow.L_ZENSLEEP_BLINK,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_ZENSLEEP_BLINK,PPOperators.LOWEST);
	}	
	
	override void OnUpdate(float delta)
	{
		super.OnUpdate(delta);
		
		m_SleepingCurrent = m_SleepingCurrent + ((m_SleepingValue - m_SleepingCurrent) * delta * 0.1);	
		m_SleepingCurrent = Math.Clamp(m_SleepingCurrent, 0, 1);	

		if (m_SleepingCurrent == 0) 
		{
			m_SleepingOffset = Math.Clamp(m_SleepingOffset - (delta * 0.2 * m_SleepingFxSpeed), 0, 1);	
		}
		else
		{
			m_SleepingOffset = m_SleepingOffset + (delta * 0.2 * m_SleepingFxSpeed);

			if (m_SleepingOffset > Math.PI2) 
				m_SleepingOffset = 0;
		}	

		float sleepOffset = Math.Clamp(Math.Sin(m_SleepingOffset) * 0.2, 0, 0.2);
		float value = Math.Clamp((2 * m_SleepingCurrent) - sleepOffset, 0, 2);
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,value,PPEGlow.L_ZENSLEEP_BLINK,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_ZENSLEEP_BLINK,PPOperators.LOWEST);
	}
}