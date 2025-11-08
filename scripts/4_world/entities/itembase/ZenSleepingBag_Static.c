class ZenSleepingBagStatic_Base extends BuildingSuper
{
    protected Object m_ZenBagClutterCutter;

    void ZenSleepingBagStatic_Base()
    {
        SetAllowDamage(false);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnZenClutterCutter, 500);
    }

    void ~ZenSleepingBagStatic_Base()
    {
        if (!GetGame())
			return;
		
        if (m_ZenBagClutterCutter != NULL)
            GetGame().ObjectDelete(m_ZenBagClutterCutter);
    }

    void SpawnZenClutterCutter()
    {
        m_ZenBagClutterCutter = GetGame().CreateObjectEx("ZenSleepingBag_ClutterCutter", GetPosition(), ECE_PLACE_ON_SURFACE);
        if (m_ZenBagClutterCutter)
        {
            m_ZenBagClutterCutter.SetOrientation(GetOrientation());
            ZenSleepFunctions.AlignToTerrain(m_ZenBagClutterCutter);
        }
    }

    override bool IsBuilding()
    {
        return false;
    }
}

class ZenSleepingBagStatic_Black extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Blue extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Grey extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Red extends ZenSleepingBagStatic_Base {};
class ZenSleepingBagStatic_Green extends ZenSleepingBagStatic_Base {};