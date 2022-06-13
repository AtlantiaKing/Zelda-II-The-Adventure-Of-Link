#include "pch.h"
#include "IronKnuckle.h"
#include "IronKnuckleSword.h"
#include "Level.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffectManager.h"

IronKnuckle::IronKnuckle(int enemyId, const Point2f& position)
	: Enemy{ enemyId, "IronKnuckle", position, Rectf{0.0f, 1.0f, 16.0f, 30.0f}, 16, 50, 24, 0.25f }
	, m_ComponentPosition{ m_Position }
{
	m_Components.push_back(new IronKnuckleSword{ "Resources/Images/IronKnuckleSwordCharge.png", m_ComponentPosition, Vector2f{ m_pTexture->GetWidth() * 0.9f, 0 }, m_pTexture->GetWidth() / 2.0f });
	m_Components.push_back(new IronKnuckleSword{ "Resources/Images/IronKnuckleSwordAttack.png", m_ComponentPosition, Vector2f{ -m_pTexture->GetWidth() * 1.0f, 0}, m_pTexture->GetWidth() / 2.0f });
}

IronKnuckle::~IronKnuckle()
{
	for (size_t i{ 0 }; i < m_Components.size(); ++i)
	{
		delete m_Components[i];
		m_Components[i] = nullptr;
	}
}

void IronKnuckle::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	UpdatePosition(actorShape, elapsedSec);

	HandleCollisions(pLevel);

	UpdateShieldHitTimer(elapsedSec);
	Enemy::Update(elapsedSec, actorShape, pLevel);

	UpdateSprite(elapsedSec, actorShape);

	if (IsInRange(actorShape))
	{
		ActivateComponent();
	}
}

void IronKnuckle::DoDamage(const Rectf& collider, int damage, SoundEffectManager* pSoundEffectManager)
{
	if (!m_IsHit && !m_IsShieldHit)
	{
		Rectf goodCollider{ CalculateCollider() };
		goodCollider.height /= 2.0f;

		Rectf wrongCollider{ goodCollider };

		const float headColliderSize{ 4.0f };
		const Rectf headCollider
		{
			goodCollider.left,
			goodCollider.bottom + goodCollider.height * 2 - headColliderSize,
			goodCollider.width,
			headColliderSize * 1.5f
		};

		if (m_CurRow == 0)
		{
			goodCollider.bottom += goodCollider.height;
		}
		else
		{
			wrongCollider.bottom += goodCollider.height;
			wrongCollider.height -= headColliderSize;
		}

		if (utils::IsOverlapping(goodCollider, collider))
		{
			const int newSpriteState{ rand() % 2 };
			switch (newSpriteState)
			{
			case 0:
				m_CurRow = 0;
				break;
			case 1:
				m_CurRow = 1;
				break;
			}

			goodCollider = CalculateCollider();
			goodCollider.height /= 2.0f;

			wrongCollider = goodCollider;

			if (m_CurRow == 0)
			{
				goodCollider.bottom += goodCollider.height;
			}
			else
			{
				wrongCollider.bottom += goodCollider.height;
				wrongCollider.height -= headColliderSize;
			}

			if (utils::IsOverlapping(goodCollider, collider))
			{
				m_Health -= damage;
				m_IsHit = true;
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
			}
			else if(utils::IsOverlapping(wrongCollider, collider))
			{
				m_IsShieldHit = true;
				m_AccuTime = 0.0f;
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Shield);
			}
			else if (utils::IsOverlapping(headCollider, collider))
			{
				m_Health -= damage;
				m_IsHit = true;
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
			}
		}
		else if (utils::IsOverlapping(wrongCollider, collider))
		{
			m_IsShieldHit = true;
			m_AccuTime = 0.0f;
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Shield);
		}
		else if (utils::IsOverlapping(headCollider, collider))
		{
			m_Health -= damage;
			m_IsHit = true;
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyHit);
		}
	}
}

void IronKnuckle::Draw() const
{
	Texture* curTexture{ GetCurrentTexture() };

	if (m_ComponentsActive)
	{
		m_Components[m_CurrentComponent]->Draw(m_ShowHitTexture, m_CurFrame, m_IsLookingRight);
	}

	const float clipWidth{ curTexture->GetWidth() / 2.0f };
	const float clipHeight{ curTexture->GetHeight() / 2.0f };

	const Rectf srcRect
	{
		clipWidth * m_CurFrame,
		curTexture->GetHeight() - clipHeight * m_CurRow,
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
		curTexture->Draw(Point2f{ -clipWidth / 2.0f, 0.0f }, srcRect);
	}
	glPopMatrix();
}

