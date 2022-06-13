#pragma once
#include "GameObject.h"
class GemStatue final : public GameObject
{
public:
	GemStatue(int objectId, const Point2f& position);
	GemStatue(const GemStatue& other) = delete;
	GemStatue(GemStatue&& other) = delete;
	virtual ~GemStatue();
	GemStatue& operator=(const GemStatue& other) = delete;
	GemStatue& operator=(GemStatue&& other) = delete;

	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager) override;
	virtual void DoHitTest(Rectf& actorShape, Vector2f& actorVelocity) override;

	virtual void Draw() const override;
	bool IsInsertingGem() const;
	bool IsAnimationDone() const;
private:
	enum class StatueState
	{
		Idle,
		Flashing,
		Idle_Gem
	};

	Texture* m_pGemTexture{};

	StatueState m_State{ StatueState::Idle };
	float m_AccuSec{};
	float m_AccuFlashTime{};
	float m_GemHeight{ 34.0f };
	bool m_IsInsertingGem{};
	int m_CurFrame{};
	bool m_IsPlayingMusic{};

	const float m_FrameTime{ 1 / 30.0f };
	const float m_GemVelocity{ 63.0f };
	const float m_MaxGemHeight{ 97.0f };
	const float m_FlashingTime{ 2.0f };
};

