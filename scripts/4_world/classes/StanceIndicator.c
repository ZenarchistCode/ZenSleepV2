// Override stance indicator to prone when lying down
modded class StanceIndicator
{
	override void DisplayStance(int stance)
	{
		super.DisplayStance(stance);

		if (m_Player && m_Player.GetZenSleepManager().IsLyingDown())
		{
			DisplayElementBase stance_element = m_Player.GetVirtualHud().GetElement(eDisplayElements.DELM_STANCE);
			if (stance_element)
			{
				stance_element.SetValue(3); // 3 = prone
			}
		}
	}
}