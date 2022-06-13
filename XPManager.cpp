#include "pch.h"
#include "XPManager.h"
#include "Texture.h"
#include "Avatar.h"
#include "SoundEffectManager.h"

XPManager::XPManager()
    : m_pLevelUpTexture{ new Texture{ "Resources/Images/LevelUp.png" } }
    , m_pNumberTexture{ new Texture{ "Resources/Images/Numbers.png" } }
    , m_pStatSelectTexture{ new Texture{ "Resources/Images/UpgradeSelected.png" } }
    , m_pCancelSelectTexture{ new Texture{ "Resources/Images/SelectArrow.png" } }
{
    Reset();
}

XPManager::~XPManager()
{
    delete m_pLevelUpTexture;
    m_pLevelUpTexture = nullptr;
    delete m_pNumberTexture;
    m_pNumberTexture = nullptr;
    delete m_pStatSelectTexture;
    m_pStatSelectTexture = nullptr;
    delete m_pCancelSelectTexture;
    m_pCancelSelectTexture = nullptr;
}

void XPManager::Update(float elapsedSec, SoundEffectManager* pSoundEffectManager)
{
    m_AccuSec += elapsedSec;
    if (m_AccuSec >= m_TickTime)
    {
        m_AccuSec -= m_TickTime;
        if (m_NeedToAddXP > 0)
        {
            --m_NeedToAddXP;
            ++m_XP;

            bool foundUpgrade{};
            for (int i{ 0 }; i < m_NrStats; ++i)
            {
                if (!m_Checked[i])
                {
                    if (m_XP >= m_XPNeeded[i])
                    {
                        foundUpgrade = true;
                        pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::InventoryOpen);
                        break;
                    }
                }
            }

            if (foundUpgrade)
            {
                m_IsUpgrading = true;

                for (int i{ 0 }; i < m_NrStats; ++i)
                {
                    if (m_XP >= m_XPNeeded[i])
                    {
                        m_Selected = i;
                        break;
                    }
                }
            }
        }
    }

    if (m_IsMenuClosing)
    {
        m_IsMenuClosing = pSoundEffectManager->IsPlaying(SoundEffectManager::SoundType::ItemPickup);
    }
}

void XPManager::AddXP(int amount)
{
    m_NeedToAddXP += amount;
}

void XPManager::MoveSelectionUp(SoundEffectManager* pSoundEffectManager)
{
    if (!m_IsUpgrading)
    {
        return;
    }

    pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::UpgradeSelect);

    --m_Selected;

    if (m_Selected < -1)
    {
        m_Selected = int(Stat::Health);
    }
    else if (m_Selected == -1)
    {
        if (m_XP >= m_XPNeeded[int(Stat::Attack)] && m_XP >= m_XPNeeded[int(Stat::Magic)] && m_XP >= m_XPNeeded[int(Stat::Health)])
        {
            m_Selected = int(Stat::Health);
        }
    }

    if (m_Selected >= int(Stat::Attack))
    {
        for (int i{ m_Selected }; m_Selected >= 0; --i)
        {
            if (m_XP >= m_XPNeeded[m_Selected])
            {
                break;
            }

            m_Selected = i - 1;
        }
    }
}

void XPManager::MoveSelectionDown(SoundEffectManager* pSoundEffectManager)
{
    if (!m_IsUpgrading)
    {
        return;
    }

    pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::UpgradeSelect);

    ++m_Selected;

    if (m_Selected > int(Stat::Health))
    {
        if (m_XP >= m_XPNeeded[int(Stat::Attack)] && m_XP >= m_XPNeeded[int(Stat::Magic)] && m_XP >= m_XPNeeded[int(Stat::Health)])
        {
            m_Selected = 0;
        }
        else
        {
            m_Selected = -1;
        }
    }

    if (m_Selected >= int(Stat::Attack))
    {
        for (int i{ m_Selected }; m_Selected <= int(Stat::Health); ++i)
        {
            if (m_XP >= m_XPNeeded[m_Selected])
            {
                break;
            }

            m_Selected = i+1;
            if (m_Selected > int(Stat::Health))
            {
                m_Selected = -1;
                break;
            }
        }
    }
}

