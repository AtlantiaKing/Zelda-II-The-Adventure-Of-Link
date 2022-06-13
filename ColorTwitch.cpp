#include "pch.h"
#include "ColorTwitch.h"
#include "utils.h"

ColorTwitch::ColorTwitch()
{
	m_Colors[0] = Color4f{ 0.77f, 0.27f, 0.02f, 1.0f };
	m_Colors[1] = Color4f{ 0.47f, 1.0f, 0.27f, 1.0f };
	m_Colors[2] = Color4f{ 0.25f, 0.21f, 0.93f, 1.0f };

	m_ColorOrder[0] = 0;
	m_ColorOrder[1] = 1;
	m_ColorOrder[2] = 2;
	m_ColorOrder[3] = 0;
	m_ColorOrder[4] = 1;
	m_ColorOrder[5] = 0;
	m_ColorOrder[6] = 2;
	m_ColorOrder[7] = 1;
}

void ColorTwitch::SetActive(bool isActive)
{
	if (m_IsActive == isActive)
	{
		return;
	}
	m_IsActive = isActive;
	m_CurColor = 0;
}

void ColorTwitch::Update(float elapsedSec)
{
	if (!m_IsActive)
	{
		return;
	}

	m_AccuSec += elapsedSec;
	if (m_AccuSec >= m_FrameTime)
	{
		m_AccuSec -= m_FrameTime;
		++m_CurColor %= m_Length;
	}
}

void ColorTwitch::Draw(float windowWidth, float windowHeight) const
{
	if (!m_IsActive)
	{
		return;
	}

	utils::SetColor(m_Colors[m_ColorOrder[m_CurColor]]);
	utils::FillRect(0.0f, 0.0f, windowWidth, windowHeight);
}
