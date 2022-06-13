#pragma once

class Texture;

class Credits final
{
public:
	Credits(float windowWidth, float windowHeight);
	Credits(const Credits& other) = delete;
	Credits(Credits&& other) = delete;
	~Credits();
	Credits operator=(const Credits& other) = delete;
	Credits operator=(Credits&& other) = delete;

	void Update(float elapsedSec);
	void Reset();

	void Draw() const;
private:
	enum class CreditsState
	{
		Waiting,
		Black,
		Credits
	};
	Texture* m_pBackground;
	Texture* m_pCredits;
	float m_AccuSec{};
	CreditsState m_State{};
	bool m_IsDone{};
	int m_CurFrame{};

	const float m_WaitTime{ 4.0f };
	const float m_BlackTime{ 4.0f };
	const float m_FadeFrameTime{ 4.0f / 30.0f };
	const float m_HoldFrameTime{ 1.0f };

	const float m_WindowWidth;
	const float m_Zoom;

	static const int m_Rows{ 22 };
	bool m_RowsToHold[m_Rows]{};
	const int m_Columns{ 5 };
};

