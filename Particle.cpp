#include "pch.h"
#include "Particle.h"
#include "Texture.h"

Particle::Particle(Texture* pTexture, const Point2f& position, int nrFrames, float particleTime)
	: m_pTexture{ pTexture }
	, m_Position{ position }
	, m_NrFrames{ nrFrames }
	, m_FrameTime{ particleTime / nrFrames }
{
}

void Particle::Update(float elapsedSec)
{
	m_AccuSec += elapsedSec;
	if (m_AccuSec >= m_FrameTime)
	{
		m_AccuSec -= m_FrameTime;
		++m_CurFrame;

		if (m_CurFrame >= m_NrFrames)
		{
			m_IsDone = true;
		}
	}
}

bool Particle::IsDone() const
{
	return m_IsDone;
}

void Particle::Draw() const
{
	if (m_IsDone)
	{
		return;
	}

	const float clipSize{ 16.0f };

	const Rectf srcRect
	{
		clipSize * m_CurFrame,
		clipSize,
		clipSize,
		clipSize
	};

	m_pTexture->Draw(m_Position, srcRect);
}
