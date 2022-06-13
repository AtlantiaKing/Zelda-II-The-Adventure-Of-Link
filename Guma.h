#pragma once
#include "Enemy.h"

class Texture;

class Guma final : public Enemy
{
public:
	Guma(int enemyId, const Point2f& position);
	Guma(const Guma& other) = delete;
	Guma(Guma&& other) = delete;
	virtual ~Guma();
	Guma& operator=(const Guma& other) = delete;
	Guma& operator=(Guma&& other) = delete;

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;

	virtual void Draw() const override;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const override;
private:
	struct GumaProjectile
	{
		bool isActive{ false };
		Point2f position{};
		int curFrame{};
		float accuFrameTime{};
		float rotation{};
		Vector2f velocity{ -48.0f, 85.0f };
	};

	Texture* m_pProjectileTexture;
	static const int m_NrProjectiles{ 7 };
	GumaProjectile m_Projectiles[m_NrProjectiles]{};
	Vector2f m_Velocity{ 35.0f, 0.0f };
	bool m_IsLookingRight{ false };

	int m_CurProjectile{};
	float m_AccuThrowTime{};
	const float m_TimeBetweenThrows{ 0.8f };

	float m_AccuMoveTime{};
	const float m_MoveTime{ 0.5f };

	int m_CurRow{};
	int m_CurFrame{};
	float m_AccuFrameTime{};
	const float m_FrameTime{ 1 / 10.0f };
	const float m_ProjectileFrameTime{ 1 / 30.0f };

	void UpdateLookingDirection(const Rectf& actorShape);
	void UpdateHorizontalVelocity(float elapsedSec);
	void HandleThrowTimer(float elapsedSec, const Rectf& actorShape);
	void UpdateGravity(float elapsedSec);
	void HandleMovement(float elapsedSec);
	void HandleProjectiles(float elapsedSec);
	void HandleCollisions(Level* pLevel);
	void UpdateSprite(float elapsedSec);
	void ThrowProjectile();
};

