#include "NetworkConfiguringState.h"
#include "NetworkConnectingState.h"
#include "GameStateManager.h"


NetworkConfiguringState::NetworkConfiguringState(NetworkedScenarioStateCreator scenario_state_creator, std::string challenge)
	: scenario_state_creator_(scenario_state_creator),
	  challenge_(challenge)
{ }


NetworkConfiguringState::~NetworkConfiguringState() = default;


void NetworkConfiguringState::Update()
{
	// if the user presses ESC from the main menu, the process will exit.
	if (CP_Input_KeyTriggered(KEY_ESCAPE))
	{
		GameStateManager::ReturnToBaseState();
		return;
	}

	port_entry_field_.Update();

	// if the user presses enter and they have entered a valid port, proceed to the next step - attempting to connect to an existing server.
	if (CP_Input_KeyTriggered(KEY_ENTER) && port_entry_field_.IsPortValid())
	{
		auto* connecting_state = new NetworkConnectingState(scenario_state_creator_, challenge_, port_entry_field_.GetPort());
		GameStateManager::SetNextState(connecting_state);
	}
}

void NetworkConfiguringState::Draw()
{
	// draw the instructions
	CP_Settings_TextSize(30);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_TOP);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("Enter port for simulation:", 0.0f, 0.0f);
	if (port_entry_field_.IsPortValid())
	{
		CP_Font_DrawText("ESC for Main Menu, ENTER to continue", 0.0f, 40.0f);
	}
	else
	{
		CP_Font_DrawText("ESC for Main Menu", 0.0f, 40.0f);
	}

	// draw the current port value
	CP_Font_DrawText(port_entry_field_.GetPortText().c_str(), 0.0f, 80.0f);
}