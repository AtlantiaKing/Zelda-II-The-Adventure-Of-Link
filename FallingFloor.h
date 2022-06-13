#pragma once
#include "GameObject.h"

class FallingFloor final : public GameObject
{
public:
	FallingFloor(int objectId, const Point2f& position);

	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager);
	virtual void Draw() const;
	virtual bool IsOnGround(const Rectf& actorShape);
	void TryFall(SoundEffectManager* pSoundEffectManager);
private:
	bool m_IsFalling{};
	float m_AccuSec{};
	const float m_FrameTime{ 8.0f / 30.0f };
	const int m_NrFrames{ 3 };
	int m_CurFrame{};
};

