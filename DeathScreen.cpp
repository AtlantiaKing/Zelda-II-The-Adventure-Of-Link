#include "pch.h"
#include "DeathScreen.h"
#include "Texture.h"
#include "SoundEffectManager.h"

DeathScreen::DeathScreen()
	: m_pGanonScreen{ new Texture{ "Resources/Images/GanonReturn.png" } }
	, m_pLinkDeath{ new Texture{ "Resources/Images/LinkDeath.png" } }
	, m_pNextLife{ new Texture{ "Resources/Images/NextLife.png" } }
	, m_pNumbers { new Texture{ "Resources/Images/Numbers.png" } }
	, m_pLetters { new Texture{ "Resources/Images/Letters.png" } }
{
}

void DeathScreen::SetActive(bool isActive)
{
	if (m_IsActive == isActive)
	{
		return;
	}
	m_IsActive = isActive;
	m_AccuSec = 0.0f;
	m_DeathScreenState = DeathScreenState::Link;
}

DeathScreen::~DeathScreen()
{
	delete m_pGanonScreen;
	m_pGanonScreen = nullptr;
	delete m_pLinkDeath;
	m_pLinkDeath = nullptr;
	delete m_pNextLife;
	m_pNextLife = nullptr;
	delete m_pNumbers;
	m_pNumbers = nullptr;
	delete m_pLetters;
	m_pLetters = nullptr;
}

void DeathScreen::Update(float elapsedSec, int nrLifes, SoundEffectManager* pSoundEffectManager)
{
	m_AccuSec += elapsedSec;
	if (m_AccuSec >= m_ColorTime)
	{
		if (nrLifes > 0)
		{
			m_DeathScreenState = DeathScreenState::NextLife;
		}
		else
		{
			m_DeathScreenState = DeathScreenState::Ganon;
			pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::GanonReturn);
		}
	}

	if (m_DeathScreenState == DeathScreenState::Link)
	{
		pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::PlayerDeath);
	}
}

void DeathScreen::Draw(float windowwidth, float windowHeight, int nrLifes, const std::string& userName) const
{
	const float textureWidth{ m_pLinkDeath->GetWidth() };
	const float textureHeight{ m_pLinkDeath->GetHeight() };

	const float textureZoom{ windowHeight / textureHeight };

	const Rectf deathTextureDst
	{
		windowwidth / 2.0f - textureWidth * textureZoom / 2.0f,
		0.0f,
		textureWidth * textureZoom,
		windowHeight
	};

	Texture* curTexture{};
	switch (m_DeathScreenState)
	{
	case DeathScreen::DeathScreenState::Link:
		curTexture = m_pLinkDeath;
		break;
	case DeathScreen::DeathScreenState::NextLife:
		curTexture = m_pNextLife;
		break;
	case DeathScreen::DeathScreenState::Ganon:
		curTexture = m_pGanonScreen;
		break;
	}

	if (curTexture != nullptr)
	{
		curTexture->Draw(deathTextureDst);
		if (m_DeathScreenState == DeathScreenState::NextLife)
		{
			const float numberWidth{ m_pNumbers->GetWidth() / 10.0f };
			const float numberHeight{ m_pNumbers->GetHeight() };
			const Rectf numberSrcRect
			{
				numberWidth * nrLifes,
				numberHeight,
				numberWidth,
				numberHeight
			};
			const Rectf numberDstRect
			{
				147.0f * textureZoom,
				128.0f * textureZoom,
				numberWidth * textureZoom,
				numberHeight * textureZoom
			};
			m_pNumbers->Draw(numberDstRect, numberSrcRect);
		}

		if (m_DeathScreenState == DeathScreen::DeathScreenState::NextLife)
		{
			const Point2f nameStartPos
			{
				43.0f * textureZoom,
				127.0f * textureZoom
			};

			for (size_t letter{ 0 }; letter < userName.size(); ++letter)
			{
				if (userName[letter] == ' ')
				{
					continue;
				}

				const float letterSize{ 8.0f };

				const Rectf letterDstRect
				{
					nameStartPos.x + letter * letterSize * textureZoom,
					nameStartPos.y,
					letterSize* textureZoom,
					letterSize* textureZoom
				};
				const Rectf letterSrcRect
				{
					int(userName[letter] - 'A') * letterSize,
					letterSize,
					letterSize,
					letterSize
				};
				m_pLetters->Draw(letterDstRect, letterSrcRect);
			}
		}
	}
}