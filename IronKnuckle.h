#pragma once
#include "Enemy.h"
#include <vector>

class IronKnuckleSword;
class Level;

class IronKnuckle final : public Enemy
{
public:
	IronKnuckle(int enemyId, const Point2f& position);
	IronKnuckle(const IronKnuckle& ik) = delete;
	IronKnuckle(IronKnuckle&& ik) = delete;
	IronKnuckle& operator=(const IronKnuckle& rhs) = delete;
	IronKnuckle& operator=(IronKnuckle&& rhs) = delete;
	virtual ~IronKnuckle();

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;
	virtual void DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager) override;

	virtual void Draw() const override;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const override;
private:
	void UpdatePosition(const Rectf& actorShape, float elapsedSec);
	void UpdateShieldHitTimer(float elapsedSec);
	void UpdateSprite(float elapsedSec, const Rectf& actorShape);
	void ChangeComponent(bool isSwordUp);
	void ActivateComponent();
	void HandleCollisions(Level* pLevel);

	bool IsInRange(const Rectf& actorShape) const;

	Point2f m_ComponentPosition{};
	std::vector<IronKnuckleSword*> m_Components{};
	int m_CurrentComponent{};
	bool m_ComponentsActive{ false };
	float m_AccuTime{};
	bool m_IsShieldHit{ false };
	bool m_IsLookingRight{};

	const float m_Speed{ 50.0f };
	const float m_ChangeTime{ 0.05f };
	const float m_ShieldHitTimer{ 0.2f };

	float m_AccuSpriteSec{};
	int m_CurFrame{};
	bool m_AttackUp{ false };
	int m_CurRow{ 0 };

	const float m_FrameTime{ 1.0f / 8.0f };
};

