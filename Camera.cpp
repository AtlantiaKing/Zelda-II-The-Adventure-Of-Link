#include "pch.h"
#include "Camera.h"
#include "utils.h"
#include "Bubble.h"

Camera::Camera(float width, float height)
	: m_Width{ width }
	, m_Height{ height }
	, m_Zoom{ 1 }
	, m_CamPosition{}
{
}

void Camera::SetZoom(const Window& window, const Rectf& levelBoundaries)
{
	m_Zoom = window.height / levelBoundaries.height;
}

void Camera::Update(const Rectf& target, const Rectf& levelBoundaries)
{
	m_CamPosition = Track(target);
	Clamp(m_CamPosition, levelBoundaries);
}

void Camera::Translate() const
{
	glTranslatef(-m_CamPosition.x, -m_CamPosition.y, 0.0f);
}

void Camera::TranslateParalax() const
{
	//glTranslatef(-m_CamPosition.x / 10.0f, -m_CamPosition.y, 0.0f);
}

void Camera::Scale() const
{
	glScalef(m_Zoom, m_Zoom, 1.0f);
}

bool Camera::IsVisible(const Point2f& position)
{
	const float margin{ 50.0f };

	if (-m_CamPosition.x + position.x < -margin ||
		-m_CamPosition.x + position.x > m_Width / m_Zoom + margin)
	{
		return false;
	}
	return true;
}

float Camera::GetWidth() const
{
	return m_Width / m_Zoom;
}

float Camera::GetZoom() const
{
	return m_Zoom;
}

bool Camera::IsClamped(const Rectf& target, const Rectf& levelBoundaries)
{
	Point2f bottomLeftPos{ Track(target) };
	if (bottomLeftPos.x < levelBoundaries.left)
	{
		return true;
	}
	else if (bottomLeftPos.x > levelBoundaries.left + levelBoundaries.width - m_Width / m_Zoom)
	{
		return true;
	}
	return false;
}

Point2f Camera::Track(const Rectf& target) const
{
	Point2f camPosition { 
		(target.left + target.width / 2.0f) * m_Zoom  - m_Width / 2.0f,
		(target.bottom + target.height / 2.0f) * m_Zoom - m_Height / 2.0f
	};

	camPosition.x /= m_Zoom;
	camPosition.y /= m_Zoom;

	return camPosition;
}

void Camera::Clamp(Point2f& bottomLeftPos, const Rectf& levelBoundaries) const
{
	if (bottomLeftPos.x < levelBoundaries.left)
	{
		bottomLeftPos.x = levelBoundaries.left;
	}
	else if (bottomLeftPos.x > levelBoundaries.left + levelBoundaries.width - m_Width / m_Zoom)
	{
		bottomLeftPos.x = levelBoundaries.left + levelBoundaries.width - m_Width / m_Zoom;
	}

	if (bottomLeftPos.y < levelBoundaries.bottom)
	{
		bottomLeftPos.y = levelBoundaries.bottom;
	}
	else if (bottomLeftPos.y > levelBoundaries.bottom + levelBoundaries.height - m_Height / m_Zoom)
	{
		bottomLeftPos.y = levelBoundaries.bottom + levelBoundaries.height - m_Height / m_Zoom;
	}
}

void Camera::HandleBubbleCollision(const Rectf& target, const Rectf& levelBoundaries, Bubble* pBubble) const
{
	const float collisionMargin{ 2.0f };
	const Point2f bubblePosition{ pBubble->GetPosition() };

	const float xDistance{ abs(m_CamPosition.x + (m_Width / 2.0f) / m_Zoom - bubblePosition.x) };

	if (pBubble->IsActive() && xDistance > (m_Width / 2.0f) / m_Zoom && xDistance < (m_Width / 2.0f) / m_Zoom + collisionMargin)
	{
		pBubble->InverseVelocity(true, false);
	}
	else if (xDistance > (m_Width / 2.0f) / m_Zoom)
	{
		pBubble->SetActive(false);
	}
	else
	{
		pBubble->SetActive(true);
	}

	if (bubblePosition.y < levelBoundaries.bottom || bubblePosition.y > levelBoundaries.bottom + levelBoundaries.height)
	{
		pBubble->InverseVelocity(false, true);
	}
}

void Camera::HandleBoundaryCollision(Rectf& actorShape, Vector2f& velocity) const
{
	const float leftRelativePos{ actorShape.left - m_CamPosition.x };
	const float rightRelativePos{ actorShape.left - m_CamPosition.x + actorShape.width };

	if (leftRelativePos < 0)
	{
		actorShape.left = m_CamPosition.x;
		velocity.x = 0.0f;
	}
	else if (rightRelativePos > m_Width / m_Zoom)
	{
		actorShape.left = m_CamPosition.x + m_Width / m_Zoom - actorShape.width;
		velocity.x = 0.0f;
	}
}

Point2f Camera::GetCenterPosition() const
{
	return Point2f
	{
		m_CamPosition.x + m_Width / m_Zoom / 2.0f,
		m_CamPosition.y + m_Height / m_Zoom
	};
}
