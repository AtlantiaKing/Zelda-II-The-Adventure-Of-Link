#include "pch.h"
#include "BossKey.h"
#include "Level.h"

BossKey::BossKey(const Point2f& position) 
	: GameObject{ -1, "Resources/Images/Key.png", ObjectType::Key, Rectf{ position.x - 8.0f, position.y - 81.0f, 16.0f, 16.0f }, Point2f{ 16.0f, 16.0f } }
{
}

void BossKey::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
	const float gravity{ 256.0f };

	m_Velocity.y -= gravity * elapsedSec;
	m_Shape.bottom += m_Velocity.y * elapsedSec;

	if (m_Velocity.y > 0)
	{
		const float restitution{ 1250.0f };
		m_Velocity.y -= restitution * elapsedSec;
	}

	pLevel->HandleCollision(m_Shape, m_Velocity, true);
}
