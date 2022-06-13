#pragma once
#include "enums.h"
#include <vector>
#include <unordered_map>

class Particle;
class Texture;

class ParticleManager final
{
public:
	ParticleManager();
	ParticleManager(const ParticleManager& other) = delete;
	ParticleManager(ParticleManager&& other) = delete;
	~ParticleManager();
	ParticleManager& operator=(const ParticleManager& other) = delete;
	ParticleManager& operator=(ParticleManager&& other) = delete;

	void Update(float elapsedSec);
	void CreateParticle(ParticleType type, const Point2f position, int data = 0);

	void Draw() const;
private:
	std::vector<Particle*> m_pParticles;
	std::unordered_map<ParticleType, Texture*> m_pParticleTextures;
};