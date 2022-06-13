#include "pch.h"
#include "MainMenu.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffect.h"
#include "LoadGame.h"
#include <array>
#include "SaveGame.h"

MainMenu::MainMenu(float windowWidth, float windowHeight)
	: m_pSelectionScreen{ new Texture{ "Resources/Images/Main Menu/GameSelect.png" } }
	, m_pRegisterScreen{ new Texture{ "Resources/Images/Main Menu/Register.png" } }
	, m_pElimationScreen{ new Texture{ "Resources/Images/Main Menu/Elimination.png" } }
	, m_pFairy{ new Texture{ "Resources/Images/Fairy.png" } }
	, m_pLetters{ new Texture{ "Resources/Images/Letters.png" } }
	, m_pMenuSelectSound{ new SoundEffect{ "Resources/Sounds/MenuSelect.mp3" } }
	, m_pLetterSelectSound{ new SoundEffect{ "Resources/Sounds/LetterSelect.mp3" } }
	, m_pLetterAddSound{ new SoundEffect{ "Resources/Sounds/LetterAdd.mp3" } }
	, m_pUserRemoveSound{ new SoundEffect{ "Resources/Sounds/UserRemove.mp3" } }
	, m_Zoom{ windowHeight / m_pSelectionScreen->GetHeight() }
	, m_WindowWidth{ windowWidth }
	, m_CurFairyPos{ 3 }
{
	const float distanceBetweenNames{ 24.0f };
	for (int i{ 0 }; i < 3; ++i)
	{
		m_SelectionFairyPositions[i] = Point2f
		{
			28.0f,
			159.0f - i * distanceBetweenNames
		};
	}
	m_SelectionFairyPositions[3] = Point2f{ 28.0f, 79.0f };
	m_SelectionFairyPositions[4] = Point2f{ 28.0f, 55.0f };

	for (int i{ 0 }; i < m_RegisterFairyNrPos; ++i)
	{
		m_RegisterFairyPositions[i] = Point2f
		{
			77.0f, 
			179.0f - i * distanceBetweenNames 
		};
	}

	LoadGame saveFile{};
	std::array<std::string, m_NrUsers> userNames{ saveFile.GetUsernames<m_NrUsers>() };
	int curName{};

	for (const std::string& userName : userNames)
	{
		m_Users[curName] = userName;
		m_SavedUsers[curName] = IsAName(m_Users[curName]);
		for (size_t nrLetter{ userName.size() }; nrLetter < m_MaxUserNameLength; ++nrLetter)
		{
			m_Users[curName] += ' ';
		}
		++curName;
	}
}

MainMenu::~MainMenu()
{
	SaveGame saveFile{};
	saveFile.SaveUsernames(m_Users, m_NrUsers);

	delete m_pSelectionScreen;
	m_pSelectionScreen = nullptr;
	delete m_pRegisterScreen;
	m_pRegisterScreen = nullptr;
	delete m_pElimationScreen;
	m_pElimationScreen = nullptr;
	delete m_pFairy;
	m_pFairy = nullptr;
	delete m_pLetters;
	m_pLetters = nullptr;
	delete m_pMenuSelectSound;
	m_pMenuSelectSound = nullptr;
	delete m_pLetterSelectSound;
	m_pLetterSelectSound = nullptr;
	delete m_pLetterAddSound;
	m_pLetterAddSound = nullptr;
	delete m_pUserRemoveSound;
	m_pUserRemoveSound = nullptr;
}

void MainMenu::Update(float elapsedSec)
{
	m_AccuFairySec += elapsedSec;
	if (m_AccuFairySec >= m_FairyFrameTime)
	{
		m_AccuFairySec -= m_FairyFrameTime;
		++m_CurFairyFrame %= 2;
	}

	m_AccuLetterSec += elapsedSec;
	if (m_AccuLetterSec >= m_LetterTime)
	{
		m_AccuLetterSec -= m_LetterTime;
		m_ShowLetter = !m_ShowLetter;
	}
}

