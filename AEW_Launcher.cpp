// AEW_Launcher.cpp : Contains main logic, memory override for AEW v1.0 @ runtime //
#include "ProcessMain.h"
#include "ProcessUtils.h"
#include <iostream>
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Hide Console
#pragma once 

using namespace std;
ProcessMain::ProcessMeta pMeta;
char	 moduleName[] = "AEWFightForever-Win64-Shipping.exe";

void UpdateAEWInstruction() {
	DWORD64 modBase = pMeta.clientBase;
	uint8_t assemblyData;
	DWORD64 integFunctionPtr = modBase + /* func address */ 0x1036254;
	DWORD64 packFunctionPtr = modBase + /* func address */ 0x2C458FF;
	// Check Memory

	// Integrity Override
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(integFunctionPtr), &assemblyData, sizeof(assemblyData), NULL);
	if (assemblyData == 0x75) {
		assemblyData = 0x74; // Changes "JNE" instruction to "JE", bypassing process integrity check
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(integFunctionPtr), &assemblyData, sizeof(assemblyData), NULL);
	}

	// PAK override
	uint32_t assemblyDataPAK;
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(packFunctionPtr), &assemblyDataPAK, sizeof(assemblyDataPAK), NULL);
	if (assemblyDataPAK == 0x4C304688) {
		assemblyDataPAK = 0x4C909090; // NOP override
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(packFunctionPtr), &assemblyDataPAK, sizeof(assemblyDataPAK), NULL);
	}
}

void PatchAEWProcess() {
	//Get Base Address
	while (pMeta.clientBase == 0x0) {
		pMeta.clientBase = dwGetModuleBaseAddress(_T(moduleName), pMeta.processID);
	}

	// Overrides process terminate functions
	UpdateAEWInstruction();
}

int main()
{
	const std::string cCurrentPath = getexepath();
	std::string gamePath = reDir(cCurrentPath, moduleName);

	//Open Game and retrieve  metadata
	pMeta = ProcessMain::LaunchProcessHandle(gamePath.c_str());

	PatchAEWProcess();
}

