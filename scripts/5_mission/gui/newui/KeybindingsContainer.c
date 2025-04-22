// Disable admin sleep key binding if player is not an admin to avoid any confusion.
modded class KeybindingElementNew
{
	override void SetElementTitle( ButtonWidget btnWidget, UAInput pInput, int iDeviceFlags )
	{
		super.SetElementTitle(btnWidget, pInput, iDeviceFlags);

		if (pInput == GetUApi().GetInputByName("UAZenSleep_RefreshConfig"))
		{
			if (!ZenSleepConfig.IS_CLIENT_ADMIN)
			{
				btnWidget.Enable(false);
				btnWidget.SetText("ADMIN USE ONLY.");
			}
		}
	}
}