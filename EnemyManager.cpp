#include "pch.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Wosu.h"
#include "utils.h"
#include "Camera.h"
#include "Level.h"
#include "Bubble.h"
#include <typeinfo>
#include <fstream>
#include <sstream>
#include "IronKnuckle.h"
#include "Bot.h"
#include "Stalfos.h"
#include "ParticleManager.h"
#include "XPManager.h"
#include "Guma.h"
#include "Horsehead.h"
#include "GameObjectManager.h"
#include "SoundEffectManager.h"

EnemyManager::EnemyManager(ParticleManager* pParticleManager, XPManager* pXPManager)
	: m_pParticleManager{ pParticleManager }
	, m_pXPManager{ pXPManager }
{
}

EnemyManager::~EnemyManager()
{
	for (Enemy* pEnemy : m_pEnemies)
	{
		delete pEnemy;
	}
}

void EnemyManager::Update(float elapsedSec, const Rectf& actorShape, bool actorLookingRight, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager)
{
	TrySpawnWosu(elapsedSec, actorShape, actorLookingRight, pLevel, pCamera);

	UpdateEnemies(elapsedSec, actorShape, pLevel, pCamera, pGameObjectManager, pSoundEffectManager);
}

void EnemyManager::Draw() const
{
	for (Enemy* pEnemy : m_pEnemies)
	{
		pEnemy->Draw();
	}
}

void EnemyManager::LoadEnemies(const std::string& filePath, int chunkId)
{
	Reset();

	LoadEnemySpawns(filePath);

	SpawnEnemies(chunkId);
}

bool EnemyManager::DoHitTest(Rectf& fullActorShape, Rectf& noShieldShape, Rectf& shieldShape, Vector2f& actorVelocity, int& hp, SoundEffectManager* pSoundEffectManager) const
{
	for (Enemy* pEnemy : m_pEnemies)
	{
		if (typeid(*pEnemy) == typeid(Stalfos) ||
			typeid(*pEnemy) == typeid(IronKnuckle) ||
			typeid(*pEnemy) == typeid(Guma))
		{
			Vector2f tempVelocity{};

			if (pEnemy->DoHitTest(shieldShape, tempVelocity))
			{
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::SwordSlash);
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::Swordbeam);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Shield);
			}
			else if (pEnemy->DoHitTest(noShieldShape, actorVelocity))
			{
				hp -= pEnemy->GetDamage();
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Hurt);
				return true;
			}
		}
		else
		{
			if (pEnemy->DoHitTest(fullActorShape, actorVelocity))
			{
				hp -= pEnemy->GetDamage();
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::Hurt);
				return true;
			}
		}
	}
	return false;
}

bool EnemyManager::IsInBossBattle() const
{
	for (size_t i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		if (typeid(*m_pEnemies[i]) == typeid(Horsehead))
		{
			return static_cast<Horsehead*>(m_pEnemies[i])->IsActive();
		}
	}
	return false;
}

bool EnemyManager::IsBossBattleCompleted() const
{
	for (size_t i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		if (typeid(*m_pEnemies[i]) == typeid(Horsehead))
		{
			return static_cast<Horsehead*>(m_pEnemies[i])->IsDeathAnimationBusy();
		}
	}
	return false;
}

int EnemyManager::GetBossHP() const
{
	for (size_t i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		if (typeid(*m_pEnemies[i]) == typeid(Horsehead))
		{
			return static_cast<Horsehead*>(m_pEnemies[i])->GetHealth();
		}
	}
	return 0;
}

const std::vector<int>& EnemyManager::GetKilledEnemies() const
{
	return m_KilledEnemies;
}


void EnemyManager::TryHitEnemies(const Rectf& collider, bool isSwordBeam, SoundEffectManager* pSoundEffectManager)
{
	for (size_t i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		if (isSwordBeam)
		{
			if (typeid(*m_pEnemies[i]) == typeid(Stalfos) || 
				typeid(*m_pEnemies[i]) == typeid(IronKnuckle) ||
				typeid(*m_pEnemies[i]) == typeid(Guma) ||
				typeid(*m_pEnemies[i]) == typeid(Horsehead))
			{
				continue;
			}
		}

		m_pEnemies[i]->DoDamage(collider, m_pXPManager->GetDamage(), pSoundEffectManager);

		if (m_pEnemies[i]->GetHealth() <= 0)
		{
			if (typeid(*m_pEnemies[i]) == typeid(Horsehead))
			{
				Horsehead* horseHead{ static_cast<Horsehead*>(m_pEnemies[i]) };
				if (!horseHead->IsDeathAnimationFinished())
				{
					horseHead->ActivateDeathAnimation();
				}
			}
			else
			{
				KillEnemy(i, pSoundEffectManager);
				pSoundEffectManager->StopSound(SoundEffectManager::SoundType::EnemyHit);
				pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::EnemyKill);
			}
			break;
		}
	}
}

