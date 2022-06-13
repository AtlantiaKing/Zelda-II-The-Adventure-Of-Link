#include "pch.h"
#include "XPBag.h"
#include "GameObjectManager.h"
#include "Level.h"

XPBag::XPBag(int objectId, const Point2f& position)
	: GameObject{ objectId, "Resources/Images/XPBag.png", ObjectType::XPBag, Rectf{ position.x, position.y, 8.0f, 16.0f }, Point2f{ 8.0f, 16.0f } }
{
}

void XPBag::Update(float elapsedSec, Level* pLevel, GameObjectManager* pGameObjectManager)
{
	if (!pGameObjectManager->IsOnGround(m_Shape))
	{
		const float gravity{ 150.0f };

		m_Shape.bottom -= gravity * elapsedSec;

		if (m_Shape.bottom < pLevel->GetBoundaries().bottom)
		{
			m_IsDestroyed = true;
		}
	}
}
