// AEW_Launcher.cpp : Contains main logic, memory override for AEW v1.0 @ runtime //
#include "ProcessMain.h"
#include "ProcessUtils.h"
//#include <iostream>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Hide Console
#pragma once 

using namespace std;
ProcessMain::ProcessMeta pMeta = { 0,0,0 };
char	 moduleName[] = "AEWFightForever-Win64-Shipping.exe";

void UpdateAEWInstruction() {
	DWORD64 modBase = pMeta.clientBase;
	uint8_t assemblyData;
	DWORD64 integFunctionPtr = modBase + /* func address */ 0x1036254;
	DWORD64 packFunctionPtr = modBase + /* func address */ 0x2C458FF;

	// Check Memory
	DWORD assemblyDataPAK;

	// Integrity Override
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(integFunctionPtr), &assemblyData, sizeof(assemblyData), NULL);
	if (assemblyData == 0x74) {
		assemblyData = 0x75; // Changes "JNE" instruction to "JE", bypassing process integrity check
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(integFunctionPtr), &assemblyData, sizeof(assemblyData), NULL);
	}

	// PAK override	
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(packFunctionPtr), &assemblyDataPAK, sizeof(assemblyDataPAK), NULL);
	if (assemblyDataPAK == 0x4C304688) {
		assemblyDataPAK = 0x4C909090; // NOP override
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(packFunctionPtr), &assemblyDataPAK, sizeof(assemblyDataPAK), NULL);
	}
	
	CloseHandle(pMeta.pHandle);
}

void PatchAEWProcess() {
	//Get Base Address
	while (pMeta.clientBase == 0x0) {
		pMeta.clientBase = dwGetModuleBaseAddress(_T(moduleName), pMeta.processID);
	}

	// Overrides process terminate functions
	UpdateAEWInstruction();
}


void GetAEWProcess() {
	DWORD pID = 0x0;
	HWND hGameWindow;
	HANDLE pHandle;
	char EACGAme[] = "AEWFightForever.exe";

	// Get EXE path
	const std::string cCurrentPath = getexepath();
	std::string gamePath = reDir(cCurrentPath, EACGAme);

	// Open Game
	ProcessMain::LaunchProcessHandle(gamePath.c_str());

	// Get Process ID using exe Name
	while (pMeta.processID == 0x0) {
		pMeta = ProcessMain::GetProcessIdFromExeName(moduleName);
	}

	pMeta.pHandle = ProcessMain::GetProcessHandle(pMeta.processID, PROCESS_ALL_ACCESS);
}

int main()
{

	GetAEWProcess();
	PatchAEWProcess();
}


