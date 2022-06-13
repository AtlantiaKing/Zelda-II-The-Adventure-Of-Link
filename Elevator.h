#pragma once
#include "GameObject.h"
#include "Vector2f.h"
#include "utils.h"

#include <vector>

class Texture;
class Avatar;
class Level;
class SoundEffect;

class Elevator final : public GameObject
{
public:
	Elevator(int objectId, const Point2f& pos);

	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager) override;
	virtual void DoHitTest(Rectf& actorShape, Vector2f& velocity) override;
	virtual bool IsOnGround(const Rectf& actorShape) override;
private:
	std::vector<Point2f> m_FrameColliders{};
	bool m_IsPlayerColliding{};
	Vector2f m_Velocity{};

	const float m_VerSpeed{ 64.0f };

	void InitCollider();
	void HandleCollisions(Level* pLevel);
	void HandleMovement(float elapsedSec, SoundEffectManager* pSoundEffectManager);
	void UpdateCollider();
	void StopPlayer(Rectf& actorShape, utils::HitInfo& hitInfo, Vector2f& velocity);

	Rectf GetWorldCollider() const;
};

