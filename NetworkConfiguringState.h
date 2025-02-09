#pragma once
#include "GameState.h"
#include "NetworkedScenarioState.h"
#include "PortEntryField.h"


class NetworkConfiguringState :
    public GameState
{
public:
    NetworkConfiguringState(NetworkedScenarioStateCreator scenario_state_creator, std::string challenge);
    ~NetworkConfiguringState() override;

    // Inherited via GameState
    virtual void Update() override;
    virtual void Draw() override;

private:
    NetworkedScenarioStateCreator scenario_state_creator_;
    std::string challenge_;
    PortEntryField port_entry_field_;
};