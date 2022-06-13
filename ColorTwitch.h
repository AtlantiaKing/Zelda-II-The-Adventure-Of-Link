#pragma once
class ColorTwitch final
{
public:
	ColorTwitch();

	void SetActive(bool isActive);
	void Update(float elapsedSec);
	void Draw(float windowWidth, float windowHeight) const;
private:
	float m_AccuSec{};
	int m_CurColor{};
	bool m_IsActive{};

	static const int m_NrColors{ 3 };
	Color4f m_Colors[m_NrColors];

	static const int m_Length{ 8 };
	int m_ColorOrder[m_Length];

	const float m_FrameTime{ 1.0f / 40.0f };
};