void EnemyManager::SetKilledEnemies(const std::vector<int>& killedEnemies)
{
	m_KilledEnemies = killedEnemies;
}

void EnemyManager::ResetEnemies()
{
	m_KilledEnemies.clear();
}

void EnemyManager::Reset()
{
	for (size_t i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		delete m_pEnemies[i];
		m_pEnemies[i] = nullptr;
	}

	m_pEnemies.clear();
	m_EnemySpawns.clear();
}

bool EnemyManager::IsKilled(int enemyId) const
{
	for (int destroyedId : m_KilledEnemies)
	{
		if (destroyedId == enemyId)
		{
			return true;
		}
	}
	return false;
}

void EnemyManager::LoadEnemySpawns(const std::string& filePath)
{
	std::ifstream file;
	file.open(filePath + "enemies.txt");

	if (file.good())
	{
		std::string line{};
		int nrLine{};
		while (std::getline(file, line))
		{
			std::stringstream stream{ line };
			std::string segment;

			EnemySpawn spawn{};

			int nrSegment{};
			while (std::getline(stream, segment, ';'))
			{
				const int data{ std::stoi(segment) };

				switch (nrSegment)
				{
				case 0:
					spawn.type = Enemy::EnemyType(data);
					break;
				case 1:
					spawn.position.x = float(data);
					break;
				case 2:
					spawn.position.y = float(data);
					break;
				}

				++nrSegment;
			}

			m_EnemySpawns.push_back(spawn);
			++nrLine;
		}
	}

	file.close();
}

void EnemyManager::KillEnemy(size_t enemyIndex, SoundEffectManager* pSoundEffectManager)
{
	m_pXPManager->AddXP(m_pEnemies[enemyIndex]->GetXP());

	const Point2f enemyPosition{ m_pEnemies[enemyIndex]->GetPosition() };
	int nrExplosions{ 2 };

	if (typeid(*m_pEnemies[enemyIndex]) == typeid(Bubble) ||
		typeid(*m_pEnemies[enemyIndex]) == typeid(Bot))
	{
		nrExplosions = 1;
	}

	const float explosionSize{ 16.0f };
	for (int i{ 0 }; i < nrExplosions; ++i)
	{
		const Point2f explosionPosition
		{
			enemyPosition.x,
			enemyPosition.y + explosionSize * i
		};

		m_pParticleManager->CreateParticle(ParticleType::Explosion, explosionPosition);
		if (i == 0)
		{
			if (m_pEnemies[enemyIndex]->GetXP() > 0)
			{
				if (nrExplosions == 0)
				{
					m_pParticleManager->CreateParticle(ParticleType::Xp, explosionPosition, m_pEnemies[enemyIndex]->GetXP());
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::XP);
				}
				else
				{
					const Point2f xpPos{ explosionPosition.x, explosionPosition.y + explosionSize / 2.0f };
					m_pParticleManager->CreateParticle(ParticleType::Xp, xpPos, m_pEnemies[enemyIndex]->GetXP());
					pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::XP);
				}
			}
		}
	}

	if (typeid(*m_pEnemies[enemyIndex]) == typeid(Stalfos) ||
		typeid(*m_pEnemies[enemyIndex]) == typeid(IronKnuckle) ||
		typeid(*m_pEnemies[enemyIndex]) == typeid(Guma) ||
		typeid(*m_pEnemies[enemyIndex]) == typeid(Horsehead))
	{
		m_KilledEnemies.push_back(m_pEnemies[enemyIndex]->GetID());
	}

	delete m_pEnemies[enemyIndex];
	m_pEnemies[enemyIndex] = m_pEnemies[int(m_pEnemies.size()) - 1];
	m_pEnemies.pop_back();
}

