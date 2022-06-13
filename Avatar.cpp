#include "pch.h"
#include "Avatar.h"
#include "Level.h"
#include "utils.h"
#include "Texture.h"
#include "GameObjectManager.h"
#include "EnemyManager.h"
#include "SwordBeam.h"
#include "Sprite.h"
#include "Inventory.h"
#include "XPManager.h"
#include "Camera.h"
#include "SoundEffectManager.h"

Avatar::Avatar(XPManager* pXPManager)
	: m_pSprite{ new Sprite{ "Resources/Images/AvatarSheet.png", 48.0f, 32.0f, 10, 10 }}
	, m_pInventory{ new Inventory{ pXPManager } }
{
}

Avatar::~Avatar()
{
	delete m_pSprite;
	m_pSprite = nullptr;

	delete m_pInventory;
	m_pInventory = nullptr;
}

void Avatar::Update(float elapsedSec, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, SoundEffectManager* pSoundEffectManager)
{
	UpdateHealth(elapsedSec, pSoundEffectManager);

	HandleInputAndState(pLevel, pGameObjectManager, pSoundEffectManager, elapsedSec);

	if (m_ActionState != ActionState::fairy_pickup && m_ActionState != ActionState::item_pickup)
	{
		MovePlayer(elapsedSec);

		HandleCollision(pLevel, pCamera, pGameObjectManager, pEnemyManager, pXPManager, pSoundEffectManager);
	}

	UpdateAnimation(elapsedSec);
}

void Avatar::Attack(SwordBeam* swordBeam, SoundEffectManager* pSoundEffectManager)
{
	if (m_ActionState == ActionState::hurt || m_ActionState == ActionState::duck_hit)
	{
		return;
	}

	float direction{ 1 };
	if (!m_IsLookingRight)
	{
		direction = -1;
	}

	const float swordDistanceFromCenter{ 15.0f };
	const float swordDistanceFromBottom{ 18.0f };
	const float duckSwordDistanceFromBottom{ 8.0f };

	Point2f swordPosition{
		m_Shape.left + m_Shape.width / 2.0f + direction * swordDistanceFromCenter,
		m_Shape.bottom + swordDistanceFromBottom
	};

	if (m_ActionState == ActionState::duck)
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (m_Velocity.y > 0 && !(pStates[SDL_SCANCODE_DOWN] || pStates[SDL_SCANCODE_S]))
		{
			m_ActionState = ActionState::hit;
			m_HasHit = false;

			if (m_HP == m_MaxHP)
			{
				if (!swordBeam->IsActive())
				{
					swordBeam->Activate(swordPosition, direction);
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Swordbeam, true);
				}
				else
				{
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
				}
			}
			else
			{
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
			}
		}
		else
		{
			m_ActionState = ActionState::duck_hit;
			m_HasHit = false;

			if (m_HP == m_MaxHP)
			{
				if (!swordBeam->IsActive())
				{
					swordPosition.y = m_Shape.bottom + duckSwordDistanceFromBottom;
					swordBeam->Activate(swordPosition, direction);
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Swordbeam, true);
				}
				else
				{
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
				}
			}
			else
			{
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
			}
		}
	}
	else if (m_ActionState != ActionState::duck_hit)
	{
		m_ActionState = ActionState::hit;
		m_HasHit = false;

		if (m_HP == m_MaxHP)
		{
			if (!swordBeam->IsActive())
			{
				swordBeam->Activate(swordPosition, direction);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Swordbeam, true);
			}
			else
			{
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
			}
		}
		else
		{
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::SwordSlash, true);
		}
	}

	m_pSprite->ResetAnimation();

	if (abs(m_Velocity.y) < FLT_EPSILON)
	{
		m_Velocity.x = 0;
	}
}

void Avatar::Teleport(const Rectf& target)
{
	m_Shape = target;
}

void Avatar::Respawn()
{
	m_HP = m_MaxHP;
	m_IsDead = false;
	m_ActionState = ActionState::idle;
	m_Velocity.x = 0.0f;
	m_Velocity.y = 0.0f;
}

