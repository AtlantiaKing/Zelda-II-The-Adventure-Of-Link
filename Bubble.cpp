#include "pch.h"
#include "Bubble.h"
#include "Level.h"
#include "Texture.h"

Bubble::Bubble(const Point2f& position)
	: Enemy{ 0, "Bubble", position, Rectf{ 0, 0, 16, 15}, 255, 50, 8, 1.0f }
{
	const float minSpeed{ 60.0f };
	const float maxSpeed{ 120.0f };
	const float speed{ rand() % int(maxSpeed - minSpeed) + minSpeed };

	m_Velocity.x = speed;
	m_Velocity.y = speed;
}

void Bubble::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	if (!m_IsActive)
	{
		return;
	}

	Enemy::Update(elapsedSec, actorShape, pLevel);

	if (!m_IsHit)
	{
		HandleMovement(elapsedSec);

		HandleCollisions(pLevel);
	}

	UpdateSprite(elapsedSec);
}

void Bubble::SetActive(bool isActive)
{
	m_IsActive = isActive;
}

void Bubble::InverseVelocity(bool xDirection, bool yDirection)
{
	if (xDirection)
	{
		m_Velocity.x = -m_Velocity.x;
	}
	if (yDirection)
	{
		m_Velocity.y = -m_Velocity.y;
	}
}

void Bubble::Draw() const
{
	if (!m_IsActive)
	{
		return;
	}

	const Rectf srcRect
	{
		m_pTexture->GetWidth() / 4.0f * m_TextureState,
		m_pTexture->GetHeight(),
		m_pTexture->GetWidth() / 4.0f,
		m_pTexture->GetHeight()
	};

	m_pTexture->Draw(m_Position, srcRect);
}

bool Bubble::IsActive() const
{
	return m_IsActive;
}

bool Bubble::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	if (m_IsHit)
	{
		return false;
	}

	return Enemy::DoHitTest(actorShape, actorVelocity);
}


void Bubble::HandleMovement(float elapsedSec)
{
	m_Position.x += m_Velocity.x * elapsedSec;
	m_Position.y += m_Velocity.y * elapsedSec;
}

void Bubble::HandleCollisions(Level* pLevel)
{
	Rectf collider{ CalculateCollider() };

	pLevel->HandleCollision(collider, m_Velocity, true);

	m_Position.x = collider.left - m_Collider.left;
	m_Position.y = collider.bottom - m_Collider.bottom;
}

void Bubble::UpdateSprite(float elapsedSec)
{
	float frameTime{ 1.0f / m_FPS };
	if (m_IsHit)
	{
		frameTime = 1.0f / 10.0f;
	}
	m_AccuSec += elapsedSec;
	if (m_AccuSec >= frameTime)
	{
		m_TextureState = (m_TextureState + 1) % 4;
		m_AccuSec = 0;
	}
}