#include "windows.h"
#include <TlHelp32.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <regex>
#pragma once

using namespace std;

static DWORD64 dwGetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
	DWORD64 dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32))
	{
		do {
			if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
			{
				dwModuleBaseAddress = (DWORD64)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32));

	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

static std::string getexepath() {
	char result[MAX_PATH];
	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
}
//note: TCHAR* _T prefix requires VS Studio project settings set to 'MultiByte' chars

static std::vector<std::string> split(const string& input, const string& regex) {
	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

static std::string reDir(const string& input, const string& regex) {
	std::string outPath;
	std::vector<std::string> pVect = split(input, R"(\\)");
	pVect.pop_back(); pVect.push_back(regex);
	for (const auto& dir : pVect) outPath += dir + "\\";
	outPath.pop_back();
	return outPath;
}

static bool isRunning(LPCSTR pName)
{
	HWND hwnd;
	hwnd = FindWindow(NULL, pName);
	if (hwnd != 0) {
		return true;
	}
	else {
		return false;
	}
}
