#pragma once
#include "DoubleOrbitControl.h"
#include "Player.h"
#include "ScenarioState.h"


class SinglePlayerScenarioState final :
    public ScenarioState
{
public:
    SinglePlayerScenarioState();
    ~SinglePlayerScenarioState() override;

    void Update() override;
    void Draw() override;
    
    std::string GetDescription() const override;
    std::string GetInstructions() const override;
	
private:
    Player player_;
    DoubleOrbitControl double_orbit_control_;
};