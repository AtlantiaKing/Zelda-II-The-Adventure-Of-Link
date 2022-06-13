#include "pch.h"
#include <iostream>

#include "Game.h"
#include "TitleScreen.h"
#include "SaveMenu.h"
#include "utils.h"
#include "Level.h"
#include "Avatar.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "SwordBeam.h"
#include "SoundStream.h"
#include "ParticleManager.h"
#include "GameObjectManager.h"
#include "EnemyManager.h"
#include "LevelLoader.h"
#include "XPManager.h"
#include "Texture.h"
#include "Hud.h"
#include "ColorTwitch.h"
#include "DeathScreen.h"
#include "InventoryHUD.h"
#include "HorseheadHUD.h"
#include "MainMenu.h"
#include "SoundEffectManager.h"
#include "LoadGame.h"
#include "Credits.h"

#include <fstream>
#include "SaveGame.h"

Game::Game( const Window& window )
	: m_Window{ window }
	, m_pTitleScreen{ new TitleScreen{ window.width, window.height } }
	, m_pMainMenu{ new MainMenu{ window.width, window.height } }
	, m_pXPManager{ new XPManager{} }
	, m_pLevel{ new Level{} }
	, m_pAvatar{ new Avatar{ m_pXPManager } }
	, m_pCamera{ new Camera{ m_Window.width, m_Window.height } }
	, m_pSwordBeam{ new SwordBeam{} }
	, m_pParticleManager{ new ParticleManager{} }
	, m_pGameObjectManager{ new GameObjectManager{} }
	, m_pColorTwitch{ new ColorTwitch{} }
	, m_pDeathScreen{ new DeathScreen{}  }
	, m_InventoryHUD{ new InventoryHUD{} }
	, m_HorseheadHUD{ new HorseheadHUD{} }
	, m_pSoundEffectManager{ new SoundEffectManager{} }
	, m_pSaveMenu{ new SaveMenu{ m_Window.width, m_Window.height } }
	, m_pHud{ new Hud{ Point2f{ 0.0f, m_Window.height }, m_Window.width }}
	, m_pCredits{ new Credits{ m_Window.width, m_Window.height } }
{
	m_pEnemyManager = new EnemyManager{ m_pParticleManager, m_pXPManager };
	m_pLevelLoader = new LevelLoader(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager);

	Initialize( );
}

Game::~Game( )
{
	Cleanup();
}

void Game::Initialize( )
{
	m_pCamera->SetZoom(m_Window, m_pLevel->GetBoundaries());

	const std::string mainMusicPath{ "Resources/Sounds/titlescreen.ogg" };
	m_pMusic = new SoundStream{ mainMusicPath };

	if (m_pMusic->IsLoaded())
	{
		m_pMusic->Play(true);
	}

	LoadVolume();
}

void Game::Cleanup()
{
	SaveVolume();

	delete m_pLevel;
	delete m_pAvatar;
	delete m_pCamera;
	delete m_pSwordBeam;
	delete m_pMusic;
	delete m_pParticleManager;
	delete m_pGameObjectManager;
	delete m_pEnemyManager;
	delete m_pXPManager;
	delete m_pLevelLoader;
	delete m_pHud;
	delete m_pTitleScreen;
	delete m_pMainMenu;
	delete m_pColorTwitch;
	delete m_pDeathScreen;
	delete m_InventoryHUD;
	delete m_HorseheadHUD;
	delete m_pSoundEffectManager;
	delete m_pSaveMenu;
	delete m_pCredits;
}

void Game::Update( float elapsedSec )
{
	switch (m_GameState)
	{
	case GameState::TitleScreen:
		m_pTitleScreen->Update(elapsedSec);
		break;
	case GameState::MainMenu:
		m_pMainMenu->Update(elapsedSec);
		break;
	case GameState::InGame:
		UpdateGame(elapsedSec);
		break;
	case GameState::Credits:
		m_pCredits->Update(elapsedSec);
		break;
	}
}

