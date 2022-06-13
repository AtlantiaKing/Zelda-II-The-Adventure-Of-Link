#pragma once
#include "Hud.h"

class Texture;
class Avatar;
class SoundEffectManager;

class XPManager final
{
public:
	enum class Stat {
		Attack,
		Magic,
		Health
	};

	XPManager();
	XPManager(const XPManager& other) = delete;
	XPManager(XPManager&& other) = delete;
	~XPManager();
	XPManager& operator=(const XPManager& other) = delete;
	XPManager& operator=(XPManager&& other) = delete;

	void Update(float elapsedSec, SoundEffectManager* pSoundEffectManager);
	void AddXP(int amount);
	void MoveSelectionUp(SoundEffectManager* pSoundEffectManager);
	void MoveSelectionDown(SoundEffectManager* pSoundEffectManager);
	void Select(Avatar* pAvatar, SoundEffectManager* pSoundEffectManager);
	void SetData(int xp, int powerLevel, int magicLevel, int healthLevel);
	void UpdateNeededXP(int i);
	void Reset();

	void Draw() const;
	int GetDamage() const;
	int GetXP() const;
	int GetNeededXP() const;
	bool IsUpgrading() const;
	int GetLevel(Stat stat) const;

	friend void Hud::Draw(int health, float healthPercent, XPManager* xpManager) const;
private:
	Texture* m_pLevelUpTexture;
	Texture* m_pNumberTexture;
	Texture* m_pStatSelectTexture;
	Texture* m_pCancelSelectTexture;

	float m_AccuSec{};
	int m_NeedToAddXP{};
	int m_XP{ };
	bool m_IsUpgrading{ false };
	bool m_IsMenuClosing{ false };

	static const int m_NrStats{ 3 };
	int m_Levels[m_NrStats];
	int m_XPNeeded[m_NrStats];
	bool m_Checked[m_NrStats]{};
	int m_Selected{};

	const float m_TickTime{ 1.0f / 60.0f };

	void UpgradeStat(Avatar* pAvatar);

	void DrawNumber(int copyAttack, const Point2f& attackXpPos) const;
};

