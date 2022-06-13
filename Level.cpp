#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "utils.h"
#include "SVGParser.h"
#include "Elevator.h"
#include <fstream>
#include "Camera.h"

Level::Level()
	: m_pBackgroundTexture{ nullptr }
	, m_pForegroundTexture{ nullptr }
{
}

Level::~Level()
{
	delete m_pBackgroundTexture;
	m_pBackgroundTexture = nullptr;
	delete m_pForegroundTexture;
	m_pForegroundTexture = nullptr;
}

void Level::DrawBackground() const
{
	m_pBackgroundTexture->Draw();
}

void Level::DrawForeground() const
{
	m_pForegroundTexture->Draw();
}

void Level::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, bool inverseVelocity)
{
	utils::HitInfo hitInfo{};
	
	const Point2f rayP1{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom + actorShape.height };
	const Point2f rayP2{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom };

	for (size_t svgIdx{ 0 }; svgIdx < m_Vertices.size(); ++svgIdx)
	{
		for (int i{ 0 }; i < 2; ++i)
		{
			const Point2f rayVerticalMiddle{ actorShape.left + actorShape.width * i + powf(-1.0f, float(i)), actorShape.bottom + actorShape.height / 2.0f };
			const Point2f rayVerticalTop{ actorShape.left + actorShape.width * i + powf(-1.0f, float(i)), actorShape.bottom + actorShape.height };
			const Point2f rayVerticalBottom{ actorShape.left + actorShape.width * i + powf(-1.0f, float(i)), actorShape.bottom };

			if (utils::Raycast(m_Vertices[svgIdx], rayVerticalMiddle, rayVerticalTop, hitInfo))
			{
				actorShape.bottom = hitInfo.intersectPoint.y - actorShape.height;

				if (inverseVelocity)
				{
					actorVelocity.y = -actorVelocity.y;
				}
				else
				{
					actorVelocity.y = 0;
				}
				break;
			}
			else if (utils::Raycast(m_Vertices[svgIdx], rayVerticalMiddle, rayVerticalBottom, hitInfo))
			{
				actorShape.bottom = hitInfo.intersectPoint.y;

				if (inverseVelocity)
				{
					actorVelocity.y = -actorVelocity.y;
				}
				else
				{
					actorVelocity.y = 0;
				}
				break;
			}
		}

		for (int i{ 0 }; i < 2; ++i)
		{
			const Point2f rayHorizontalMiddle{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom + actorShape.height * i + powf(-1.0f, float(i)) };
			const Point2f rayHorizontalP1{ actorShape.left, actorShape.bottom + actorShape.height * i + powf(-1.0f, float(i)) };
			const Point2f rayHorizontalP2{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height * i + powf(-1.0f,  float(i)) };

			if (utils::Raycast(m_Vertices[svgIdx], rayHorizontalMiddle, rayHorizontalP1, hitInfo))
			{
				actorShape.left = hitInfo.intersectPoint.x;

				if (inverseVelocity)
				{
					actorVelocity.x = -actorVelocity.x;
				}
				else
				{
					actorVelocity.x = 0;
				}
				break;
			}
			else if (utils::Raycast(m_Vertices[svgIdx], rayHorizontalMiddle, rayHorizontalP2, hitInfo))
			{
				actorShape.left = hitInfo.intersectPoint.x - actorShape.width;

				if (inverseVelocity)
				{
					actorVelocity.x = -actorVelocity.x;
				}
				else
				{
					actorVelocity.x = 0;
				}
				break;
			}
		}
	}
}

bool Level::IsOnGround(const Rectf& actorShape) const
{
	for (size_t i{ 0 }; i < m_Vertices.size(); ++i)
	{
		utils::HitInfo hitInfo{};

		const Point2f rayP1{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom + actorShape.height };
		const Point2f rayP2{ actorShape.left + actorShape.width / 2.0f, actorShape.bottom - 1 };

		if (utils::Raycast(m_Vertices[i], rayP1, rayP2, hitInfo))
		{
			return true;
		}
	}
	return false;
}

Rectf Level::GetBoundaries() const
{
	return m_Boundaries;
}

void Level::LoadLevelData(const std::string& filePath)
{
	if (m_pBackgroundTexture != nullptr)
	{
		delete m_pBackgroundTexture;
		m_pBackgroundTexture = nullptr;
	}
	if (m_pForegroundTexture != nullptr)
	{
		delete m_pForegroundTexture;
		m_pForegroundTexture = nullptr;
	}

	m_Vertices.clear();

	m_pBackgroundTexture = new Texture{ filePath + "background.png" };
	m_pForegroundTexture = new Texture{ filePath + "foreground.png" };
	SVGParser::GetVerticesFromSvgFile(filePath + "level.svg", m_Vertices);
	m_Boundaries = Rectf{ 0.0f, 0.0f, m_pForegroundTexture->GetWidth(), m_pForegroundTexture->GetHeight() };
}