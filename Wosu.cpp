#include "pch.h"
#include "Wosu.h"
#include "Texture.h"
#include "Level.h"
#include "Camera.h"

Wosu::Wosu(const Point2f& position, bool lookingRight)
	: Enemy(0, "Wosu", position, Rectf{ 0, 3, 16, 29}, 1, 0, 8, 0.0f)
{
	const float minSpeed{ 51.0f };
	const float maxSpeed{ 85.0f };
	const float walkSpeed{ rand() % int(maxSpeed - minSpeed) + minSpeed };

	m_Velocity.x = walkSpeed;
	if (!lookingRight)
	{
		m_Velocity.x *= -1;
	}
}

void Wosu::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Rectf collider{ CalculateCollider() };

	Vector2f copyVelocity{ m_Velocity.x, 0.0f };
	pLevel->HandleCollision(collider, copyVelocity, true);
	m_Velocity.x = copyVelocity.x;

	if (pLevel->IsOnGround(collider))
	{
		const float jumpForce{ 40.0f };
		m_Velocity.y = jumpForce;

		m_TextureState = (m_TextureState + 1) % 2;
	}

	const float gravity{ -240.0f };

	m_Velocity.y += gravity * elapsedSec;

	m_Position.x += m_Velocity.x * elapsedSec;
	m_Position.y += m_Velocity.y * elapsedSec;
}

void Wosu::Draw() const
{
	const Rectf srcRect
	{
		m_pTexture->GetWidth() / 2.0f * m_TextureState,
		m_pTexture->GetHeight(),
		m_pTexture->GetWidth() / 2.0f,
		m_pTexture->GetHeight()
	};

	glPushMatrix();
	{
		glTranslatef(m_Position.x + m_pTexture->GetWidth() / 4.0f, m_Position.y, 0.0f);
		if (m_Velocity.x > 0)
		{
			glScalef(-1.0f, 1.0f, 1.0f);
		}

		m_pTexture->Draw(Point2f{ -m_pTexture->GetWidth() / 4.0f, 0.0f }, srcRect);
	}
	glPopMatrix();
}
