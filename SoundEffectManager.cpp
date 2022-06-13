#include "pch.h"
#include "SoundEffectManager.h"
#include "SoundEffect.h"

SoundEffectManager::SoundEffectManager()
{
	m_Sounds.insert(std::make_pair(SoundType::Elevator, Sound{ new SoundEffect{ "Resources/Sounds/Elevator.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::EnemyHit, Sound{ new SoundEffect{ "Resources/Sounds/EnemyHit.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::FairyFound, Sound{ new SoundEffect{ "Resources/Sounds/FairyFound.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::HealthAdd, Sound{ new SoundEffect{ "Resources/Sounds/HealthAdd.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::Hurt, Sound{ new SoundEffect{ "Resources/Sounds/Hurt.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::UpgradeSelect, Sound{ new SoundEffect{ "Resources/Sounds/UpgradeSelect.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::EnemyKill, Sound{ new SoundEffect{ "Resources/Sounds/EnemyKill.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::Swordbeam, Sound{ new SoundEffect{ "Resources/Sounds/Swordbeam.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::SwordSlash, Sound{ new SoundEffect{ "Resources/Sounds/SwordSlash.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::Shield, Sound{ new SoundEffect{ "Resources/Sounds/Shield.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::Door, Sound{ new SoundEffect{ "Resources/Sounds/Door.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::InventoryOpen, Sound{ new SoundEffect{ "Resources/Sounds/Inventory.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::ItemPickup, Sound{ new SoundEffect{ "Resources/Sounds/ItemPickup.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::FallingFloor, Sound{ new SoundEffect{ "Resources/Sounds/FallingFloor.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::XP, Sound{ new SoundEffect{ "Resources/Sounds/XP.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::BossExplode, Sound{ new SoundEffect{ "Resources/Sounds/BossExplode.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::KeySpawn, Sound{ new SoundEffect{ "Resources/Sounds/KeySpawn.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::PlayerDeath, Sound{ new SoundEffect{ "Resources/Sounds/PlayerDeath.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::GanonReturn, Sound{ new SoundEffect{ "Resources/Sounds/GanonReturn.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::LowHealth, Sound{ new SoundEffect{ "Resources/Sounds/LowHealth.mp3" }, -1 }));
	m_Sounds.insert(std::make_pair(SoundType::GemInsert, Sound{ new SoundEffect{ "Resources/Sounds/GemInsert.mp3" }, -1 }));
}

SoundEffectManager::~SoundEffectManager()
{
	for (const std::pair<SoundType, Sound>& sound : m_Sounds)
	{
		delete sound.second.pSoundEffect;
	}
}

void SoundEffectManager::Update()
{
	for (int i{ int(m_PlayingSounds.size()) - 1 }; i >= 0; --i)
	{
		SoundType soundType{ m_PlayingSounds[i] };
		if (!IsPlaying(soundType))
		{
			m_Sounds[m_PlayingSounds[i]].curChannel = -1;
			m_PlayingSounds[i] = m_PlayingSounds[m_PlayingSounds.size() - 1];
			m_PlayingSounds.pop_back();
		}
	}
}

void SoundEffectManager::PlaySound(SoundType sound, bool restart)
{
	if (m_Sounds.contains(sound))
	{
		if (!IsPlaying(sound))
		{
			std::pair<bool, int> playResult{ m_Sounds[sound].pSoundEffect->Play(0) };
			if (playResult.first)
			{
				m_Sounds[sound].curChannel = playResult.second;
				m_PlayingSounds.push_back(sound);
			}
		}
		else if(restart)
		{
			StopSound(sound);
			std::pair<bool, int> playResult{ m_Sounds[sound].pSoundEffect->Play(0) };
			if (playResult.first)
			{
				m_Sounds[sound].curChannel = playResult.second;
			}
		}
	}
}

void SoundEffectManager::SetVolume(int volume)
{
	for (const std::pair<SoundType, Sound>& sound : m_Sounds)
	{
		sound.second.pSoundEffect->SetVolume(volume);
	}
}

bool SoundEffectManager::IsPlaying(SoundType sound) const
{
	int channel{ m_Sounds.at(sound).curChannel};
	if (channel >= 0)
	{
		return Mix_Playing(channel);
	}
	return false;
}

void SoundEffectManager::StopSound(SoundType sound) const
{
	int channel{ m_Sounds.at(sound).curChannel};
	if (channel >= 0)
	{
		 Mix_HaltChannel(channel);
	}
}