void XPManager::Select(Avatar* pAvatar, SoundEffectManager* pSoundEffectManager)
{
    if (!m_IsUpgrading)
    {
        return;
    }

    m_IsUpgrading = false;
    m_IsMenuClosing = true;

    if (m_Selected == -1)
    {
        for (int i{ 0 }; i < m_NrStats; ++i)
        {
            if (m_XP >= m_XPNeeded[i])
            {
                m_Checked[i] = true;
            }
        }
        return;
    }

    UpgradeStat(pAvatar);

    pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::ItemPickup);
}

void XPManager::UpgradeStat(Avatar* pAvatar)
{
    ++m_Levels[m_Selected];
    m_XP -= m_XPNeeded[m_Selected];

    if (m_Selected == int(Stat::Health))
    {
        pAvatar->SetHealth(m_Levels[m_Selected]);
    }

    UpdateNeededXP(m_Selected);

    for (int i{ 0 }; i < m_NrStats; ++i)
    {
        if (m_XP < m_XPNeeded[i])
        {
            m_Checked[i] = false;
        }
    }
}

void XPManager::SetData(int xp, int powerLevel, int magicLevel, int healthLevel)
{
    m_Levels[int(Stat::Attack)] = powerLevel;
    m_Levels[int(Stat::Magic)] = magicLevel;
    m_Levels[int(Stat::Health)] = healthLevel;
    m_XP = xp;

    for (int i{ 0 }; i <= int(Stat::Health); ++i)
    {
        UpdateNeededXP(i);

        m_Checked[i] = m_XP >= m_XPNeeded[i];
    }
}

void XPManager::Reset()
{
    m_XPNeeded[int(Stat::Attack)] = 200;
    m_Levels[int(Stat::Attack)] = 1;

    m_XPNeeded[int(Stat::Magic)] = 100;
    m_Levels[int(Stat::Magic)] = 1;

    m_XPNeeded[int(Stat::Health)] = 50;
    m_Levels[int(Stat::Health)] = 1;
    
    m_NeedToAddXP = 0;
    m_XP = 0;
    m_IsUpgrading = false;
    m_IsMenuClosing = false;

    for (int i{ 0 }; i <= int(Stat::Health); ++i)
    {
        m_Checked[i] = false;
    }
    m_Selected = 0;
}

void XPManager::Draw() const
{
    if (m_IsUpgrading || m_IsMenuClosing)
    {
        const Point2f position
        {
            110.0f,
            32.0f
        };

        const Point2f attackXpPos
        {
            10.0f,
            104.0f
        };
        const Point2f magicXpPos
        {
            10.0f,
            64.0f
        };
        const Point2f healthXpPos
        {
            10.0f,
            24.0f
        };
        Point2f statUpgradePos
        {
            49.0f,
            16.0f
        };
        const Point2f cancelPos
        {
            49.0f,
            136.0f
        };

        glPushMatrix();
        {
            glTranslatef(position.x, position.y, 0.0f);

            m_pLevelUpTexture->Draw();

            DrawNumber(m_XPNeeded[int(Stat::Attack)], attackXpPos);
            DrawNumber(m_XPNeeded[int(Stat::Magic)], magicXpPos);
            DrawNumber(m_XPNeeded[int(Stat::Health)], healthXpPos);

            if (int(m_Selected) >= 0)
            {
                statUpgradePos.y += (2 - int(m_Selected)) * 40.0f;
                m_pStatSelectTexture->Draw(statUpgradePos);
            }
            else
            {
                m_pCancelSelectTexture->Draw(cancelPos);
            }
        }
        glPopMatrix();
    }
}

