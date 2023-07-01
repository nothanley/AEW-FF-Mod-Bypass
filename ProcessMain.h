/* Launches process and retrieves process metadata */
#include "ProcessUtils.h"
#pragma once

class ProcessMain {

public:

	struct ProcessMeta {
		DWORD processID;
		HANDLE pHandle;
		DWORD64 clientBase;
	};

	static ProcessMeta LaunchProcessHandle(LPCSTR lpApplicationName)
	{
		// additional information
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		CreateProcessA
		(
			lpApplicationName,   // the path
			NULL,                // Command line
			NULL,                   // Process handle not inheritable
			NULL,                   // Thread handle not inheritable
			FALSE,                  // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,     // Opens file in a separate console
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi           // Pointer to PROCESS_INFORMATION structure
		);

		HANDLE pHandle = pi.hProcess;
		DWORD processId = pi.dwProcessId;

		ProcessMeta pMeta{ processId, pHandle, 0 };

		return pMeta;
	}
};