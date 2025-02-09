#pragma once
#include "ScenarioState.h"


class NetworkedScenarioState :
    public ScenarioState
{
public:
    NetworkedScenarioState(const SOCKET socket, const bool is_host);
    ~NetworkedScenarioState() override;

    // Inherited via GameState
    virtual void Update() override;

protected:
    SOCKET socket_;
    bool is_host_;
};

typedef NetworkedScenarioState* (*NetworkedScenarioStateCreator)(const SOCKET, const bool);
