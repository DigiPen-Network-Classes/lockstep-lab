#include "framework.h"
#include "GameStateManager.h"
#include "MainMenuState.h"
#include "NetworkConfiguringState.h"
#include "SinglePlayerScenarioState.h"
#include "LockstepScenarioState.h"
#include "DumbClientScenarioState.h"
#include "OptimisticScenarioState.h"


/// <summary>
/// The size of the description text.
/// </summary>
const float kMenuOptionTextSize = 30.0f;

/// <summary>
/// The color of the description text.
/// </summary>
const CP_Color kMenuOptionTextColor = CP_Color_Create(255, 255, 255, 255);


MainMenuState::MainMenuState() = default;
MainMenuState::~MainMenuState() = default;


void MainMenuState::Update()
{
	// if the user presses ESC from the main menu, the process will exit.
	if (CP_Input_KeyTriggered(KEY_ESCAPE))
	{
		CP_Engine_Terminate();
		return;
	}

	// transition between game modes
	// -- note that redundant selections will still reset the game state
	if (CP_Input_KeyTriggered(KEY_1))
	{
		auto* game_state = new SinglePlayerScenarioState();
		GameStateManager::SetNextState(game_state);
	}
	else if (CP_Input_KeyTriggered(KEY_2))
	{
		auto* game_state = new NetworkConfiguringState(
			[](const SOCKET socket, const bool is_host) -> NetworkedScenarioState*
			{ 
				return new LockstepScenarioState(socket, is_host); 
			}, "Lockstep");
		GameStateManager::SetNextState(game_state);
	}
	else if (CP_Input_KeyTriggered(KEY_3))
	{
		auto* game_state = new NetworkConfiguringState(
			[](const SOCKET socket, const bool is_host) -> NetworkedScenarioState*
			{
				return new DumbClientScenarioState(socket, is_host);
			}, "DumbClient");
		GameStateManager::SetNextState(game_state);
	}
	else if (CP_Input_KeyTriggered(KEY_4))
	{
		auto* game_state = new NetworkConfiguringState(
			[](const SOCKET socket, const bool is_host) -> NetworkedScenarioState*
			{
				return new OptimisticScenarioState(socket, is_host);
			}, "Optimistic");
		GameStateManager::SetNextState(game_state);
	}
}


void MainMenuState::Draw()
{
	// draw the menu options
	CP_Settings_TextSize(kMenuOptionTextSize);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_Fill(kMenuOptionTextColor);
	CP_Font_DrawText("Press 1 for Single Player", 10.0f, 10.0f);
	CP_Font_DrawText("Press 2 for Lockstep (2 player)", 10.0f, 40.0f);
	CP_Font_DrawText("Press 3 for Dumb Client (2 player)", 10.0f, 70.0f);
	CP_Font_DrawText("Press 4 for Optimistic (2 player)", 10.0f, 100.0f);
}