void Avatar::SetHealth(int level, float levelPercentage)
{
	m_MaxHP = 64 + 16 * (level - 1);
	m_HP = int(levelPercentage * m_MaxHP);
}

void Avatar::SetLifes(int lifes)
{
	m_NrLifes = lifes;
}

void Avatar::Reset(XPManager* pXPManager)
{
	delete m_pInventory;
	m_pInventory = new Inventory{ pXPManager };

	m_MaxHP = 64;
	m_IsLookingRight = true;
	m_Invincible = false;
	m_HasHit = false;
	m_IncomingDamage = 0;
	m_NrLifes = 3;

	Respawn();
}

void Avatar::AddKeys(int nrKeys) const
{
	for (int i{ 0 }; i < nrKeys; ++i)
	{
		m_pInventory->AddItemFromObject(ObjectType::Key);
	}
}

void Avatar::GiveCandle() const
{
	m_pInventory->AddItemFromObject(ObjectType::Candle);
}

bool Avatar::IsLookingRight() const
{
	return m_IsLookingRight;
}

void Avatar::Draw() const
{
	if (!m_pSprite->GetVisibility())
	{
		return;
	}

	const Point2f position
	{
		m_Shape.left + m_Shape.width / 2.0f,
		m_Shape.bottom
	};

	m_pSprite->Draw(position, !m_IsLookingRight, int(m_ActionState));

	if (m_ActionState == ActionState::item_pickup)
	{
		const Point2f topPosition
		{
			position.x,
			position.y + m_Shape.height
		};

		m_pInventory->Draw(topPosition);
	}
}

Rectf Avatar::GetShape() const
{
	return m_Shape;
}

bool Avatar::IsDead() const
{
	return m_IsDead;
}

bool Avatar::IsPickingUpItem() const
{
	return m_ActionState == ActionState::item_pickup;
}

float Avatar::GetHealthPercentage() const
{
	return m_HP / float(m_MaxHP);
}

int Avatar::GetMaxHealth() const
{
	return m_MaxHP;
}

int Avatar::GetRemainingLifes() const
{
	return m_NrLifes;
}

int Avatar::GetKeysAmount() const
{
	return m_pInventory->GetKeysAmount();
}

bool Avatar::HasTakenCandle() const
{
	return m_pInventory->HasTakenCandle();
}


void Avatar::UpdateHealth(float elapsedSec, SoundEffectManager* pSoundEffectManager)
{
	if (m_HP <= 0)
	{
		m_IsDead = true;
		--m_NrLifes;
	}

	m_AccuDamageTime += elapsedSec;
	if (m_AccuDamageTime >= m_TimePerHealthChange)
	{
		m_AccuDamageTime -= m_TimePerHealthChange;
		if (m_ActionState == ActionState::fairy_pickup)
		{
			if (m_HP < m_MaxHP)
			{
				++m_HP;
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::HealthAdd);
			}
			else
			{
				m_ActionState = ActionState::jump_down;
			}
		}
	}

	const int healthBeepLevel{ 16 };
	if (m_HP <= healthBeepLevel)
	{
		m_AccuBeepSoundSec += elapsedSec;
		if (m_AccuBeepSoundSec >= m_TimeBetweenBeeps)
		{
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::LowHealth);
			m_AccuBeepSoundSec -= m_TimeBetweenBeeps;
		}
	}
}

void Avatar::HandleInputAndState(Level* pLevel, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager, float elapsedSec)
{
	Rectf collider{ CalculateCollider() };

	bool isOnGround{ pLevel->IsOnGround(collider) || pGameObjectManager->IsOnFloor(collider, pSoundEffectManager) };
	bool isOnGameObject{ pGameObjectManager->IsOnGround(collider) };

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	bool moving{ false };
	bool downInput{ pStates[SDL_SCANCODE_S] || pStates[SDL_SCANCODE_DOWN] };
	bool jumpInput{ bool(pStates[SDL_SCANCODE_SPACE]) };

	if (pStates[SDL_SCANCODE_RIGHT] || pStates[SDL_SCANCODE_D])
	{
		moving = true;
		m_IsLookingRight = true;
	}
	else if (pStates[SDL_SCANCODE_LEFT] || pStates[SDL_SCANCODE_Q] || pStates[SDL_SCANCODE_A])
	{
		moving = true;
		m_IsLookingRight = false;
	}

	UpdateState(elapsedSec, isOnGround, isOnGameObject, moving, downInput, jumpInput);

	if (m_ActionState != ActionState::fairy_pickup)
	{
		UpdateVelocity(elapsedSec, isOnGround, isOnGameObject, moving, jumpInput);
	}
}

