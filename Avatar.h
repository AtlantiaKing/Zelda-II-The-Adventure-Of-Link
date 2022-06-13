#pragma once
#include "Vector2f.h"

class Level;
class Texture;
class GameObjectManager;
class EnemyManager;
class SwordBeam;
class Sprite;
class Inventory;
class XPManager;
class Camera;
class SoundEffectManager;

class Avatar final
{
public:
	enum class ActionState
	{
		idle,
		moving,
		hit,
		duck,
		jump_down,
		duck_hit,
		hurt,
		item_pickup,
		fairy_pickup
	};

	Avatar(XPManager* pXPManager);
	Avatar(const Avatar& other) = delete;
	Avatar(Avatar&& other) = delete;
	~Avatar();
	Avatar& operator=(const Avatar& other) = delete;
	Avatar& operator=(Avatar&& other) = delete;

	void Update(float elapsedSec, Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, SoundEffectManager* pSoundEffectManager);
	void Attack(SwordBeam* swordBeam, SoundEffectManager* pSoundEffectManager);
	void Teleport(const Rectf& target);
	void Respawn();
	void SetHealth(int level, float levelPercentage = 1.0f);
	void SetLifes(int lifes);
	void Reset(XPManager* pXPManager);

	void Draw() const;
	Rectf GetShape() const;
	bool IsDead() const;
	bool IsPickingUpItem() const;
	float GetHealthPercentage() const;
	int GetMaxHealth() const;
	int GetRemainingLifes() const;
	int GetKeysAmount() const;
	bool HasTakenCandle() const;
	void AddKeys(int nrKeys) const;
	void GiveCandle() const;
	bool IsLookingRight() const;
private:
	Rectf m_Shape{ 50, 50, 48, 32 };
	Sprite* m_pSprite;
	Inventory* m_pInventory;
	Vector2f m_Velocity{ 0.0f, 0.0f };
	ActionState m_ActionState{ ActionState::idle };
	bool m_IsLookingRight{ true };
	bool m_Invincible{ false };
	bool m_HasHit{ false };
	int m_IncomingDamage{};
	int m_MaxHP{ 64 };
	int m_HP{ 64 };
	float m_AccuDamageTime{};
	bool m_IsDead{};
	float m_AccuSec{ 0.0f };
	float m_AccuInvincibleSec{ 0.0f };
	int m_NrLifes{ 3 };
	float m_AccuBeepSoundSec{};

	const float m_TimePerHealthChange{ 1.0f / 30.0f };
	const float m_HorSpeed{ 96.0f };
	const float m_JumpSpeed{ 255.0f };
	const Rectf m_Collider{ 19, 1, 12, 29 };
	const float m_MaxHurtSec{ 0.5f };
	const float m_MaxInvincibleSec{ 0.5f };
	const float m_InvincibleInterval{ 1.0f / 64.0f };
	const float m_PickupAnimTime{ 2.0f };
	const float m_TimeBetweenBeeps{ 1.0f };
	const Vector2f m_Acceleration{ 0, -981.0f };

	void UpdateHealth(float elapsedSec, SoundEffectManager* pSoundEffectManager);
	void HandleInputAndState(Level* pLevel, GameObjectManager* pGameObjectManager, SoundEffectManager* pSoundEffectManager, float elapsedSec);
	void MovePlayer(float elapsedSec);
	void HandleCollision(Level* pLevel, Camera* pCamera, GameObjectManager* pGameObjectManager, EnemyManager* pEnemyManager, XPManager* pXPManager, SoundEffectManager* pSoundEffectManager);
	void UpdateAnimation(float elapsedSec);
	void UpdateVelocity(float elapsedSec, bool isOnGround, bool isOnGameObject, bool moving, bool jumpInput);
	void UpdateState(float elapsedSec, bool isOnGround, bool isOnGameObject, bool moving, bool downInput, bool jumpInput);

	Rectf CalculateCollider() const;
	Rectf CalculateSrcRect() const;
};

