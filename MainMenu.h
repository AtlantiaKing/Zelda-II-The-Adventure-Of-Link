#pragma once
#include <vector>

class Texture;
class SoundEffect;

class MainMenu final
{
public:
	MainMenu(float windowWidth, float windowHeight);
	MainMenu(const MainMenu& other) = delete;
	MainMenu(MainMenu&& other) = delete;
	~MainMenu();
	MainMenu& operator=(const MainMenu& other) = delete;
	MainMenu& operator=(MainMenu&& other) = delete;

	void Update(float elapsedSec);
	void Select();
	void MoveButton(bool playSound = true);
	void MoveLetter(int x, int y);
	void AddSpace();
	void Reset();
	void SetVolume(int volume);

	void Draw() const;
	bool IsDone() const;
	const std::string& GetCurrentUser() const;
private:
	enum class MenuState
	{
		Selection,
		Register,
		Elimination
	};

	Texture* m_pSelectionScreen;
	Texture* m_pRegisterScreen;
	Texture* m_pElimationScreen;
	Texture* m_pFairy;
	Texture* m_pLetters;
	SoundEffect* m_pMenuSelectSound;
	SoundEffect* m_pLetterSelectSound;
	SoundEffect* m_pLetterAddSound;
	SoundEffect* m_pUserRemoveSound;

	float m_AccuFairySec{};
	float m_AccuLetterSec{};
	int m_CurFairyFrame{};
	bool m_IsDone{};
	MenuState m_MenuState{ MenuState::Selection };

	const static int m_NrUsers{ 3 };
	const int m_MaxUserNameLength{ 8 };
	std::string m_Users[m_NrUsers];
	bool m_SavedUsers[m_NrUsers];

	int m_CurFairyPos{};
	bool m_ShowLetter{};
	int m_CurLetter{};
	int m_CurNamePos{};
	static const int m_SelectionFairyNrPos{ 5 };
	Point2f m_SelectionFairyPositions[m_SelectionFairyNrPos];
	static const int m_RegisterFairyNrPos{ 4 };
	Point2f m_RegisterFairyPositions[m_RegisterFairyNrPos];

	const int m_NrLettersPerRow{ 11 };
	const float m_LetterTime{ 0.15f };
	const float m_FairyFrameTime{ 0.07f };
	const float m_Zoom;
	const float m_WindowWidth;

	void ResetRegister();
	void SubmitRegister();
	void SubmitElimination();

	void DrawSelectionScreen() const;
	void DrawRegisterScreen() const;
	void DrawEliminationScreen() const;
	int GetUserAmount() const;
	bool IsAName(const std::string& name) const;
};