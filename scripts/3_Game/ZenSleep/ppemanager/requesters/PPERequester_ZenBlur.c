//! Credit to the Syberia Project dev team for most of this PPE codebase.
class PPERequester_ZenBlur : PPERequester_GameplayBase
{
	private float m_BlurValue = 0;
	private float m_BlurOffset = 0;
	
	void SetVisualEffect(float value)
	{
		m_BlurValue = value;
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop();

		m_BlurValue = 0;
		m_BlurOffset = 0;
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,false,0,PPEGaussFilter.L_ZENSLEEP_BLUR,PPOperators.HIGHEST);
	}	
	
	override void OnUpdate(float delta)
	{
		super.OnUpdate(delta);

		float blurEffect = 0;
		if (m_BlurValue > 0)
		{
			m_BlurOffset = m_BlurOffset + (delta * 0.1);

			if (m_BlurOffset >= Math.PI2)
				m_BlurOffset = 0;

			float sinVal = Math.Clamp(Math.Sin(m_BlurOffset) * 0.05, 0, 0.05);
			blurEffect = blurEffect + (m_BlurValue + sinVal);
		}
		
		blurEffect = Math.Clamp(blurEffect, 0, 1);
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,false,blurEffect,PPEGaussFilter.L_ZENSLEEP_BLUR,PPOperators.HIGHEST);
	}
}