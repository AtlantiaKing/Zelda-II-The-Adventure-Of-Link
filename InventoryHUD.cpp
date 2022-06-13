#include "pch.h"
#include "InventoryHUD.h"
#include "Texture.h"
#include "SoundEffectManager.h"

InventoryHUD::InventoryHUD()
	: m_pTexture{ new Texture{ "Resources/Images/Inventory.png" } }
	, m_pNumbers{ new Texture{ "Resources/Images/Numbers.png" } }
	, m_pCandleTexture{ new Texture{ "Resources/Images/Candle.png" } }
{
}

InventoryHUD::~InventoryHUD()
{
	delete m_pTexture;
	m_pTexture = nullptr;
	delete m_pNumbers;
	m_pNumbers = nullptr;
	delete m_pCandleTexture;
	m_pCandleTexture = nullptr;
}

void InventoryHUD::Toggle(SoundEffectManager* pSoundEffectManager)
{
	m_IsActive = !m_IsActive;
	if (m_IsActive)
	{
		pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::InventoryOpen, true);
	}
}

bool InventoryHUD::IsActive()
{
	return m_IsActive;
}

void InventoryHUD::Draw(int lifes, int keys, bool hasCandle, float zoom) const
{
	if (!m_IsActive)
	{
		return;
	}

	const Rectf dstRect
	{
		112 * zoom,
		32 * zoom,
		m_pTexture->GetWidth() * zoom,
		m_pTexture->GetHeight() * zoom
	};
	m_pTexture->Draw(dstRect);

	const float numberWidth{ m_pNumbers->GetWidth() / 10.0f };
	const float numberHeight{ m_pNumbers->GetHeight() };
	const float numberSizeFactor{ 1.0f };

	const Rectf lifesDstRect
	{
		dstRect.left + 24 * zoom,
		dstRect.bottom + 25 * zoom,
		numberWidth * numberSizeFactor * zoom,
		numberHeight * numberSizeFactor * zoom
	};
	const Rectf lifesSrcRect
	{
		lifes * numberWidth,
		numberHeight,
		numberWidth,
		numberHeight
	};
	m_pNumbers->Draw(lifesDstRect, lifesSrcRect);

	const Rectf keysDstRect
	{
		dstRect.left + 24 * zoom,
		dstRect.bottom + 17 * zoom,
		numberWidth * numberSizeFactor* zoom,
		numberHeight * numberSizeFactor* zoom
	};
	const Rectf keysSrcRect
	{
		keys * numberWidth,
		numberHeight,
		numberWidth,
		numberHeight
	};
	m_pNumbers->Draw(keysDstRect, keysSrcRect);

	const Rectf rupeesDstRect
	{
		dstRect.left + 24 * zoom,
		dstRect.bottom + 9 * zoom,
		numberWidth * numberSizeFactor * zoom,
		numberHeight * numberSizeFactor * zoom
	};
	const Rectf rupeesSrcRect
	{
		0.0f,
		numberHeight,
		numberWidth,
		numberHeight
	};
	m_pNumbers->Draw(rupeesDstRect, rupeesSrcRect);

	if (hasCandle)
	{
		const Rectf candleDstRect
		{
			dstRect.left + 36 * zoom,
			dstRect.bottom + 8 * zoom,
			m_pCandleTexture->GetWidth() * zoom,
			m_pCandleTexture->GetHeight()* zoom
		};
		m_pCandleTexture->Draw(candleDstRect);
	}
}
