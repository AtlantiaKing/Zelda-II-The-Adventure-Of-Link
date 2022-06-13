#pragma once
#include "Vector2f.h"

class Texture;

class TitleScreen final
{
public:
	TitleScreen(float windowWidth, float windowHeight);
	TitleScreen(const TitleScreen& other) = delete;
	TitleScreen(TitleScreen&& other) = delete;
	~TitleScreen();
	TitleScreen& operator=(const TitleScreen& other) = delete;
	TitleScreen& operator=(TitleScreen&& other) = delete;

	void Update(float elapsedSec);
	void Reset();

	void Draw() const;
private:
	Texture* m_pBackground;
	Texture* m_pForeground;
	Texture* m_pTextScroll;

	float m_AccuCometSec{};
	float m_AccuWaitSec{};
	float m_AccuBackgroundFrameSec{};
	float m_AccuForegroundFrameSec{};
	bool m_IsWaiting{};
	Point2f m_ScrollPosition;
	int m_CurBackgroundFrame{};
	int m_CurForegroundFrame{};

	const float m_WaitForComet{ 4.5f };
	const float m_CometTime{ 1.3f };
	const Point2f m_CometStartPos{ 52.0f, 222.0f };
	const Vector2f m_CometDirection{ 57.0f, -38.0f };
	const float m_ForegroundFrameTime{ 1 / 8.0f };
	const float m_BackgroundFrameTime{ 1 / 4.0f };
	const float m_WaitHeight{ -125.0f };
	const float m_ScrollSpeed{ 15.5f };
	const float m_HoldTime{ 30.0f };
	const float m_Zoom;
	const float m_WindowWidth;
	const int m_NrFrames{ 4 };

	void ResetScroll();
};

