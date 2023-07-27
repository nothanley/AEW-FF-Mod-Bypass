#include "ProcessUtils.h"
#pragma once

class ReaderUtils {

public:


	static uint16_t ReadUInt16(ifstream& in) {
		uint16_t value = 0;
		in.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));

		return value;
	}


	static uint32_t ReadUInt32(ifstream& in) {

		uint32_t value = 0;
		in.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));

		return value;
	}

	static uint8_t ReadByte(ifstream& in) {

		uint8_t value = 0;
		in.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));

		return value;
	}

	static uint32_t ReadBytes_3(ifstream& in) {

		uint16_t value_short = 0;
		in.read(reinterpret_cast<char*>(&value_short), sizeof(uint16_t));

		uint8_t value_byte = 0;
		in.read(reinterpret_cast<char*>(&value_byte), sizeof(uint8_t));

		return value_short+value_byte;
	}


	static DWORD SearchFileForSigFunc(const char* filePath) {

		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open()) {
			return 0;
		}

		std::streampos fileSize;
		DWORD offset = 0x0;

		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(offset, std::ios::beg);

		while (offset <= fileSize) {
			if (ReadByte(file) == 0xA6) {
				if (ReadUInt32(file) == 0x4D000000) {
					if (ReadUInt16(file) == 0xC78B) {
						file.close();
						return offset;
					}
					offset+=2;
				}
				offset+=4;
			}
			offset++;
		}
		file.close();
		return 0;
	}

	static DWORD SearchFileForPackFunc(const char* filePath) {

		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open()) {
			return 0;
		}

		std::streampos fileSize;
		DWORD offset = 0x0;

		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(offset, std::ios::beg);

		while (offset <= fileSize) {

			if (ReadByte(file) == 0x88) {
				if (ReadUInt16(file) == 0x3046) {
					if (ReadByte(file) == 0x4C) {
						file.close();
						return offset;
					}
					offset++;
				}
				offset+=2;
			}

			offset++;
		}

		file.close();
		return 0;
	}

	static DWORD SearchFileForIntegFunc(const char* filePath) {

		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open()) {
			return 0;
		}

		std::streampos fileSize;
		DWORD offset = 0x0;

		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(offset, std::ios::beg);

		while (offset <= fileSize) {

			// Find Byte
			if (ReadByte(file) == 0x84) {
				// Find Short
				if (ReadUInt16(file) == 0x75C0) {
					// Skips two, compares next two bytes
					file.seekg(offset + 1, std::ios::beg);
					if (ReadUInt16(file) == 0x01B1) {
						file.close();
						return offset - 1;
					}
					offset += 3;
				}
				offset += 2;
			}
			offset++;
		}

		file.close();
		return 0;
	}


	static DWORD GetPackOffset(char moduleName[]) {

		DWORD offset = ReaderUtils::SearchFileForPackFunc(moduleName);
		//std::cout << "\n\nAddress: " << std::hex << offset;
		if (offset == 0) {
			std::cout << "\nCould not find patch address.";
		}
		return offset;
	}

	static DWORD GetInterfaceOffset(char moduleName[]) {

		DWORD offset = ReaderUtils::SearchFileForIntegFunc(moduleName);
		//std::cout << "\n\nAddress: " << std::hex << offset;
		if (offset == 0) {
			std::cout << "\nCould not find patch address.";
		}
		return offset;
	}

	static DWORD GetSigOffset(char moduleName[]) {
		DWORD offset = ReaderUtils::SearchFileForSigFunc(moduleName);
		//std::cout << "\n\nAddress: " << std::hex << offset - 2;
		if (offset == 0) {
			std::cout << "\nCould not find patch address.";
		}
		return offset - 2;
	}

};