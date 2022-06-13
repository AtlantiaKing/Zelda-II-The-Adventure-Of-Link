#include "pch.h"
#include "FallingFloor.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffectManager.h"

FallingFloor::FallingFloor(int objectId, const Point2f& position)
    : GameObject{ objectId, "Resources/Images/FallingFloor.png", ObjectType::Falling_Floor, Rectf{ position.x, position.y, 16.0f, 16.0f }, Point2f{ 16.0f, 16.0f } }
{
}

void FallingFloor::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
    if (m_IsFalling)
    {
        m_AccuSec += elapsedSec;
        if (m_AccuSec >= m_FrameTime)
        {
            ++m_CurFrame;
            m_AccuSec -= m_FrameTime;

            if (m_CurFrame >= m_NrFrames)
            {
                m_IsDestroyed = true;
            }
        }
    }
}

void FallingFloor::Draw() const
{
    const float clipSize{ 16.0f };
    const Rectf srcRect
    {
        m_CurFrame * clipSize,
        clipSize,
        clipSize,
        clipSize
    };
    m_pTexture->Draw(m_Shape, srcRect);
}

bool FallingFloor::IsOnGround(const Rectf& actorShape)
{
    bool isOnGround{};
    utils::HitInfo hitInfo{};

    const Point2f leftTop{ actorShape.left + 1, actorShape.bottom + actorShape.height };
    const Point2f leftBottom{ actorShape.left + 1, actorShape.bottom - 1 };

    const Point2f rightTop{ actorShape.left + actorShape.width - 1, actorShape.bottom + actorShape.height };
    const Point2f rightBottom{ actorShape.left + actorShape.width - 1, actorShape.bottom - 1 };

    std::vector<Point2f> floor
    {
        Point2f
        {
            m_Shape.left,
            m_Shape.bottom + m_Shape.height
        },
        Point2f
        {
            m_Shape.left + m_Shape.width,
            m_Shape.bottom + m_Shape.height
        } 
    };

    if (utils::Raycast(floor, leftTop, leftBottom, hitInfo) || utils::Raycast(floor, rightTop, rightBottom, hitInfo))
    {
        isOnGround = true;
    }

    return isOnGround;
}

void FallingFloor::TryFall(SoundEffectManager* pSoundEffectManager)
{
    if (!m_IsFalling)
    {
        m_IsFalling = true;
        pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::FallingFloor);
    }
}
