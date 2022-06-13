#include "pch.h"
#include "Guma.h"
#include "Texture.h"
#include "Level.h"
#include "utils.h"

Guma::Guma(int enemyId, const Point2f& position)
	: Enemy{ enemyId, "Guma", position,  Rectf{ 4.0f, 1.0f, 16.0f, 30.0f}, 16, 50, 24, 0.25f }
	, m_pProjectileTexture{ new Texture{ "Resources/Images/GumaProjectile.png" } }
{
}

Guma::~Guma()
{
	delete m_pProjectileTexture;
	m_pProjectileTexture = nullptr;
}

void Guma::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Enemy::Update(elapsedSec, actorShape, pLevel);

	UpdateLookingDirection(actorShape);

	UpdateHorizontalVelocity(elapsedSec);

	HandleThrowTimer(elapsedSec, actorShape);

	UpdateGravity(elapsedSec);

	HandleMovement(elapsedSec);

	HandleProjectiles(elapsedSec);

	HandleCollisions(pLevel);

	UpdateSprite(elapsedSec);
}

void Guma::Draw() const
{
	Texture* pCurTexture{ GetCurrentTexture() };

	const float clipWidth{ pCurTexture->GetWidth() / 2.0f };
	const float clipHeight{ pCurTexture->GetHeight() / 2.0f };

	const Rectf srcRect
	{
		clipWidth * m_CurFrame,
		clipHeight * 2 - clipHeight * m_CurRow,
		clipWidth,
		clipHeight
	};

	glPushMatrix();
	{
		glTranslatef(m_Position.x + clipWidth / 2.0f, m_Position.y, 0.0f);
		if (m_IsLookingRight)
		{
			glScalef(-1.0f, 1.0f, 1.0f);
		}
		pCurTexture->Draw(Point2f{ -clipWidth / 2.0f, 0.0f }, srcRect);
	}
	glPopMatrix();

	const float projectileWidth{ m_pProjectileTexture->GetWidth() / 3.0f };
	const float projectileHeight{ m_pProjectileTexture->GetHeight() };

	for (int i{}; i < m_NrProjectiles; ++i)
	{
		if (m_Projectiles[i].isActive)
		{
			const Rectf srcRect
			{
				projectileWidth * m_Projectiles[i].curFrame,
				projectileHeight,
				projectileWidth,
				projectileHeight
			};

			glPushMatrix();
			{
				glTranslatef(m_Projectiles[i].position.x + projectileWidth + 2.0f, m_Projectiles[i].position.y + projectileHeight / 2.0f, 0.0f);
				glRotatef(m_Projectiles[i].rotation, 0.0f, 0.0f, 1.0f);
				m_pProjectileTexture->Draw(Point2f{ -projectileWidth + 2.0f, -projectileHeight + 2.0f }, srcRect);
			}
			glPopMatrix();
		}
	}
}

bool Guma::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	for (int i{}; i < m_NrProjectiles; ++i)
	{
		if (m_Projectiles[i].isActive)
		{
			const Rectf projectileCollider
			{
				m_Projectiles[i].position.x,
				m_Projectiles[i].position.y,
				m_pProjectileTexture->GetWidth() / 3.0f,
				m_pProjectileTexture->GetHeight()
			};

			if (utils::IsOverlapping(actorShape, projectileCollider))
			{
				const float direction{ actorShape.left - projectileCollider.left };
				const float knockbackXForce{ 80.0f };
				const float knockbackYForce{ 80.0f };
				Vector2f knockback{ direction / abs(direction) * knockbackXForce, knockbackYForce };

				if (actorVelocity.x / knockback.x < 0)
				{
					knockback.x *= 2.0f;
				}

				actorVelocity = knockback;

				return true;
			}
		}
	}
 	return Enemy::DoHitTest(actorShape, actorVelocity);
}

void Guma::UpdateLookingDirection(const Rectf& actorShape)
{
	if (actorShape.left - m_Position.x > 0)
	{
		m_IsLookingRight = true;
	}
	else
	{
		m_IsLookingRight = false;
	}
}

