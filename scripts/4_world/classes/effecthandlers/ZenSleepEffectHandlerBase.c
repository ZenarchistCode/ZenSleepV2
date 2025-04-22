class ZenSleepEffectHandlerBase extends Managed
{
	protected PlayerBase m_Player;

	void ZenSleepEffectHandlerBase(PlayerBase player)
	{
		m_Player = player; 

		Init();
	}

	void Init();

	void Update(float deltaTime = 1)
	{
	}
}