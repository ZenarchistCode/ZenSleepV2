//! Credit to the Syberia Project dev team for most of this PPE codebase.
class PPERequester_ZenHallucinate : PPERequester_GameplayBase
{
	private float m_HallucinateValue = 0;
	private float m_HallucinateOffset = 0;

	void ResetVisualEffect()
	{
		m_HallucinateOffset = 0;
	}
	
	void SetVisualEffect(float value)
	{
		m_HallucinateValue = value;
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop();

		m_HallucinateValue = 0;
		m_HallucinateOffset = 0;

		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERX,false,0,PPEChromAber.L_ZENSLEEP_HALLUCINATE,PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERY,false,0,PPEChromAber.L_ZENSLEEP_HALLUCINATE,PPOperators.HIGHEST);
	}	
	
	float matChromaOffset;
	override void OnUpdate(float delta)
	{
		super.OnUpdate(delta);

		matChromaOffset = 0;

		if (m_HallucinateValue > 0)
		{
			m_HallucinateOffset = m_HallucinateOffset + (delta * 0.5);
			if (m_HallucinateOffset >= Math.PI2) 
				m_HallucinateOffset = 0;
			
			float sinVal = Math.Clamp(Math.Sin(m_HallucinateOffset), 0, 1);
			sinVal = ((m_HallucinateValue - 0.1) + (sinVal * 0.05));
			sinVal = Math.Clamp(sinVal, 0, 1);
			matChromaOffset = matChromaOffset + sinVal;
		}

		matChromaOffset = Math.Clamp(matChromaOffset, 0, 1);

		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERX,false,matChromaOffset,PPEChromAber.L_ZENSLEEP_HALLUCINATE,PPOperators.HIGHEST);
		SetTargetValueFloat(PostProcessEffectType.ChromAber,PPEChromAber.PARAM_POWERY,false,matChromaOffset,PPEChromAber.L_ZENSLEEP_HALLUCINATE,PPOperators.HIGHEST);
	}

	int GetDebugValue()
	{
		return m_HallucinateValue;
	}
}