void Avatar::MovePlayer(float elapsedSec)
{
	m_Shape.left += m_Velocity.x * elapsedSec;
	m_Shape.bottom += m_Velocity.y * elapsedSec;
}

void Avatar::HandleCollision(Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, SoundEffectManager* pSoundEffectManager)
{
	Rectf collider{ CalculateCollider() };

	Rectf hittableCollider{ collider };
	Rectf shieldCollider{ collider };
	shieldCollider.width /= 4.0f;
	if (m_IsLookingRight)
	{
		shieldCollider.left = collider.left + collider.width - shieldCollider.width;
	}

	switch (m_ActionState)
	{
	case Avatar::ActionState::idle:
	case Avatar::ActionState::moving:
	case Avatar::ActionState::hit:
		shieldCollider.height /= 2.0f;
		shieldCollider.bottom += shieldCollider.height;
		break;
	case Avatar::ActionState::duck:
	case Avatar::ActionState::duck_hit:
		shieldCollider.height /= 2.0f;
		break;
	}

	if (m_Shape.bottom < pLevel->GetBoundaries().bottom)
	{
		if (pGameObjectManager->HasHazardZone())
		{
			m_HP = 0;
		}
	}

	bool isHit{ m_ActionState == ActionState::hurt };

	pGameObjectManager->TryOpenDoor(collider, m_pInventory, pSoundEffectManager);

	if (!isHit && !m_Invincible)
	{
		isHit |= pEnemyManager->DoHitTest(collider, hittableCollider, shieldCollider, m_Velocity, m_HP, pSoundEffectManager);
	}

	pGameObjectManager->DoHitTest(collider, m_Velocity);

	pLevel->HandleCollision(collider, m_Velocity);
	if (pEnemyManager->IsInBossBattle())
	{
		pCamera->HandleBoundaryCollision(collider, m_Velocity);
	}

	if (isHit)
	{
		m_ActionState = ActionState::hurt;
	}
	else if (!m_HasHit)
	{
		Rectf swordCollider{};

		if (m_ActionState == ActionState::duck_hit)
		{
			swordCollider = Rectf
			{
				m_Shape.left,
				m_Shape.bottom + 2,
				14.0f,
				4
			};
			m_HasHit = true;
		}
		else if (m_ActionState == ActionState::hit)
		{
			if (m_pSprite->GetCurrentFrame() == 1)
			{
				swordCollider = Rectf
				{
					m_Shape.left,
					m_Shape.bottom + m_Shape.height / 2.0f + 2,
					14.0f,
					4
				};
				m_HasHit = true;
			}
		}

		if (m_HasHit)
		{
			if (m_IsLookingRight)
			{
				swordCollider.left += m_Shape.width / 3.0f * 2.0f;
			}

			pEnemyManager->TryHitEnemies(swordCollider, false, pSoundEffectManager);
			ObjectType pickupItem{ pGameObjectManager->TryPickUp(swordCollider) };
			if (!(pickupItem == ObjectType::Nothing))
			{
				m_pInventory->AddItemFromObject(pickupItem);
			}
		}
	}

	ObjectType specialPickup{ pGameObjectManager->TrySpecialPickUp(collider, pSoundEffectManager) };
	if (specialPickup != ObjectType::Nothing)
	{
		switch (specialPickup)
		{
		case ObjectType::Candle:
			m_pInventory->AddItemFromObject(specialPickup);
			m_ActionState = ActionState::item_pickup;
			break;
		case ObjectType::Fairy:
			m_ActionState = ActionState::fairy_pickup;
			break;
		}
	}

	m_Shape.left = collider.left - m_Collider.left;
	m_Shape.bottom = collider.bottom - m_Collider.bottom;
}

