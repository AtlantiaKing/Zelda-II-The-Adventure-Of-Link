#include "pch.h"
#include "LoadGame.h"
#include "LevelLoader.h"
#include "XPManager.h"
#include "Avatar.h"
#include "EnemyManager.h"
#include "GameObjectManager.h"
#include <iostream>
#include <vector>
#include <sstream>

bool LoadGame::LoadUser(const std::string& name, LevelLoader* pLevelLoader, XPManager* pXPManager, Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager) const
{

	std::ifstream inputFile;
	const std::string fileName{ "Resources/SaveFiles/" + name + ".txt" };
	inputFile.open(fileName);

	if (!inputFile)
	{
		return false;
	}

	pAvatar->Reset(pXPManager);

	std::string property{};
	std::string data{};

	int curLevel{};
	int powerLevel{};
	int magicLevel{};
	int healthLevel{};
	int xp{};
	EntryType entryType{};
	bool hasCandle{};


	while (inputFile)
	{
		inputFile >> property;
		inputFile >> data;

		if (property == "CurrentLevel:")
		{
			curLevel = std::stoi(data);
		}
		else if (property == "PowerLevel:")
		{
			powerLevel = std::stoi(data);
		}
		else if (property == "MagicLevel:")
		{
			magicLevel = std::stoi(data);
		}
		else if (property == "HealthLevel:")
		{
			healthLevel = std::stoi(data);
		}
		else if (property == "XP:")
		{
			xp = std::stoi(data);
		}
		else if (property == "Health:")
		{
			pAvatar->SetHealth(healthLevel, std::stof(data));
		}
		else if (property == "Lifes:")
		{
			pAvatar->SetLifes(std::stoi(data));
		}
		else if (property == "KilledEnemies:")
		{
			if (data != "{}")
			{
				std::vector<int> killedEnemies{};
				std::stringstream dataStream{ data };
				std::string enemy{};
				while (std::getline(dataStream, enemy, ','))
				{
					killedEnemies.push_back(std::stoi(enemy));
				}
				pEnemyManager->SetKilledEnemies(killedEnemies);
			}
		}
		else if (property == "DestroyedItems:")
		{
			if (data != "{}")
			{
				std::vector<int> destroyedObj{};
				std::stringstream dataStream{ data };
				std::string item{};
				while (std::getline(dataStream, item, ','))
				{
					destroyedObj.push_back(std::stoi(item));
				}
				pGameObjectManager->SetDestroyedObjects(destroyedObj);
			}
		}
		else if (property == "Keys:")
		{
			pAvatar->AddKeys(std::stoi(data));
		}
		else if (property == "TakenCandle:")
		{
			if (data == "1")
			{
				pAvatar->GiveCandle();
			}
		}
		else if (property == "EntryType:")
		{
			entryType = EntryType(std::stoi(data));
		}
	}

	pXPManager->SetData(xp, powerLevel, magicLevel, healthLevel);
	pLevelLoader->LoadLevel(pLevel, pAvatar, pGameObjectManager, pEnemyManager, curLevel, entryType);

	inputFile.close();

	return true;
}