void MainMenu::Select()
{
	switch (m_MenuState)
	{
	case MainMenu::MenuState::Selection:
		if (m_CurFairyPos == 3)
		{
			m_MenuState = MenuState::Register;
			ResetRegister();
		}
		else if (m_CurFairyPos  == 4)
		{
			m_MenuState = MenuState::Elimination;
			if (GetUserAmount() > 0)
			{
				m_CurFairyPos = 0;
			}
			else
			{
				m_CurFairyPos = 3;
			}
		}
		else if(m_CurFairyPos < 3)
		{
			m_IsDone = true;
		}
		break;
	case MainMenu::MenuState::Register:
		SubmitRegister();
		break;
	case MainMenu::MenuState::Elimination:
		SubmitElimination();
		break;
	}
}

void MainMenu::MoveButton(bool playSound)
{
	const int prevPos{ m_CurFairyPos };
	switch (m_MenuState)
	{
	case MainMenu::MenuState::Selection:
	{
		++m_CurFairyPos %= m_SelectionFairyNrPos;

		if (m_CurFairyPos < m_SelectionFairyNrPos - 2 && !IsAName(m_Users[m_CurFairyPos]))
		{
			MoveButton(false);
		}
		else if (GetUserAmount() == 0 && m_CurFairyPos == m_SelectionFairyNrPos - 1)
		{
			MoveButton(false);
		}
	}
	break;
	case MainMenu::MenuState::Register:
	{
		++m_CurFairyPos %= m_RegisterFairyNrPos;
		m_CurNamePos = 0;
		if (m_CurFairyPos < m_RegisterFairyNrPos - 1 && m_SavedUsers[m_CurFairyPos])
		{
			MoveButton(false);
		}
	}
	break;
	case MainMenu::MenuState::Elimination:
	{
		++m_CurFairyPos %= m_RegisterFairyNrPos;

		if (m_CurFairyPos < m_RegisterFairyNrPos - 1 && !IsAName(m_Users[m_CurFairyPos]))
		{
			MoveButton(false);
		}
	}
	}

	if (playSound && prevPos != m_CurFairyPos)
	{
		m_pMenuSelectSound->Play(0);
	}
}

void MainMenu::MoveLetter(int x, int y)
{
	if (m_MenuState != MenuState::Register)
	{
		return;
	}
	if (m_CurFairyPos == m_RegisterFairyNrPos - 1)
	{
		return;
	}

	(m_CurLetter += x - y * m_NrLettersPerRow) %= ('Z'-'A' + 2);
	if (m_CurLetter < 0)
	{
		m_CurLetter = ('Z' - 'A' + 1);
	}
	m_pLetterSelectSound->Play(0);
}

void MainMenu::AddSpace()
{
	if (m_MenuState != MenuState::Register)
	{
		return;
	}
	if (m_CurFairyPos == m_RegisterFairyNrPos - 1)
	{
		return;
	}
	++m_CurNamePos %= m_MaxUserNameLength;
	m_pLetterAddSound->Play(0);
}

void MainMenu::Reset()
{
	m_CurFairyPos = 3;
	m_IsDone = false;
}

void MainMenu::SetVolume(int volume)
{
	m_pMenuSelectSound->SetVolume(volume);
	m_pLetterSelectSound->SetVolume(volume);
	m_pLetterAddSound->SetVolume(volume);
	m_pUserRemoveSound->SetVolume(volume);
}

void MainMenu::Draw() const
{
	glPushMatrix();
	{
		glTranslatef(m_WindowWidth / 2.0f - m_pSelectionScreen->GetWidth() * m_Zoom / 2.0f, 0.0f, 0.0f);
		glScalef(m_Zoom, m_Zoom, 1.0f);
		switch (m_MenuState)
		{
		case MainMenu::MenuState::Selection:
			DrawSelectionScreen();
			break;
		case MainMenu::MenuState::Register:
			DrawRegisterScreen();
			break;
		case MainMenu::MenuState::Elimination:
			DrawEliminationScreen();
			break;
		}
	}
	glPopMatrix();
}

