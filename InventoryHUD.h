#pragma once

class Texture;
class SoundEffectManager;

class InventoryHUD final
{
public:
	InventoryHUD();
	InventoryHUD(const InventoryHUD& other) = delete;
	InventoryHUD(InventoryHUD&& other) = delete;
	InventoryHUD& operator= (const InventoryHUD& other) = delete;
	InventoryHUD& operator= (InventoryHUD&& other) = delete;
	~InventoryHUD();

	void Toggle(SoundEffectManager* pSoundEffectManager);
	bool IsActive();

	void Draw(int lifes, int keys, bool hasCandle, float zoom) const;
private:
	Texture* m_pTexture;
	Texture* m_pNumbers;
	Texture* m_pCandleTexture;

	bool m_IsActive{};
};

