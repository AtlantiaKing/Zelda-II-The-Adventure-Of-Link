#pragma once
#include "Particle.h"

class XPParticle final : public Particle
{
public:
	XPParticle(Texture* pTexture, int amount, const Point2f position);

	virtual void Update(float elapsedSec) override;

	virtual void Draw() const override;
private:
	const float m_Lifespan{ .65f };
	float m_AccuSec{};
	float m_Speed{ 32.0f };
	Rectf m_SrcRect;
};

