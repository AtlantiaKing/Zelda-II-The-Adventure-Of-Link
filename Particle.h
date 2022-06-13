#pragma once

class Texture;

class Particle
{
public:
	Particle(Texture* pTexture, const Point2f& position, int nrFrames, float particleTime);
	Particle(const Particle& other) = delete;
	Particle(Particle&& other) = delete;
	virtual ~Particle() = default;
	Particle& operator=(const Particle& other) = delete;
	Particle& operator=(Particle&& other) = delete;

	virtual void Update(float elapsedSec);

	virtual void Draw() const;
	bool IsDone() const;
private:
	const int m_NrFrames;
	int m_CurFrame{};
	const float m_FrameTime;
	float m_AccuSec{};
protected:
	Texture* m_pTexture;
	Point2f m_Position;
	bool m_IsDone{};
};

