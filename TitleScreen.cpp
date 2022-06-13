#include "pch.h"
#include "TitleScreen.h"
#include "Texture.h"
#include "utils.h"

TitleScreen::TitleScreen(float windowWidth, float windowHeight)
	: m_pBackground{ new Texture{ "Resources/Images/Main Menu/Background0.png" } }
	, m_pForeground{ new Texture{ "Resources/Images/Main Menu/Foreground0.png" } }
	, m_pTextScroll{ new Texture{ "Resources/Images/Main Menu/TextScroll.png" } }
	, m_Zoom{ windowHeight / m_pBackground->GetHeight() }
	, m_WindowWidth{ windowWidth }
	, m_ScrollPosition{ 0.0f, -358.0f }
{
}

TitleScreen::~TitleScreen()
{
	delete m_pBackground;
	m_pBackground = nullptr;
	delete m_pForeground;
	m_pForeground = nullptr;
	delete m_pTextScroll;
	m_pTextScroll = nullptr;
}

void TitleScreen::Update(float elapsedSec)
{
	m_AccuCometSec += elapsedSec;

	m_AccuForegroundFrameSec += elapsedSec;
	if (m_AccuForegroundFrameSec >= m_ForegroundFrameTime)
	{
		m_AccuForegroundFrameSec -= m_ForegroundFrameTime;
		++m_CurForegroundFrame %= m_NrFrames;
	}

	m_AccuBackgroundFrameSec += elapsedSec;
	if (m_AccuBackgroundFrameSec >= m_BackgroundFrameTime)
	{
		m_AccuBackgroundFrameSec -= m_BackgroundFrameTime;
		++m_CurBackgroundFrame %= m_NrFrames;
	}

	if (!m_IsWaiting)
	{
		m_ScrollPosition.y += m_ScrollSpeed * elapsedSec;
		if (m_AccuWaitSec < FLT_EPSILON)
		{
			if (m_ScrollPosition.y >= m_WaitHeight)
			{
				m_IsWaiting = true;
			}
		}
		else
		{
			if (m_ScrollPosition.y >= m_pBackground->GetHeight())
			{
				ResetScroll();
			}
		}
	}
	else
	{
		m_AccuWaitSec += elapsedSec;
		if (m_AccuWaitSec > m_HoldTime)
		{
			m_IsWaiting = false;
		}
	}
}

void TitleScreen::Reset()
{
	m_AccuCometSec = 0.0f;
	m_AccuWaitSec = 0.0f;
	m_AccuBackgroundFrameSec = 0.0f;
	m_AccuForegroundFrameSec = 0.0f;
	m_IsWaiting = false;
	m_CurBackgroundFrame = 0;
	m_CurForegroundFrame = 0;
	m_ScrollPosition = Point2f{ 0.0f, -338.0f };
}

void TitleScreen::Draw() const
{
	const Point2f leftBottom{ 0.0f, 0.0f };
	const float clipWidth{ m_pBackground->GetWidth() / m_NrFrames };
	const float clipHeight{ m_pBackground->GetHeight() };

	const Rectf srcBackgroundRect
	{
		m_CurBackgroundFrame * clipWidth,
		clipHeight,
		clipWidth,
		clipHeight
	};
	const Rectf srcForegroundRect
	{
		m_CurForegroundFrame * clipWidth,
		clipHeight,
		clipWidth,
		clipHeight
	};

	glPushMatrix();
	{
		glTranslatef(m_WindowWidth / 2.0f - clipWidth * m_Zoom / 2.0f, 0.0f, 0.0f);
		glScalef(m_Zoom, m_Zoom, 1.0f);
		m_pBackground->Draw(leftBottom, srcBackgroundRect); 
		if (m_AccuCometSec >= m_WaitForComet && m_AccuCometSec <= m_WaitForComet + m_CometTime)
		{
			const Point2f cometPos{ m_CometStartPos + m_CometDirection / m_CometTime * (m_AccuCometSec - m_WaitForComet) };
			utils::FillRect(cometPos.x, cometPos.y, 1.0f, 1.0f);
		}
		m_pTextScroll->Draw(m_ScrollPosition);
		m_pForeground->Draw(leftBottom, srcForegroundRect);
	}
	glPopMatrix();
}

void TitleScreen::ResetScroll()
{
	m_ScrollPosition = Point2f{ 0.0f, -212.0f };
	m_AccuWaitSec = 0.0f;
	m_AccuCometSec = 0.0f;
}