void Guma::UpdateHorizontalVelocity(float elapsedSec)
{
	m_AccuMoveTime += elapsedSec;
	if (m_AccuMoveTime >= m_MoveTime)
	{
		m_AccuMoveTime -= m_MoveTime;
		m_Velocity.x = -m_Velocity.x;
	}
}

void Guma::HandleThrowTimer(float elapsedSec, const Rectf& actorShape)
{
	m_AccuThrowTime += elapsedSec;
	if (m_AccuThrowTime >= m_TimeBetweenThrows)
	{
		m_AccuThrowTime -= m_TimeBetweenThrows;

		ThrowProjectile();
	}
	else if (m_AccuThrowTime >= m_TimeBetweenThrows / 2.0f && m_AccuThrowTime - elapsedSec < m_TimeBetweenThrows / 2.0f)
	{
		if (m_CurProjectile == m_NrProjectiles - 1)
		{
			const float jumpSpeed{ 140.0f };
			m_Velocity.y = jumpSpeed;
		}
	}
}

void Guma::UpdateGravity(float elapsedSec)
{
	const float gravity{ 240.0f };
	m_Velocity.y -= gravity * elapsedSec;
}

void Guma::HandleMovement(float elapsedSec)
{
	m_Position.y += m_Velocity.y * elapsedSec;

	if (m_IsHit)
	{
		const float knockback{ 50.0f };
		float direction{ 1.0f };
		if (!m_IsLookingRight)
		{
			direction = -direction;
		}
		m_Position.x -= direction / abs(direction) * knockback * elapsedSec;
	}
	else
	{
		m_Position.x += m_Velocity.x * elapsedSec;
	}
}

void Guma::HandleProjectiles(float elapsedSec)
{
	const float gravity{ 70.0f };

	for (int i{}; i < m_NrProjectiles; ++i)
	{
		if (m_Projectiles[i].isActive)
		{
			m_Projectiles[i].velocity.y -= gravity * elapsedSec;

			m_Projectiles[i].position.x += m_Projectiles[i].velocity.x * elapsedSec;
			m_Projectiles[i].position.y += m_Projectiles[i].velocity.y * elapsedSec;
		}
	}
}

void Guma::HandleCollisions(Level* pLevel)
{
	Rectf collider{ CalculateCollider() };
	pLevel->HandleCollision(collider, m_Velocity);
	m_Position.x = collider.left - m_Collider.left;
	m_Position.y = collider.bottom - m_Collider.bottom;
}

void Guma::ThrowProjectile()
{
	Point2f projectilePosition
	{
		m_Position.x + m_pTexture->GetWidth() / 4.0f,
		m_Position.y + m_pTexture->GetHeight() / 4.0f
	};
	if (!m_IsLookingRight)
	{
		projectilePosition.x -= -m_pProjectileTexture->GetWidth() / 3.0f;
	}

	m_Projectiles[m_CurProjectile] = GumaProjectile{};
	m_Projectiles[m_CurProjectile].isActive = true;
	m_Projectiles[m_CurProjectile].position = projectilePosition;
	if (m_IsLookingRight)
	{
		m_Projectiles[m_CurProjectile].velocity.x = -m_Projectiles[m_CurProjectile].velocity.x;
	}

	++m_CurProjectile %= m_NrProjectiles;
}

void Guma::UpdateSprite(float elapsedSec)
{
	if (m_AccuThrowTime >= m_TimeBetweenThrows / 2.0f)
	{
		m_CurRow = 0;
	}
	else
	{
		m_CurRow = 1;
	}

	m_AccuFrameTime += elapsedSec;
	if (m_AccuFrameTime >= m_FrameTime)
	{
		m_AccuFrameTime -= m_FrameTime;
		++m_CurFrame %= 2;
	}

	for (int i{}; i < m_NrProjectiles; ++i)
	{
		if (m_Projectiles[i].isActive)
		{
			m_Projectiles[i].accuFrameTime += elapsedSec;
			if (m_Projectiles[i].accuFrameTime > m_ProjectileFrameTime)
			{
				m_Projectiles[i].accuFrameTime -= m_ProjectileFrameTime;

				++m_Projectiles[i].curFrame %= 3;
				if (m_Projectiles[i].curFrame == 1)
				{
					m_Projectiles[i].rotation += 90.0f;
				}
			}
		}
	}
}