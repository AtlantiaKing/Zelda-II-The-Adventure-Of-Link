#pragma once
#include "Enemy.h"

class Texture;
class Level;

class Stalfos final : public Enemy
{
public:
	Stalfos(int enemyId, const Point2f& position);
	Stalfos(const Stalfos& stalfos) = delete;
	Stalfos(Stalfos&& stalfos) = delete;
	virtual ~Stalfos();
	Stalfos& operator=(const Stalfos& rhs) = delete;
	Stalfos& operator=(Stalfos&& rhs) = delete;

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;
	virtual void DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager) override;

	virtual void Draw() const override;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const override;
private:
	Texture* m_pSwordTexture;
	const float m_MoveSpeed{ 50.0f };
	Vector2f m_Velocity{};
	bool m_IsShieldHit{};
	bool m_IsLookingRight{};
	int m_BodyFrame{};
	int m_LegFrame{};
	float m_AccuLegTime{};
	float m_AccuBodyTime{};
	float m_AccuShieldTime{};

	const float m_LegFrameTime{ 1 / 5.0f };
	const float m_BodyFrameTime{ 1 / 4.0f };
	const float m_ShieldHitTime{ 0.2f };
	const float m_MinimalDistance;

	void UpdateShieldHitTimer(float elapsedSec);
	void HandleMovement(bool isInRange, const Rectf& actorShape, float elapsedSec);
	void HandleCollisions(Level* pLevel);
	void HandleState(bool isInRange, float elapsedSec);

	bool IsInRange(const Rectf& actorShape) const;


};

