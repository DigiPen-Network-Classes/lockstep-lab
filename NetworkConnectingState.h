#pragma once
#include "framework.h"
#include "GameState.h"
#include "NetworkedScenarioState.h"


class NetworkConnectingState :
    public GameState
{
public:
    NetworkConnectingState(NetworkedScenarioStateCreator scenario_state_creator, std::string challenge, u_short port);
    ~NetworkConnectingState() override;

    // Inherited via GameState
    virtual void Update() override;
    virtual void Draw() override;

private:
    bool HandleSocketError(const char* error_text);

    NetworkedScenarioStateCreator scenario_state_creator_;
    std::string challenge_;
    u_short connecting_port_;
    SOCKET connecting_socket_;
    float connecting_timer_secs_;
    std::string operation_description_;
};