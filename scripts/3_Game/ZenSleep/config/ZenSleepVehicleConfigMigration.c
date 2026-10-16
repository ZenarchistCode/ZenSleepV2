// Presence check for this one migrated field. Actual values are read by
// JsonFileLoader; omitted bools otherwise become false in DayZ's serializer.
class ZenSleepVehicleConfigMigration
{
	bool HasGeneralSetting;
	bool HasLegacySetting;

	void ZenSleepVehicleConfigMigration(string json)
	{
		int depth;
		bool inGeneral;
		bool generalNext;
		for (int i = 0; i < json.Length(); i++)
		{
			string character = json.Substring(i, 1);
			if (character == "\"")
			{
				int start = ++i;
				while (i < json.Length())
				{
					character = json.Substring(i, 1);
					if (character == "\\")
						i++;
					else if (character == "\"")
						break;
					i++;
				}
				string key = json.Substring(start, i - start);
				int next = i + 1;
				while (next < json.Length() && (json.Substring(next, 1) == " " || json.Substring(next, 1) == "\t" || json.Substring(next, 1) == "\r" || json.Substring(next, 1) == "\n"))
					next++;
				if (next < json.Length() && json.Substring(next, 1) == ":")
				{
					generalNext = depth == 1 && key == "GeneralSleepConfig";
					if (key == "AllowVehicleSleep")
					{
						if (depth == 1)
							HasLegacySetting = true;
						else if (depth == 2 && inGeneral)
							HasGeneralSetting = true;
					}
				}
			}
			else if (character == "{" || character == "[")
			{
				depth++;
				if (depth == 2)
					inGeneral = generalNext && character == "{";
				generalNext = false;
			}
			else if (character == "}" || character == "]")
			{
				if (depth == 2)
					inGeneral = false;
				depth--;
			}
		}
	}
}

class ZenSleepLegacyVehicleConfig
{
	bool AllowVehicleSleep;
}
