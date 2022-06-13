#pragma once
#include "Enemy.h"
#include "Vector2f.h"

class Level;

class Wosu final : public Enemy
{ 
public:
	Wosu(const Point2f& position, bool lookingRight);

	virtual void Update(float elapsedSec, const Rectf& actorShape, Level* pLevel) override;

	virtual void Draw() const override;
private:
	Vector2f m_Velocity{};
	int m_TextureState{};
};

