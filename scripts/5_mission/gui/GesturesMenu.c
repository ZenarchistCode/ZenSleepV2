modded class GesturesMenu
{
    override protected void GetGestureItems(out array<ref GestureMenuItem> gesture_items, GestureCategories category)
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

    override protected void UpdateQuickbarItemCard(GestureMenuItem gesture_item)
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
}

modded class GestureMenuItem
{
    void ZenSetName(string name)
    {
        m_Name = name;
    }
}