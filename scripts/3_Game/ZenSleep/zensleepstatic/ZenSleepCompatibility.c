// Shared by Game, World and Mission. The server syncs the selector with client effects.
class ZenSleepCompatibility
{
	static bool IsZenSleepEnabled()
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSleep"))
			return false;
		#endif
		#ifdef TERJE_MEDICINE_MOD
		return GetZenSleepConfig().OverrideTerje;
		#else
		return true;
		#endif
	}

	static bool OverridesTerje()
	{
		#ifdef TERJE_MEDICINE_MOD
		return IsZenSleepEnabled();
		#else
		return false;
		#endif
	}
}
