// AEW_Launcher.cpp : Contains main logic, memory override for AEW v1.0 @ runtime //
#include "ProcessMain.h"
#include "ProcessUtils.h"
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Hide Console
#pragma once 

using namespace std;
ProcessMain::ProcessMeta pMeta;
char	 moduleName[] = "AEWFightForever-Win64-Shipping.exe";

void UpdateAEWInstruction() {
	DWORD64 modBase = pMeta.clientBase;
	uint8_t assemblyData;
	DWORD64 functionPtr = modBase + /* func address */ 0x1036254;

	// Check Memory
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(functionPtr), &assemblyData, sizeof(assemblyData), NULL);

	if (assemblyData == 0x75) {
		assemblyData = 0x74; // Changes "JNE" instruction to "JE", bypassing process integrity check
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(functionPtr), &assemblyData, sizeof(assemblyData), NULL);
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

