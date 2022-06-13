#include "pch.h"
#include "Stalfos.h"
#include "Texture.h"
#include "Level.h"
#include "utils.h"
#include "SoundEffectManager.h"

Stalfos::Stalfos(int enemyId, const Point2f& position)
	: Enemy{ enemyId, "Stalfos", position, Rectf{0.0f, 1.0f, 16.0f, 30.0f}, 8, 30, 16, 0.25f }
	, m_MinimalDistance{ m_pTexture->GetWidth() / 9.0f * 4.0f }
	, m_pSwordTexture{ new Texture{ "Resources/Images/StalfosSword.png" }}
{
}

Stalfos::~Stalfos()
{
	delete m_pSwordTexture;
	m_pSwordTexture = nullptr;
}

void Stalfos::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Enemy::Update(elapsedSec, actorShape, pLevel);
	UpdateShieldHitTimer(elapsedSec);

	const bool isInRange{ IsInRange(actorShape) };

	HandleMovement(isInRange, actorShape, elapsedSec);

	HandleCollisions(pLevel);

	HandleState(isInRange, elapsedSec);
}

void Stalfos::DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager)
{
	if (!m_IsHit && !m_IsShieldHit)
	{
		Rectf goodCollider{ CalculateCollider() };
		goodCollider.height /= 2.0f;

		Rectf wrongCollider{ goodCollider };

		wrongCollider.bottom += goodCollider.height;

		if (utils::IsOverlapping(goodCollider, collider))
		{
			m_Health -= damage;
			m_IsHit = true;
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
		}
		else if (utils::IsOverlapping(wrongCollider, collider))
		{
			m_IsShieldHit = true;
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Shield);
		}
	}
}

void Stalfos::Draw() const
{
	Texture* pCurTexture{ GetCurrentTexture() };

	const float tileWidth{ pCurTexture->GetWidth() / 4.0f };
	const float tileHeight{ pCurTexture->GetHeight() / 2.0f };

	int realBodyFrame{ m_BodyFrame };

	if (realBodyFrame > 0)
	{
		--realBodyFrame;
	}

	const Rectf srcBodyRect
	{
		tileWidth * realBodyFrame,
		tileHeight,
		tileWidth,
		tileHeight
	};

	const Point2f bodyPosition 
	{
		-tileWidth / 2.0f,
		tileHeight
	};

	const Rectf srcLegRect
	{
		tileWidth * m_LegFrame,
		tileHeight * 2.0f,
		tileWidth,
		tileHeight
	};

	const Point2f legPosition
	{
		-tileWidth / 2.0f,
		0.0f
	};

	glPushMatrix();

	glTranslatef(m_Position.x + tileWidth / 2.0f, m_Position.y, 0.0f);
	if (m_IsLookingRight)
	{
		glScalef(-1.0f, 1.0f, 1.0f);
	}

	pCurTexture->Draw(bodyPosition, srcBodyRect);

	pCurTexture->Draw(legPosition, srcLegRect);

	if (m_BodyFrame == 3)
	{
		const Point2f swordPosition
		{
			-tileWidth / 2.0f * 3.0f,
			tileHeight
		};

		const float swordSize{ m_pSwordTexture->GetHeight() };

		Rectf swordSrcRect
		{
			0.0f,
			swordSize,
			swordSize,
			swordSize
		};

		if (m_ShowHitTexture)
		{
			swordSrcRect.left += swordSize;
		}

		m_pSwordTexture->Draw(swordPosition, swordSrcRect);
	}

	glPopMatrix();


	Rectf goodCollider{ CalculateCollider() };
	goodCollider.height /= 2.0f;

	Rectf wrongCollider{ goodCollider };

	wrongCollider.bottom += goodCollider.height;
}

bool Stalfos::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	if (m_BodyFrame == 3)
	{
		const float tileWidth{ m_pTexture->GetWidth() / 4.0f };
		const float tileHeight{ m_pTexture->GetHeight() / 2.0f };
		Rectf swordRect
		{
			m_Position.x,
			m_Position.y + tileHeight + 2,
			16.0f,
			4.0f
		};

		if (m_IsLookingRight)
		{
			swordRect.left += tileWidth;
		}
		else
		{
			swordRect.left -= tileWidth;
		}

		if (utils::IsOverlapping(swordRect, actorShape))
		{
			const float direction{ actorShape.left - m_Position.x };
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
	return Enemy::DoHitTest(actorShape, actorVelocity);
}


void Stalfos::UpdateShieldHitTimer(float elapsedSec)
{
	if (m_IsShieldHit)
	{
		m_AccuShieldTime += elapsedSec;
		if (m_AccuShieldTime >= m_ShieldHitTime)
		{
			m_IsShieldHit = false;
			m_AccuShieldTime = 0.0f;
		}
	}
}

void Stalfos::HandleMovement(bool isInRange, const Rectf& actorShape, float elapsedSec)
{
	const float direction{ actorShape.left + actorShape.width / 6.0f - (m_Position.x + m_pTexture->GetWidth() / 8.0f) };
	if (!m_IsHit && !m_IsShieldHit)
	{
		if (!isInRange)
		{
			m_Velocity.x = direction / abs(direction) * m_MoveSpeed;
		}
		else
		{
			m_Velocity.x = 0.0f;
		}
	}
	else if (m_IsHit)
	{
		const float knockback{ 50.0f };
		m_Velocity.x = -direction / abs(direction) * knockback;
	}
	else if (m_IsShieldHit)
	{
		const float knockback{ 25.0f };
		m_Velocity.x = -direction / abs(direction) * knockback;
	}

	if (m_Velocity.y < 0.0f)
	{
		m_Velocity.x = 0.0f;
	}

	const float gravity{ 240.0f };
	m_Velocity.y -= gravity * elapsedSec;

	m_Position.x += m_Velocity.x * elapsedSec;
	m_Position.y += m_Velocity.y * elapsedSec;
}

void Stalfos::HandleCollisions(Level* pLevel)
{
	Rectf collider{ CalculateCollider() };
	pLevel->HandleCollision(collider, m_Velocity);
	m_Position.x = collider.left;
	m_Position.y = collider.bottom - m_Collider.bottom;
}

void Stalfos::HandleState(bool isInRange, float elapsedSec)
{
	if (isInRange || m_BodyFrame > 0)
	{
		m_AccuBodyTime += elapsedSec;
		if (m_AccuBodyTime >= m_BodyFrameTime)
		{
			m_BodyFrame = ++m_BodyFrame % 4;
			m_AccuBodyTime = 0.0f;
		}
	}

	if (m_Velocity.y >= 0)
	{
		m_AccuLegTime += elapsedSec;
		if (m_AccuLegTime >= m_LegFrameTime)
		{
			m_LegFrame = ++m_LegFrame % 2;
			m_AccuLegTime = 0.0f;
		}
	}
	else
	{
		if (isInRange)
		{
			m_LegFrame = 3;
		}
		else
		{
			m_LegFrame = 2;
		}
	}

	if (!m_IsHit && !m_IsShieldHit)
	{
		if (m_Velocity.x > FLT_EPSILON)
		{
			m_IsLookingRight = true;
		}
		else if (abs(m_Velocity.x) > FLT_EPSILON)
		{
			m_IsLookingRight = false;
		}
	}
}

bool Stalfos::IsInRange(const Rectf& actorShape) const
{
	const float direction{ actorShape.left + actorShape.width / 2.0f - (m_Position.x + m_pTexture->GetWidth() / 8.0f) };
	if (abs(direction) < m_MinimalDistance)
	{
		return true;
	}
	return false;
}