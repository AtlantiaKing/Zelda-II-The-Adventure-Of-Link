#pragma once

class Texture;
class EnemyManager;
class SoundEffectManager;

class SwordBeam final
{
public:
	SwordBeam();
	SwordBeam(const SwordBeam& sword) = delete;
	SwordBeam(SwordBeam&& sword) = delete;
	~SwordBeam();
	SwordBeam& operator=(const SwordBeam& rhs) = delete;
	SwordBeam& operator=(SwordBeam&& rhs) = delete;

	void Activate(const Point2f& position, float direction);
	void Update(float elapsedSec, EnemyManager* pLevel, SoundEffectManager* pSoundEffectManager);
	void Draw() const;
	bool IsActive() const;
private:
	Texture* m_pTexture;
	bool m_IsActive;
	Point2f m_Position;
	float m_Velocity;
	float m_AccuSeconds;
	float m_AccuFrameSeconds;
	int m_Column;
	bool m_IsDestroyed;

	const float m_Lifespan{ 0.5f };
	const float m_FrameTime{ 1.0f / 30.0f };

	void UpdateSprite(float elapsedSec);
};

