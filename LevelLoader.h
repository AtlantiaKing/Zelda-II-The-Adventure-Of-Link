#pragma once
#include "enums.h"

class Level;
class Avatar;
class GameObjectManager;
class EnemyManager;

class LevelLoader final
{
public:
	LevelLoader(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager);

	void Update(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager);
	void LoadLevel(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, int chunkId, EntryType entryType);
	void LoadLevel(Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, bool reset = false);
	EntryType GetLastEntryType() const;
	int GetChunkID() const;
private:
	void LoadChunkID(const std::string& filePath, EntryType entryType);

	void TeleportPlayer(const std::string& filePath, EntryType entryType, const Rectf& m_Boundaries, GameObjectManager* pGameObjectManager, Avatar* pAvatar) const;
	
	EntryType m_LastEntryType{ EntryType::right };
	int m_ChunkID;
};

