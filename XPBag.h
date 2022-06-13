#pragma once
#include "GameObject.h"

class Level;
class GameObjectManager;

class XPBag final : public GameObject
{
public:
	XPBag(int objectId, const Point2f& position);
	void Update(float elapsedSec, Level* pLevel, GameObjectManager* pGameObjectManager);
private:
};

