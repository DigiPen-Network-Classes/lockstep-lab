#include "DumbClientScenarioState.h"
#include "GameStateManager.h"


DumbClientScenarioState::DumbClientScenarioState(const SOCKET socket, const bool is_host)
	: NetworkedScenarioState(socket, is_host),
	host_control_(200.0f, 250.0f, 100.0f, 1.0f),
	non_host_control_(200.0f, 150.0f, 100.0f, 2.0f),
	is_remote_paused_(false),
	local_frame_(0),
	remote_frame_(0)
{
	local_player_.color = CP_Color_Create(255, 0, 0, 255);
	remote_player_.color = CP_Color_Create(0, 0, 255, 255);
	ZeroMemory(buffer_, BUF_SIZE * sizeof(char));
}


DumbClientScenarioState::~DumbClientScenarioState() = default;


void DumbClientScenarioState::Update()
{
	NetworkedScenarioState::Update();

	// host is on top - map host/non-host to local/remote
	if (CP_Input_KeyTriggered(CP_KEY::KEY_W))
	{
		is_frame_waiting_ = !is_frame_waiting_;
	}

	auto* local_control = is_host_ ? &host_control_ : &non_host_control_;
	auto* remote_control = is_host_ ? &non_host_control_ : &host_control_;

	// "simulate" the next frame ONLY IF we received a remote frame for the last frame we sent
	// -- or, if we're not waiting... 
	if (!is_frame_waiting_ || (local_frame_ <= remote_frame_))
	{
		const bool is_local_paused = CP_Input_KeyDown(KEY_SPACE);
		// only the host actually applies the control data to the FigureEightControls!  
		// -- the non-host *never* runs these Updates...
		if (is_host_)
		{
			if (!is_local_paused)
			{
				local_control->Update();
			}
			if (!is_remote_paused_)
			{
				remote_control->Update();
			}
			// the host only has to set positions during the simulation update
			local_player_.SetPosition(local_control->GetCurrentX(), local_control->GetCurrentY());
			remote_player_.SetPosition(remote_control->GetCurrentX(), remote_control->GetCurrentY());
		}
		
		ZeroMemory(buffer_, BUF_SIZE * sizeof(char));
		auto* write_buffer = buffer_;
		*reinterpret_cast<u_long*>(write_buffer) = ++local_frame_;
		write_buffer += sizeof(local_frame_);
		if (is_host_)
		{
			// the host sends all *state* information, since the non-host isn't simulating
			auto* write_float = reinterpret_cast<float*>(write_buffer);
			*write_float = host_control_.GetCurrentX();
			++write_float;
			*write_float = host_control_.GetCurrentY();
			++write_float;
			*write_float = non_host_control_.GetCurrentX();
			++write_float;
			*write_float = non_host_control_.GetCurrentY();
		}
		else
		{
			// the non-host only sends their control information (basically the state of SPACE, above)
			*reinterpret_cast<bool*>(write_buffer) = is_local_paused;
		}
		send(socket_, buffer_, BUF_SIZE, 0);
	}

	// if we don't have the matching remote frame yet... check for it on the network
	// -- or, go ahead, if we're not waiting...
	if (!is_frame_waiting_ || (remote_frame_ <= local_frame_))
	{
		const auto res = recv(socket_, buffer_, BUF_SIZE, 0);
		if (res > 0)
		{
			auto* read_buffer = buffer_;
			auto received_frame = *reinterpret_cast<u_long*>(read_buffer);
			read_buffer += sizeof(received_frame);
			// ONLY use the remote data if it's newer
			if (received_frame > remote_frame_)
			{
				remote_frame_ = received_frame;
				// inverse of the logic above
				if (is_host_)
				{
					is_remote_paused_ = *reinterpret_cast<bool*>(read_buffer);
				}
				else
				{
					auto* read_float = reinterpret_cast<float*>(read_buffer);
					const auto host_x = *read_float;
					++read_float;
					const auto host_y = *read_float;
					++read_float;
					const auto non_host_x = *read_float;
					++read_float;
					const auto non_host_y = *read_float;
					// non-host only has to update player position when receiving a new packet
					local_player_.SetPosition(non_host_x, non_host_y);
					remote_player_.SetPosition(host_x, host_y);
				}
			}
		}
	}
}


void DumbClientScenarioState::Draw()
{
	ScenarioState::Draw();

	local_player_.Draw();
	remote_player_.Draw();
}


std::string DumbClientScenarioState::GetDescription() const
{
	std::string description("DumbClient Scenario, ");
	description += is_host_ ? "Host(Server), " : "Non-Host(Client), ";
	description += "Port: ";
	description += std::to_string(GetSocketPort());
	description += ", ";
	description += "Local: ";
	description += std::to_string(local_frame_);
	description += ", Remote: ";
	description += std::to_string(remote_frame_);
	description += is_frame_waiting_ ? ", Waiting for Frame" : ", Not Waiting for Frame";
	return description;
}


std::string DumbClientScenarioState::GetInstructions() const
{
	return "Hold SPACE to halt the local (red) player. Press W to toggle frame-waiting";
}


bool DumbClientScenarioState::HandleSocketError(const char* error_text)
{
	const auto wsa_error = WSAGetLastError();

	// ignore WSAEWOULDBLOCK
	if (wsa_error == WSAEWOULDBLOCK)
	{
		return false;
	}

	// log unexpected errors and return to the default game mode
	std::cerr << "DumbClient WinSock Error: " << error_text << wsa_error << std::endl;

	// close the socket and clear it
	// -- this should trigger a GameStateManager reset in the next Update
	closesocket(socket_);
	socket_ = INVALID_SOCKET;

	return true;
}