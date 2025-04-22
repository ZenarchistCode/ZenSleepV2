/*
	Version 2 of my Sleeping Mod, re-written from scratch to be far more robust and utilize
	more vanilla mechanics to enhance compatibility and future-proof the mod as best as possible.

	Source code & mod config info available at my GitHub: https://zenarchist.com

	Sweet dreams, survivor.

	Credits:
	Half moon stat icon: https://www.flaticon.com/free-icon/sleep_835605
	Syberia Project code helped me understand adding custom sleep icon to HUD.
	Medical Attention Mod for adding the first sleep mod to DayZ: https://steamcommunity.com/sharedfiles/filedetails/?id=2293429999
	Sleeping Bags: Unfortunately I forgot where I got that model from way back, can't find it anymore online.
	Stringtable Creator: https://germandayz.gg/filebase/file/81-stringtable-creator/
*/

class CfgPatches
{
	class ZenSleep
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};

class CfgMods
{
	class ZenSleep
	{
		dir="ZenSleep";
		picture=""; 
		action="";
		hideName=1;
		hidePicture=1;
		name="ZenSleep";
		credits=""; 
		author="Zenarchist";
		authorID="0";  
		version="2.0";
		extra=0;
		type="mod";
		inputs="ZenSleep/data/inputs.xml";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"ZenSleep/scripts/common",
					"ZenSleep/scripts/3_game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"ZenSleep/scripts/common",
					"ZenSleep/scripts/4_world"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"ZenSleep/scripts/common",
					"ZenSleep/scripts/5_mission"
				};
			};
		};
	};
};

class CfgVehicles
{
	class Inventory_Base;
	class HouseNoDestruct;

	class ZenSleepingBag_Base: Inventory_Base
	{
		scope=1;
		displayName="$STR_CfgVehicles_ZenSleepingBag0";
		descriptionShort="$STR_CfgVehicles_ZenSleepingBag1";
		model="ZenSleep\data\models\sleepingbags\sleepingbag.p3d";
		soundImpactType="textile";
		simpleHiddenSelections[]= 
		{
			"packed",
			"deployed"
	    };
		hiddenSelections[]=
		{
			"packed_texture",
			"deployed"
		};
		inventorySlot[] =
		{
			"Backpack_1"
		};
		hiddenSelectionsTextures[]={};
		weight=1000;
		absorbency=0.30000001;
		varWetMax=0.49000001;
		itemSize[]={4,2};
		repairableWithKits[] = { 5,2 };
		repairCosts[] = { 30,25 };
		itemBehaviour=2;
		rotationFlags=16;
		class Cargo // only visible when deployed
		{
			itemsCargoSize[]={4,2};
			openable=0;
			allowOwnedCargoManipulation=1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=250;

