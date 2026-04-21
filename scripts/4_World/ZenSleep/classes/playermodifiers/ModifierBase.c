// Used to inform sleep manager of any activated/deactivated modifiers so we can apply 
// any config'd fatigue effects.
modded class ModifierBase
{
	override void Activate()
	{
		super.Activate();

		// Don't re-apply modifer config upon reconnection or players can abuse it by logging out/in repeatedly
		if (m_ActivationType != EActivationType.TRIGGER_EVENT_ON_CONNECT)
		{
			if (m_Player && m_Player.GetZenSleepManager())
			{
				m_Player.GetZenSleepManager().OnModifierActivate(ClassName());
			}
		}
	}

	override void Deactivate(bool trigger = true)
	{
		super.Deactivate(trigger);

		if (m_Player && m_Player.GetZenSleepManager())
		{
			m_Player.GetZenSleepManager().OnModifierDeactivate(ClassName());
		}
	}
}