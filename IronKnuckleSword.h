#pragma once
#include "Vector2f.h"

class Texture;

class IronKnuckleSword final
{
public:
	IronKnuckleSword(const std::string& filePath, const Point2f& startPosition, const Vector2f& target, float size);
	IronKnuckleSword(const IronKnuckleSword& other) = delete;
	IronKnuckleSword(IronKnuckleSword&& other) = delete;
	~IronKnuckleSword();
	IronKnuckleSword& operator=(const IronKnuckleSword& other) = delete;
	IronKnuckleSword& operator=(IronKnuckleSword&& other) = delete;

	void Activate();
	void Update(float elapsedSec, bool isLookingRight);
	void SetSwordState(bool isSwordUp);

	void Draw(bool showHurtColors, int curHurtColor, bool isLookingRight) const;
	bool IsActive() const;
	bool IsOverlapping(const Rectf& shape) const;
private:
	Texture* m_pTexture;
	const Point2f m_ShapeSize;
	const float m_Size;
	const Point2f& m_StartPosition;
	const Vector2f m_Target;
	const float m_Speed;
	bool m_IsActive;
	bool m_IsHalfWay;
	float m_AccumTime;
	Point2f m_Position;
	const float m_PlayTime;
	bool m_Up;

	void UpdateTimer(float elapsedSec);
	void UpdatePosition(bool isLookingRight);
};