void Game::UpdateGame(float elapsedSec)
{
	m_pColorTwitch->SetActive(m_pAvatar->IsDead() || m_pAvatar->IsPickingUpItem() || m_pEnemyManager->IsBossBattleCompleted());
	m_pDeathScreen->SetActive(m_pAvatar->IsDead());

	m_pColorTwitch->Update(elapsedSec);

	m_pSoundEffectManager->Update();

	if (m_pSoundEffectManager->IsPlaying(SoundEffectManager::SoundType::ItemPickup) || m_pEnemyManager->IsBossBattleCompleted())
	{
		m_pMusic->Pause();
	}
	else
	{
		m_pMusic->Resume();

		const bool newBossState{ m_pEnemyManager->IsInBossBattle() };
		if (newBossState)
		{
			if (!m_IsInBossBattle)
			{
				const std::string musicPath{ "Resources/Sounds/bossmusic.ogg" };
				delete m_pMusic;
				m_pMusic = new SoundStream{ musicPath };
				m_pMusic->Play(true);
			}
		}
		else
		{
			if (m_IsInBossBattle)
			{
				const std::string musicPath{ "Resources/Sounds/dungeon.ogg" };
				delete m_pMusic;
				m_pMusic = new SoundStream{ musicPath };
				m_pMusic->Play(true);
			}
		}
		m_IsInBossBattle = newBossState;
	}

	if (m_pAvatar->IsDead())
	{
		if (m_pMusic->IsPlaying())
		{
			m_pMusic->Stop();
		}
		m_pDeathScreen->Update(elapsedSec, m_pAvatar->GetRemainingLifes(), m_pSoundEffectManager);

		m_AccuSec += elapsedSec;
		if (m_AccuSec > m_RespawnTime)
		{
			if (m_pAvatar->GetRemainingLifes() <= 0)
			{
				m_GameState = GameState::SaveMenu;
			}
			ResetCurrentMode();
		}
		return;
	}

	if (m_pXPManager->IsUpgrading())
	{
		m_pXPManager->Update(elapsedSec, m_pSoundEffectManager);
		return;
	}

	// Update game action
	if (!m_InventoryHUD->IsActive())
	{
		if (!m_pGameObjectManager->IsGameDone())
		{
			m_pAvatar->Update(elapsedSec, m_pLevel, m_pCamera, m_pGameObjectManager, m_pEnemyManager, m_pXPManager, m_pSoundEffectManager);
		}
		else
		{
			m_pMusic->Stop();
		}

		m_pEnemyManager->Update(elapsedSec, m_pAvatar->GetShape(), m_pAvatar->IsLookingRight(), m_pLevel, m_pCamera, m_pGameObjectManager, m_pSoundEffectManager);

		if (!m_pEnemyManager->IsInBossBattle())
		{
			m_pCamera->Update(m_pAvatar->GetShape(), m_pLevel->GetBoundaries());
		}

		m_pGameObjectManager->Update(elapsedSec, m_pLevel, m_pCamera, m_pSoundEffectManager);

		m_pSwordBeam->Update(elapsedSec, m_pEnemyManager, m_pSoundEffectManager);
		m_pParticleManager->Update(elapsedSec);

		m_pLevelLoader->Update(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager);

		m_pXPManager->Update(elapsedSec, m_pSoundEffectManager);
	}

	if (m_pGameObjectManager->IsGemInserted())
	{
		m_GameState = GameState::Credits;
		ResetCurrentMode();
	}
}

void Game::HandleReturnButton()
{
	switch (m_GameState)
	{
	case Game::GameState::TitleScreen:
		m_GameState = GameState::MainMenu;
		ResetCurrentMode();
		break;
	case Game::GameState::MainMenu:
		m_pMainMenu->Select();
		if (m_pMainMenu->IsDone())
		{
			m_GameState = GameState::InGame;
			ResetCurrentMode();
		}
		break;
	case Game::GameState::InGame:
		m_pXPManager->Select(m_pAvatar, m_pSoundEffectManager);
		break;
	case Game::GameState::SaveMenu:
	{
		bool hasSaved{ m_pSaveMenu->Select() };
		if (hasSaved)
		{
			if (m_pAvatar->IsDead())
			{
				m_pLevelLoader->LoadLevel(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager, 0, EntryType::right);
				m_pAvatar->SetLifes(3);
				m_pEnemyManager->ResetEnemies();
				m_pXPManager->SetData(0, m_pXPManager->GetLevel(XPManager::Stat::Attack), m_pXPManager->GetLevel(XPManager::Stat::Magic), m_pXPManager->GetLevel(XPManager::Stat::Health));
			}
			SaveGame saveFile{};
			saveFile.SaveUser(m_pMainMenu->GetCurrentUser(), m_pAvatar, m_pGameObjectManager, m_pEnemyManager, m_pXPManager, m_pLevelLoader->GetChunkID(), m_pLevelLoader->GetLastEntryType());
			m_GameState = GameState::TitleScreen;
			m_IsInGame = false;
			ResetCurrentMode();
		}
		else
		{
			if (m_pAvatar->IsDead())
			{
				m_GameState = GameState::InGame;
				m_pLevelLoader->LoadLevel(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager, 0, EntryType::right);
				m_pAvatar->SetLifes(3);
				m_pEnemyManager->ResetEnemies();
				m_pXPManager->SetData(0, m_pXPManager->GetLevel(XPManager::Stat::Attack), m_pXPManager->GetLevel(XPManager::Stat::Magic), m_pXPManager->GetLevel(XPManager::Stat::Health));
			}
			else
			{
				m_GameState = GameState::InGame;
			}
			m_pMusic->Resume();
		}
		break;
	}
	case Game::GameState::Credits:
	{
		m_GameState = GameState::TitleScreen;
		ResetCurrentMode();
		break;
	}
	}
}

