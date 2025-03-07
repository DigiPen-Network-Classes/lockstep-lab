//---------------------------------------------------------
// file:	CS261_Replication_Tester.cpp
// author:	Matthew Picioccio
// email:	matthew.picioccio@digipen.edu
//
// brief:	Entry point
//
// C-Processing documentation link:
// https://inside.digipen.edu/main/GSDP:GAM100/CProcessing
//
// Copyright � 2021 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "framework.h"
#include "GameStateManager.h"
#include "MainMenuState.h"


/// <summary>
/// Enable the console, so we can see useful cout and cerr output.
/// For reference, see one of the newer answers in https://stackoverflow.com/questions/311955/redirecting-cout-to-a-console-in-windows.
/// </summary>
/// <remarks>In modern Windows SDKs, the old freopen method is not sufficient to capture cout/cerr.  This method does work.</remarks>
void ShowConsole()
{
	AllocConsole();

	static std::ofstream conout("CONOUT$", std::ios::out);
	std::cout.rdbuf(conout.rdbuf());
	std::cerr.rdbuf(conout.rdbuf());

	// NOTE: CP_Engine_Run will call FreeConsole() for us when the program is exiting.
}


/// <summary>
/// Entry point for the process
/// </summary>
/// <returns>0 if successful, non-zero for process errors</returns>
int main(void)
{
	ShowConsole();

	// initialize WinSock
	WSADATA wsa_data;
	const auto res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res != 0)
	{
		std::cerr << "Error in WSAStartup: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// establish the initial window settings
	CP_System_SetWindowTitle("CS 261 Replication Tester");
	CP_System_SetWindowSize(1024, 768);

	// start the simulation
	auto* main_menu_state = new MainMenuState();
	GameStateManager::EstablishBaseState(main_menu_state);
	CP_Engine_Run();

	// clean up WinSock
	WSACleanup();

	return 0;
}
