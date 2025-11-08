/* 
This code triggers a weird runtime error when enabled, despite not touching the OnMenuRelease function in this code:

scripts/5_mission_gui/gesturesmenu.c(162): Method 'OnMenuRelease' is protected

I only seem to get this error on servers which are not running my full modpack. Not sure what to make of it,
so I've disabled it for now til I have time to figure out what's going on.
*/

/*
modded class GesturesMenu
{
    override void GetGestureItems(out array<ref GestureMenuItem> gesture_items, GestureCategories category)
	{
        array<ref GestureMenuItem> gesture_items = new array<ref GestureMenuItem>;

        super.GetGestureItems(gesture_items, category);

        #ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

        for (int i = 0; i < gesture_items.Count(); i++)
        {
            if (gesture_items.Get(i).GetID() == EmoteConstants.ID_EMOTE_LYINGDOWN)
            {
                gesture_items.Get(i).ZenSetName("#STR_DIK_SLEEP");
            }
        }
    }

    override void UpdateQuickbarItemCard(GestureMenuItem gesture_item)
	{
        super.UpdateQuickbarItemCard(gesture_item);

        #ifdef ZENMODPACK
		if (!ZenModEnabled("ZenSleep"))
		{
			return;
		}
		#endif

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

        int fatigueState = player.GetZenSleepManager().GetFatigueStatState();

        if (fatigueState <= 2)
            return;

		Widget gesture_item_card_widget = gesture_item.GetRadialItemCard();
		RichTextWidget gesture_text = RichTextWidget.Cast(gesture_item_card_widget.FindAnyWidget(GESTURE_TEXT));
		RichTextWidget input_text = RichTextWidget.Cast(gesture_item_card_widget.FindAnyWidget(INPUT_TEXT));

		if (gesture_item.GetCanPerformEmote() && gesture_item.GetID() == EmoteConstants.ID_EMOTE_LYINGDOWN)
		{
            if (fatigueState >= 4)
            {
                // red
                gesture_text.SetColor(ARGB(255,250,0,0));
			    input_text.SetColor(ARGB(255,250,0,0));
            }
            else
            {
                // yellow
                gesture_text.SetColor(ARGB(255,250,250,0));
			    input_text.SetColor(ARGB(255,250,250,0));
            }
		}
	}

    // overriding method that is causing troubles
	override static void CloseMenu()
	{
		//instance.OnMenuRelease(); // <-- buggy line is commented out
		GetGame().GetUIManager().Back();
	}
}

modded class GestureMenuItem
{
    void ZenSetName(string name)
    {
        m_Name = name;
    }
}
*/