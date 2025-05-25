modded class VPPAdminHud
{
	override void CreateSubMenu(typename subMenuType)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			super.CreateSubMenu(subMenuType);
			return;
		}
		#endif

		if (GetSubMenuByType(subMenuType) == NULL)
		{
			AdminHudSubMenu menu = AdminHudSubMenu.Cast(subMenuType.Spawn());

			if (subMenuType == MenuPlayerManager)
				menu.OnCreateZenSleep(layoutRoot); // Hijack this method so we don't have to call super.() to create the regular VPPAT menu.
			else
				menu.OnCreate(layoutRoot);

			menu.ShowSubMenu();
			M_SUB_MENUS.Insert(menu);
		}
	}
}