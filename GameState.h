//---------------------------------------------------------
// file:	GameState.h
// author:	Matthew Picioccio
// email:	matthew.picioccio@digipen.edu
//
// brief:	Base class for all game states. 
//			Game States are a foundational concept for C-Processing.
//
// Copyright © 2021 DigiPen, All rights reserved.
//---------------------------------------------------------
#pragma once


class GameState
{
public:
	GameState();
	virtual ~GameState();
	
	virtual void Update() = 0;
	virtual void Draw() = 0;
};