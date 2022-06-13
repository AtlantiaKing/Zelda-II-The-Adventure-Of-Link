#pragma once

#include "Vector2f.h"

class TitleScreen;
class MainMenu;
class Level;
class Avatar;
class Camera;
class SwordBeam;
class SoundStream;
class ParticleManager;
class GameObjectManager;
class EnemyManager;
class LevelLoader;
class XPManager;
class Hud;
class ColorTwitch;
class DeathScreen;
class HorseheadHUD;
class SoundEffectManager;
class SaveMenu;
class InventoryHUD;
class Credits;

class Game final
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;

	void DrawGame() const;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	enum class GameState
	{
		TitleScreen,
		MainMenu,
		InGame,
		SaveMenu,
		Credits
	};
	// DATA MEMBERS
	const Window m_Window;

	TitleScreen* m_pTitleScreen;
	MainMenu* m_pMainMenu;

	SaveMenu* m_pSaveMenu;

	SoundEffectManager* m_pSoundEffectManager;
	XPManager* m_pXPManager;
	SoundStream* m_pMusic;
	Level *m_pLevel;
	Avatar* m_pAvatar;
	Camera* m_pCamera;
	SwordBeam* m_pSwordBeam;
	ParticleManager* m_pParticleManager;
	GameObjectManager* m_pGameObjectManager;
	EnemyManager* m_pEnemyManager;
	LevelLoader* m_pLevelLoader;
	Hud* m_pHud;
	ColorTwitch* m_pColorTwitch;
	DeathScreen* m_pDeathScreen;
	InventoryHUD* m_InventoryHUD;
	HorseheadHUD* m_HorseheadHUD;

	Credits* m_pCredits;

	float m_AccuSec{};
	const float m_RespawnTime{ 5.0f };
	int m_ChunkID{};
	GameState m_GameState{ GameState::TitleScreen };
	bool m_IsInBossBattle{ false };
	bool m_IsInGame{ false };
	int m_Volume{ 119 };

	// FUNCTIONS
	void Initialize( );
	void Cleanup();
	void ClearBackground( ) const;
	void UpdateGame(float elapsedSec);
	void HandleReturnButton();
	void ResetCurrentMode();
	void LoadVolume();
	void SaveVolume();
};