#include "pch.h"
#include "Hud.h"
#include "Texture.h"
#include "utils.h"
#include "XPManager.h"

Hud::Hud(const Point2f& topLeft, float windowWidth)
	: m_pTexture{ new Texture{ "Resources/Images/HUD.png" } }
	, m_pNumberTexture{ new Texture{ "Resources/Images/Numbers.png" } }
	, m_pBar{ new Texture{ "Resources/Images/HealthBar.png" } }
{
	m_Scale = windowWidth / m_pTexture->GetWidth();
	m_DstRect = Rectf
	{
		topLeft.x,
		topLeft.y - m_pTexture->GetHeight() * m_Scale,
		m_pTexture->GetWidth() * m_Scale,
		m_pTexture->GetHeight() * m_Scale
	};
}

Hud::~Hud()
{
	delete m_pTexture;
	m_pTexture = nullptr;
	delete m_pNumberTexture;
	m_pNumberTexture = nullptr;
	delete m_pBar;
	m_pBar = nullptr;
}

void Hud::Draw(int maxHP, float healthPercent, XPManager* pXpManager) const
{
	int neededXP{ pXpManager->GetNeededXP() };
	int xp{ pXpManager->m_XP };

	Point2f xpPos
	{ 
		m_DstRect.left + 176.0f * m_Scale, 
		m_DstRect.bottom + 1.0f * m_Scale 
	};

	const float digitSize{ 8.0f };
	const int nrDigits{ 4 };

	for (int i{ 0 }; i < nrDigits; ++i)
	{
		const int digit = xp / int(powf(10.0f, nrDigits - 1.0f - i));
		xp -= digit * int(powf(10.0f, nrDigits - 1.0f - i));
		
		const Rectf srcRect
		{
			digit * digitSize,
			m_pNumberTexture->GetHeight(),
			digitSize,
			m_pNumberTexture->GetHeight()
		};

		const Rectf dstRect
		{
			xpPos.x + i * digitSize * m_Scale,
			xpPos.y,
			digitSize * m_Scale,
			m_pNumberTexture->GetHeight() * m_Scale
		};

		m_pNumberTexture->Draw(dstRect, srcRect);
	}

	xpPos = Point2f
	{ 
		m_DstRect.left + 216.0f * m_Scale, 
		m_DstRect.bottom + 1.0f * m_Scale 
	};

	for (int i{ 0 }; i < nrDigits; ++i)
	{
		const int digit = neededXP / int(powf(10.0f, nrDigits - 1.0f - i));
		neededXP -= digit * int(powf(10.0f, nrDigits - 1.0f - i));

		const Rectf srcRect
		{
			digit * digitSize,
			m_pNumberTexture->GetHeight(),
			digitSize,
			m_pNumberTexture->GetHeight()
		};

		const Rectf dstRect
		{
			xpPos.x + i * digitSize * m_Scale,
			xpPos.y,
			digitSize * m_Scale,
			m_pNumberTexture->GetHeight() * m_Scale
		};

		m_pNumberTexture->Draw(dstRect, srcRect);
	}

	const Rectf dstPowerRect = Rectf{
		m_DstRect.left + 17.0f * m_Scale, 
		m_DstRect.bottom + 1.0f * m_Scale,
		digitSize * m_Scale,
		m_pNumberTexture->GetHeight() * m_Scale
	};
	const Rectf srcPowerRect
	{
		pXpManager->m_Levels[int(XPManager::Stat::Attack)] * digitSize,
		m_pNumberTexture->GetHeight(),
		digitSize,
		m_pNumberTexture->GetHeight()
	};
	m_pNumberTexture->Draw(dstPowerRect, srcPowerRect);


	const Rectf dstMagicRect = Rectf{
		m_DstRect.left + 80.0f * m_Scale,
		m_DstRect.bottom + 8.0f * m_Scale,
		digitSize * m_Scale,
		m_pNumberTexture->GetHeight() * m_Scale
	};
	const Rectf srcMagicRect
	{
		pXpManager->m_Levels[int(XPManager::Stat::Magic)] * digitSize,
		m_pNumberTexture->GetHeight(),
		digitSize,
		m_pNumberTexture->GetHeight()
	};
	m_pNumberTexture->Draw(dstMagicRect, srcMagicRect);


	const Rectf dstHealthRect = Rectf{
		m_DstRect.left + 145.0f * m_Scale,
		m_DstRect.bottom + 8.0f * m_Scale,
		digitSize * m_Scale,
		m_pNumberTexture->GetHeight() * m_Scale
	};
	const Rectf srcHealthRect
	{
		pXpManager->m_Levels[int(XPManager::Stat::Health)] * digitSize,
		m_pNumberTexture->GetHeight(),
		digitSize,
		m_pNumberTexture->GetHeight()
	};
	m_pNumberTexture->Draw(dstHealthRect, srcHealthRect);

	const int healthPerBlock{ 32 };
	const int healthBlockOffset{ 2 };
	const int extraHealthBlocks{ maxHP / healthPerBlock - healthBlockOffset };

	const Point2f healthPos
	{ 
		m_DstRect.left + 112.0f * m_Scale, 
		m_DstRect.bottom + 2.0f * m_Scale 
	};

	const float healthWidth{ 31.0f + 8.0f * extraHealthBlocks };
	const float healthHeight{ 5.0f };

	utils::SetColor(Color4f{ 0.843f, 0.156f, 0.0f, 1.0f });
	utils::FillRect(healthPos.x, healthPos.y, healthWidth * m_Scale * healthPercent, healthHeight * m_Scale);

	for (int i{ 0 }; i < extraHealthBlocks; ++i)
	{
		const Rectf healthBorder
		{
			healthPos.x + 31.0f * m_Scale + 31.0f / 4.0f * m_Scale * i,
			healthPos.y - m_Scale,
			m_pBar->GetWidth() * m_Scale,
			m_pBar->GetHeight() * m_Scale
		};
		m_pBar->Draw(healthBorder);
	}

	m_pTexture->Draw(m_DstRect);
}
