#pragma once
#include "Enemy.h"

class Camera;
class ParticleManager;

class Horsehead final : public Enemy
{
public:
	Horsehead(int enemyId, const Point2f& position);
	Horsehead(const Horsehead& stalfos) = delete;
	Horsehead(Horsehead&& stalfos) = delete;
	virtual ~Horsehead();
	Horsehead& operator=(const Horsehead& rhs) = delete;
	Horsehead& operator=(Horsehead&& rhs) = delete;

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;
	void UpdateDeathAnimation(float elapsedSec, ParticleManager* pParticleManager, SoundEffectManager* pSoundEffectManager);
	virtual void DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager) override;
	void HandleCameraCollision(Camera* pCamera);
	void ActivateDeathAnimation();

	virtual void Draw() const override;
	bool IsActive() const;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const override;
	virtual int GetHealth() const override;
	bool IsDeathAnimationFinished() const;
	bool IsDeathAnimationBusy() const;
private:
	Texture* m_pBatTexture;
	Vector2f m_Velocity{};
	bool m_IsBodyHit{};
	bool m_IsLookingRight{};
	int m_BodyFrame{};
	int m_LegFrame{};
	float m_AccuLegTime{};
	float m_AccuBodyTime{};
	float m_AccuBodyHitTime{};
	float m_AccuRunTime{};
	bool m_IsRunning{};
	bool m_IsActive{};
	bool m_IsAttacking{};
	int m_MaxHealth{};
	bool m_IsDeathAnimFinished{};
	bool m_IsDeathAnimBusy{};
	float m_AccuDeathAnimSec{};

	const float m_MoveSpeed{ 20.0f };
	const float m_LegFrameTime{ 1 / 5.0f };
	const float m_BodyFrameTime{ 1 / 4.0f };
	const float m_BodyHitTime{ 0.4f };
	const float m_RunTime{ 0.5f };
	const float m_TimeBetweenRuns{ 1.0f };
	const float m_RunDistance{ 80.0f };
	const float m_AttackDistance{ 40.0f };
	const float m_DeathAnimLength{ 5.0f };

	void UpdateBodyHitTimer(float elapsedSec);
	bool IsInRange(const Rectf& actorShape);
	void HandleMovement(bool isInRange, const Rectf& actorShape, float elapsedSec);
	void HandleState(bool isInRange, float elapsedSec);
};

