#include "pch.h"
#include "Fairy.h"
#include "Texture.h"

Fairy::Fairy(int objectId, const Point2f& position)
	: GameObject{ objectId, "Resources/Images/Fairy.png", ObjectType::Fairy, Rectf{ position.x, position.y, 8.0f, 16.0f }, Point2f{ 8.0f, 16.0f } }
{
}

void Fairy::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
	m_AccuSecDirection += elapsedSec;
	if (m_AccuSecDirection >= m_ChangeDirectionTime)
	{
		m_AccuSecDirection -= m_ChangeDirectionTime;
		m_Direction = -m_Direction;
	}

	const float flySpeed{ 40.0f };

	m_Shape.bottom += flySpeed * m_Direction * elapsedSec;

	m_AccuSecFrame += elapsedSec;
	if (m_AccuSecFrame >= m_FrameTime)
	{
		m_AccuSecFrame -= m_FrameTime;
		++m_CurFrame %= 2;
	}
}

void Fairy::Draw() const
{
	const Rectf srcRect
	{
		m_Shape.width * m_CurFrame,
		m_Shape.height,
		m_Shape.width,
		m_Shape.height
	};

	m_pTexture->Draw(m_Shape, srcRect);
}
