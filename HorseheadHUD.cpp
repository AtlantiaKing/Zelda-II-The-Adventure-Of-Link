#include "pch.h"
#include "HorseheadHUD.h"
#include "utils.h"

void HorseheadHUD::Draw(int health) const
{
	const Point2f startPos
	{
		16.0f,
		112.0f
	};

	const int maxHealth{ 8 };
	const float healthBlockWidth{ 8.0f };
	const float healthBlockHeight{ 5.0f };
	const float healthBlockSpacing{ 3.0f };

	for (int i{ 0 }; i < maxHealth; ++i)
	{

		const Rectf dstRect
		{
			startPos.x,
			startPos.y + i * (healthBlockHeight + healthBlockSpacing),
			healthBlockWidth,
			healthBlockHeight
		};

		if (i < health)
		{
			utils::SetColor(Color4f{ 0.84f, 0.152f, 0.0f, 1.0f });
		}
		else
		{
			utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		utils::FillRect(dstRect);
	}
}