void Avatar::UpdateAnimation(float elapsedSec)
{
	if (m_Invincible)
	{
		m_AccuInvincibleSec += elapsedSec;
		if (m_AccuInvincibleSec > m_InvincibleInterval)
		{
			m_AccuInvincibleSec -= m_InvincibleInterval;
			m_pSprite->SetVisibility(!m_pSprite->GetVisibility());
		}
	}

	switch (m_ActionState)
	{
	case Avatar::ActionState::hurt:
		m_pSprite->SetFPS(32);
		break;
	case Avatar::ActionState::moving:
		m_pSprite->SetFPS(16);
		break;
	case Avatar::ActionState::hit:
		m_pSprite->SetFPS(7);
		break;
	case Avatar::ActionState::duck_hit:
		m_pSprite->SetFPS(5);
		break;
	}

	m_pSprite->Update(elapsedSec);

	const int currentFrame{ m_pSprite->GetCurrentFrame() };

	if (m_pSprite->GetTime() < FLT_EPSILON)
	{
		if (currentFrame == 0 && m_ActionState == ActionState::hit)
		{
			m_ActionState = ActionState::idle;
		}
		else if (m_ActionState == ActionState::duck_hit && currentFrame == 1)
		{
			m_ActionState = ActionState::duck;
		}
	}
}

void Avatar::UpdateState(float elapsedSec, bool isOnGround, bool isOnGameObject, bool moving, bool downInput, bool jumpInput)
{
	switch (m_ActionState)
	{
	case ActionState::idle:
		if (m_Velocity.y < 0)
		{
			m_ActionState = ActionState::jump_down;
		}
		else if (downInput || m_Velocity.y > 0)
		{
			if (!isOnGameObject)
			{
				m_ActionState = ActionState::duck;
			}
		}
		else if (moving)
		{
			m_ActionState = ActionState::moving;
		}
		break;
	case ActionState::moving:
		if (m_Velocity.y < 0)
		{
			m_ActionState = ActionState::jump_down;
		}
		else if (downInput || m_Velocity.y > 0)
		{
			if (isOnGround)
			{
				m_ActionState = ActionState::duck;
			}
			else
			{
				m_ActionState = ActionState::idle;
			}
		}
		else if (!moving)
		{
			m_ActionState = ActionState::idle;
		}
		break;
	case ActionState::duck:
		if (m_Velocity.y < 0)
		{
			m_ActionState = ActionState::jump_down;
		}
		else if (!downInput && m_Velocity.y < FLT_EPSILON)
		{
			if (moving)
			{
				m_ActionState = ActionState::moving;
			}
			else
			{
				m_ActionState = ActionState::idle;
			}
		}
		else if (downInput && !isOnGround && isOnGameObject)
		{
			m_ActionState = ActionState::idle;
		}
		break;
	case ActionState::jump_down:
		if (isOnGround || isOnGameObject)
		{
			if (moving)
			{
				m_ActionState = ActionState::moving;
			}
			else
			{
				m_ActionState = ActionState::idle;
			}
		}
		break;
	case ActionState::hit:
		if (isOnGround && jumpInput)
		{
			m_ActionState = ActionState::duck;
		}
		else if (m_pSprite->GetCurrentFrame() > 1)
		{
			if (m_Velocity.y > FLT_EPSILON)
			{
				m_ActionState = ActionState::duck;
			}
			else if (abs(m_Velocity.y) > FLT_EPSILON)
			{
				m_ActionState = ActionState::jump_down;
			}
			else if (downInput)
			{
				m_ActionState = ActionState::duck;
			}
			else if (moving)
			{
				m_ActionState = ActionState::moving;
			}
		}
		break;
	case Avatar::ActionState::duck_hit:
		if (isOnGround && jumpInput)
		{
			m_ActionState = ActionState::duck;
		}
		break;
	case Avatar::ActionState::hurt:
	{
		m_AccuSec += elapsedSec;
		if (m_AccuSec > m_MaxHurtSec)
		{
			m_AccuSec = 0;
			m_ActionState = ActionState::idle;
			m_Invincible = true;
		}
	}
		break;
	case Avatar::ActionState::item_pickup:
	{
		m_AccuSec += elapsedSec;
		if (m_AccuSec > m_PickupAnimTime)
		{
			m_AccuSec = 0;
			m_ActionState = ActionState::idle;
		}
	}
		break;
	}

	if (m_Invincible)
	{
		m_AccuSec += elapsedSec;
		if (m_AccuSec > m_MaxInvincibleSec)
		{
			m_Invincible = false;
			m_AccuSec = 0;
			m_pSprite->SetVisibility(true);
		}
	}
}

