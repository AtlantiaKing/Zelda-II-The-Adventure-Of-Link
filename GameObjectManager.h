#pragma once

#include "Vector2f.h"
#include "enums.h"
#include <vector>

class Level;
class GameObject;
class ParticleManager;
class Inventory;
class SoundEffectManager;
class Camera;

class GameObjectManager final
{
public:
	GameObjectManager() = default;
	GameObjectManager(const GameObjectManager& manager) = delete;
	GameObjectManager(GameObjectManager&& manager) = delete;
	~GameObjectManager();
	GameObjectManager& operator=(const GameObjectManager& manager) = delete;
	GameObjectManager& operator=(GameObjectManager&& manager) = delete;

	void LoadLevel(const std::string& filePath, int chunkID, EntryType entryType);
	void Update(float elapsedSec, Level* pLevel, Camera* pCamera, SoundEffectManager* pSoundEffectManager);
	void Draw();
	void DoHitTest(Rectf& actorShape, Vector2f& actorVelocity);
	ObjectType TryPickUp(const Rectf& sword);
	bool IsOnGround(const Rectf& actorShape);
	bool IsOnFloor(const Rectf& actorShape, SoundEffectManager* pSoundEffectManager);
	void TryOpenDoor(const Rectf& actorShape, Inventory* pInventory, SoundEffectManager* pSoundEffectManager);
	Point2f GetElevatorPosition();
	ObjectType TrySpecialPickUp(const Rectf& actorShape, SoundEffectManager* pSoundEffectManager);
	void SpawnBossKey(const Point2f& position);
	void SetDestroyedObjects(const std::vector<int>& destroyedObjects);
	void ResetObjects();
	bool IsGameDone();
	bool IsGemInserted();

	const std::vector<int>& GetDestroyedObjects() const;
	bool HasHazardZone() const;
private:
	std::vector<int> m_DestroyedObjects{};
	std::vector<GameObject*> m_pGameObjects{};
	bool m_HasFoundFairy{ false };

	void ClearObjects();
	bool IsDestroyed(int objectId);
	void DeleteObject(int i);
};

