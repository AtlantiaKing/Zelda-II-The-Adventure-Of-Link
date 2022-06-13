#include "pch.h"
#include "GameObject.h"
#include "Texture.h"
#include "utils.h"

GameObject::GameObject(int objectId, const std::string& texturePath, ObjectType type, const Rectf& shape, const Point2f& colliderSize)
	: m_pTexture{ new Texture{ texturePath } }
	, m_Shape{ shape }
	, m_WorldColliderSize{ colliderSize }
	, m_Type{ type }
	, m_ObjectId{ objectId }
{
}

GameObject::~GameObject()
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void GameObject::Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager)
{
}

void GameObject::Draw() const
{
	m_pTexture->Draw(m_Shape);
}

void GameObject::DoHitTest(Rectf& actorShape, Vector2f& actorVelocity)
{
}

bool GameObject::IsOnGround(const Rectf& actorShape)
{
	return false;
}

Point2f GameObject::GetCenterPosition() const
{
	return Point2f{ m_Shape.left + m_Shape.width / 2.0f, m_Shape.bottom };
}

bool GameObject::IsOverlapping(const Rectf& actorShape) const
{
	return utils::IsOverlapping(actorShape, m_Shape);
}

ObjectType GameObject::GetType() const
{
	return m_Type;
}

bool GameObject::IsDestroyed() const
{
	return m_IsDestroyed;
}

int GameObject::GetID() const
{
	return m_ObjectId;
}
