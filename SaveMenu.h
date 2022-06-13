#pragma once
class Texture;
class SoundEffect;

class SaveMenu final
{
public:
	SaveMenu(float windowWidth, float windowHeight);
	SaveMenu(const SaveMenu& other) = delete;
	SaveMenu(SaveMenu&& other) = delete;
	SaveMenu& operator=(const SaveMenu& other) = delete;
	SaveMenu& operator=(SaveMenu&& other) = delete;
	~SaveMenu();

	void MoveButton();
	void Reset();
	void SetVolume(int volume, bool playSound);

	bool Select() const;
	void Draw(int volume) const;
private:
	Texture* m_pBackground;
	Texture* m_pBottleTexture;
	Texture* m_pVolumeBarTexture;
	SoundEffect* m_pMoveButtonSound;

	int m_CurButton{};

	const float m_WindowWidth;
	const float m_Zoom;
};