void Avatar::UpdateVelocity(float elapsedSec, bool isOnGround, bool isOnGameObject, bool moving, bool jumpInput)
{
	if (isOnGround && jumpInput)
	{
		m_Velocity.y = m_JumpSpeed;
	}

	const float speedUpTime{ 0.2f };
	bool slowDownMovement{};
	float slowDownTime{ 0.2f };

	switch (m_ActionState)
	{
	case Avatar::ActionState::idle:
	case Avatar::ActionState::duck:
	case Avatar::ActionState::hurt:
	case Avatar::ActionState::jump_down:
	case Avatar::ActionState::hit:
		if (abs(m_Velocity.y) > FLT_EPSILON)
		{
			slowDownTime = 10;
			slowDownMovement = true;
		}
		else if (m_ActionState != ActionState::jump_down)
		{
			slowDownMovement = true;
		}
		if (slowDownMovement)
		{
			if (m_Velocity.x < 0.0f)
			{
				m_Velocity.x += m_HorSpeed / slowDownTime * elapsedSec;
				if (m_Velocity.x >= 0.0f)
				{
					m_Velocity.x = 0.0f;
				}
			}
			else if (m_Velocity.x > 0.0f)
			{
				m_Velocity.x += -m_HorSpeed / slowDownTime * elapsedSec;
				if (m_Velocity.x <= 0.0f)
				{
					m_Velocity.x = 0.0f;
				}
			}
		}
		if (m_ActionState == ActionState::duck && abs(m_Velocity.y) < FLT_EPSILON)
		{
			break;
		}
		else if (!moving)
		{
			break;
		}
		else if (isOnGameObject)
		{
			break;
		}
	case Avatar::ActionState::moving:
		if (m_IsLookingRight)
		{
			m_Velocity.x += m_HorSpeed / speedUpTime * elapsedSec;
			if (m_Velocity.x > m_HorSpeed)
			{
				m_Velocity.x = m_HorSpeed;
			}
		}
		else
		{
			m_Velocity.x += -m_HorSpeed / speedUpTime * elapsedSec;
			if (m_Velocity.x < -m_HorSpeed)
			{
				m_Velocity.x = -m_HorSpeed;
			}
		}
		break;
	}

	if (isOnGround || isOnGameObject)
	{
		if (m_Velocity.y < 0.0f)
		{
			m_Velocity.y = 0.0f;
		}
	}
	else
	{
		m_Velocity.y += m_Acceleration.y * elapsedSec;
	}
}

Rectf Avatar::CalculateCollider() const
{
	return Rectf
	{
		m_Shape.left + m_Collider.left,
		m_Shape.bottom + m_Collider.bottom,
		m_Collider.width,
		m_Collider.height
	};
}

Rectf Avatar::CalculateSrcRect() const
{
	const int currentRow{ int(m_ActionState) };

	Point2f spriteSize{ m_pSprite->GetSize() };

	return Rectf
	{
		m_pSprite->GetCurrentFrame() * spriteSize.x,
		(currentRow + 1) * spriteSize.y,
		spriteSize.x,
		spriteSize.y
	};
}

