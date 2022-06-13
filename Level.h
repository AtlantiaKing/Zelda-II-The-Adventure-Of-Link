#pragma once
#include "Vector2f.h"
#include "enums.h"
#include <vector>

class Texture;

class Level final
{
public:
	Level();
	Level(const Level& other) = delete;
	Level(Level&& other) = delete;
	~Level();
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) = delete;

	void LoadLevelData(const std::string& filePath);
	void HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, bool inverseVelocity = false);

	void DrawBackground() const;
	void DrawForeground() const;
	bool IsOnGround(const Rectf& actorShape) const;
	Rectf GetBoundaries() const;
private:
	std::vector<std::vector<Point2f>> m_Vertices;
	Texture* m_pBackgroundTexture;
	Texture* m_pForegroundTexture;
	Rectf m_Boundaries;
	int m_Exits[int(EntryType::down) + 1]{};
};

