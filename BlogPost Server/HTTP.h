#pragma once

#include <CommandLib.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>

namespace HTTPNS {
	std::string urlDecode(const std::string& value) {
		std::ostringstream sstream;
		sstream.fill('0');
		sstream << std::hex;

		for (std::string::const_iterator i = value.begin(); i < value.end(); ++i) {
			std::string::value_type c = *i;

			if (c == '%' && i < value.end() - 2) {
				try {
					std::string n(i + 1, i + 3);
					unsigned char x = std::stoul(n, nullptr, 16);
					sstream << static_cast<char>(x);
					i += 2;
				}
				catch (...) {
					sstream << c;
				}
				continue;
			}

			sstream << c;
		}

		return sstream.str();
	}
}

struct HTTPSendInformation {
	HTTPSendInformation() = default;

	int status = 200;
	const char* statusMessage = "OK";

	void setDataC(const char* c) {
		char* cb = const_cast<char*>(c);
		data = cb;
	}

	std::string data;
	std::string fullPath;
};

class HTTPWriteProtocol {
public:
	HTTPWriteProtocol() = default;

	int status = 200;
	std::string statusMessage = "OK";
	std::map<std::string, std::string> HTTPMap;

	void Create(std::string& all, const HTTPSendInformation& info) {
		all = "";
		std::string httpKeys;
		for (std::pair<std::string, std::string> pair : HTTPMap) {
			httpKeys += "\n" + pair.first + ": " + pair.second;
		}
		size_t fullSizeLength = 22 + statusMessage.length() + all.length() + info.data.size();
		//all = "HTTP/2.0 " + std::to_string(info.status) + " " + statusMessage + " " + all + "\r\n\r\n" + info.data + "\r\n\r\n";
		all.reserve(fullSizeLength);
		all.append("HTTP/2.0 ", 9);
		all.append(std::move(std::to_string(info.status).c_str()), 3);
		all.append(" ", 1);
		all.append(std::move(statusMessage));
		all.append(" ", 1);
		all.append(std::move(httpKeys));
		all.append("\r\n\r\n");
		all.append(std::move(info.data));
		all.append("\r\n\r\n");
	}

	void AddArgument(const char* key, const char* value) {
		HTTPMap[key] = value;
	}
};

class HTTPReadProtocol {
public:
	HTTPReadProtocol(std::string http)
		: http(http)
	{
		convert();
	}

	void convert() {
		int splits = 0;
		int nlSplits = 0;
		std::string spValue;
		std::string keyValue;
		bool readingDescriptor = false;
		for (std::string::iterator i = http.begin(); i < http.end(); i++) {
			if (*i == ' ') {
				switch (splits) {
				case 0:
					isGetProtocol = spValue == "GET";
					spValue = "";
					splits += 1;
					break;
				case 1:
					encodedPath = spValue;
					spValue = "";
					splits += 1;
					break;
				}
				if(!readingDescriptor) // If it is not a http value
					continue;
			}
			if (*i == ':' && (i + 1) < http.end() && *(i + 1) == ' ' && !readingDescriptor) {
				readingDescriptor = true;
				++i;
				keyValue.swap(spValue);
				spValue = "";
				continue;
			}
			else if (*i == '\r' && i + 1 < http.end() && *(i + 1) == '\n') {
				++i;
				if (nlSplits++ > 0) {
					if (readingDescriptor) {
						HTTPMap[keyValue] = spValue;
						readingDescriptor = false;
						spValue = "";
						keyValue = "";
						continue;
					}
					else {
						continue;
					}
				}
				else {
					protocol = spValue;
					spValue = "";
					continue;
				}
			}
			spValue += *i;
		}
	}

	bool protocolInvalid = false;
	std::string protocol;
	std::string encodedPath;
	bool isGetProtocol;
	std::string http;
	std::map<std::string, std::string> HTTPMap;
};