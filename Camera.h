#pragma once
#include "Vector2f.h"

class Bubble;

class Camera final
{
public:
	Camera(float width, float height);

	void Update(const Rectf& target, const Rectf& levelBoundaries);
	void SetZoom(const Window& window, const Rectf& levelBoundaries);
	bool IsClamped(const Rectf& target, const Rectf& levelBoundaries);
	bool IsVisible(const Point2f& position);

	void Translate() const;
	void TranslateParalax() const;
	void Scale() const;
	float GetWidth() const;
	float GetZoom() const;
	void HandleBubbleCollision(const Rectf& ref, const Rectf& levelBoundaries, Bubble* pBubble) const;
	void HandleBoundaryCollision(Rectf& actorShape, Vector2f& velocity) const;
	Point2f GetCenterPosition() const;
private:
	const float m_Width;
	const float m_Height;
	float m_Zoom;
	Point2f m_CamPosition;

	Point2f Track(const Rectf& target) const;
	void Clamp(Point2f& bottomLeftPos, const Rectf& levelBoundaries) const;

};

