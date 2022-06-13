#pragma once
#include "GameObject.h"

class Level;

class Fairy final : public GameObject
{
public:
	Fairy(int objectId, const Point2f& position);
	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager) override;
	virtual void Draw() const override;
private:
	float m_Direction{ 1 };
	float m_AccuSecDirection{};
	float m_AccuSecFrame{};
	int m_CurFrame{};

	const float m_ChangeDirectionTime{ 0.5f };
	const float m_FrameTime{ 1.0f / 15.0f };
};

