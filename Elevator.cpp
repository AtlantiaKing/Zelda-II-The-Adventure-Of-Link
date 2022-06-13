#include "pch.h"
#include "Elevator.h"
#include "Texture.h"
#include "Avatar.h"
#include "Level.h"
#include "SoundEffectManager.h"

Elevator::Elevator(int objectId, const Point2f& pos)
	: GameObject{ objectId, "Resources/Images/Elevator.png", ObjectType::Elevator, Rectf{ pos.x, pos.y, 24, 48 }, Point2f{ 24.0f, 32.0f } }
{
	InitCollider();
}

void Elevator::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
	HandleMovement(elapsedSec, pSoundEffectManager);

	HandleCollisions(pLevel);
}

void Elevator::DoHitTest(Rectf& actorShape, Vector2f& velocity)
{
	utils::HitInfo hitInfo{};

	const Point2f leftTop{ actorShape.left + 1, actorShape.bottom + actorShape.height };
	const Point2f leftBottom{ actorShape.left + 1, actorShape.bottom - 1 };

	const Point2f rightTop{ actorShape.left + actorShape.width - 1, actorShape.bottom + actorShape.height };
	const Point2f rightBottom{ actorShape.left + actorShape.width - 1, actorShape.bottom - 1 };

	if (utils::Raycast(m_FrameColliders, leftTop, leftBottom, hitInfo))
	{
		StopPlayer(actorShape, hitInfo, velocity);
	}
	else if (utils::Raycast(m_FrameColliders, rightTop, rightBottom, hitInfo))
	{
		StopPlayer(actorShape, hitInfo, velocity);
	}
	else
	{
		m_IsPlayerColliding = false;
	}
}

bool Elevator::IsOnGround(const Rectf& actorShape)
{
	utils::HitInfo hitInfo{};

	const Point2f leftTop{ actorShape.left + 1, actorShape.bottom + actorShape.height };
	const Point2f leftBottom{ actorShape.left + 1, actorShape.bottom - 1 };

	const Point2f rightTop{ actorShape.left + actorShape.width - 1, actorShape.bottom + actorShape.height };
	const Point2f rightBottom{ actorShape.left + actorShape.width - 1, actorShape.bottom - 1 };

	if (utils::Raycast(m_FrameColliders, leftTop, leftBottom, hitInfo) || utils::Raycast(m_FrameColliders, rightTop, rightBottom, hitInfo))
	{
		return true;
	}
	return false;
}


void Elevator::StopPlayer(Rectf& actorShape, utils::HitInfo& hitInfo, Vector2f& velocity)
{
	actorShape.bottom = hitInfo.intersectPoint.y;
	velocity.y = 0;
	m_IsPlayerColliding = true;
}

void Elevator::InitCollider()
{
	const Rectf collider{ 0, 0, 24, 8 };

	m_FrameColliders.push_back(Point2f{ m_Shape.left + collider.left, m_Shape.bottom + collider.bottom + collider.height });
	m_FrameColliders.push_back(Point2f{ m_Shape.left + collider.left + collider.width, m_Shape.bottom + collider.bottom + collider.height });
}

void Elevator::HandleCollisions(Level* pLevel)
{
	Rectf worldCollider{ GetWorldCollider() };

	pLevel->HandleCollision(worldCollider, m_Velocity);

	m_Shape.bottom = worldCollider.bottom + worldCollider.height / 2.0f - m_Shape.height / 2.0f;
}

void Elevator::HandleMovement(float elapsedSec, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsPlayerColliding)
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);

		if (pStates[SDL_SCANCODE_DOWN] || pStates[SDL_SCANCODE_S])
		{
			m_Velocity.y = -m_VerSpeed;
		}
		else if (pStates[SDL_SCANCODE_UP] || pStates[SDL_SCANCODE_Z] || pStates[SDL_SCANCODE_W])
		{
			m_Velocity.y = m_VerSpeed;
		}
		else
		{
			m_Velocity.y = 0.0f;
		}

		if (abs(m_Velocity.y) > FLT_EPSILON)
		{
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Elevator);
		}
		else
		{
			pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Elevator);
		}

		m_Shape.bottom += m_Velocity.y * elapsedSec;

		UpdateCollider();
	}
}

void Elevator::UpdateCollider()
{
	const Rectf collider{ 0, 0, 24, 8 };

	m_FrameColliders[0].y = m_Shape.bottom + collider.bottom + collider.height;
	m_FrameColliders[1].y = m_Shape.bottom + collider.bottom + collider.height;
}

Rectf Elevator::GetWorldCollider() const
{
	Rectf worldCollider
	{
		m_Shape.left,
		m_Shape.bottom + m_Shape.height / 2.0f - m_WorldColliderSize.y / 2.0f,
		m_WorldColliderSize.x,
		m_WorldColliderSize.y
	};
	return worldCollider;
}
