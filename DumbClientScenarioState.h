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
    // both server and client sends 4 bytes for frame number,
    // server sends 4 * 4 bytes (20 total)
    // client sends 1 byte ... but both send 20 to keep the code easier to read...
    static const int BUF_SIZE = 20;
    char buffer_[BUF_SIZE];
};
