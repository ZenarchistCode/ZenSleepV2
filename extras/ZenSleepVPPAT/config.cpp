class CfgPatches
{
	class ZenSleepVPPAT
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"ZenSleep"
		};
	};
};

class CfgMods
{
	class ZenSleepVPPAT
	{
		dir="ZenSleepVPPAT";
		picture=""; 
		action="";
		hideName=1;
		hidePicture=1;
		name="ZenSleepVPPAT";
		credits="";
		author="Zenarchist";
		authorID="0";
		version="2.0";
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
					"ZenSleepVPPAT/scripts/3_game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"ZenSleepVPPAT/scripts/4_world"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"ZenSleepVPPAT/scripts/5_mission"
				};
			};
		};
	};
};