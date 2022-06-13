#pragma once
#include <vector>
#include "enums.h"

class XPManager;
class Texture;

class Inventory final
{
public:
	enum class ItemType
	{
		Candle
	};

	Inventory(XPManager* pXPManager);
	Inventory(const Inventory& other) = delete;
	Inventory(Inventory&& other) = delete;
	~Inventory();
	Inventory& operator=(const Inventory& other) = delete;
	Inventory& operator=(Inventory&& other) = delete;

	void UseKey();
	void AddItemFromObject(ObjectType type);

	bool HasKey() const;
	void Draw(const Point2f& abovePlayerPosition) const;
	int GetKeysAmount() const;
	bool HasTakenCandle() const;
private:
	int m_NrKeys{};
	std::vector<ItemType> m_Items{};
	XPManager* m_pXPManager;
	Texture* m_pPickedUpTexture{ nullptr };
};

