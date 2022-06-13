#pragma once

class Texture;

class Sprite final
{
public:
	Sprite(const std::string& filePath, float clipWidth, float clipHeight, int nrOfFrames, int nrFramesPerSec);
	Sprite(const Sprite& sprite) = delete;
	Sprite(Sprite&& sprite) = delete;
	Sprite& operator=(const Sprite& rhs) = delete;
	Sprite& operator=(Sprite&& rhs) = delete;
	~Sprite();

	void Update(float elapsedSec);
	void Draw(const Point2f& position, bool flipSprite, int row = 0);
	void ResetAnimation();

	int GetCurrentFrame() const;
	bool GetVisibility() const;
	Point2f GetSize() const;
	float GetTime() const;

	void SetVisibility(bool isVisible);
	void SetFPS(int fps);
private:
	Texture* m_pSpritesTexture;
	float m_ClipWidth;
	float m_ClipHeight;
	int m_NrOfFrames;
	int m_NrFramesPerSec;
	float m_AnimTime;
	int m_AnimFrame;
	bool m_SpriteVisible;
};

