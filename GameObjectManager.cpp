#include "pch.h"
#include "GameObjectManager.h"
#include "Elevator.h"
#include <fstream>
#include <sstream>
#include "FallingFloor.h"
#include "XPBag.h"
#include "Door.h"
#include "Fairy.h"
#include "BossKey.h"
#include "SoundEffectManager.h"
#include "Camera.h"
#include "GemStatue.h"

GameObjectManager::~GameObjectManager()
{
	ClearObjects();
}

void GameObjectManager::LoadLevel(const std::string& filePath, int chunkID, EntryType entryType)
{
	ClearObjects();

	std::ifstream file;
	file.open(filePath + "objects.txt");

	std::string line{};
	int nrLine{};
	while (std::getline(file, line))
	{
		const int objectId{ chunkID * 100 + nrLine };

		if (IsDestroyed(objectId))
		{
			++nrLine;
			continue;
		}

		std::stringstream stream{ line };
		std::string segment;

		ObjectType objectType{};
		Point2f position{};

		int nrSegment{};
		while (std::getline(stream, segment, ';'))
		{
			const int data{ std::stoi(segment) };

			switch (nrSegment)
			{
			case 0:
				objectType = ObjectType(data);
				break;
			case 1:
				position.x = float(data);
				break;
			case 2:
				position.y = float(data);
				break;
			}

			++nrSegment;
		}

		switch (objectType)
		{
		case ObjectType::Elevator:
		{
			const float bottomPosition{ 0.0f };
			const float topPosition{ 183.0f };

			switch (entryType)
			{
			case EntryType::down:
				position.y = topPosition;
				break;
			case EntryType::up:
				position.y = bottomPosition;
				break;
			}

			m_pGameObjects.push_back(new Elevator{ objectId, position });
		}
			break;
		case ObjectType::Key:
			m_pGameObjects.push_back(new GameObject{ objectId, "Resources/Images/Key.png", ObjectType::Key, Rectf{ position.x, position.y, 16.0f, 16.0f }, Point2f{ 16.0f, 16.0f } });
			break;
		case ObjectType::Candle:
			m_pGameObjects.push_back(new GameObject{ objectId, "Resources/Images/Candle.png", ObjectType::Candle, Rectf{ position.x, position.y, 16.0f, 16.0f }, Point2f{ 16.0f, 16.0f } });
			break;
		case ObjectType::Falling_Floor:
			m_pGameObjects.push_back(new FallingFloor{ objectId, position });
			break;
		case ObjectType::Door:
		case ObjectType::Boss_Door:
			m_pGameObjects.push_back(new Door{ objectId, objectType == ObjectType::Boss_Door });
			break;
		case ObjectType::XPBag:
			m_pGameObjects.push_back(new XPBag{ objectId, position });
			break;
		case ObjectType::Fairy:
			m_pGameObjects.push_back(new Fairy{ objectId, position });
			break;
		case ObjectType::Hazard_Zone:
			m_pGameObjects.push_back(new GameObject{ objectId, "Resources/Images/Hazard.png", ObjectType::Hazard_Zone, Rectf{ position.x, position.y, 640.0f, 32.0f }, Point2f{ 0.0f,0.0f } });
			break;
		case ObjectType::Gem_Statue:
			m_pGameObjects.push_back(new GemStatue{ objectId, position });
			break;
		}

		++nrLine;
	}
	file.close();
}

void GameObjectManager::Update(float elapsedSec, Level* pLevel, Camera* pCamera, SoundEffectManager* pSoundEffectManager)
{
	for (int i{ int(m_pGameObjects.size() - 1) }; i >= 0; --i)
	{
		if (m_pGameObjects[i]->GetType() == ObjectType::XPBag)
		{
			static_cast<XPBag*>(m_pGameObjects[i])->Update(elapsedSec, pLevel, this);
		}
		else
		{
			m_pGameObjects[i]->Update(elapsedSec, pLevel, pSoundEffectManager);
		}

		if (typeid(*m_pGameObjects[i]) == typeid(Fairy))
		{
			if (!m_HasFoundFairy)
			{
				if (pCamera->IsVisible(m_pGameObjects[i]->GetCenterPosition()))
				{
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::FairyFound);
					m_HasFoundFairy = true;
				}
			}
		}

		if (m_pGameObjects[i]->IsDestroyed())
		{
			DeleteObject(i);
		}
	}
}

void GameObjectManager::Draw()
{
	for (GameObject* pGameObject : m_pGameObjects)
	{
		pGameObject->Draw();
	}
}

void GameObjectManager::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity)
{
	for (GameObject* pGameObject : m_pGameObjects)
	{
		pGameObject->DoHitTest(actorShape, actorVelocity);
	}
}

ObjectType GameObjectManager::TryPickUp(const Rectf& sword)
{
	for (int i{ int(m_pGameObjects.size() - 1) }; i >= 0; --i)
	{
		ObjectType type{ m_pGameObjects[i]->GetType() };

		if (type == ObjectType::Key || type == ObjectType::XPBag)
		{
			if (m_pGameObjects[i]->IsOverlapping(sword))
			{
				ObjectType type{ m_pGameObjects[i]->GetType() };
				DeleteObject(i);
				return type;
			}
		}
	}
	return ObjectType::Nothing;
}

