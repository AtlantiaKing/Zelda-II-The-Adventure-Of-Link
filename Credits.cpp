#include "pch.h"
#include "Credits.h"
#include "Texture.h"
#include "utils.h"

Credits::Credits(float windowWidth, float windowHeight)
	: m_pBackground{ new Texture{ "Resources/Images/CreditsBackground.png" } }
	, m_pCredits{ new Texture{ "Resources/Images/Credits.png" } }
	, m_Zoom{ windowHeight / m_pBackground->GetHeight() }
	, m_WindowWidth{ windowWidth }
{
	m_RowsToHold[2] = true;
	m_RowsToHold[4] = true;
	m_RowsToHold[7] = true;
	m_RowsToHold[9] = true;
	m_RowsToHold[12] = true;
	m_RowsToHold[14] = true;
	m_RowsToHold[17] = true;
	m_RowsToHold[19] = true;
}

Credits::~Credits()
{
	delete m_pBackground;
	m_pBackground = nullptr;
	delete m_pCredits;
	m_pCredits = nullptr;
}

void Credits::Update(float elapsedSec)
{
	if (m_IsDone)
	{
		return;
	}

	m_AccuSec += elapsedSec;
	switch (m_State)
	{
	case Credits::CreditsState::Waiting:
	{
		if (m_AccuSec >= m_WaitTime)
		{
			m_AccuSec = 0.0f;
			m_State = CreditsState::Black;
		}
	}
		break;
	case Credits::CreditsState::Black:
	{
		if (m_AccuSec >= m_BlackTime)
		{
			m_AccuSec = 0.0f;
			m_State = CreditsState::Credits;
		}
	}
		break;
	case Credits::CreditsState::Credits:
	{
		const int curRow{ m_CurFrame / m_Columns };
		if (m_RowsToHold[curRow])
		{
			if (m_AccuSec >= m_HoldFrameTime)
			{
				++m_CurFrame;
				m_AccuSec -= m_HoldFrameTime;
			}
		}
		else
		{
			if (m_AccuSec >= m_FadeFrameTime)
			{
				++m_CurFrame;
				m_AccuSec -= m_FadeFrameTime;
			}
		}
	}
		break;
	}
	
	if (m_CurFrame >= m_Rows * m_Columns)
	{
		m_CurFrame = m_Rows * m_Columns - 1;
		m_IsDone = true;
	}
}

void Credits::Reset()
{
	m_AccuSec = 0.0f;
	m_State = CreditsState::Waiting;
	m_IsDone = false;
	m_CurFrame = 0;
}

void Credits::Draw() const
{
	if (m_State == CreditsState::Waiting)
	{
		return;
	}

	const Rectf dstBackgroundRect
	{
		0.0f,
		0.0f,
		m_pBackground->GetWidth() * m_Zoom,
		m_pBackground->GetHeight() * m_Zoom
	};

	if (m_State == CreditsState::Black)
	{
		utils::SetColor(Color4f{ 0.0f, 0.0f, 0.0f, 1.0f });
		utils::FillRect(dstBackgroundRect);
		return;
	}

	const int curRow{ m_CurFrame / m_Columns };
	const int curColumn{ m_CurFrame - curRow * m_Columns };

	const float tileWidth{ m_pCredits->GetWidth() / m_Columns };
	const float tileHeight{ m_pCredits->GetHeight() / m_Rows };

	const Rectf srcCreditsRect
	{
		tileWidth * curColumn,
		tileHeight + tileHeight * curRow,
		tileWidth, 
		tileHeight
	};

	const Rectf dstCreditsRect
	{
		70.0f * m_Zoom,
		70.0f * m_Zoom,
		tileWidth * m_Zoom,
		tileHeight * m_Zoom
	};

	m_pBackground->Draw(dstBackgroundRect);
	m_pCredits->Draw(dstCreditsRect, srcCreditsRect);
}
