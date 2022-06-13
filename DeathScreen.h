#pragma once

class Texture;
class SoundEffectManager;

class DeathScreen final
{
public:
	DeathScreen();
	DeathScreen(const DeathScreen& other) = delete;
	DeathScreen(DeathScreen&& other) = delete;
	DeathScreen& operator=(const DeathScreen& other) = delete;
	DeathScreen& operator=(DeathScreen&& other) = delete;
	~DeathScreen();
	
	void Update(float elapsedSec, int nrLifes, SoundEffectManager* pSoundEffectManager);
	void SetActive(bool isActive);

	void Draw(float windowWidth, float windowHeight, int nrLifes, const std::string& userName) const;
private:
	enum class DeathScreenState
	{
		Link,
		NextLife,
		Ganon
	};

	Texture* m_pLinkDeath;
	Texture* m_pGanonScreen;
	Texture* m_pNextLife;
	Texture* m_pNumbers;
	Texture* m_pLetters;

	bool m_IsActive{};
	float m_AccuSec{};
	const float m_ColorTime{ 2.0f };

	DeathScreenState m_DeathScreenState{ };
};

