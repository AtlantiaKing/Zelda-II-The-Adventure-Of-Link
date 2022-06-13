#include "pch.h"
#include "Bot.h"
#include "Level.h"
#include "Texture.h"
#include "utils.h"

Bot::Bot(const Point2f& position)
	: Enemy{ 0, "Bot", position, Rectf{ 0.0f, 2.0f, 16.0f, 12.0f }, 4, 2, 16, 0.35f }
{

}

void Bot::Update(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Enemy::Update(elapsedSec, actorShape, pLevel);

	HandleState(elapsedSec, actorShape, pLevel);

	HandleMovement(elapsedSec, pLevel);

	HandleCollisions(pLevel);
}

void Bot::Draw() const
{
	Texture* curTexture{ GetCurrentTexture() };

	const float textureWidth{ curTexture->GetWidth() };
	const float textureHeight{ curTexture->GetHeight() };
	const float clipWidth{ textureWidth / 3.0f };
	const float clipHeight{ textureHeight };

	Rectf srcRect
	{
		0.0,
		clipHeight,
		clipWidth,
		clipHeight
	};

	switch (m_State)
	{
	case Bot::BotState::idle:
		srcRect.left = int(m_Frame) * clipWidth;
		break;
	case Bot::BotState::walking:
		srcRect.left = int(BotState::jumping) * clipWidth;
		break;
	case Bot::BotState::walk_idle:
		srcRect.left = int(BotState::idle) * clipWidth;
		break;
	case Bot::BotState::jumping:
		srcRect.left = int(BotState::jumping) * clipWidth;
		break;
	}

	curTexture->Draw(m_Position, srcRect);
}


void Bot::HandleState(float elapsedSec, const Rectf& actorShape, Level* pLevel)
{
	Rectf collider{ CalculateCollider() };

	if (pLevel->IsOnGround(collider))
	{
		switch (m_State)
		{
		case Bot::BotState::idle:
		{
			m_AccuSeconds += elapsedSec;
			if (m_AccuSeconds >= m_IdleTime)
			{
				RandomizeState();
			}
			else
			{
				m_AccuSpriteTimer += elapsedSec;
				if (m_AccuSpriteTimer >= m_FrameTime)
				{
					m_AccuSpriteTimer = 0.0f;
					m_Frame = (m_Frame + 1) % 2;
				}
			}
		}
		break;
		case Bot::BotState::walking:
		{
			if (m_Jumps == 4)
			{
				RandomizeState();
			}
			else
			{
				m_AccuSeconds += elapsedSec;
				if (m_AccuSeconds >= m_JumpTime)
				{
					m_State = BotState::walk_idle;
					m_Velocity.x = 0.0f;
					m_AccuSeconds = 0.0f;
				}
			}
		}
		break;
		case Bot::BotState::walk_idle:
		{
			m_AccuSeconds += elapsedSec;
			if (m_AccuSeconds >= m_JumpIdleTime)
			{
				Walk();
				m_State = BotState::walking;
			}
		}
		break;
		case Bot::BotState::jumping:
		{
			Jump(actorShape);
			RandomizeState();
		}
		break;
		}
	}
}

void Bot::HandleMovement(float elapsedSec, Level* pLevel)
{
	const float gravity{ 240.0f };

	m_Velocity.y -= gravity * elapsedSec;
		
	m_Position.x += m_Velocity.x * elapsedSec;
	m_Position.y += m_Velocity.y * elapsedSec;
}

void Bot::HandleCollisions(Level* pLevel)
{
	Rectf collider{ CalculateCollider() };

	pLevel->HandleCollision(collider, m_Velocity);

	m_Position.x = collider.left;
	m_Position.y = collider.bottom - m_Collider.bottom;
}

void Bot::Walk()
{
	const float speed{ 10.0f /* * ((m_JumpTime - m_AccuSeconds) / m_JumpTime) */ };

	m_Velocity.x = m_Direction * speed;

	++m_Jumps;
	m_AccuSeconds = 0.0f;
}

void Bot::Jump(const Rectf& actorShape)
{
	const float speed{ 50.0f };
	const float jumpSpeed{ 100.0f };

	const float direction{ actorShape.left - m_Position.x };

	if (direction < 0)
	{
		m_Velocity.x = -speed;
	}
	else
	{
		m_Velocity.x = speed;
	}
	m_Velocity.y = jumpSpeed;
}

void Bot::RandomizeState()
{
	BotState newState{};
	do
	{
		newState = BotState(rand() % (int(BotState::jumping) + 1));
	} while (m_State == newState && (m_State == BotState::idle || m_State == BotState::jumping));

	m_State = newState;

	switch (m_State)
	{
	case Bot::BotState::idle:
	{
		m_AccuSeconds = 0.0f;
		m_Velocity.x = 0.0f;
		m_Velocity.y = 0.0f;
	}
		break;
	case Bot::BotState::walking:
	{
		m_Jumps = 0;
		m_Direction = utils::GetRandomDirection();
		m_AccuSeconds = 0.0f;
	}
		break;
	}
}
