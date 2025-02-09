#pragma once
#include "framework.h"
#include "GameState.h"
#include "NetworkedScenarioState.h"


class NetworkHostingState :
    public GameState
{
public:
    NetworkHostingState(NetworkedScenarioStateCreator scenario_state_creator, u_short port);
    ~NetworkHostingState() override;

    // Inherited via GameState
    virtual void Update() override;
    virtual void Draw() override;

private:
    bool HandleSocketError(const char* error_text);

    NetworkedScenarioStateCreator scenario_state_creator_;
    u_short hosting_port_;
    SOCKET hosting_socket_;
    std::string operation_description_;
};