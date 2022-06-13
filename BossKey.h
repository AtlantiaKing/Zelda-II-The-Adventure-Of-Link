#pragma once
#include "GameObject.h"

class BossKey final : public GameObject
{
public:
	BossKey(const Point2f& position);
	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager) override;
private:
	Vector2f m_Velocity{};
};

