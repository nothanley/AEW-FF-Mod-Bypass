// AEW_Launcher.cpp : Contains main logic, memory override for AEW v1.0 @ runtime //
#include "ProcessMain.h"
#include "ReaderUtils.h"
#include <iostream>
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Hide Console
#pragma once 

using namespace std;
ProcessMain::ProcessMeta pMeta = { 0,0,0 };
char	 moduleName[] = "AEWFightForever-Win64-Shipping.exe";


void patchAEWExecutable(DWORD interfaceOffset, DWORD packOffset, DWORD sigOffset ) {
	
	// Create EXE backup
	std::cout << "\n\nCreating backup...";
	createBackup(moduleName);
	std::vector<uint8_t> data;

	std::cout << "\nPatching file...";

	//Patch 1
	data = { 0xEB };
	if (!PatchBytesAtAddress(moduleName, data, interfaceOffset)) {
		std::cout << "\nCould not patch executable...";
		system("pause");
	}


	//Patch 2
	data = { 0x90, 0x90, 0x90, 0x4C };
	PatchBytesAtAddress(moduleName, data, packOffset);

	//Patch 3
	data = { 0xE9 , 0xA7, 0x00 , 0x00, 0x00 , 0x90, 0x4D , 0x8B };
	PatchBytesAtAddress(moduleName, data, sigOffset);

	std::cout << "\n\n\nPatcher completed succesfully\n";
	system("pause");

}

int main()
{
	// Search for local offsets
	std::cout << "\nLocating data...";
	DWORD interfaceOffset = ReaderUtils::GetInterfaceOffset(moduleName);
	DWORD packOffset = ReaderUtils::GetPackOffset(moduleName);
	DWORD sigOffset = ReaderUtils::GetSigOffset(moduleName);

	if (interfaceOffset == 0x0 || packOffset == 0x0 || sigOffset == 0x0) {
		return 0;
	}

	//std::cout << "\n\nRVA: " << std::hex << interfaceOffset << std::endl;
	//std::cout << "RVA: " << std::hex << packOffset << std::endl;
	//std::cout << "RVA: " << std::hex << sigOffset << std::endl;

	// Launch process and acquire handle
	patchAEWExecutable(interfaceOffset,packOffset,sigOffset);
	
}