#include "pch.h"
#include "Sprite.h"
#include "Texture.h"

Sprite::Sprite(const std::string& filePath, float clipWidth, float clipHeight, int nrOfFrames, int nrFramesPerSec)
	: m_pSpritesTexture{ new Texture(filePath) }
	, m_ClipWidth{ clipWidth }
	, m_ClipHeight{ clipHeight }
	, m_NrOfFrames{ nrOfFrames }
	, m_NrFramesPerSec{ nrFramesPerSec }
	, m_AnimTime{ 0 }
	, m_AnimFrame{ 0 }
	, m_SpriteVisible{ true }
{
}

Sprite::~Sprite()
{
	delete m_pSpritesTexture;
	m_pSpritesTexture = nullptr;
}

void Sprite::Update(float elapsedSec)
{
	m_AnimTime += elapsedSec;
	float frameTime{ 1.0f / m_NrFramesPerSec };

	if (m_AnimTime > frameTime)
	{
		m_AnimTime = 0;
		m_AnimFrame = (m_AnimFrame + 1) % m_NrOfFrames;
	}
}

void Sprite::Draw(const Point2f& position, bool flipSprite, int row)
{
	glPushMatrix();
	{
		glTranslatef(position.x, position.y, 0.0f);

		if (flipSprite)
		{
			glScalef(-1.0f, 1.0f, 1.0f);
		}

		const Rectf dstRect
		{
			-m_ClipWidth / 2.0f,
			0.0f,
			m_ClipWidth,
			m_ClipHeight
		};

		Rectf srcRect
		{
			m_AnimFrame * m_ClipWidth,
			(row + 1) * m_ClipHeight,
			m_ClipWidth,
			m_ClipHeight
		};

		m_pSpritesTexture->Draw(dstRect, srcRect);
	}
	glPopMatrix();
}

void Sprite::ResetAnimation()
{
	m_AnimFrame = 0;
	m_AnimTime = 0;
}

int Sprite::GetCurrentFrame() const
{
	return m_AnimFrame;
}

bool Sprite::GetVisibility() const
{
	return m_SpriteVisible;
}

Point2f Sprite::GetSize() const
{
	return Point2f
	{
		m_ClipWidth,
		m_ClipHeight
	};
}

float Sprite::GetTime() const
{
	return m_AnimTime;
}

void Sprite::SetVisibility(bool isVisible)
{
	m_SpriteVisible = isVisible;
}

void Sprite::SetFPS(int fps)
{
	m_NrFramesPerSec = fps;
}
