#include "pch.h"
#include "ParticleManager.h"
#include "XPParticle.h"
#include "Texture.h"

ParticleManager::ParticleManager()
{
	m_pParticleTextures[ParticleType::Explosion] = new Texture{ "Resources/Images/Explosion.png" };
	m_pParticleTextures[ParticleType::Xp] = new Texture{ "Resources/Images/XP.png" };
}

ParticleManager::~ParticleManager()
{
	for (Particle* pParticle : m_pParticles)
	{
		delete pParticle;
	}

	for (const std::pair<ParticleType, Texture*>& pParticleTexture : m_pParticleTextures)
	{
		delete pParticleTexture.second;
	}
}

void ParticleManager::Update(float elapsedSec)
{
	for (Particle* pParticle : m_pParticles)
	{
		pParticle->Update(elapsedSec);
	}

	for (int i{ int(m_pParticles.size()) - 1}; i >= 0; --i)
	{
		if (m_pParticles[i]->IsDone())
		{
			delete m_pParticles[i];
			m_pParticles[i] = m_pParticles[int(m_pParticles.size()) - 1];
			m_pParticles.pop_back();
		}
	}
}

void ParticleManager::Draw() const
{
	for (Particle* pParticle : m_pParticles)
	{
		pParticle->Draw();
	}
}

void ParticleManager::CreateParticle(ParticleType type, const Point2f position, int data)
{
	switch (type)
	{
	case ParticleType::Explosion:
	{
		const int nrFrames{ 18 };
		const float particleLength{ 0.35f };
		m_pParticles.push_back(new Particle{ m_pParticleTextures[type], position, nrFrames, particleLength });
	}
		break;
	case ParticleType::Xp:
		m_pParticles.push_back(new XPParticle{ m_pParticleTextures[type], data, position });
		break;
	}
}
