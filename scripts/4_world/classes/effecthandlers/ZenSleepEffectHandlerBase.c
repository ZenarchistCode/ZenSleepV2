class ZenSleepEffectHandlerBase extends Managed
{
	protected PlayerBase m_Player;

	void ZenSleepEffectHandlerBase(PlayerBase player)
	{
		m_Player = player; 

		Init();

		Print("[ZenSleep::" + ClassName() + "] Initialized " + m_Player.GetCachedID());
	}

	void Init();

	void Update(float deltaTime = 1)
	{
	}
}