bool MainMenu::IsDone() const
{
	return m_IsDone;
}

const std::string& MainMenu::GetCurrentUser() const
{
	return m_Users[m_CurFairyPos];
}

int MainMenu::GetUserAmount() const
{
	int size{};
	for (int i{ 0 }; i < m_NrUsers; ++i)
	{
		if (IsAName(m_Users[i]))
		{
			++size;
		}
	}
	return size;
}

bool MainMenu::IsAName(const std::string& name) const
{
	for (int i{ 0 }; i < name.size(); ++i)
	{
		if (name[i] != ' ')
		{
			return true;
		}
	}
	return false;
}

void MainMenu::ResetRegister()
{
	m_CurNamePos = 0;
	if (GetUserAmount() > 0)
	{
		for (int i{ 0 }; i < m_NrUsers; ++i)
		{
			if (!IsAName(m_Users[i]))
			{
				m_CurFairyPos = i;
				break;
			}
		}
	}
	else
	{
		m_CurFairyPos = 0;
	}
}

void MainMenu::SubmitRegister()
{
	switch (m_CurFairyPos)
	{
	case m_RegisterFairyNrPos - 1:
		m_MenuState = MenuState::Selection;
		m_CurFairyPos = 3;
		for (int i{ 0 }; i < m_NrUsers; ++i)
		{
			if (!m_SavedUsers[i] && IsAName(m_Users[i]))
			{
				m_SavedUsers[i] = true;
			}
		}
		break;
	default:
		if (m_CurLetter > 'Z' - 'A')
		{
			m_Users[m_CurFairyPos][m_CurNamePos] = ' ';
		}
		else
		{
			m_Users[m_CurFairyPos][m_CurNamePos] = 'A' + m_CurLetter;
		}
		++m_CurNamePos %= m_MaxUserNameLength;
		m_pLetterAddSound->Play(0);
	}
}

void MainMenu::SubmitElimination()
{
	switch (m_CurFairyPos)
	{
	case m_RegisterFairyNrPos - 1:
		m_MenuState = MenuState::Register;
		m_CurNamePos = 0;
		if (GetUserAmount() > 0)
		{
			for (int i{ 0 }; i < m_NrUsers; ++i)
			{
				if (!IsAName(m_Users[i]))
				{
					m_CurFairyPos = i;
					break;
				}
			}
		}
		else
		{
			m_CurFairyPos = 0;
		}
		break;
	default:
		m_Users[m_CurFairyPos] = "";
		for (int i{ 0 }; i < m_MaxUserNameLength; ++i)
		{
			m_Users[m_CurFairyPos] += ' ';
		}
		m_SavedUsers[m_CurFairyPos] = false;
		MoveButton(false);
		m_pUserRemoveSound->Play(0);
	}
}

void MainMenu::DrawSelectionScreen() const
{
	const float letterSize{ 8.0f };
	const float fairyWidth{ m_pFairy->GetWidth() / 2.0f };
	const float fairyHeight{ m_pFairy->GetHeight() };
	const Rectf fairySrcRect
	{
		fairyWidth * m_CurFairyFrame,
		fairyHeight,
		fairyWidth, fairyHeight
	};
	const Point2f leftBottom{ 0.0f, 0.0f };

	m_pSelectionScreen->Draw(leftBottom);
	m_pFairy->Draw(m_SelectionFairyPositions[m_CurFairyPos], fairySrcRect);

	for (size_t i{ 0 }; i < m_NrUsers; ++i)
	{
		const float distanceBetweenNames{ 24.0f };
		const Point2f startPos
		{
			57.0f,
			168.0f - distanceBetweenNames * i
		};

		for (size_t letter{ 0 }; letter < m_Users[i].size(); ++letter)
		{
			if (m_Users[i][letter] == ' ')
			{
				continue;
			}
			const Point2f letterPos
			{
				startPos.x + letter * letterSize,
				startPos.y
			};
			const Rectf letterSrcRect
			{
				int(m_Users[i][letter] - 'A') * letterSize,
				letterSize,
				letterSize,
				letterSize
			};
			m_pLetters->Draw(letterPos, letterSrcRect);
		}
	}
}

