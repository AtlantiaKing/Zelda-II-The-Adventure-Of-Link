#include "pch.h"
#include "Horsehead.h"
#include "Texture.h"
#include "utils.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "SoundEffectManager.h"

Horsehead::Horsehead(int enemyId, const Point2f& position)
	: Enemy{ enemyId, "Horsehead", position, Rectf{ 0.0f, 0.0f, 16.0f, 48.0f }, 32, 50, 32, 0.4f, 0.035f }
	, m_pBatTexture{ new Texture{ "Resources/Images/HorseheadBat.png" } }
	, m_MaxHealth{ 32 }
{
}

Horsehead::~Horsehead()
{
	delete m_pBatTexture;
	m_pBatTexture = nullptr;
}

void Horsehead::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Enemy::Update(elapsedSec, actorShape, pLevel);

	if (m_IsDeathAnimBusy)
	{
		m_ShowHurtColors = true;
		m_AccuHurtTime = 0.0f;
		return;
	}

	if (!m_IsActive)
	{
		const float triggerDistance{ 86.0f };
		if (abs(actorShape.left - m_Position.x) < triggerDistance)
		{
			m_IsActive = true;
		}
		else
		{
			return;
		}
	}

	UpdateBodyHitTimer(elapsedSec);

	const bool isInRange{ IsInRange(actorShape) };

	HandleMovement(isInRange, actorShape, elapsedSec);

	HandleState(isInRange, elapsedSec);
}

void Horsehead::UpdateDeathAnimation(float elapsedSec, ParticleManager* pParticleManager, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsDeathAnimBusy)
	{
		m_AccuDeathAnimSec += elapsedSec;
		m_IsDeathAnimFinished = m_AccuDeathAnimSec >= m_DeathAnimLength;
		if (m_AccuDeathAnimSec >= m_DeathAnimLength / 2.0f)
		{
			if (int(m_AccuDeathAnimSec * 100) % 5 == 0)
			{
				const int explosionSize{ 16 };
				const int offset{ 8 };

				const Point2f explositionPos
				{
					m_Position.x + (rand() % int(m_Collider.width) + explosionSize) - explosionSize - offset,
					m_Position.y + offset + rand() % (int(m_Collider.height) - explosionSize + offset)
				};

				pParticleManager->CreateParticle(ParticleType::Explosion, explositionPos);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::BossExplode);
			}
		}
	}
}

void Horsehead::DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsHit)
	{
		return;
	}

	const Rectf hitCollider
	{
		m_Position.x + m_Collider.left,
		m_Position.y + m_Collider.bottom,
		m_Collider.width,
		m_Collider.height / 3.0f * 2.0f
	};
	const Rectf head
	{
		m_Position.x + m_Collider.left,
		m_Position.y + m_Collider.bottom + m_Collider.height / 3.0f * 2.0f,
		m_Collider.width,
		m_Collider.height / 3.0f
	};
	if (utils::IsOverlapping(collider, hitCollider))
	{
		m_IsBodyHit = true;
		pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
		pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
		pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Shield);
	}
	else if (utils::IsOverlapping(collider, head))
	{
		m_Health -= damage;
		m_IsHit = true;
		pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
		pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
		pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
	}
}

void Horsehead::HandleCameraCollision(Camera* pCamera)
{
	Rectf collider{ m_Position.x, m_Position.y, m_Collider.width, m_Collider.height };

	pCamera->HandleBoundaryCollision(collider, m_Velocity);

	m_Position.x = collider.left;
}

void Horsehead::ActivateDeathAnimation()
{
	m_IsDeathAnimBusy = true;
}

void Horsehead::Draw() const
{
	Texture* curTexture{ GetCurrentTexture() };

	const float textureWidth{ curTexture->GetWidth()};
	const float textureHeight{ curTexture->GetHeight() };

	const Rectf legsSrcRect
	{
		textureWidth / 4.5f * m_LegFrame,
		textureHeight,
		textureWidth / 4.5f,
		textureHeight / 3.0f
	};
	const Rectf bodySrcRect
	{
		textureWidth / 3.0f * m_BodyFrame,
		textureHeight / 3.0f * 2.0f,
		textureWidth / 3.0f ,
		textureHeight / 3.0f
	};
	const Point2f bodyPos
	{
		m_Position.x,
		m_Position.y + textureHeight / 3.0f
	};
	const Rectf faceSrcRect
	{
		0.0f,
		textureHeight / 3.0f,
		textureWidth / 4.5f,
		textureHeight / 3.0f
	};
	const Point2f facePos
	{
		m_Position.x,
		m_Position.y + textureHeight / 3.0f * 2.0f
	};

	curTexture->Draw(m_Position, legsSrcRect);
	curTexture->Draw(bodyPos, bodySrcRect);
	curTexture->Draw(facePos, faceSrcRect);

	if (m_BodyFrame == 2)
	{
		const Point2f batPosition
		{
			bodyPos.x - m_pBatTexture->GetWidth(),
			bodyPos.y
		};
		m_pBatTexture->Draw(batPosition);
	}
}