void Game::ResetCurrentMode()
{
	m_AccuSec = 0.0f;
	switch (m_GameState)
	{
	case Game::GameState::TitleScreen:
	{
		const std::string mainMusicPath{ "Resources/Sounds/titlescreen.ogg" };
		delete m_pMusic;
		m_pMusic = new SoundStream{ mainMusicPath };
		m_pMusic->Play(true);
		m_pTitleScreen->Reset();
		break;
	}
	case Game::GameState::MainMenu:
		m_pMainMenu->Reset();
		m_pMusic->Stop();
		break;
	case Game::GameState::InGame:
	{
		const std::string musicPath{ "Resources/Sounds/dungeon.ogg" };
		delete m_pMusic;
		m_pMusic = new SoundStream{ musicPath };
		m_pMusic->Play(true);

		if (m_IsInGame)
		{
			m_pLevelLoader->LoadLevel(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager);
			m_pAvatar->Respawn();
		}
		else
		{
			LoadGame saveFile{};
			bool foundSavedUser{ saveFile.LoadUser(m_pMainMenu->GetCurrentUser(), m_pLevelLoader, m_pXPManager, m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager) };
			if (!foundSavedUser)
			{
				m_pLevelLoader->LoadLevel(m_pLevel, m_pAvatar, m_pGameObjectManager, m_pEnemyManager, true);
				m_pAvatar->Reset(m_pXPManager);
				m_pXPManager->Reset();
			}
		}

		if (m_InventoryHUD->IsActive())
		{
			m_InventoryHUD->Toggle(m_pSoundEffectManager);
		}

		m_IsInGame = true;
	}
		break;
	case Game::GameState::SaveMenu:
	{
		m_pSaveMenu->Reset();
		break;
	}
	case Game::GameState::Credits:
	{
		m_IsInGame = false;
		m_pCredits->Reset();
		const std::string musicPath{ "Resources/Sounds/Ending.mp3" };
		delete m_pMusic;
		m_pMusic = new SoundStream{ musicPath };
		m_pMusic->Play(true);
		break;
	}
	}
}

void Game::LoadVolume()
{
	std::ifstream volumeFile{ "Resources/SaveFiles/Volume.txt" };
	if (volumeFile)
	{
		int volume{};
		volumeFile >> volume;
		m_Volume = volume;
	}
	volumeFile.close();

	m_pMusic->SetVolume(m_Volume);
	m_pSoundEffectManager->SetVolume(m_Volume);
	m_pMainMenu->SetVolume(m_Volume);
	m_pSaveMenu->SetVolume(m_Volume, false);
}

void Game::SaveVolume()
{
	std::ofstream volumeFile{ "Resources/SaveFiles/Volume.txt" };
	if (volumeFile)
	{
		volumeFile << std::to_string(m_Volume);
	}
	volumeFile.close();
}

void Game::Draw( ) const
{
	ClearBackground( );

	switch (m_GameState)
	{
	case GameState::TitleScreen:
		m_pTitleScreen->Draw();
		break;
	case GameState::MainMenu:
		m_pMainMenu->Draw();
		break;
	case GameState::InGame:
		DrawGame();
		break;
	case GameState::SaveMenu:
		m_pSaveMenu->Draw(m_Volume);
		break;
	case GameState::Credits:
		DrawGame();
		m_pCredits->Draw();
		break;
	}
}

