#include "pch.h"
#include "Door.h"
#include "utils.h"
#include "Texture.h"
#include "Inventory.h"
#include "SoundEffectManager.h"

Door::Door(int objectId, bool isBossDoor)
	: GameObject{ objectId, "Resources/Images/Door.png", ObjectType::Door, Rectf{ 285.0f, 32.0f, 8.0f, 48.0f }, Point2f{ 16.0f, 48.0f } }
{
	if (isBossDoor)
	{
		m_Shape.left = 560.0f;
	}
}

void Door::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsOpen)
	{
		const float openDoorSpeed{ 2.0f };

		m_Progress += openDoorSpeed * elapsedSec;
		if (m_Progress >= 1.0f)
		{
			m_IsDestroyed = true;
		}
	}
}

void Door::Draw() const
{
	const float clipWidth{ m_pTexture->GetWidth() / 2.0f };
	const float clipHeight{ m_pTexture->GetHeight() };

	const Rectf srcRectLock
	{
		0.0f,
		clipHeight,
		clipWidth,
		clipHeight
	};

	const Rectf srcRectDoor
	{
		clipWidth,
		clipHeight,
		clipWidth,
		clipHeight
	};

	Point2f topDoorPosition
	{
		m_Shape.left,
		m_Shape.bottom + clipHeight * (2.0f - m_Progress)
	};

	Point2f bottomDoorPosition
	{
		m_Shape.left,
		m_Shape.bottom + clipHeight * m_Progress
	};

	Point2f lockPosition
	{
		m_Shape.left,
		m_Shape.bottom + clipHeight
	};

	m_pTexture->Draw(topDoorPosition, srcRectDoor);
	m_pTexture->Draw(bottomDoorPosition, srcRectDoor);
	m_pTexture->Draw(lockPosition, srcRectLock);
}

void Door::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity)
{
	utils::HitInfo hitInfo{};

	const Point2f rayP1{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom + actorShape.height };
	const Point2f rayP2{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom };

	std::vector<Point2f> vertices
	{
		Point2f{ m_Shape.left, m_Shape.bottom },
		Point2f{ m_Shape.left, m_Shape.bottom + m_Shape.height },
		Point2f{ m_Shape.left + m_Shape.width, m_Shape.bottom + m_Shape.height },
		Point2f{ m_Shape.left + m_Shape.width, m_Shape.bottom }
	};

	const Point2f rayHorizontalMiddle{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom + actorShape.height / 2.0f };
	const Point2f rayHorizontalP1{ actorShape.left, actorShape.bottom + actorShape.height / 2.0f };
	const Point2f rayHorizontalP2{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height / 2.0f };

	if (utils::Raycast(vertices, rayHorizontalMiddle, rayHorizontalP1, hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x;

		actorVelocity.x = 0;
		return;
	}

	if (utils::Raycast(vertices, rayHorizontalMiddle, rayHorizontalP2, hitInfo))
	{
		actorShape.left = hitInfo.intersectPoint.x - actorShape.width;

		actorVelocity.x = 0;
		return;
	}
}

void Door::TryOpen(const Rectf& actorShape, Inventory* pInventory, SoundEffectManager* pSoundEffectManager)
{
	if (m_IsOpen)
	{
		return;
	}

	if (utils::IsOverlapping(actorShape, m_Shape))
	{
		if (pInventory->HasKey())
		{
			m_IsOpen = true;
			pInventory->UseKey();
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Door);
		}
	}
}
