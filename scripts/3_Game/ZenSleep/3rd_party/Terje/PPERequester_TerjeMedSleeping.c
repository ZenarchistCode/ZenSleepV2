#ifdef TERJE_MEDICINE_MOD
modded class PPERequester_TerjeMedSleeping
{
	override void OnUpdate(float delta)
	{
		if (ZenSleepCompatibility.OverridesTerje())
		{
			// Clear the existing layers as well as preventing subsequent Terje sleep VFX.
			OnStop();
			return;
		}

		super.OnUpdate(delta);
	}
}
#endif
