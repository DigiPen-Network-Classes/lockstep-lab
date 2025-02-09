//---------------------------------------------------------
// file:	GameState.h
// author:	Matthew Picioccio
// email:	matthew.picioccio@digipen.edu
//
// brief:	Maps C-style global game state management in C-Processing
//			into a C++-friendly pattern.
//
// Copyright © 2021 DigiPen, All rights reserved.
//---------------------------------------------------------
#pragma once

// forward declarations
class GameState;

/// <summary>
/// Maps C-style global game state management in C-Processing into a C++-friendly pattern.
/// </summary>
class GameStateManager
{
public:
	/// <summary>
	/// Set the next game state
	/// </summary>
	/// <param name="game_state">The next game state.</param>
	/// <remarks>The change will be applied on the next frame.</remarks>
	static void SetNextState(GameState* game_state);

	/// <summary>
	/// Establish the base game state, that can be easily returned to later.
	/// </summary>
	/// <param name="base_game_state">The base game state, likely a main menu.</param>
	static void EstablishBaseState(GameState* base_game_state);

	/// <summary>
	/// Return to the base game state.
	/// </summary>
	static void ReturnToBaseState();

private:
	/// <summary>
	/// Per-frame update for C-Processing's engine.
	/// </summary>
	static void Update();
};