void EnemyManager::SpawnEnemies(int chunkId)
{
	for (size_t i{ 0 }; i < m_EnemySpawns.size(); ++i)
	{
		const int enemyId{ chunkId * 10 + int(i) };

		if (IsKilled(enemyId))
		{
			continue;
		}

		const EnemySpawn spawn{ m_EnemySpawns[i] };

		switch (spawn.type)
		{
		case Enemy::EnemyType::Bubble:
		{
			Bubble* bubble = new Bubble{ spawn.position };
			m_pEnemies.push_back(bubble);
		}
		break;
		case Enemy::EnemyType::Bot:
		{
			Bot* bot = new Bot{ spawn.position };
			m_pEnemies.push_back(bot);
		}
		break;
		case Enemy::EnemyType::Iron_Knuckle:
		{
			IronKnuckle* ironKnuckle = new IronKnuckle{ enemyId, spawn.position };
			m_pEnemies.push_back(ironKnuckle);
		}
		break;
		case Enemy::EnemyType::Stalfos:
		{
			Stalfos* stalfos = new Stalfos{ enemyId, spawn.position };
			m_pEnemies.push_back(stalfos);
		}
		break;
		case Enemy::EnemyType::Guma:
		{
			Guma* guma = new Guma{ enemyId, spawn.position };
			m_pEnemies.push_back(guma);
		}
		break;
		case Enemy::EnemyType::Horsehead:
		{
			Horsehead* boss = new Horsehead{ enemyId, spawn.position };
			m_pEnemies.push_back(boss);
		}
		break;
		}
	}
}

void EnemyManager::TrySpawnWosu(float elapsedSec, const Rectf& actorShape, bool actorLookingRight, Level* pLevel, Camera* pCamera)
{
	if (!pCamera->IsClamped(actorShape, pLevel->GetBoundaries()))
	{
		m_AccumSec += elapsedSec;
		if (m_AccumSec > m_WosuTime)
		{
			m_AccumSec = 0;

			const float spawnDistance{ 200.0f };

			for (EnemySpawn spawn : m_EnemySpawns)
			{
				if (spawn.type == Enemy::EnemyType::Wosu)
				{
					if (utils::GetDistance(Point2f{ actorShape.left + actorShape.width / 2.0f, 0.0f }, Point2f{ spawn.position.x, 0.0f }) < spawnDistance)
					{
						Point2f wosuSpawn{};
						bool lookingRight{};

						do
						{
							wosuSpawn = Point2f{ actorShape.left + actorShape.width / 2.0f, spawn.position.y };
							if (actorLookingRight)
							{
								if (rand() % 10 >= 2)
								{
									wosuSpawn.x += pCamera->GetWidth() / 2.0f;
									lookingRight = false;
								}
								else
								{
									wosuSpawn.x -= pCamera->GetWidth() / 2.0f;
									lookingRight = true;
								}
							}
							else
							{
								if (rand() % 10 >= 8)
								{
									wosuSpawn.x += pCamera->GetWidth() / 2.0f;
									lookingRight = false;
								}
								else
								{
									wosuSpawn.x -= pCamera->GetWidth() / 2.0f;
									lookingRight = true;
								}
							}
						} while (!pLevel->IsOnGround(Rectf{ wosuSpawn.x, wosuSpawn.y, 2, 2 }));

						Wosu* wosu = new Wosu(wosuSpawn, lookingRight);
						m_pEnemies.push_back(wosu);
						break;
					}
				}
			}
		}
	}
}

void EnemyManager::UpdateEnemies(float elapsedSec, const Rectf& actorShape, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager)
{
	for (int i{ 0 }; i < m_pEnemies.size(); ++i)
	{
		Enemy* pEnemy{ m_pEnemies[i] };

		if (typeid(*pEnemy) == typeid(Bubble))
		{
			const float spawnMargin{ 10.0f };

			pCamera->HandleBubbleCollision(actorShape, pLevel->GetBoundaries(), (Bubble*)pEnemy);
		}

		if (typeid(*pEnemy) == typeid(Wosu))
		{
			if (!pCamera->IsVisible(pEnemy->GetPosition()))
			{
				delete pEnemy;
				pEnemy = nullptr;
				iter_swap(m_pEnemies.begin() + i, m_pEnemies.begin() + m_pEnemies.size() - 1);
				m_pEnemies.pop_back();
			}
			else
			{
				pEnemy->Update(elapsedSec, actorShape, pLevel);
			}
		}
		else
		{
			const Point2f enemyPosition{ pEnemy->GetPosition() };

			if (typeid(*pEnemy) == typeid(Guma) || pCamera->IsVisible(enemyPosition))
			{
				pEnemy->Update(elapsedSec, actorShape, pLevel);

				if (typeid(*pEnemy) == typeid(Horsehead))
				{
					Horsehead* horseHead{ static_cast<Horsehead*>(pEnemy) };
					if (horseHead->IsActive())
					{
						horseHead->HandleCameraCollision(pCamera);
						horseHead->UpdateDeathAnimation(elapsedSec, m_pParticleManager, pSoundEffectManager);
						if (horseHead->IsDeathAnimationFinished())
						{
							KillEnemy(i, pSoundEffectManager);
							pGameObjectManager->SpawnBossKey(pCamera->GetCenterPosition());
							pSoundEffectManager->PlaySound(SoundEffectManager::SoundType::KeySpawn);
						}
					}
				}
			}
		}
	}
}
