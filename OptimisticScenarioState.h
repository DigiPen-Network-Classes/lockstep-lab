#pragma once
#include "NetworkedScenarioState.h"


class OptimisticScenarioState final :
    public NetworkedScenarioState
{
public:
    OptimisticScenarioState(const SOCKET socket, const bool is_host);
    ~OptimisticScenarioState() override;

    void Update() override;
    void Draw() override;

    std::string GetDescription() const override;
    std::string GetInstructions() const override;
};