					// RVMATs are handled by script
					healthLevels[]=
					{
						
						{
							1,
							
							{
								""
							}
						},
						
						{
							0.69999999,
							
							{
								""
							}
						},
						
						{
							0.5,
							
							{
								""
							}
						},
						
						{
							0.30000001,
							
							{
								""
							}
						},
						
						{
							0,
							
							{
								""
							}
						}
					};
				};
			};
		};
	};

	class ZenSleepingBag_Black : ZenSleepingBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_black_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_black_co.paa"
		};
	};
	class ZenSleepingBag_Blue : ZenSleepingBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_blue_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_blue_co.paa"
		};
	};
	class ZenSleepingBag_Green : ZenSleepingBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_green_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_green_co.paa"
		};
	};
	class ZenSleepingBag_Grey : ZenSleepingBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_grey_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_grey_co.paa"
		};
	};
	class ZenSleepingBag_Red : ZenSleepingBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_red_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_red_co.paa"
		};
	};

	// Custom clutter cutter which doesn't count as "inside building" when standing on top of it 
	class ZenSleepingBag_ClutterCutter: HouseNoDestruct
	{
		scope=1;
		model="ZenSleep\data\models\ClutterCutter\Cutter.p3d";
	};

	// Static Sleeping Bags for placement with DayZ Editor
	class ZenSleepingBagStatic_Base: HouseNoDestruct
	{
		scope=1;
		displayName="$STR_CfgVehicles_ZenSleepingBag0";
		descriptionShort="$STR_CfgVehicles_ZenSleepingBag1";
		model="ZenSleep\data\models\sleepingbags\sleepingbag.p3d";
		simpleHiddenSelections[]= 
		{
			"packed",
			"deployed"
	    };
		hiddenSelections[]=
		{
			"packed_texture",
			"deployed"
		};
		weight=10000;
	};
	class ZenSleepingBagStatic_Black : ZenSleepingBagStatic_Base
	{
		scope=1;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_black_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_black_co.paa"
		};
	};
	class ZenSleepingBagStatic_Blue : ZenSleepingBagStatic_Base
	{
		scope=1;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_blue_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_blue_co.paa"
		};
	};
	class ZenSleepingBagStatic_Green : ZenSleepingBagStatic_Base
	{
		scope=1;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_green_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_green_co.paa"
		};
	};
	class ZenSleepingBagStatic_Grey : ZenSleepingBagStatic_Base
	{
		scope=1;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_grey_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_grey_co.paa"
		};
	};
	class ZenSleepingBagStatic_Red : ZenSleepingBagStatic_Base
	{
		scope=1;
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\sleepingbags\sleepingbag_packed_red_co.paa",
			"ZenSleep\data\textures\sleepingbags\sleepingbag_red_co.paa"
		};
	};

	class ZenSleep_Anesthetic: Inventory_Base
	{
		scope=2;
		displayName="$STR_CfgVehicles_ZenSleep_Anesthetic0";
		descriptionShort="$STR_CfgVehicles_ZenSleep_Anesthetic1";
		model="\dz\gear\medical\Morphine.p3d";
		rotationFlags=16;
		itemSize[]={1,2};
		weight=60;
		soundImpactType="plastic";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\syringe\anesthetic.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=50;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\gear\medical\data\Morphine.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\gear\medical\data\Morphine.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\gear\medical\data\Morphine_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\gear\medical\data\Morphine_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\gear\medical\data\Morphine_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Morphine_spear
				{
					soundSet="Morphine_spear_SoundSet";
					id=201;
				};
				class Morphine_out
				{
					soundSet="Morphine_out_SoundSet";
					id=202;
				};
			};
		};
	};
	class ZenSleep_Amphetamine: Inventory_Base
	{
		scope=2;
		displayName="$STR_CfgVehicles_ZenSleep_Amphetamine0";
		descriptionShort="$STR_CfgVehicles_ZenSleep_Amphetamine1";
		model="\dz\gear\medical\Morphine.p3d";
		rotationFlags=16;
		itemSize[]={1,2};
		weight=60;
		soundImpactType="plastic";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"ZenSleep\data\textures\syringe\amphetamine.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=50;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\gear\medical\data\Morphine.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\gear\medical\data\Morphine.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\gear\medical\data\Morphine_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\gear\medical\data\Morphine_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\gear\medical\data\Morphine_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Morphine_spear
				{
					soundSet="Morphine_spear_SoundSet";
					id=201;
				};
				class Morphine_out
				{
					soundSet="Morphine_out_SoundSet";
					id=202;
				};
			};
		};
	};
};

