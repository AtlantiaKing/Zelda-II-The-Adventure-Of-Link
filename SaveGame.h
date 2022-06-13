#pragma once
#include "enums.h"

class Avatar;
class XPManager;
class EnemyManager;
class GameObjectManager;

class SaveGame final
{
public:
	void SaveUsernames(const std::string* pUsers, int nrUsers) const;
	void SaveUser(const std::string& name, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, int currentChunkId, EntryType lastEntryType) const;
private:
};

