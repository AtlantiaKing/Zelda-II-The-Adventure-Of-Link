#pragma once

class Texture;
class XPManager;

class Hud final
{
public:
	Hud(const Point2f& topLeft, float windowWidth);
	~Hud();

	void Draw(int maxHP, float healthPercent, XPManager* xpManager) const;
private:
	Texture* m_pTexture;
	Texture* m_pBar;
	Texture* m_pNumberTexture;
	Rectf m_DstRect;
	float m_Scale;
};

