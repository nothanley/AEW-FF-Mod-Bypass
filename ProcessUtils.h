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


DWORD GetProcessIdFromWindow(const char* windowTitle, const char* exeName)
{
	DWORD processId = 0;

	HWND hwnd = FindWindowA(NULL, windowTitle);
	if (hwnd != NULL)
	{
		GetWindowThreadProcessId(hwnd, &processId);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 moduleEntry;
			moduleEntry.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(hSnapshot, &moduleEntry))
			{
				if (_stricmp(moduleEntry.szModule, exeName) != 0)
				{
					processId = 0;
					printf( "\n %s" ,moduleEntry.szModule);
				}
			}

			CloseHandle(hSnapshot);
		}
	}

	return processId;
}

#include <fstream>
#include <iostream>

static void createBackup(const std::string& filepath) {

	std::ifstream inputFile(filepath, std::ios::binary);
	if (!inputFile) {
		std::cerr << "\nError opening file: " << filepath << std::endl;
		return;
	}

	std::string backupFilePath = filepath + ".bak";
	std::ofstream outputFile(backupFilePath, std::ios::binary);
	if (!outputFile) {
		std::cerr << "\nError creating backup file: " << backupFilePath << std::endl;
		return;
	}

	outputFile << inputFile.rdbuf();
	inputFile.close();
	outputFile.close();

	std::cout << "\nBackup created: " << backupFilePath << std::endl;
}

static DWORD GetRVAFromFileOffset(const char* filePath, DWORD fileOffset) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Failed to open the file." << std::endl;
		return 0;
	}

	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	IMAGE_DOS_HEADER dosHeader;
	file.read(reinterpret_cast<char*>(&dosHeader), sizeof(IMAGE_DOS_HEADER));

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
		std::cerr << "Invalid DOS signature." << std::endl;
		file.close();
		return 0;
	}

	file.seekg(dosHeader.e_lfanew, std::ios::beg);

	IMAGE_NT_HEADERS32 ntHeaders;
	file.read(reinterpret_cast<char*>(&ntHeaders), sizeof(IMAGE_NT_HEADERS32));

	if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
		std::cerr << "Invalid PE signature." << std::endl;
		file.close();
		return 0;
	}

	DWORD sectionOffset = dosHeader.e_lfanew + offsetof(IMAGE_NT_HEADERS32, OptionalHeader) +
		ntHeaders.FileHeader.SizeOfOptionalHeader;

	file.seekg(sectionOffset, std::ios::beg);

	IMAGE_SECTION_HEADER sectionHeader;
	DWORD sectionIndex = 0;

	while (sectionIndex < ntHeaders.FileHeader.NumberOfSections) {
		file.read(reinterpret_cast<char*>(&sectionHeader), sizeof(IMAGE_SECTION_HEADER));

		if (fileOffset >= sectionHeader.PointerToRawData &&
			fileOffset < (sectionHeader.PointerToRawData + sectionHeader.SizeOfRawData)) {
			DWORD rva = sectionHeader.VirtualAddress + (fileOffset - sectionHeader.PointerToRawData);
			file.close();
			return rva;
		}

		sectionIndex++;
	}

	std::cerr << "File offset not found in any section." << std::endl;
	file.close();
	return 0;

}

// Function to overwrite bytes in a file at a given address
bool PatchBytesAtAddress(const std::string& filepath, const std::vector<unsigned char>& data, std::streampos address) {

	std::fstream file(filepath, std::ios::in | std::ios::out | std::ios::binary);
	if (!file) {
		std::cerr << "\nError: Unable to open file: " << filepath << std::endl;
		return false;
	}

	file.seekp(address, std::ios::beg);
	file.write(reinterpret_cast<const char*>(data.data()), data.size());

	file.close();
	return true;
}