#include "pch.h"
#include "IronKnuckleSword.h"
#include "Texture.h"
#include "utils.h"

IronKnuckleSword::IronKnuckleSword(const std::string& filePath, const Point2f& startPosition, const Vector2f& target, float size)
    : m_pTexture{ new Texture{filePath} }
    , m_AccumTime{ 0 }
    , m_StartPosition{ startPosition }
    , m_Target{ target }
    , m_IsActive{ false }
    , m_IsHalfWay{ false }
    , m_Size{ size }
    , m_Speed{ size }
    , m_PlayTime{ 0.4f }
    , m_Up{ false },
    m_ShapeSize{ 16.0f, 16.0f }
{
}

IronKnuckleSword::~IronKnuckleSword()
{
    delete m_pTexture;
    m_pTexture = nullptr;
}

void IronKnuckleSword::Activate()
{
    m_AccumTime = 0;
    m_IsActive = true;
    m_IsHalfWay = false;
}

void IronKnuckleSword::Update(float elapsedSec, bool isLookingRight)
{
    if (!m_IsActive)
    {
        return;
    }
    
    UpdateTimer(elapsedSec);

    UpdatePosition(isLookingRight);
}

void IronKnuckleSword::SetSwordState(bool isSwordUp)
{
    m_Up = isSwordUp;
}

void IronKnuckleSword::Draw(bool showHurtColors, int curHurtColor, bool isLookingRight) const
{
    if (!m_IsActive)
    {
        return;
    }

    Rectf srcRect
    {
        0.0f,
        m_pTexture->GetHeight(),
        m_ShapeSize.x,
        m_ShapeSize.y
    };

    if (m_Up)
    {
        srcRect.bottom -= m_ShapeSize.y;
    }

    if (showHurtColors)
    {
        srcRect.left += m_ShapeSize.x * (curHurtColor + 1);
    }

    glPushMatrix();
    {
        glTranslatef(m_Position.x + m_ShapeSize.x / 2.0f, m_Position.y, 0.0f);
        if (isLookingRight)
        {
            glScalef(-1.0f, 1.0f, 1.0f);
        }
        m_pTexture->Draw(Point2f{ -m_ShapeSize.x / 2.0f, 0.0f }, srcRect);
    }
    glPopMatrix();
}

bool IronKnuckleSword::IsActive() const
{
    return m_IsActive;
}

bool IronKnuckleSword::IsOverlapping(const Rectf& shape) const
{
    if (utils::IsOverlapping(shape, Rectf{ m_Position.x, m_Position.y,  m_ShapeSize.x,  m_ShapeSize.y / 2.0f }))
    {
        return true;
    }
    return false;
}


void IronKnuckleSword::UpdatePosition(bool isLookingRight)
{
    m_Position.x = m_StartPosition.x;
    m_Position.y = m_StartPosition.y;

    const float speed = m_Target.x / m_PlayTime;

    if (!m_IsHalfWay)
    {
        if (isLookingRight)
        {
            m_Position.x -= speed * m_AccumTime;
        }
        else
        {
            m_Position.x += speed * m_AccumTime;
        }
    }
    else
    {
        if (isLookingRight)
        {
            m_Position.x -= speed * m_PlayTime / 2.0f - speed * (m_AccumTime - m_PlayTime / 2.0f);
        }
        else
        {
            m_Position.x += speed * m_PlayTime / 2.0f - speed * (m_AccumTime - m_PlayTime / 2.0f);
        }
    }
}

void IronKnuckleSword::UpdateTimer(float elapsedSec)
{
    m_AccumTime += elapsedSec;

    if (m_AccumTime > m_PlayTime / 2.0f)
    {
        m_IsHalfWay = true;

        if (m_AccumTime > m_PlayTime)
        {
            m_IsActive = false;
        }
    }
}
