#include "pch.h"
#include "LevelLoader.h"
#include "Avatar.h"
#include "Level.h"
#include "GameObjectManager.h"
#include "EnemyManager.h"
#include <fstream>

LevelLoader::LevelLoader(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager)
	: m_ChunkID{ 0 }
{
	LoadLevel(pLevel, pAvatar, pGameObjectManager, pEnemyManager);
}

void LevelLoader::Update(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager)
{
	const Rectf actorShape{ pAvatar->GetShape() };
	const Rectf m_Boundaries{ pLevel->GetBoundaries() };

	bool newLevel{};
	EntryType entryType{};

	if (actorShape.bottom + actorShape.height / 2.0f < m_Boundaries.bottom)
	{
		newLevel = true;
		entryType = EntryType::down;
	}
	else if (actorShape.bottom + actorShape.height + actorShape.height / 2.0f > m_Boundaries.bottom + m_Boundaries.height)
	{
		newLevel = true;
		entryType = EntryType::up;
	}
	else if (actorShape.left + actorShape.width / 4.0f < m_Boundaries.left)
	{
		newLevel = true;
		entryType = EntryType::left;
	}
	else if (actorShape.left + actorShape.width / 4.0f * 3.0f > m_Boundaries.left + m_Boundaries.width)
	{
		newLevel = true;
		entryType = EntryType::right;
	}

	if (!newLevel)
	{
		return;
	}

	m_LastEntryType = entryType;

	std::string filePath{ "Resources/Levels/Level" + std::to_string(m_ChunkID) + "/" };

	LoadChunkID(filePath, entryType);

	filePath = "Resources/Levels/Level" + std::to_string(m_ChunkID) + "/";

	pLevel->LoadLevelData(filePath);
	pGameObjectManager->LoadLevel(filePath, m_ChunkID, entryType);
	pEnemyManager->LoadEnemies(filePath, m_ChunkID);

	TeleportPlayer(filePath, entryType, m_Boundaries, pGameObjectManager, pAvatar);
}

EntryType LevelLoader::GetLastEntryType() const
{
	return m_LastEntryType;
}

int LevelLoader::GetChunkID() const
{
	return m_ChunkID;
}


void LevelLoader::LoadChunkID(const std::string& filePath, EntryType entryType)
{
	std::ifstream file;
	file.open(filePath + "exits.txt");

	std::string line{};
	int nrLine{};
	while (std::getline(file, line))
	{
		if (nrLine == int(entryType))
		{
			m_ChunkID = std::stoi(line);
			break;
		}
		++nrLine;
	}

	file.close();
}

void LevelLoader::TeleportPlayer(const std::string& filePath, EntryType entryType, const Rectf& m_Boundaries, GameObjectManager* pGameObjectManager, Avatar* pAvatar) const
{
	Rectf actorShape{ pAvatar->GetShape() };

	std::ifstream file;
	if (entryType == EntryType::left || entryType == EntryType::right)
	{
		file.open(filePath + "entries.txt");

		const float prevShapeBottom{ actorShape.bottom };
		if (file)
		{
			std::string line{};
			int nrLine{ 0 };
			while (std::getline(file, line))
			{
				if (nrLine == int(entryType))
				{
					actorShape.bottom = std::stof(line);
					break;
				}
				++nrLine;
			}
		}
		const float margin{ 3.0f };
		if (abs(prevShapeBottom - actorShape.bottom) < margin)
		{
			const float floorHeight{ 32.0f };
			actorShape.bottom = floorHeight;
		}

		file.close();

		if (entryType == EntryType::right)
		{
			actorShape.left = 0.0f;
		}
		else
		{
			actorShape.left = m_Boundaries.width - actorShape.width;
		}
	}
	else
	{
		const Point2f elevatorPosition{ pGameObjectManager->GetElevatorPosition() };
		const float elevatorPlatformSize{ 8.0f };

		actorShape.bottom = elevatorPosition.y + elevatorPlatformSize;
		actorShape.left = elevatorPosition.x - actorShape.width / 2.0f;
	}

	pAvatar->Teleport(actorShape);
}

void LevelLoader::LoadLevel(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, bool reset)
{
	if (reset)
	{
		m_ChunkID = 0;
		m_LastEntryType = EntryType::right;
		pGameObjectManager->ResetObjects();
		pEnemyManager->ResetEnemies();
	}

	const std::string filePath{ "Resources/Levels/Level" + std::to_string(m_ChunkID) + "/" };
	pLevel->LoadLevelData(filePath);
	pGameObjectManager->LoadLevel(filePath, m_ChunkID, m_LastEntryType);
	pEnemyManager->LoadEnemies(filePath, m_ChunkID);

	TeleportPlayer(filePath, m_LastEntryType, pLevel->GetBoundaries(), pGameObjectManager, pAvatar);
}

void LevelLoader::LoadLevel(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, int chunkId, EntryType entryType)
{
	m_ChunkID = chunkId;
	m_LastEntryType = entryType;
	LoadLevel(pLevel, pAvatar, pGameObjectManager, pEnemyManager);
}