void MainMenu::DrawEliminationScreen() const
{
	const float fairyWidth{ m_pFairy->GetWidth() / 2.0f };
	const float fairyHeight{ m_pFairy->GetHeight() };
	const Rectf fairySrcRect
	{
		fairyWidth * m_CurFairyFrame,
		fairyHeight,
		fairyWidth, fairyHeight
	};
	const Point2f leftBottom{ 0.0f, 0.0f };

	m_pElimationScreen->Draw(leftBottom);
	m_pFairy->Draw(m_RegisterFairyPositions[m_CurFairyPos], fairySrcRect);
	for (size_t i{ 0 }; i < m_NrUsers; ++i)
	{
		const Point2f startPos
		{
			105.0f,
			184.0f - 24.0f * i
		};
		for (size_t letter{ 0 }; letter < m_Users[i].size(); ++letter)
		{
			if (m_Users[i][letter] == ' ')
			{
				continue;
			}
			const Point2f letterPos
			{
				startPos.x + letter * 8.0f,
				startPos.y
			};
			const Rectf letterSrcRect
			{
				int(m_Users[i][letter] - 'A') * 8.0f,
				8.0f,
				8.0f,
				8.0f
			};
			m_pLetters->Draw(letterPos, letterSrcRect);
		}
	}
}

void MainMenu::DrawRegisterScreen() const
{
	const float letterSize{ 8.0f };
	const float fairyWidth{ m_pFairy->GetWidth() / 2.0f };
	const float fairyHeight{ m_pFairy->GetHeight() };
	const Rectf fairySrcRect
	{
		fairyWidth * m_CurFairyFrame,
		fairyHeight,
		fairyWidth, fairyHeight
	};
	const Point2f firstLetterPos
	{
		48.0f,
		88.0f
	};
	const int letterY{ m_CurLetter / m_NrLettersPerRow };
	const int letterX{ m_CurLetter - letterY * m_NrLettersPerRow };
	const float distanceBetweenLetters{ 16.0f };
	const Point2f leftBottom{ 0.0f, 0.0f };

	m_pRegisterScreen->Draw(leftBottom);
	m_pFairy->Draw(m_RegisterFairyPositions[m_CurFairyPos], fairySrcRect);

	if (m_ShowLetter && m_CurFairyPos != m_RegisterFairyNrPos - 1)
	{
		const Point2f curLetterPos
		{
			firstLetterPos.x + letterX * distanceBetweenLetters,
			firstLetterPos.y - letterY * distanceBetweenLetters
		};

		utils::SetColor(Color4f{ 0.91f, 0.62f, 0.13f, 1.0f });
		utils::FillRect(curLetterPos, letterSize, letterSize);
	}
	for (size_t i{ 0 }; i < m_NrUsers; ++i)
	{
		const Point2f startPos
		{
			105.0f,
			184.0f - 24.0f * i
		};

		for (size_t letter{ 0 }; letter < m_Users[i].size(); ++letter)
		{
			if (m_Users[i][letter] == ' ')
			{
				continue;
			}
			const Point2f letterPos
			{
				startPos.x + letter * 8.0f,
				startPos.y
			};
			const Rectf letterSrcRect
			{
				int(m_Users[i][letter] - 'A') * 8.0f,
				8.0f,
				8.0f,
				8.0f
			};
			m_pLetters->Draw(letterPos, letterSrcRect);
		}

		if (m_ShowLetter && i == m_CurFairyPos)
		{
			const Point2f curNameLetterPos
			{
				startPos.x + m_CurNamePos * letterSize,
				startPos.y
			};
			utils::SetColor(Color4f{ 0.91f, 0.62f, 0.13f, 1.0f });
			utils::FillRect(curNameLetterPos, letterSize, letterSize);
		}
	}
}
