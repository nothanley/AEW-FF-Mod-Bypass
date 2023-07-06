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

	static ProcessMeta GetProcessIdFromExeName(const char* exeName)
	{
		DWORD processId = 0;
		ProcessMeta pMeta = ProcessMeta{ 0, 0, 0 };
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 processEntry;
			processEntry.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(hSnapshot, &processEntry))
			{
				while (Process32Next(hSnapshot, &processEntry))
				{
					const char* processExeName = strrchr(processEntry.szExeFile, '\\');
					if (processExeName != nullptr)
						processExeName++; // Move past the backslash
					else
						processExeName = processEntry.szExeFile; // No backslash found, use the full name

					if (_stricmp(processExeName, exeName) == 0)
					{
						processId = processEntry.th32ProcessID;
						pMeta = ProcessMeta{ processId, hSnapshot, 0 };
						break;
					}
				}
			}

			CloseHandle(hSnapshot);
		}

		return pMeta;
	}

	static HANDLE GetProcessHandle(DWORD processId, DWORD dwDesiredAccess)
	{
		HANDLE hProcess = OpenProcess(dwDesiredAccess, FALSE, processId);
		return hProcess;
	}

};