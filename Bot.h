#pragma once
#include "Enemy.h"

class Bot final : public Enemy
{
public:
	Bot(const Point2f& position);

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;

	virtual void Draw() const override;
private:
	enum class BotState
	{
		idle,
		walking,
		jumping,
		walk_idle,
	};

	Vector2f m_Velocity{};
	BotState m_State{ BotState::idle };
	int m_Direction{ 1 };
	float m_AccuSeconds{ 0.0f };
	int m_Jumps{ 0 };

	const float m_IdleTime{ 1.0f };
	const float m_JumpTime{ 0.25f };
	const float m_JumpIdleTime{ 0.25f };

	const float m_FrameTime{ 1.0f / 16.0f };
	float m_AccuSpriteTimer{ 0.0f };
	int m_Frame{ 0 };

	void HandleState(float elapsedSec, const Rectf& actorShape, Level* pLevel);
	void HandleMovement(float elapsedSec, Level* pLevel);
	void HandleCollisions(Level* pLevel);

	void Walk();
	void Jump(const Rectf& actorShape);
	void RandomizeState();
};