bool Horsehead::IsActive() const
{
	return m_IsActive;
}

bool Horsehead::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	if (m_IsDeathAnimBusy)
	{
		return false;
	}

	const Rectf batRect
	{
		m_Position.x - m_pBatTexture->GetWidth(),
		m_Position.y + m_pTexture->GetHeight() / 3.0f,
		m_pBatTexture->GetWidth(),
		m_pBatTexture->GetHeight()
	};
	if (m_BodyFrame == 2 && utils::IsOverlapping(actorShape, batRect))
	{
		const float direction{ actorShape.left - batRect.left };
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
	else
	{
		return Enemy::DoHitTest(actorShape, actorVelocity);
	}
}

int Horsehead::GetHealth() const
{
	return int(m_Health / float(m_MaxHealth) * 8);
}

bool Horsehead::IsDeathAnimationFinished() const
{
	return m_IsDeathAnimFinished;
}

bool Horsehead::IsDeathAnimationBusy() const
{
	return m_IsDeathAnimBusy;
}

void Horsehead::UpdateBodyHitTimer(float elapsedSec)
{
	if (m_IsBodyHit)
	{
		m_AccuBodyHitTime += elapsedSec;
		if (m_AccuBodyHitTime >= m_BodyHitTime)
		{
			m_IsBodyHit = false;
			m_AccuBodyHitTime = 0.0f;
		}
	}
}

bool Horsehead::IsInRange(const Rectf& actorShape)
{
	return abs(actorShape.left - m_Position.x) < m_AttackDistance;
}

void Horsehead::HandleMovement(bool isInRange, const Rectf& actorShape, float elapsedSec)
{
	float direction{ actorShape.left + actorShape.width / 2.0f - (m_Position.x + actorShape.width / 2.0f) };
	direction /= abs(direction);

	if (m_IsBodyHit)
	{
		const float knockback{ 80.0f };
		m_Velocity.x = -direction * knockback;
	}
	else if (m_IsHit)
	{
		const float knockback{ 100.0f };
		m_Velocity.x = -direction * knockback;
	}
	else
	{
		if (isInRange)
		{
			return;
		}
		else
		{
			if (abs(actorShape.left - m_Position.x) < m_RunDistance)
			{
				m_AccuRunTime += elapsedSec;
				if (m_IsRunning)
				{
					m_Velocity.x = direction * m_MoveSpeed;

					if (m_AccuRunTime >= m_RunTime)
					{
						m_AccuRunTime -= m_RunTime;
						m_IsRunning = false;
					}
				}
				else
				{
					m_Velocity.x = 0.0f;

					if (m_AccuRunTime >= m_TimeBetweenRuns)
					{
						m_AccuRunTime -= m_TimeBetweenRuns;
						m_IsRunning = true;
					}
				}
			}
			else
			{
				m_Velocity.x = direction * m_MoveSpeed;
			}
		}
	}

	m_Position.x += m_Velocity.x * elapsedSec;
}

void Horsehead::HandleState(bool isInRange, float elapsedSec)
{
	if (isInRange)
	{
		m_IsAttacking = true;
	}

	if (abs(m_Velocity.x) > FLT_EPSILON)
	{
		m_AccuLegTime += elapsedSec;
		if (m_AccuLegTime >= m_LegFrameTime)
		{
			m_AccuLegTime -= m_LegFrameTime;
			++m_LegFrame %= 2;
		}
	}
	if (m_IsAttacking)
	{
		m_AccuBodyTime += elapsedSec;
		if (m_AccuBodyTime >= m_BodyFrameTime)
		{
			m_AccuBodyTime -= m_BodyFrameTime;
			++m_BodyFrame %= 3;
			if (m_BodyFrame == 0)
			{
				m_IsAttacking = false;
			}
		}
	}

	if (m_IsBodyHit)
	{
		m_AccuBodyHitTime += elapsedSec;
		if (m_AccuBodyHitTime >= m_BodyHitTime)
		{
			m_AccuBodyHitTime -= m_BodyHitTime;
			m_IsBodyHit = false;
		}
	}
}