void Game::DrawGame() const
{
	if (m_pAvatar->IsDead())
	{
		m_pColorTwitch->Draw(m_Window.width, m_Window.height);
		m_pDeathScreen->Draw(m_Window.width, m_Window.height, m_pAvatar->GetRemainingLifes(), m_pMainMenu->GetCurrentUser());
	}
	else
	{
		glPushMatrix();
		{
			m_pCamera->Scale();
			glPushMatrix();
			{
				// Camera's transform
				m_pCamera->Translate();

				glPushMatrix();
				{
					// Paralax transform
					m_pCamera->TranslateParalax();

					// Draw Foreground
					m_pLevel->DrawBackground();
				}
				glPopMatrix();
			}
			glPopMatrix();

			m_pColorTwitch->Draw(m_Window.width, m_Window.height);

			glPushMatrix();
			{
				// Camera's transform
				m_pCamera->Translate();

				// Draw Foreground
				m_pLevel->DrawForeground();

				if (!m_InventoryHUD->IsActive())
				{
					// GameObjects
					m_pGameObjectManager->Draw();

					// Enemies
					m_pEnemyManager->Draw();

					// Particles
					m_pParticleManager->Draw();

					// Actor
					m_pAvatar->Draw();

					// Sword beam
					m_pSwordBeam->Draw();
				}
			}
			glPopMatrix();

			// Level Up Screen
			m_pXPManager->Draw();

			if (m_pEnemyManager->IsInBossBattle())
			{
				m_HorseheadHUD->Draw(m_pEnemyManager->GetBossHP());
			}
		}
		glPopMatrix();

		// HUD
		m_pHud->Draw(m_pAvatar->GetMaxHealth(), m_pAvatar->GetHealthPercentage(), m_pXPManager);

		m_InventoryHUD->Draw(m_pAvatar->GetRemainingLifes(), m_pAvatar->GetKeysAmount(), m_pAvatar->HasTakenCandle(), m_pCamera->GetZoom());
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	if (m_GameState == GameState::InGame)
	{
		if (e.keysym.sym == SDLK_LCTRL || e.keysym.sym == SDLK_RCTRL)
		{
			m_pAvatar->Attack(m_pSwordBeam, m_pSoundEffectManager);
		}
	}
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_i:
	{
		std::cout << "Zelda II: The Adventure of Link - Sander De Keukelaere 1DAE21N" << std::endl;
		std::cout << "-- Menu Controls --" << std::endl;
		std::cout << "Enter : Submit" << std::endl;
		std::cout << "Tab : Move cursor" << std::endl;
		std::cout << "Arrow keys : Move letter cursor" << std::endl;
		std::cout << "-- Game Controls --" << std::endl;
		std::cout << "Q/A and D / Left and right arrows : Move left and right" << std::endl;
		std::cout << "S / Down arrow : Crouch" << std::endl;
		std::cout << "Space : Jump" << std::endl;
		std::cout << "Left or right control : Attack/Shoot beam" << std::endl;
		std::cout << "Tab : Open inventory" << std::endl;
		std::cout << "Escape / P : Open pause menu" << std::endl;
	}
	case SDLK_UP:
		if (m_GameState == GameState::InGame)
		{
			m_pXPManager->MoveSelectionUp(m_pSoundEffectManager);
		}
		else if (m_GameState == GameState::MainMenu)
		{
			m_pMainMenu->MoveLetter(0, 1);
		}
		break;
	case SDLK_DOWN:
		if (m_GameState == GameState::InGame)
		{
			m_pXPManager->MoveSelectionDown(m_pSoundEffectManager);
		}
		else if (m_GameState == GameState::MainMenu)
		{
			m_pMainMenu->MoveLetter(0, -1);
		}
		break;
	case SDLK_LEFT:
		if (m_GameState == GameState::MainMenu)
		{
			m_pMainMenu->MoveLetter(-1, 0);
		}
		else if (m_GameState == GameState::SaveMenu)
		{
			const int maxVolume{ 120 };
			const int volumeSteps{ 10 };
			m_Volume -= maxVolume / volumeSteps;
			if (m_Volume < 0)
			{
				m_Volume = 0;
			}
			m_pMusic->SetVolume(m_Volume);
			m_pSoundEffectManager->SetVolume(m_Volume);
			m_pMainMenu->SetVolume(m_Volume);
			m_pSaveMenu->SetVolume(m_Volume, true);
		}
		break;
	case SDLK_RIGHT:
		if (m_GameState == GameState::MainMenu)
		{
			m_pMainMenu->MoveLetter(1, 0);
		}
		else if (m_GameState == GameState::SaveMenu)
		{
			const int maxVolume{ 120 };
			const int volumeSteps{ 10 };
			m_Volume += maxVolume / volumeSteps;
			if (m_Volume > maxVolume - 1)
			{
				m_Volume = maxVolume - 1;
			}
			m_pMusic->SetVolume(m_Volume);
			m_pSoundEffectManager->SetVolume(m_Volume);
			m_pMainMenu->SetVolume(m_Volume);
			m_pSaveMenu->SetVolume(m_Volume, true);
		}
		break;
	case SDLK_RETURN:
		HandleReturnButton();
		break;
	case SDLK_p:
	case SDLK_ESCAPE:
		if (m_GameState == GameState::InGame && !m_pAvatar->IsDead())
		{
			m_GameState = GameState::SaveMenu;
			ResetCurrentMode();
			m_pMusic->Pause();
		}
		break;
	case SDLK_TAB:
			switch (m_GameState)
		{
		case Game::GameState::MainMenu:
			m_pMainMenu->MoveButton();
			break;
		case Game::GameState::InGame:
			if (!m_pXPManager->IsUpgrading())
			{
				m_InventoryHUD->Toggle(m_pSoundEffectManager);
			}
			break;
		case Game::GameState::SaveMenu:
			m_pSaveMenu->MoveButton();
			break;
		}
		break;
	case SDLK_SPACE:
		if (m_GameState == GameState::MainMenu)
		{
			m_pMainMenu->AddSpace();
		}
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}