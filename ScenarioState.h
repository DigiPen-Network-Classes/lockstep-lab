#pragma once
#include "framework.h"
#include "GameState.h"


class ScenarioState :
    public GameState
{
public:
    ScenarioState();
    ~ScenarioState() override;

    virtual void Draw() override;

    virtual std::string GetDescription() const = 0;
    virtual std::string GetInstructions() const = 0;
};