bool IronKnuckle::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) const
{
	for (size_t i{ 0 }; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->IsActive())
		{
			if (m_Components[i]->IsOverlapping(actorShape))
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
	}
	return Enemy::DoHitTest(actorShape, actorVelocity);
}


void IronKnuckle::UpdatePosition(const Rectf& actorShape, float elapsedSec)
{
	const float direction{ actorShape.left + actorShape.width / 6.0f - (m_Position.x + m_pTexture->GetWidth() / 4.0f) };
	if (!m_IsHit && !m_IsShieldHit)
	{
		if (!IsInRange(actorShape))
		{
			m_Position.x += direction / abs(direction) * m_Speed * elapsedSec;
			if (direction > 0.0f)
			{
				m_IsLookingRight = true;
			}
			else if(direction < 0.0f)
			{
				m_IsLookingRight = false;
			}
		}
	}
	else if (m_IsHit)
	{
		const float knockback{ 80.0f };
		m_Position.x -= direction / abs(direction) * knockback * elapsedSec;
	}
	else if (m_IsShieldHit)
	{
		const float knockback{ 35.0f };
		m_Position.x -= direction / abs(direction) * knockback * elapsedSec;
	}
}

void IronKnuckle::UpdateShieldHitTimer(float elapsedSec)
{
	if (m_IsShieldHit)
	{
		m_AccuTime += elapsedSec;
		if (m_AccuTime >= m_ShieldHitTimer)
		{
			m_IsShieldHit = false;
		}
	}
}

void IronKnuckle::UpdateSprite(float elapsedSec, const Rectf& actorShape)
{
	if (!IsInRange(actorShape))
	{
		m_AccuSpriteSec += elapsedSec;
		if (m_AccuSpriteSec > m_FrameTime)
		{
			++m_CurFrame %= 2;
			m_AccuSpriteSec -= m_FrameTime;
		}
	}
	else
	{
		m_CurFrame = 0;
	}

	m_ComponentPosition.x = m_Position.x;
	if (m_AttackUp)
	{
		m_ComponentPosition.y = m_Position.y + m_pTexture->GetHeight() / 4.0f;
	}
	else
	{
		m_ComponentPosition.y = m_Position.y;
	}

	if (m_ComponentsActive)
	{
		m_Components[m_CurrentComponent]->Update(elapsedSec, m_IsLookingRight);

		if (!m_Components[m_CurrentComponent]->IsActive())
		{
			m_CurrentComponent++;
			if (m_CurrentComponent < int(m_Components.size()))
			{
				m_Components[m_CurrentComponent]->Activate();
			}
			else
			{
				m_ComponentsActive = false;
			}
		}
	}
}

void IronKnuckle::ChangeComponent(bool isSwordUp)
{
	m_Components[0]->SetSwordState(isSwordUp);
	m_Components[1]->SetSwordState(isSwordUp);

	m_AttackUp = isSwordUp;
}

void IronKnuckle::ActivateComponent()
{
	if (!m_ComponentsActive)
	{
		const int newComponentState{ rand() % 2 };
		ChangeComponent(newComponentState == 0);

		if (m_Components.size() > 0)
		{
			m_ComponentsActive = true;
			m_CurrentComponent = 0;
			m_Components[m_CurrentComponent]->Activate();
		}
	}
}

void IronKnuckle::HandleCollisions(Level* pLevel)
{
	Rectf collider{ CalculateCollider() };
	Vector2f tempVelocity{};
	pLevel->HandleCollision(collider, tempVelocity);
	m_Position.x = collider.left - m_Collider.left;
}

bool IronKnuckle::IsInRange(const Rectf& actorShape) const
{
	const float direction{ actorShape.left + actorShape.width / 2.0f - (m_Position.x + m_pTexture->GetWidth() / 8.0f) };
	if (abs(direction) < m_pTexture->GetWidth() / 3.0f * 2.0f)
	{
		return true;
	}
	return false;
}