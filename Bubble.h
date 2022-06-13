#pragma once
#include "Enemy.h"

class Bubble final : public Enemy
{
public:
	Bubble(const Point2f& position);

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;
	void SetActive(bool isActive);
	void InverseVelocity(bool xDirection, bool yDirection);

	bool IsActive() const;
	virtual void Draw() const override;
	virtual bool DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const override;
private:
	Vector2f m_Velocity{};

	bool m_IsActive{ false };
	int m_TextureState{};
	float m_AccuSec{ 0.0f };
	float m_FPS{ 60.0f };

	void HandleMovement(float elapsedSec);
	void HandleCollisions(Level* pLevel);
	void UpdateSprite(float elapsedSec);
};

