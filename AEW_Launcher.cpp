// AEW_Launcher.cpp : Contains main logic, memory override for AEW v1.0 @ runtime //
#include "ProcessMain.h"
#include "ProcessUtils.h"
//#include <iostream>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Hide Console
#pragma once 

using namespace std;
ProcessMain::ProcessMeta pMeta = { 0,0,0 };
char	 moduleName[] = "AEWFightForever-Win64-Shipping.exe";

void UpdateAEWModule() {
	// (As of 7/5 update)
	DWORD64 integFunctionPtr = pMeta.clientBase + 0x10360D4; /* Original ASM terminates if AntiCheat interface is disabled */
	DWORD64 packFunctionPtr = pMeta.clientBase + 0x2C45D6F; /* Original ASM defines external PAK mounts */
	DWORD64 sigFunctionPtr = pMeta.clientBase + 0x15FE912; /* Original ASM skips PAK if no SIG file is found */

	// Custom Assembly
	uint8_t asmDataAntiCheat;
	uint32_t asmDataPAK;
	uint16_t asmDataSig;

	// Integrity Override
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(integFunctionPtr), &asmDataAntiCheat, sizeof(asmDataAntiCheat), NULL);
	if (asmDataAntiCheat == 0x75) {
		asmDataAntiCheat = 0x74; // Changes "JNE" instruction to "JE"
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(integFunctionPtr), &asmDataAntiCheat, sizeof(asmDataAntiCheat), NULL);
	}

	// PAK override	
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(packFunctionPtr), &asmDataPAK, sizeof(asmDataPAK), NULL);
	if (asmDataPAK == 0x4C304688) {
		asmDataPAK = 0x4C909090; // NOPs flag, allows External PAKs
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(packFunctionPtr), &asmDataPAK, sizeof(asmDataPAK), NULL);
	}

	// SIG override	
	ReadProcessMemory(pMeta.pHandle, (LPCVOID)(sigFunctionPtr), &asmDataSig, sizeof(asmDataSig), NULL);
	if (asmDataSig == 0x840F) {
		DWORD64 asmQWORD = 0x8B4D90000000A7E9; // Alters "JE" instruction to "JMP", bypasses missing sig method
		WriteProcessMemory(pMeta.pHandle, (LPVOID)(sigFunctionPtr), &asmQWORD, sizeof(asmQWORD), NULL);
	}
	
	CloseHandle(pMeta.pHandle);
}


void PatchAEWProcess() {
	//Get Base Address
	while (pMeta.clientBase == 0x0) {
		pMeta.clientBase = dwGetModuleBaseAddress(_T(moduleName), pMeta.processID);
	}

	// Overrides process terminate functions
	UpdateAEWModule();
}


void GetAEWProcess() {
	DWORD pID = 0x0;
	HWND hGameWindow;
	HANDLE pHandle;

	// Get EXE path
	const std::string cCurrentPath = getexepath();
	std::string gamePath = reDir(cCurrentPath, moduleName);

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