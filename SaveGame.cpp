#include "pch.h"
#include "SaveGame.h"
#include "Avatar.h"
#include "XPManager.h"
#include "Level.h"
#include "EnemyManager.h"
#include "GameObjectManager.h"
#include <fstream>

void SaveGame::SaveUsernames(const std::string* pUsers, int nrUsers) const
{
	std::ofstream outputFile;
	const std::string fileName{ "Resources/SaveFiles/users.txt" };
	outputFile.open(fileName);
	std::string output{};

	for (int i{0}; i < nrUsers; ++i)
	{
		output += pUsers[i] + '\n';
	}

	outputFile << output;

	outputFile.close();
}

void SaveGame::SaveUser(const std::string& name, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, int currentChunkId, EntryType lastEntryType) const
{
	std::ofstream outputFile;
	const std::string fileName{ "Resources/SaveFiles/" + name + ".txt" };
	outputFile.open(fileName);
	std::string output{};

	output += "CurrentLevel: " + std::to_string(currentChunkId) + '\n';
	output += "PowerLevel: " + std::to_string(pXPManager->GetLevel(XPManager::Stat::Attack)) + '\n';
	output += "MagicLevel: " + std::to_string(pXPManager->GetLevel(XPManager::Stat::Magic)) + '\n';
	output += "HealthLevel: " + std::to_string(pXPManager->GetLevel(XPManager::Stat::Health)) + '\n';
	output += "Health: " + std::to_string(pAvatar->GetHealthPercentage()) + '\n';
	output += "Lifes: " + std::to_string(pAvatar->GetRemainingLifes()) + '\n';
	output += "XP: " + std::to_string(pXPManager->GetXP()) + '\n';
	const std::vector<int>& killedEnemies{ pEnemyManager->GetKilledEnemies() };
	std::string killedEnemiesString{};
	if (killedEnemies.size() == 0)
	{
		killedEnemiesString += "{}";
	}
	else
	{
		for (const int& killedEnemy : killedEnemies)
		{
			if (killedEnemy != killedEnemies[0])
			{
				killedEnemiesString += ",";
			}
			killedEnemiesString += std::to_string(killedEnemy);
		}
	}
	output += "KilledEnemies: " + killedEnemiesString + '\n';
	const std::vector<int>& destroyedItems{ pGameObjectManager->GetDestroyedObjects() };
	std::string destroyedItemsString{};
	if (destroyedItems.size() == 0)
	{
		destroyedItemsString += "{}";
	}
	else
	{
		for (const int& item : destroyedItems)
		{
			if (item != destroyedItems[0])
			{
				destroyedItemsString += ",";
			}
			destroyedItemsString += std::to_string(item);
		}
	}
	output += "DestroyedItems: " + destroyedItemsString + '\n';
	output += "Keys: " + std::to_string(pAvatar->GetKeysAmount()) + '\n';
	output += "TakenCandle: " + std::to_string(pAvatar->HasTakenCandle()) + '\n';
	output += "EntryType: " + std::to_string(int(lastEntryType));

	outputFile << output;

	outputFile.close();
}
