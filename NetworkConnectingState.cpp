#include "NetworkConnectingState.h"
#include "GameStateManager.h"
#include "NetworkHostingState.h"


const float Connecting_Timeout_Secs = 3.0f;


NetworkConnectingState::NetworkConnectingState(NetworkedScenarioStateCreator scenario_state_creator, std::string challenge, u_short port)
	: scenario_state_creator_(scenario_state_creator),
	  challenge_(challenge),
	  connecting_port_(port),
	  connecting_socket_(INVALID_SOCKET),
	  connecting_timer_secs_(Connecting_Timeout_Secs)
{
	operation_description_ = "Connecting on ";
	operation_description_ += std::to_string(connecting_port_);
	operation_description_ += ", waiting for response from host...  ";

	// create a UDP socket for connecting to a scenario host
	connecting_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if ((connecting_socket_ == INVALID_SOCKET) &&
		HandleSocketError("Error creating socket: "))
	{
		return;
	}

	// make the socket non-blocking
	u_long nonblocking = 1;
	auto res = ioctlsocket(connecting_socket_, FIONBIO, &nonblocking);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error setting non-blocking state on socket: "))
	{
		return;
	}

	// set the UDP socket to reference the specified port on the local machine (127.0.0.1)
	SOCKADDR_IN connecting_address;
	memset(&connecting_address, 0, sizeof(connecting_address));
	connecting_address.sin_family = AF_INET;
	connecting_address.sin_port = htons(connecting_port_);
	res = inet_pton(AF_INET, "127.0.0.1", &connecting_address.sin_addr);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error creating a localhost address for the socket to connect to: "))
	{
		return;
	}
	res = connect(connecting_socket_, reinterpret_cast<SOCKADDR*>(&connecting_address), sizeof(connecting_address));
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error 'connect'ing socket: "))
	{
		return;
	}

	// send the scenario-specific challenge message to the server, hoping for a response
	res = send(connecting_socket_, challenge.c_str(), challenge.length(), 0);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error sending challenge on socket: "))
	{
		return;
	}

	std::cout << "Attempting to connect to a scenario server on port " << connecting_port_ << std::endl;
}


NetworkConnectingState::~NetworkConnectingState() = default;


void NetworkConnectingState::Update()
{
	// if the user presses ESC from the main menu, the process will exit.
	if (CP_Input_KeyTriggered(KEY_ESCAPE) && (connecting_socket_ != INVALID_SOCKET))
	{
		closesocket(connecting_socket_);
		connecting_socket_ = INVALID_SOCKET;
	}

	if (connecting_socket_ == INVALID_SOCKET)
	{
		GameStateManager::ReturnToBaseState();
		return;
	}

	// reduce the timer by CP_System_GetDt(), and if expired, give up and move on to Hosting
	connecting_timer_secs_ -= CP_System_GetDt();
	if (connecting_timer_secs_ <= 0.0f)
	{
		std::cout << "Timeout waiting for a response from a server on port " << connecting_port_ << ", attempting to host instead..." << std::endl;

		closesocket(connecting_socket_);
		connecting_socket_ = INVALID_SOCKET;

		auto game_state = new NetworkHostingState(scenario_state_creator_, connecting_port_);
		GameStateManager::SetNextState(game_state);
		return;
	}

	// attempt to receive a response from a hosting server
	char buffer[100];
	const auto res = recv(connecting_socket_, buffer, 100, 0);
	if ((res == SOCKET_ERROR) &&
		HandleSocketError("Error receiving on connection socket: "))
	{
		return;
	}

	// if any bytes are received, then assume we're ready to start, and transition to the scenario, re-using the socket, as non-host
	// TODO: incorporate proper validation of the host's response
	if (res > 0)
	{
		std::cout << "Received a response from a server on port " << connecting_port_ << ", moving on to the scenario..." << std::endl;
		auto game_state = scenario_state_creator_(connecting_socket_, false);
		GameStateManager::SetNextState(game_state);
		return;
	}
}

void NetworkConnectingState::Draw()
{
	// build the timer description
	auto timer_description = std::to_string(connecting_timer_secs_) + " seconds remaining...";

	// draw the descriptions
	CP_Settings_TextSize(30);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText(operation_description_.c_str(), 0.0f, 0.0f);
	CP_Font_DrawText(timer_description.c_str(), 0.0f, 35.0f);
}


bool NetworkConnectingState::HandleSocketError(const char* error_text)
{
	const auto wsa_error = WSAGetLastError();

	// ignore WSAEWOULDBLOCK
	if (wsa_error == WSAEWOULDBLOCK)
	{
		return false;
	}

	// we expect WSAECONNRESET - it means we should give up and try hosting instead
	if (wsa_error == WSAECONNRESET)
	{
		std::cout << "Received WSAECONNRESET when attempting to connect to a server on port " << connecting_port_ << ", attempting to host instead..." << std::endl;
		closesocket(connecting_socket_);
		connecting_socket_ = INVALID_SOCKET;

		auto game_state = new NetworkHostingState(scenario_state_creator_, connecting_port_);
		GameStateManager::SetNextState(game_state);
		return true;
	}

	// log unexpected errors and return to the default game mode
	std::cerr << "Connecting Winsock Error: " << error_text << wsa_error << std::endl;

	// close the socket and clear it
	// -- this should trigger a GameStateManager reset in the next Update
	closesocket(connecting_socket_);
	connecting_socket_ = INVALID_SOCKET;

	return true;
}