// All sounds uncopyrighted - recorded by me with Zoom H1 field recorder and Shure SM7B.
class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	class ZenSleepSoundShaderBase :  baseCharacter_SoundShader
	{
		volume = 0.7;
		range = 30;
		rangeCurve[] = { {0, 1 },{0.5,0.7},{1,0} };
	};

	// Male yawns
	class ZenSleep_Yawn1_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn1_m", 1 } };
	};
	class ZenSleep_Yawn2_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn2_m", 1 } };
	};
	class ZenSleep_Yawn3_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn3_m", 1 } };
	};

	// Female yawns
	class ZenSleep_Yawn1_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn1_f", 1 } };
	};
	class ZenSleep_Yawn2_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn2_f", 1 } };
	};
	class ZenSleep_Yawn3_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\yawn3_f", 1 } };
	};

	// Male sleep sounds
	class ZenSleep_Snore1_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore1_m", 1 } };
	};
	class ZenSleep_Snore2_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore2_m", 1 } };
	};
	class ZenSleep_Snore3_Male_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore3_m", 1 } };
	};

	// Female sleep sounds
	class ZenSleep_Snore1_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore1_f", 1 } };
	};
	class ZenSleep_Snore2_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore2_f", 1 } };
	};
	class ZenSleep_Snore3_Female_SoundShader : ZenSleepSoundShaderBase
	{
		samples[] = { { "ZenSleep\data\sounds\character\snore3_f", 1 } };
	};

	// Sleeping bag pack sounds 
	class Zen_SleepingBagPack_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			{ "ZenSleep\data\sounds\sleepingbag\bagpack1", 1 }
		};

		volume=0.3;
	};
	class Zen_SleepingBagUnpack_SoundShader: baseCharacter_SoundShader
	{
		samples[]=
		{
			{ "ZenSleep\data\sounds\sleepingbag\bagpack2", 1 }
		};

		volume=0.3;
	};
};

class CfgSoundSets
{
	class baseCharacter_SoundSet;
	class ZenSleepSoundSetBase : baseCharacter_SoundSet
	{
		chanceToPlay = 1.0;
		minSecsAsleepToPlay = 25.0;
	};

	class Zen_SleepingBagPack_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = { "Zen_SleepingBagPack_SoundShader" };
	};
	class Zen_SleepingBagUnpack_SoundSet: baseCharacter_SoundSet
	{
		soundShaders[] = { "Zen_SleepingBagUnpack_SoundShader" };
	};
	
	// Male yawns
	class ZenSleep_Yawn1_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn1_Male_SoundShader" };
	};
	class ZenSleep_Yawn2_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn2_Male_SoundShader" };
	};
	class ZenSleep_Yawn3_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn3_Male_SoundShader" };
	};

	// Female yawns
	class ZenSleep_Yawn1_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn1_Female_SoundShader" };
	};
	class ZenSleep_Yawn2_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn2_Female_SoundShader" };
	};
	class ZenSleep_Yawn3_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Yawn3_Female_SoundShader" };
	};

	// Male sleep sounds
	class ZenSleep_Snore1_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore1_Male_SoundShader" };
	};
	class ZenSleep_Snore2_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore2_Male_SoundShader" };
		chanceToPlay = 0.1;
		minSecsAsleepToPlay = 60.0;
	};
	class ZenSleep_Snore3_Male_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore3_Male_SoundShader" };
		chanceToPlay = 0.1;
		minSecsAsleepToPlay = 60.0;
	};

	// Female sleep sounds
	class ZenSleep_Snore1_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore1_Female_SoundShader" };
	};
	class ZenSleep_Snore2_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore2_Female_SoundShader" };
		chanceToPlay = 0.1;
		minSecsAsleepToPlay = 60.0;
	};
	class ZenSleep_Snore3_Female_SoundSet : ZenSleepSoundSetBase
	{
		soundShaders[] = { "ZenSleep_Snore3_Female_SoundShader" };
		chanceToPlay = 0.1;
		minSecsAsleepToPlay = 60.0;
	};
};

class CfgSoundEffects
{
	class AttenuationsEffects
	{
		class ZenSleepAttenuation
		{
			class Equalizer0
			{
				center[] = { 196, 2000, 8367, 10000 };
				bandwidth[] = { 2, 2, 0.1, 2 };
				gain[] = { 1.4125376, 0.12589253, 1.9952624, 0.12589253 };
			};
			class Equalizer1
			{
				center[] = { 60, 1000, 2000, 10000 };
				bandwidth[] = { 2, 1, 0.1, 2 };
				gain[] = { 1, 0.3548134, 0.25118864, 0.12589253 };
			};
			class Echo
			{
				WetDryMix = 0.25;
				Feedback = 0.2;
				Delay = 50;
			};
		};
	};
};