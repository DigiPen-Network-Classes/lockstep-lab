#include "LockstepScenarioState.h"
#include "GameStateManager.h"


LockstepScenarioState::LockstepScenarioState(const SOCKET socket, const bool is_host)
	: NetworkedScenarioState(socket, is_host),
	  host_control_(200.0f, 150.0f, 100.0f, 1.0f),
	  non_host_control_(200.0f, 250.0f, 100.0f, 2.0f),
	  isRemotePaused_(false),
	  local_frame_(0),
	  remote_frame_(0)
{
	local_player_.color = CP_Color_Create(255, 0, 0, 255);
	remote_player_.color = CP_Color_Create(0, 0, 255, 255);
	ZeroMemory(buffer_, 100 * sizeof(char));
}


LockstepScenarioState::~LockstepScenarioState() = default;


void LockstepScenarioState::Update()
{
	NetworkedScenarioState::Update();

	// host is on top - map host/non-host to local/remote
	auto* local_control = is_host_ ? &host_control_ : &non_host_control_;
	auto* remote_control = is_host_ ? &non_host_control_ : &host_control_;

	// simulate the next frame ONLY IF we received a remote frame for the last frame we sent
	// TODO: consider if this logic can be improved to handle out-of-order...
	if (local_frame_ == remote_frame_)
	{
		// local update based on local control
		const bool is_local_paused = CP_Input_KeyDown(KEY_SPACE);
		if (!is_local_paused)
		{
			local_control->Update();
		}
		// remote update based on remote control
		if (!isRemotePaused_)
		{
			remote_control->Update();
		}

		// increment the frame, and send it as well as the local pause state
		// -- note, this is symmetric!  "host" doesn't matter...
		ZeroMemory(buffer_, 100 * sizeof(char));
		auto* write_buffer = buffer_;
		*reinterpret_cast<u_long*>(write_buffer) = ++local_frame_;
		write_buffer += sizeof(local_frame_);
		*reinterpret_cast<bool*>(write_buffer) = is_local_paused;
		send(socket_, buffer_, 100, 0);
	}

	// if we don't have the matching remote frame yet... check for it on the network
	// TODO: consider if this logic can be improved to handle out-of-order...
	if (remote_frame_ != local_frame_)
	{
		const auto res = recv(socket_, buffer_, 100, 0);
		if (res > 0)
		{
			// read in the remote packet for the frame and pause data
			auto* read_buffer = buffer_;
			auto received_frame = *reinterpret_cast<u_long*>(read_buffer);
			read_buffer += sizeof(received_frame);
			// ONLY use the remote data if it's newer
			if (received_frame > remote_frame_)
			{
				remote_frame_ = received_frame;
				isRemotePaused_ = *reinterpret_cast<bool*>(read_buffer);
			}
		}
	}

	local_player_.SetPosition(local_control->GetCurrentX(), local_control->GetCurrentY());
	remote_player_.SetPosition(remote_control->GetCurrentX(), remote_control->GetCurrentY());
}


void LockstepScenarioState::Draw()
{
    ScenarioState::Draw();

	local_player_.Draw();
	remote_player_.Draw();
}


std::string LockstepScenarioState::GetDescription() const
{
	std::string description("Lockstep Scenario, ");
	description += is_host_ ? "Host, " : "Non-Host, ";
	description += "Local: ";
	description += std::to_string(local_frame_);
	description += ", Remote: ";
	description += std::to_string(remote_frame_);
	return description;
}


std::string LockstepScenarioState::GetInstructions() const
{
	return "Hold SPACE to halt the local (red) player";
}


bool LockstepScenarioState::HandleSocketError(const char* error_text)
{
	const auto wsa_error = WSAGetLastError();

	// ignore WSAEWOULDBLOCK
	if (wsa_error == WSAEWOULDBLOCK)
	{
		return false;
	}

	// log unexpected errors and return to the default game mode
	std::cerr << "Lockstep WinSock Error: " << error_text << wsa_error << std::endl;

	// close the socket and clear it
	// -- this should trigger a GameStateManager reset in the next Update
	closesocket(socket_);
	socket_ = INVALID_SOCKET;

	return true;
}