int XPManager::GetDamage() const
{
    switch (m_Levels[int(Stat::Attack)])
    {
    case 1:
    case 2:
    case 3:
        return 2 + 1 * (m_Levels[int(Stat::Attack)] - 1);
    case 4:
    case 5:
        return 4 + 2 * (m_Levels[int(Stat::Attack)] - 3);
    case 6:
    case 7:
        return 8 + 4 * (m_Levels[int(Stat::Attack)] - 5);
    case 8:
        return 16 + 8;
    }
    return 0;
}

int XPManager::GetXP() const
{
    return m_XP;
}

int XPManager::GetNeededXP() const
{
    int smallest = INT16_MAX;

    for (int i{ 0 }; i < 3; ++i)
    {
        if (m_XP < m_XPNeeded[i])
        {
            if (smallest > m_XPNeeded[i])
            {
                smallest = m_XPNeeded[i];
            }
        }
    }

    return smallest;
}

bool XPManager::IsUpgrading() const
{
    return m_IsUpgrading || m_IsMenuClosing;
}

int XPManager::GetLevel(Stat stat) const
{
    return m_Levels[int(stat)];
}

void XPManager::UpdateNeededXP(int stat)
{
    switch (stat)
    {
        case int(Stat::Health) :
        {
            m_XPNeeded[stat] = 50;
            const int xpAddition{ 100 };
            for (int prevLevel{ 0 }; prevLevel <= m_Levels[stat]; ++prevLevel)
            {
                switch (prevLevel)
                {
                case 2:
                case 3:
                case 4:
                    m_XPNeeded[stat] += xpAddition + 150 * (prevLevel - 2);
                    break;
                case 5:
                case 6:
                    m_XPNeeded[stat] += xpAddition + 150 * 2 + 300 * (prevLevel - 4);
                    break;
                case 7:
                    m_XPNeeded[stat] += xpAddition + 150 * 2 + 300 * 2 + 500;
                    break;
                }
            }
            break;
        }
        case int(Stat::Magic) :
            switch (m_Levels[stat])
            {
            case 2:
                m_XPNeeded[stat] = 300;
                break;
            case 3:
                m_XPNeeded[stat] = 700;
                break;
            case 4:
                m_XPNeeded[stat] = 1200;
                break;
            case 5:
                m_XPNeeded[stat] = 2200;
                break;
            case 6:
                m_XPNeeded[stat] = 3500;
                break;
            case 7:
                m_XPNeeded[stat] = 600;
                break;
            }
        break;
        case int(Stat::Attack) :
            switch (m_Levels[stat])
            {
            case 2:
                m_XPNeeded[stat] = 500;
                break;
            case 3:
                m_XPNeeded[stat] = 1000;
                break;
            case 4:
                m_XPNeeded[stat] = 2000;
                break;
            case 5:
                m_XPNeeded[stat] = 3500;
                break;
            case 6:
                m_XPNeeded[stat] = 5000;
                break;
            case 7:
                m_XPNeeded[stat] = 8000;
                break;
            }
        break;
    }
}

void XPManager::DrawNumber(int xp, const Point2f& attackXpPos) const
{
    const float digitSize{ 8.0f };
    for (int i{ 0 }; i < 4; ++i)
    {
        if (i == 0 && xp < 1000)
        {
            continue;
        }
        if (i == 1 && xp < 100)
        {
            continue;
        }

        const int digit = xp / int(powf(10.0f, 3.0f - i));
        xp -= digit * int(powf(10.0f, 3.0f - i));

        Rectf srcRect
        {
            digit * digitSize,
            m_pNumberTexture->GetHeight(),
            digitSize,
            m_pNumberTexture->GetHeight()
        };

        Rectf dstRect
        {
            attackXpPos.x + i * digitSize,
            attackXpPos.y,
            digitSize,
            m_pNumberTexture->GetHeight()
        };

        m_pNumberTexture->Draw(dstRect, srcRect);
    }
}