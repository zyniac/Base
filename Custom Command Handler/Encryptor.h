#pragma once
#include <string>
#include <sstream>

namespace Encryptor {

	std::string XORCrypt(std::string key, std::string value) {
		for (int i = 0; i < value.size(); i++) {
			value[i] = value[i] ^ key[i % key.size()];
		}
		return value;
	}

	std::string reverse(std::string value) {
		std::string output = "";
		for (int i = value.size(); i > 0; i--) {
			output += value[i];
		}
		return output;
	}

	std::string hashEncrypt(std::string value) {
		uint64_t hash = 0;
		unsigned int magic = 3240;
		for (int i = 0; i < value.size(); i++) {
			hash = hash ^ value[i];
			hash = hash * magic;
		}
		std::stringstream stream;
		stream << std::hex << hash;
		return stream.str();
	}

	std::string byteHashEncrypt(std::string value) {
		std::string hash = "H";
		for (int i = 0; i < value.size(); i++) {
			hash += *(hash.end() - 1) ^ value[i];
		}
		return hash;
	}

	bool hashCompare(std::string value, std::string hash) {
		return hashEncrypt(value) == hash;
	}

	bool byteHashCompare(std::string value, std::string hash) {
		return byteHashEncrypt(value) == hash;
	}
};