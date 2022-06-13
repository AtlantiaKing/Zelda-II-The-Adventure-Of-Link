#pragma once
#include "Enemy.h"
#include "Vector2f.h"
#include <vector>

class Camera;
class ParticleManager;
class XPManager;
class GameObjectManager;
class SoundEffectManager;

class EnemyManager final
{
public:
	EnemyManager(ParticleManager* pParticleManager, XPManager* pXPManager);
	EnemyManager(const EnemyManager& manager) = delete;
	EnemyManager(const EnemyManager&& manager) = delete;
	~EnemyManager();
	EnemyManager& operator=(const EnemyManager& rhs) = delete;
	EnemyManager& operator=(EnemyManager&& rhs) = delete;

	void Update(float elapsedSec, const Rectf& actorShape, bool actorLookingRight, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager);
	void LoadEnemies(const std::string& filePath, int chunkId);
	void TryHitEnemies(const Rectf& collider, bool isSwordBeam, SoundEffectManager* pSoundEffectManager);
	void SetKilledEnemies(const std::vector<int>& killedEnemies);
	void ResetEnemies();

	void Draw() const;
	bool DoHitTest(Rectf& fullActorShape, Rectf& noShieldShape, Rectf& shieldShape, Vector2f& actorVelocity, int& hp, SoundEffectManager* pSoundEffectManager) const;
	bool IsInBossBattle() const;
	bool IsBossBattleCompleted() const;
	int GetBossHP() const;
	const std::vector<int>& GetKilledEnemies() const;
private:
	struct EnemySpawn
	{
		Enemy::EnemyType type;
		Point2f position;
	};

	std::vector<int> m_KilledEnemies;
	ParticleManager* m_pParticleManager;
	XPManager* m_pXPManager;
	float m_WosuTime{ 2.5f };
	float m_AccumSec{};
	bool m_HasWosu{ true };
	std::vector<Enemy*> m_pEnemies;
	std::vector<EnemySpawn> m_EnemySpawns;

	void Reset();
	void LoadEnemySpawns(const std::string& filePath);
	void SpawnEnemies(int chunkId);
	void KillEnemy(size_t enemyIndex, SoundEffectManager* pSoundEffectManager);
	void TrySpawnWosu(float elapsedSec, const Rectf& actorShape, bool actorLookingRight, Level* pLevel, Camera* pCamera);
	void UpdateEnemies(float elapsedSec, const Rectf& actorShape, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager);

	bool IsKilled(int enemyId) const;
};

