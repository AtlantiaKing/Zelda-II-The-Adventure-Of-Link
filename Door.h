#pragma once
#include "GameObject.h"

class Inventory;
class SoundEffectManager;

class Door final : public GameObject
{
public:
	Door(int objectId, bool isBossDoor);

	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager);
	virtual void Draw() const;
	virtual void DoHitTest(Rectf& actorShape, Vector2f& actorVelocity);
	void TryOpen(const Rectf& actorShape, Inventory* pInventory, SoundEffectManager* pSoundEffectManager);
private:
	bool m_IsOpen{};
	float m_Progress{};
};

