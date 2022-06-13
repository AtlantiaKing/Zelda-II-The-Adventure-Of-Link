#include "pch.h"
#include "SwordBeam.h"
#include "Texture.h"
#include "EnemyManager.h"
#include "XPManager.h"

SwordBeam::SwordBeam()
	: m_pTexture{ new Texture{"Resources/Images/swordbeam.png"} }
	, m_IsActive{ false }
	, m_IsDestroyed{ false }
	, m_Position{}
	, m_Velocity{ 100.0f }
	, m_AccuSeconds{}
	, m_AccuFrameSeconds{}
	, m_Column{}
{
}

SwordBeam::~SwordBeam()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void SwordBeam::Activate(const Point2f& position, float direction)
{
	if (!m_IsActive)
	{
		const float beamInTextureOffset{ 6.0f };

		m_Position = position;
		m_Position.y -= beamInTextureOffset;
		m_IsActive = true;
		m_Velocity = abs(m_Velocity) * direction;
		m_AccuSeconds = 0;
		m_IsDestroyed = false;
	}
}

void SwordBeam::Update(float elapsedSec, EnemyManager* pEnemyManager, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsActive)
	{
		m_Position.x += m_Velocity * elapsedSec;

		if (!m_IsDestroyed)
		{
			const Rectf collider
			{
				m_Position.x,
				m_Position.y,
				m_pTexture->GetWidth() / 2.0f,
				m_pTexture->GetHeight() / 2.0f
			};

			pEnemyManager->TryHitEnemies(collider, true, pSoundEffectManager);
		}

		UpdateSprite(elapsedSec);
	}
}

void SwordBeam::Draw() const
{
	if (m_IsActive)
	{
		Rectf srcRect
		{
			m_Column * m_pTexture->GetWidth() / 2.0f,
			m_pTexture->GetHeight() / 2.0f,
			m_pTexture->GetWidth() / 2.0f,
			m_pTexture->GetHeight() / 2.0f
		};

		if (m_IsDestroyed)
		{
			srcRect.bottom += m_pTexture->GetHeight() / 2.0f;
		}

		glPushMatrix();
		{
			glTranslatef(m_Position.x, m_Position.y, 0.0f);
			if (m_Velocity < 0)
			{
				glScalef(-1.0f, 1.0f, 1.0f);
			}
			m_pTexture->Draw(Point2f{0.0f, 0.0f}, srcRect);
		}
		glPopMatrix();
	}
}

bool SwordBeam::IsActive() const
{
	return m_IsActive;
}


void SwordBeam::UpdateSprite(float elapsedSec)
{
	m_AccuSeconds += elapsedSec;
	m_IsDestroyed = m_AccuSeconds > m_Lifespan;

	m_AccuFrameSeconds += elapsedSec;

	float usedFrameTime{ m_FrameTime };
	if (m_IsDestroyed)
	{
		usedFrameTime *= 2.0f;
	}

	if (m_AccuFrameSeconds > usedFrameTime)
	{
		if (m_IsDestroyed && m_Column == 1)
		{
			m_IsActive = false;
		}
		m_Column = (m_Column + 1) % 2;
		m_AccuFrameSeconds = 0;
	}
}