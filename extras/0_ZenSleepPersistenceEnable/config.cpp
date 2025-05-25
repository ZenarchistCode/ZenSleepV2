class CfgPatches
{
	class 0_ZenSleepPersistenceEnable
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};

class CfgMods
{
	class 0_ZenSleepPersistenceEnable
	{
		dir="0_ZenSleepPersistenceEnable";
		picture=""; 
		action="";
		hideName=1;
		hidePicture=1;
		name="0_ZenSleepPersistenceEnable";
		credits=""; 
		author="Zenarchist";
		authorID="0";  
		version="1.0";
		extra=0;
		type="mod";
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
					"0_ZenSleepPersistenceEnable/scripts/common"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"0_ZenSleepPersistenceEnable/scripts/common"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"0_ZenSleepPersistenceEnable/scripts/common"
				};
			};
		};
	};
};