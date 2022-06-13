#include "pch.h"
#include "GemStatue.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffectManager.h"

GemStatue::GemStatue(int objectId, const Point2f& position)
	: GameObject{ objectId, "Resources/Images/GemStatue.png", ObjectType::Gem_Statue, Rectf{ position.x, position.y, 160.0f, 112.0f }, Point2f{ 32.0f, 112.0f } }
	, m_pGemTexture{ new Texture{ "Resources/Images/Gem.png" } }
{

}

GemStatue::~GemStatue()
{
	delete m_pGemTexture;
	m_pGemTexture = nullptr;
}

void GemStatue::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
	m_AccuSec += elapsedSec;
	if (m_AccuSec >= m_FrameTime)
	{
		m_AccuSec -= m_FrameTime;
		++m_CurFrame %= 2;
	}

	if (m_IsInsertingGem)
	{
		if (!m_IsPlayingMusic && m_State != StatueState::Idle_Gem)
		{
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::GemInsert);
			m_IsPlayingMusic = true;
		}
		m_GemHeight += m_GemVelocity * elapsedSec;
		if (m_GemHeight >= m_MaxGemHeight)
		{
			m_GemHeight = m_MaxGemHeight;
			if (m_State == StatueState::Idle)
			{
				m_State = StatueState::Flashing;
			}
		}
	}
	
	if (m_State == StatueState::Flashing)
	{
		m_AccuFlashTime += elapsedSec;
		if (m_AccuFlashTime > m_FlashingTime)
		{
			m_State = StatueState::Idle_Gem;
		}
	}

	if (m_State == StatueState::Idle_Gem)
	{
		if (!pSoundEffectManager->IsPlaying(SoundEffectManager::SoundType::GemInsert))
		{
			m_IsPlayingMusic = false;
		}
	}
}

void GemStatue::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity)
{
	if (!m_IsInsertingGem)
	{
		const Rectf collider
		{
			m_Shape.left + m_Shape.width / 2.0f - m_WorldColliderSize.x / 2.0f,
			m_Shape.bottom,
			m_WorldColliderSize.x,
			m_WorldColliderSize.y
		};

		if (utils::IsOverlapping(actorShape, collider))
		{
			m_IsInsertingGem = true;
		}
	}
}

void GemStatue::Draw() const
{
	const float statueWidth{ m_pTexture->GetWidth() / 2.0f };
	const float statueHeight{ m_pTexture->GetHeight() / 2.0f };

	const Rectf srcStatueRect
	{
		m_CurFrame * statueWidth,
		statueHeight - int(m_State) * statueHeight,
		statueWidth,
		statueHeight
	};

	m_pTexture->Draw(m_Shape, srcStatueRect);

	if (m_IsInsertingGem)
	{
		const float gemWidth{ m_pGemTexture->GetWidth() / 2.0f };
		const float gemHeight{ m_pGemTexture->GetHeight() };

		const Rectf srcGemRect
		{
			m_CurFrame * gemWidth,
			gemHeight,
			gemWidth,
			gemHeight
		};

		const Point2f gemPosition
		{
			m_Shape.left + m_Shape.width / 2.0f - m_pGemTexture->GetWidth() / 4.0f,
			m_Shape.bottom + m_GemHeight
		};

		m_pGemTexture->Draw(gemPosition, srcGemRect);
	}
}

bool GemStatue::IsInsertingGem() const
{
	return m_IsInsertingGem;
}

bool GemStatue::IsAnimationDone() const
{
	return m_State == StatueState::Idle_Gem && !m_IsPlayingMusic;
}
