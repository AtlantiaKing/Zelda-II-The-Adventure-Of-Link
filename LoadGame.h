#pragma once
#include <fstream>
#include <array>

class LevelLoader;
class Level;
class Avatar;
class GameObjectManager;
class EnemyManager;
class XPManager;

class LoadGame final
{
public:
	template<int T>
	std::array<std::string, T> GetUsernames() const
	{
		std::ifstream inputFile;
		const std::string fileName{ "Resources/SaveFiles/users.txt" };
		inputFile.open(fileName);

		std::array<std::string, T> users;
		int curUser{};

		while (inputFile)
		{
			if (curUser == T)
			{
				break;
			}
			std::string userName{};
			std::getline(inputFile, userName);
			users[curUser] = userName;
			++curUser;
		}

		inputFile.close();

		return users;
	}

	bool LoadUser(const std::string& name, LevelLoader* pLevelLoader, XPManager* pXPManager, Level* pLevel, Avatar* pAvatar, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager) const;
private:
};

