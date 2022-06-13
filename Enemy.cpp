#include "pch.h"
#include "Enemy.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffectManager.h"

Enemy::Enemy(int enemyId, const std::string& enemyName, const Point2f& position, const Rectf& collider, int health, int xp, int damage, float invisTime, float hitTextureTime)
	: m_Position{ position }
	, m_Collider{ collider }
	, m_Health{ health }
	, m_HealthTimer{ invisTime }
	, m_EnemyId{ enemyId }
	, m_pTexture{ new Texture("Resources/Images/" + enemyName + ".png")}
	, m_XP{ xp }
	, m_Damage{ damage }
	, m_HitTextureTimer{ hitTextureTime }
{
	if (enemyName != "Bubble" && enemyName != "Wosu")
	{
		m_pHurtTexture = new Texture("Resources/Images/" + enemyName + "Hurt.png");
	}
	else
	{
		m_pHurtTexture = nullptr;
	}
}

Enemy::~Enemy()
{
	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pHurtTexture;
	m_pHurtTexture = nullptr;
}

void Enemy::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	UpdateHitTimer(elapsedSec);

	UpdateHurtColorTimer(elapsedSec);
}

void Enemy::UpdateHitTimer(float elapsedSec)
{
	if (m_IsHit)
	{
		m_AccuSec += elapsedSec;
		if (m_AccuSec >= m_HealthTimer)
		{
			m_AccuSec -= m_HealthTimer;
			m_IsHit = false;
		}
	}
}

void Enemy::UpdateHurtColorTimer(float elapsedSec)
{
	if (m_IsHit)
	{
		m_ShowHurtColors = true;
	}

	if (m_ShowHurtColors)
	{
		m_AccuHitTextureTime += elapsedSec;
		if (m_AccuHitTextureTime >= m_HitTextureTimer)
		{
			m_AccuHitTextureTime -= m_HitTextureTimer;
			m_ShowHitTexture = !m_ShowHitTexture;
		}

		m_AccuHurtTime += elapsedSec;
		if (m_AccuHurtTime >= m_HurtColorTime)
		{
			m_AccuHurtTime -= m_HurtColorTime;
			m_ShowHurtColors = false;
		}
	}
	else
	{
		m_AccuHitTextureTime = 0.0f;
		m_AccuHurtTime = 0.0f;
	}
}

Texture* Enemy::GetCurrentTexture() const
{
	if (m_ShowHurtColors && m_ShowHitTexture)
	{
		return m_pHurtTexture;
	}
	return m_pTexture;
}

bool Enemy::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	if (!m_IsHit)
	{
		Rectf collider = CalculateCollider();
		if (utils::IsOverlapping(actorShape, collider))
		{
			const float direction{ actorShape.left - collider.left };
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
	return false;
}

void Enemy::DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager)
{
	if (!m_IsHit)
	{
		const Rectf enemyCollider{ CalculateCollider() };

		if (utils::IsOverlapping(enemyCollider, collider))
		{
			m_Health -= damage;
			m_IsHit = true;
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
		}
	}
}

int Enemy::GetID() const
{
	return m_EnemyId;
}

int Enemy::GetHealth() const
{
	return m_Health;
}

int Enemy::GetXP() const
{
	return m_XP;
}

int Enemy::GetDamage() const
{
	return m_Damage;
}

Rectf Enemy::CalculateCollider() const
{
	return Rectf{ m_Position.x + m_Collider.left, m_Position.y + m_Collider.bottom, m_Collider.width, m_Collider.height };
}

Point2f Enemy::GetPosition() const
{
	return m_Position;
}
