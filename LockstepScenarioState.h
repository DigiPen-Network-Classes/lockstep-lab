#pragma once
#include "DoubleOrbitControl.h"
#include "Player.h"
#include "NetworkedScenarioState.h"


class LockstepScenarioState final:
    public NetworkedScenarioState
{
public:
    LockstepScenarioState(const SOCKET socket, const bool is_host);
    ~LockstepScenarioState() override;

    LockstepScenarioState(const LockstepScenarioState&) = delete;
    LockstepScenarioState(LockstepScenarioState&&) = delete;
    LockstepScenarioState& operator=(const LockstepScenarioState&) = delete;
    LockstepScenarioState& operator=(LockstepScenarioState&&) = delete;


    void Update() override;
    void Draw() override;

    std::string GetDescription() const override;
    std::string GetInstructions() const override;
	
private:
    bool HandleSocketError(const char* error_text);
    void PrintBuffer() const;

    DoubleOrbitControl host_control_;
    DoubleOrbitControl non_host_control_;
    Player local_player_;
    Player remote_player_;
    bool isRemotePaused_;
    u_long local_frame_;
    u_long remote_frame_;
    static const int BUF_SIZE = 5;
    char buffer_[BUF_SIZE];
};