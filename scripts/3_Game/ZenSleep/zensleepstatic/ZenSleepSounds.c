class ZenSleepSounds
{
	protected static ref array<string> m_MaleYawnSoundsets =
	{
		"ZenSleep_Yawn1_Male_SoundSet",
		"ZenSleep_Yawn2_Male_SoundSet",
		"ZenSleep_Yawn3_Male_SoundSet"
	};

	protected static ref array<string> m_FemaleYawnSoundsets =
	{
		"ZenSleep_Yawn1_Female_SoundSet",
		"ZenSleep_Yawn2_Female_SoundSet",
		"ZenSleep_Yawn3_Female_SoundSet"
	};

	protected static ref array<string> m_MaleSnoreSoundsets =
	{
		"ZenSleep_Snore1_Male_SoundSet",
		"ZenSleep_Snore2_Male_SoundSet",
		"ZenSleep_Snore3_Male_SoundSet"
	};

	protected static ref array<string> m_FemaleSnoreSoundsets =
	{
		"ZenSleep_Snore1_Female_SoundSet",
		"ZenSleep_Snore2_Female_SoundSet",
		"ZenSleep_Snore3_Female_SoundSet"
	};

	protected static ref array<string> m_UniVanillaSoundsets = 
	{
		"hungry_uni_Voice_Char_SoundSet",
		"WringClothes_SoundSet"
	};

	static array<string> GetMaleYawnSounds()
	{
		return m_MaleYawnSoundsets;
	}

	static string GetMaleYawnSound(int element)
	{
		if (element < 0 || element >= GetMaleYawnSounds().Count())
			return "";

		return GetMaleYawnSounds().Get(element);
	}

	static array<string> GetMaleSnoreSounds()
	{
		return m_MaleSnoreSoundsets;
	}

	static string GetMaleSnoreSound(int element)
	{
		if (element < 0 || element >= GetMaleSnoreSounds().Count())
			return "";

		return GetMaleSnoreSounds().Get(element);
	}

	static array<string> GetFemaleYawnSounds()
	{
		return m_FemaleYawnSoundsets;
	}

	static string GetFemaleYawnSound(int element)
	{
		if (element < 0 || element >= GetFemaleYawnSounds().Count())
			return "";

		return GetFemaleYawnSounds().Get(element);
	}

	static array<string> GetFemaleSnoreSounds()
	{
		return m_FemaleSnoreSoundsets;
	}

	static string GetFemaleSnoreSound(int element)
	{
		if (element < 0 || element >= GetFemaleSnoreSounds().Count())
			return "";

		return GetFemaleSnoreSounds().Get(element);
	}

	static array<string> GetUniVanillaSounds()
	{
		return m_UniVanillaSoundsets;
	}

	static string GetUniVanillaSound(int element)
	{
		if (element < 0 || element >= GetUniVanillaSounds().Count())
			return "";

		return GetUniVanillaSounds().Get(element);
	}

	static string GetSoundset(int arrayID, int arrayElement)
	{
		string soundSet;

		switch (arrayID)
		{
			case 1:
				soundSet = GetMaleYawnSound(arrayElement);
				break;
			case 2:
				soundSet = GetMaleSnoreSound(arrayElement);
				break;
			case 3:
				soundSet = GetFemaleYawnSound(arrayElement);
				break;
			case 4:
				soundSet = GetFemaleSnoreSound(arrayElement);
				break;
			case 5:
				soundSet = GetUniVanillaSound(arrayElement);
				break;
			default:
				soundSet = "";
				break;
		}

		return soundSet;
	}
}