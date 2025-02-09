//---------------------------------------------------------
// file:	GameState.cpp
// author:	Matthew Picioccio
// email:	matthew.picioccio@digipen.edu
//
// brief:	Maps C-style global game state management in C-Processing
//			into a C++-friendly pattern.
//
// Copyright © 2021 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "framework.h"
#include <stack>
#include "GameState.h"
#include "GameStateManager.h"


GameState* base_state = nullptr;
GameState* current_state = nullptr;
GameState* next_state = nullptr;



/// <summary>
/// Set the next game state
/// </summary>
/// <param name="game_state">The next game state.</param>
/// <remarks>The change will be applied on the next frame.</remarks>
void GameStateManager::SetNextState(GameState* game_state)
{
	if ((game_state == nullptr) || (game_state == current_state))
	{
		return;
	}

	if ((next_state != nullptr) && (next_state != game_state) && (next_state != base_state))
	{
		delete next_state;
	}

	next_state = game_state;
}


/// <summary>
/// Establish the base game state, that can be easily returned to later.
/// </summary>
/// <param name="base_game_state">The base game state, likely a main menu.</param>
void GameStateManager::EstablishBaseState(GameState* new_base_state)
{	
	if ((new_base_state == nullptr) || (new_base_state == base_state))
	{
		return;
	}

	if (base_state != nullptr)
	{
		delete base_state;
	}

	base_state = new_base_state;
	ReturnToBaseState();

	CP_Engine_SetNextGameStateForced(nullptr, Update, nullptr);
}


/// <summary>
/// Return to the base game state.
/// </summary>
void GameStateManager::ReturnToBaseState()
{
	SetNextState(base_state);
}


/// <summary>
/// Per-frame update for C-Processing's engine.
/// </summary>
void GameStateManager::Update(void)
{
	// apply the next state now, before 
	if (next_state != nullptr)
	{
		if ((current_state != nullptr) && (current_state != base_state))
		{
			delete current_state;
		}

		current_state = next_state;
		next_state = nullptr;
		// if we forgot to establish the base state, do it now
		if (base_state == nullptr)
		{
			base_state = current_state;
		}
	}

	// update the current state
	if (current_state != nullptr)
	{
		current_state->Update();
	}

	// draw the current state
	CP_Settings_Background(CP_Color_Create(0, 0, 25, 255));
	if (current_state != nullptr)
	{
		current_state->Draw();
	}
}