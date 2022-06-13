#include "pch.h"
#include "XPParticle.h"
#include "Texture.h"

XPParticle::XPParticle(Texture* pTexture, int amount, const Point2f position)
	: Particle{ pTexture, position, 1, 0.65f }
{
	m_SrcRect = Rectf
	{
		0.0f,
		m_pTexture->GetHeight(),
		m_pTexture->GetWidth() / 3.0f,
		m_pTexture->GetHeight()
	};
	switch (amount)
	{
	case 30:
		m_SrcRect.left += m_SrcRect.width;
		break;
	case 50:
		m_SrcRect.left += m_SrcRect.width * 2;
		break;
	}
}

void XPParticle::Update(float elapsedSec)
{
	m_AccuSec += elapsedSec;
	if (m_AccuSec >= m_Lifespan)
	{
		m_IsDone = true;
	}
}

void XPParticle::Draw() const
{
	Point2f position{ m_Position.x, m_Position.y + m_AccuSec * m_Speed };
	m_pTexture->Draw(position, m_SrcRect);
}
