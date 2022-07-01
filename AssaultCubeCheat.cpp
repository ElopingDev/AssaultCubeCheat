// AssaultCubeCheat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include<TlHelp32.h>
#include <iostream>
#include <tchar.h> // _tcscmp
#include <vector>




DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress; // not mine
}

    

int main()
{
    std::cout << "Injecting \n";
    HWND hGameWindow = FindWindow(NULL, "AssaultCube");
    if (hGameWindow == NULL) {
        std::cout << "Please start the game first :3" << std::endl;
        return 0;
    }
    DWORD pID = NULL; //process id of game
    GetWindowThreadProcessId(hGameWindow, &pID); // gets pID of hGameWindow
    HANDLE processHandle = NULL;
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL) { //Error Handler
        std::cout << "Failed to open process" << std::endl;
        return 0;
    }


    char gameName[] = "ac_client.exe"; // process name
    DWORD gameBaseAddress = GetModuleBaseAddress(_T(gameName), pID);
    DWORD offsetGameToBaseAdress = 0x0016F338;
    std::vector<DWORD> pointsOffsets{ 0x00, 0xF8, 0x140, };
    DWORD baseAddress = NULL;
    //Get value at gamebase+offset -> store it in baseAddress
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAdress), &baseAddress, sizeof(baseAddress), NULL);
    std::cout << "debugginfo: baseaddress = " << std::hex << baseAddress << std::endl;

    DWORD ammoAddress = baseAddress; //the Address needed
    for (int i = 0; i < pointsOffsets.size() - 1; i++) // -1 because we dont want the value at the last offset

        {
            ReadProcessMemory(processHandle, (LPVOID)(ammoAddress + pointsOffsets.at(i)), &ammoAddress, sizeof(ammoAddress), NULL);
            std::cout << "debugginfo: Value at offset = " << std::hex << ammoAddress << std::endl;
        }
        ammoAddress += pointsOffsets.at(pointsOffsets.size() - 1); //Add Last offset

    //"UI"
        std::cout << "Femboyware AC Edition" << std::endl;
        std::cout << "Press Numpad 0 to EXIT" << std::endl;
        std::cout << "Press Numpad 1 to set Ammo" << std::endl;
        while (true) {
            Sleep(50);
            if (GetAsyncKeyState(VK_NUMPAD0)) { // Exit
                return 0;
            }
            if (GetAsyncKeyState(VK_NUMPAD1)) {
                std::cout << "How many ammos do you want?" << std::endl;
                int newAmmo = 0;
                std::cin >> newAmmo;
                WriteProcessMemory(processHandle, (LPVOID)(ammoAddress), &newAmmo, 4, 0);
            }
        }
    }


