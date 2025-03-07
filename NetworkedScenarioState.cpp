#include "NetworkedScenarioState.h"
#include "GameStateManager.h"


NetworkedScenarioState::NetworkedScenarioState(const SOCKET socket, const bool is_host)
	: socket_(socket), is_host_(is_host)
{ }

NetworkedScenarioState::~NetworkedScenarioState() = default;

void NetworkedScenarioState::Update()
{
	if (CP_Input_KeyTriggered(KEY_ESCAPE))
	{
		if (socket_ != INVALID_SOCKET)
		{
			closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}
	}

	if (socket_ == INVALID_SOCKET)
	{
		GameStateManager::ReturnToBaseState();
	}
}

int NetworkedScenarioState::GetSocketPort() const 
{
	if (socket_ == INVALID_SOCKET) {
		return INVALID_SOCKET;
	}
	sockaddr_in localAddr;
	int addrLen = sizeof(localAddr);
	if (getsockname(socket_, reinterpret_cast<sockaddr*>(&localAddr), &addrLen) == SOCKET_ERROR) 
	{
		return INVALID_SOCKET;
	}
	return ntohs(localAddr.sin_port);
}