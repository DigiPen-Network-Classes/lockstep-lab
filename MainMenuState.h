#pragma once
#include "GameState.h"


class MainMenuState :
    public GameState
{
public:
	MainMenuState();
	~MainMenuState() override;

	void Update() override;
	void Draw() override;
};