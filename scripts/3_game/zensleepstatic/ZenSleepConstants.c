class ZenSleepConstants
{
	static const float SL_FATIGUE_CRITICAL	= 1;				// 0.0002%	| Red flashing below this fatigue value
	static const float SL_FATIGUE_LOW		= 300;				// 6%		| Red below this fatigue value when > critical value
	static const float SL_FATIGUE_NORMAL	= 800;				// 16%		| Yellow below this fatigue value when > low value
	static const float SL_FATIGUE_HIGH		= 3500;				// 70%		| White with gap below this fatigue value when > normal value
	static const float SL_FATIGUE_MAX		= 5000;				// 100%		| Max stat value. Full white below this fatigue value when > high value

	static const float FATIGUE_BASE_SECONDS_TIL_RESTED = 600;	// How many seconds the default config expects to take approximately to reach full rest from zero

	static const float FATIGUE_SPEED_ENERGY_WALK	= 0.05;		// Fatigue loss per second when walking
	static const float FATIGUE_SPEED_ENERGY_JOG		= 0.15;		// Fatigue loss per second when jogging
	static const float FATIGUE_SPEED_ENERGY_SPRINT	= 0.3;		// Fatigue loss per second when sprinting

	static const float DEC_TRESHOLD_LOW		= 0;				// Icon arrows tendency (fatigue speed < this negative number = 1 arrow)
	static const float DEC_TRESHOLD_MED		= 0.15;				// Icon arrows tendency (fatigue speed < this negative number = 2 arrow)
	static const float DEC_TRESHOLD_HIGH	= 0.3;				// Icon arrows tendency (fatigue speed < this negative number = 3 arrow)
	static const float INC_TRESHOLD_LOW		= 0;				// Icon arrows tendency (fatigue speed > this positive number = 1 arrow)
	static const float INC_TRESHOLD_MED		= 1.0;				// Icon arrows tendency (fatigue speed > this positive number = 2 arrow)
	static const float INC_TRESHOLD_HIGH	= 10.0;				// Icon arrows tendency (fatigue speed > this positive number = 3 arrow)

	static const float FATIGUE_GAIN_BASE_RATE = 0.01;			// Fatigue gain per second when sleeping
	static const float FATIGUE_DRAIN_BASE_RATE = 0.01;			// Fatigue drain per second when idle

	static const float SLEEP_UPDATE_TICK_SECS = 1;				// How many seconds between processing ZenSleepManager and related update method ticks
	static const float SOUND_UPDATE_TICK_SECS = 0.5;			// How many seconds between processing client-side sound play events

	static const int FATIGUE_MAX_FX_TIMER = 300;				// How many seconds spent at red or red flashing tiredness until the relevant full visual FX are applied

	static const int SLEEP_SOUND_YAWN_MIN_SECS = 30;			// Minimum seconds between yawn sounds when tired 
	static const int SLEEP_SOUND_YAWN_MAX_SECS = 180;			// Maximum seconds between yawn sounds when tired

	static const int SLEEP_SOUNDSET_MALE_YAWNS = 1;				// Array ID for male yawns
	static const int SLEEP_SOUNDSET_MALE_SNORES = 2;			// Array ID for male snores
	static const int SLEEP_SOUNDSET_FEMALE_YAWNS = 3;			// Array ID for female yawns
	static const int SLEEP_SOUNDSET_FEMALE_SNORES = 4;			// Array ID for female snores

	static const int SLEEP_VFX_HALLUCINATION_YAWN = 1;			// Visual effect ID for yawning 
	static const int SLEEP_VFX_HALLUCINATION_UNCON = 2;			// Visual effect ID for just before going uncon
}