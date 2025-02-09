#pragma once
#include "DoubleOrbitControl.h"
#include "NetworkedScenarioState.h"
#include "Player.h"


class DumbClientScenarioState final
	: public NetworkedScenarioState
{
public:
    DumbClientScenarioState(const SOCKET socket, const bool is_host);
    ~DumbClientScenarioState() override;

    DumbClientScenarioState(const DumbClientScenarioState&) = delete;
    DumbClientScenarioState(DumbClientScenarioState&&) = delete;
    DumbClientScenarioState& operator=(const DumbClientScenarioState&) = delete;
    DumbClientScenarioState& operator=(DumbClientScenarioState&&) = delete;

    void Update() override;
    void Draw() override;

    std::string GetDescription() const override;
    std::string GetInstructions() const override;

private:
    bool HandleSocketError(const char* error_text);

    DoubleOrbitControl host_control_;
    DoubleOrbitControl non_host_control_;
    Player local_player_;
    Player remote_player_;
    bool is_remote_paused_;
    u_long local_frame_;
    u_long remote_frame_;
    bool is_frame_waiting_;
    char buffer_[100];
};
