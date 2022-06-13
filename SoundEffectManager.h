#pragma once
#include <unordered_map>

class SoundEffect;

class SoundEffectManager final
{
public:
	SoundEffectManager();
	SoundEffectManager(const SoundEffectManager& other) = delete;
	SoundEffectManager(SoundEffectManager&& other) = delete;
	SoundEffectManager& operator=(const SoundEffectManager& other) = delete;
	SoundEffectManager& operator=(SoundEffectManager&& other) = delete;
	~SoundEffectManager();

	enum class SoundType
	{
		Elevator,
		EnemyHit,
		FairyFound,
		HealthAdd,
		Hurt,
		UpgradeSelect,
		EnemyKill,
		Swordbeam,
		SwordSlash,
		Shield,
		Door,
		InventoryOpen,
		ItemPickup,
		FallingFloor,
		XP,
		BossExplode,
		KeySpawn,
		PlayerDeath,
		GanonReturn,
		LowHealth,
		GemInsert
	};

	void Update();
	void PlaySound(SoundType sound, bool restart = false);
	void SetVolume(int volume);

	void StopSound(SoundType sound) const;
	bool IsPlaying(SoundType sound) const;
private:
	struct Sound
	{
		SoundEffect* pSoundEffect;
		int curChannel;
	};

	std::unordered_map<SoundType, Sound> m_Sounds;
	std::vector<SoundType> m_PlayingSounds;
};

