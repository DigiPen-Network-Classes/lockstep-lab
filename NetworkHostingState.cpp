#include "NetworkHostingState.h"
#include "GameStateManager.h"


NetworkHostingState::NetworkHostingState(NetworkedScenarioStateCreator scenario_state_creator, u_short port)
	: scenario_state_creator_(scenario_state_creator),
	  hosting_port_(port)
{
	operation_description_ = "Hosting on ";
	operation_description_ += std::to_string(hosting_port_);
	operation_description_ += ", waiting for connection...  ";

	// create a UDP socket for connecting to a scenario host
	hosting_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ((hosting_socket_ == INVALID_SOCKET) &&
		HandleSocketError("Error creating socket: "))
	{
		return;
	}

	// make the socket non-blocking
	u_long nonblocking = 1;
	auto res = ioctlsocket(hosting_socket_, FIONBIO, &nonblocking);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error setting non-blocking state on socket: "))
	{
		return;
	}

	// bind the hosting socket to the specified port on the local machine (127.0.0.1)
	SOCKADDR_IN hosting_address;
	memset(&hosting_address, 0, sizeof(hosting_address));
	hosting_address.sin_family = AF_INET;
	hosting_address.sin_port = htons(hosting_port_);
	res = inet_pton(AF_INET, "127.0.0.1", &hosting_address.sin_addr);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error creating a localhost address for the socket to host on: "))
	{
		return;
	}
	res = bind(hosting_socket_, reinterpret_cast<SOCKADDR*>(&hosting_address), sizeof(hosting_address));
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error binding hosting socket: "))
	{
		return;
	}

	std::cout << "Hosting a server on port " << hosting_port_ << std::endl;
}


NetworkHostingState::~NetworkHostingState() = default;


void NetworkHostingState::Update()
{
	// if the user presses ESC from the main menu, the process will exit.
	if (CP_Input_KeyTriggered(KEY_ESCAPE) && (hosting_socket_ != INVALID_SOCKET))
	{
		closesocket(hosting_socket_);
		hosting_socket_ = INVALID_SOCKET;
	}

	if (hosting_socket_ == INVALID_SOCKET)
	{
		GameStateManager::ReturnToBaseState();
		return;
	}

	// attempt to receive a message from a connecting client
	SOCKADDR_IN other_address;
	int other_address_size = sizeof(other_address);
	char buffer[100];
	auto res = recvfrom(hosting_socket_, buffer, 100, 0, reinterpret_cast<SOCKADDR*>(&other_address),
		&other_address_size);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error receiving on hosting socket: "))
	{
		return;
	}
	// if any data is received:
	// TODO: validate the incoming challenge value
	if (res > 0)
	{
		std::cout << "Received a message from a potential connection, acknowledging..." << std::endl;

		// set the hosting socket to reference the address the message was received from
		res = connect(hosting_socket_, reinterpret_cast<SOCKADDR*>(&other_address), sizeof(other_address));
		if ((res == SOCKET_ERROR) &&
			HandleSocketError("Error 'connect'ing on hosting socket: "))
		{
			return;
		}
		// send an acknowledgement, like "LetUsBegin"
		// TODO: replace with a proper response to the challenge
		res = send(hosting_socket_, "LetUsBegin", 10, 0);
		if ((res == SOCKET_ERROR) &&
			HandleSocketError("Error sending 'LetUsBegin' from hosting socket: "))
		{
			return;
		}
		// move on to the scenario, using the hosting socket, in host mode
		std::cout << "Successfully hosting a scenario on port " << hosting_port_ << ", moving on to the scenario..." << std::endl;
		auto game_state = scenario_state_creator_(hosting_socket_, true);
		GameStateManager::SetNextState(game_state);
		return;
	}
}


void NetworkHostingState::Draw()
{
	// draw the description
	CP_Settings_TextSize(30);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText(operation_description_.c_str(), 0.0f, 0.0f);
}


bool NetworkHostingState::HandleSocketError(const char* error_text)
{
	const auto wsa_error = WSAGetLastError();

	// ignore WSAEWOULDBLOCK
	if (wsa_error == WSAEWOULDBLOCK)
	{
		return false;
	}

	// log unexpected errors and return to the default game mode
	std::cerr << "Hosting Winsock Error: " << error_text << wsa_error << std::endl;

	// close the socket and clear it
	// -- this should trigger a GameStateManager reset in the next Update
	closesocket(hosting_socket_);
	hosting_socket_ = INVALID_SOCKET;

	return true;
}