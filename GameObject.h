#pragma once

#include "Vector2f.h"
#include "enums.h"

class Texture;
class Level;
class SoundEffectManager;

class GameObject
{
public:
	GameObject(int objectId, const std::string& filePath, ObjectType type, const Rectf& shape, const Point2f& colliderSize);
	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;
	virtual ~GameObject();
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	virtual void Update(float elapsedSec, Level* pLevel, SoundEffectManager* pSoundEffectManager);
	virtual void DoHitTest(Rectf& actorShape, Vector2f& actorVelocity);
	virtual bool IsOnGround(const Rectf& actorShape);

	virtual void Draw() const;
	Point2f GetCenterPosition() const;
	bool IsOverlapping(const Rectf& actorShape) const;
	ObjectType GetType() const;
	bool IsDestroyed() const;
	int GetID() const;
protected:
	const int m_ObjectId;
	Texture* m_pTexture;
	bool m_IsDestroyed{};
	Rectf m_Shape;
	Point2f m_WorldColliderSize;
	ObjectType m_Type;
};