bool GameObjectManager::IsOnGround(const Rectf& actorShape)
{
	bool isOnGround{ false };
	for (GameObject* pGameObject : m_pGameObjects)
	{
		isOnGround = pGameObject->IsOnGround(actorShape);
		if (isOnGround)
		{
			return isOnGround;
		}
	}
	return isOnGround;
}

bool GameObjectManager::IsOnFloor(const Rectf& actorShape, SoundEffectManager* pSoundEffectManager)
{
	bool isOnFloor{ false };
	for (GameObject* pGameObject : m_pGameObjects)
	{
		if (typeid(*pGameObject) == typeid(FallingFloor))
		{
			isOnFloor = pGameObject->IsOnGround(actorShape);
			if (isOnFloor)
			{
				static_cast<FallingFloor*>(pGameObject)->TryFall(pSoundEffectManager);
				return isOnFloor;
			}
		}
	}
	return false;
}

void GameObjectManager::TryOpenDoor(const Rectf& actorShape, Inventory* pInventory, SoundEffectManager* pSoundEffectManager)
{
	for (GameObject* pGameObject : m_pGameObjects)
	{
		if (typeid(*pGameObject) == typeid(Door))
		{
			static_cast<Door*>(pGameObject)->TryOpen(actorShape, pInventory, pSoundEffectManager);
		}
	}
}

Point2f GameObjectManager::GetElevatorPosition()
{
	for (GameObject* pGameObject : m_pGameObjects)
	{
		if (pGameObject->GetType() == ObjectType::Elevator)
		{
			return pGameObject->GetCenterPosition();
		}
	}

	return Point2f();
}

ObjectType GameObjectManager::TrySpecialPickUp(const Rectf& actorShape, SoundEffectManager* pSoundEffectManager)
{
	for (int i{ int(m_pGameObjects.size() - 1) }; i >= 0; --i)
	{
		ObjectType type{ m_pGameObjects[i]->GetType() };

		if (type == ObjectType::Fairy || type == ObjectType::Candle)
		{
			if (m_pGameObjects[i]->IsOverlapping(actorShape))
			{
				DeleteObject(i);
				if (type == ObjectType::Candle)
				{
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::ItemPickup);
				}
				return type;
			}
		}
	}
	return ObjectType::Nothing;
}

void GameObjectManager::SpawnBossKey(const Point2f& position)
{
	GameObject* pBossKey{ new BossKey{ position } };

	m_pGameObjects.push_back(pBossKey);
}

void GameObjectManager::SetDestroyedObjects(const std::vector<int>& destroyedObjects)
{
	m_DestroyedObjects = destroyedObjects;
}

void GameObjectManager::ResetObjects()
{
	m_DestroyedObjects.clear();
}

bool GameObjectManager::IsGameDone()
{
	for (size_t i{ 0 }; i < m_pGameObjects.size(); ++i)
	{
		if (m_pGameObjects[i]->GetType() == ObjectType::Gem_Statue)
		{
			return static_cast<GemStatue*>(m_pGameObjects[i])->IsInsertingGem();
		}
	}
	return false;
}

bool GameObjectManager::IsGemInserted()
{
	for (size_t i{ 0 }; i < m_pGameObjects.size(); ++i)
	{
		if (m_pGameObjects[i]->GetType() == ObjectType::Gem_Statue)
		{
			return static_cast<GemStatue*>(m_pGameObjects[i])->IsAnimationDone();
		}
	}
	return false;
}

const std::vector<int>& GameObjectManager::GetDestroyedObjects() const
{
	return m_DestroyedObjects;
}

bool GameObjectManager::HasHazardZone() const
{
	for (size_t i{ 0 }; i < m_pGameObjects.size(); ++i)
	{
		if (m_pGameObjects[i]->GetType() == ObjectType::Hazard_Zone)
		{
			return true;
		}
	}
	return false;
}


void GameObjectManager::ClearObjects()
{
	for (size_t i{ 0 }; i < m_pGameObjects.size(); ++i)
	{
		delete m_pGameObjects[i];
		m_pGameObjects[i] = nullptr;
	}
	m_pGameObjects.clear();
}

bool GameObjectManager::IsDestroyed(int objectId)
{
	for (const int& destroyedId : m_DestroyedObjects)
	{
		if (destroyedId == objectId)
		{
			return true;
		}
	}
	return false;
}

void GameObjectManager::DeleteObject(int objectIndex)
{
	ObjectType type{ m_pGameObjects[objectIndex]->GetType() };
	if (type == ObjectType::Key || type == ObjectType::Door || type == ObjectType::Candle || type == ObjectType::Fairy || type == ObjectType::XPBag)
	{
		m_DestroyedObjects.push_back(m_pGameObjects[objectIndex]->GetID());
	}
	delete m_pGameObjects[objectIndex];
	m_pGameObjects[objectIndex] = m_pGameObjects[m_pGameObjects.size() - 1];
	m_pGameObjects.pop_back();
}
