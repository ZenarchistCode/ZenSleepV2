modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);

        //! SLEEPING BAGS
        actions.Insert(ActionZenPackSleepingBag);
        actions.Insert(ActionZenUnpackSleepingBag);
        actions.Insert(ActionZenSleepOnBed);

        //! SLEEP SYRINGES 
        actions.Insert(ActionInjectZenSleepSyringeSelf);
        actions.Insert(ActionInjectZenSleepSyringeTarget);
    }
}