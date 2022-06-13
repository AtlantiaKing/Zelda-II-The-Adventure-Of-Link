#pragma once
#include "Vector2f.h"

class Texture;
class Level;
class SoundEffectManager;

class Enemy
{
public:
	enum class EnemyType
	{
		Wosu,
		Bot,
		Stalfos,
		Bubble,
		Iron_Knuckle,
		Guma,
		Horsehead
	};

	Enemy(int enemyId, const std::string& filePath, const Point2f& position, const Rectf& collider, int health, int xp, int damage, float invisTime, float hitTextureTime = 0.1f);
	Enemy(const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	virtual ~Enemy();
	Enemy& operator=(const Enemy& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel);
	virtual void DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager);

	virtual void Draw() const = 0;
	Point2f GetPosition() const;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const;
	int GetID() const;
	virtual int GetHealth() const;
	int GetXP() const;
	int GetDamage() const;
protected:
	const int m_EnemyId;
	Point2f m_Position;
	Rectf m_Collider;
	Texture* m_pTexture;
	Texture* m_pHurtTexture;
	int m_Health;
	int m_XP;
	int m_Damage;
	bool m_IsHit{};
	bool m_ShowHitTexture{};
	bool m_ShowHurtColors{};
	float m_AccuHurtTime{ 0.0f };

	Rectf CalculateCollider() const;
	Texture* GetCurrentTexture() const;
private:
	float m_AccuSec{ 0.0f };
	float m_AccuHitTextureTime{};

	const float m_HitTextureTimer{ 0.1f };
	const float m_HurtColorTime{ 1.0f };
	const float m_HealthTimer{ 1.0f };

	void UpdateHitTimer(float elapsedSec);
	void UpdateHurtColorTimer(float elapsedSec);
};

