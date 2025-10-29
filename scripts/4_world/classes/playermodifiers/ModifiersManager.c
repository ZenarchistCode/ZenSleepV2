modded class ModifiersManager
{
	override void Init()
	{
		super.Init();

		#ifdef SERVER
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif
		#endif

		AddModifier(new ZenFatigueMdfr);
	}
}