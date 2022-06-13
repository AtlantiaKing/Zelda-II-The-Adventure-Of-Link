#include "pch.h"
#include "Inventory.h"
#include "XPManager.h"
#include "Texture.h"

Inventory::Inventory(XPManager* pXPManager)
    : m_pXPManager{ pXPManager }
{

}

Inventory::~Inventory()
{
    delete m_pPickedUpTexture;
    m_pPickedUpTexture = nullptr;
}

void Inventory::UseKey()
{
    --m_NrKeys;
}

void Inventory::AddItemFromObject(ObjectType type)
{
    switch (type)
    {
    case ObjectType::Key:
        ++m_NrKeys;
        break;
    case ObjectType::Candle:
        m_Items.push_back(ItemType::Candle);
        if (m_pPickedUpTexture != nullptr)
        {
            delete m_pPickedUpTexture;
            m_pPickedUpTexture = nullptr;
        }
        m_pPickedUpTexture = new Texture{ "Resources/Images/Candle.png" };
        break;
    case ObjectType::XPBag:
        m_pXPManager->AddXP(50);
        break;
    }
}

bool Inventory::HasKey() const
{
    return m_NrKeys > 0;
}

void Inventory::Draw(const Point2f& abovePlayerPosition) const
{
    m_pPickedUpTexture->Draw(abovePlayerPosition);
}

int Inventory::GetKeysAmount() const
{
    return m_NrKeys;
}

bool Inventory::HasTakenCandle() const
{
    for (const ItemType& item : m_Items)
    {
        if (item == ItemType::Candle)
        {
            return true;
        }
    }
    return false;
}
