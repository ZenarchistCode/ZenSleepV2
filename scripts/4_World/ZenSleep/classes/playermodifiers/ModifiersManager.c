modded class ModifiersManager
{
	override void Init()
	{
		super.Init();

		#ifdef SERVER
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif
		#endif

		AddModifier(new ZenFatigueMdfr);
	}
}