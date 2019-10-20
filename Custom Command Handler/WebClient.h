#pragma once

#define WEB_BUFFER_SIZE 4096

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
// Classic WinSock2 stuff
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")
#else // Linux, MAC
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h>
#define INVALID_SOCKET -1
typedef int SOCKET;
#endif

namespace {
	bool readNBytes(SOCKET fd, void* buf, std::size_t n) {
		std::size_t offset = 0;
		char* cbuf = reinterpret_cast<char*>(buf);
		while (true) {
			int ret = recv(fd, cbuf + offset, n - offset, 0);
			if (ret < 0) {
				return false; // Nothing sended and error
			}
			else if (ret == 0) {
				return false; // Stream null or unexpected end of stream
			}
			else if (offset + ret == n)
				return true; // Success, all received
			else {
				offset += ret;
			}
		}
	}

	int sockclose(SOCKET sock) {
		int status = 0;

#ifdef _WIN32
		status = shutdown(sock, SD_BOTH);
		if (status == 0) { status = closesocket(sock); }
#else
		status = shutdown(sock, SHUT_RDWR);
		if (status == 0) { status = close(sock); }
#endif

		return status;
	}
}

struct SocketExchange {
	SocketExchange() = default;

	SocketExchange(SOCKET& ConnectSocket)
		: ConnectSocket(ConnectSocket) {}

	static struct FileInfo {
		unsigned int fileLength = 0;
		unsigned int sendType = 0;
		char* file = nullptr;
	};
	static struct FileInfoNet {
		unsigned int fileLength = 0;
		unsigned int sendType = 0;
	};

	bool sendFileData(const char* data, int size, unsigned int sendType) {
		FileInfoNet fileData;
		fileData.sendType = sendType;
		fileData.fileLength = size;
		bool res1 = send(ConnectSocket, reinterpret_cast<const char*>(&fileData), sizeof(fileData), 0) != SOCKET_ERROR;
		bool res = send(ConnectSocket, data, fileData.fileLength, 0) != SOCKET_ERROR;
		return res && res1;
	}

	std::pair<unsigned int, std::vector<char>> recvFileData() {
		FileInfoNet fileData;
		if (readNBytes(ConnectSocket, &fileData, sizeof(FileInfoNet))) {
			if (fileData.fileLength <= 1000000) {
				std::vector<char> buf(fileData.fileLength);
				if (readNBytes(ConnectSocket, buf.data(), fileData.fileLength)) {
					return std::pair<unsigned int, std::vector<char>>(fileData.sendType, buf);
				}
				else {
					return std::pair<unsigned int, std::vector<char>>(false, std::vector<char>());
				}
			}
			else {
				std::cout << "Size too big to download (" << fileData.fileLength << " bytes, (d)dos protection)" << std::endl;
				return std::pair<unsigned int, std::vector<char>>(false, std::vector<char>());
			}
		}
		else {
			return std::pair<unsigned int, std::vector<char>>(false, std::vector<char>());
		}
	}

private:
	SOCKET& ConnectSocket;
};

struct WebExchange {
	WebExchange() = default;
	WebExchange(SOCKET& ConnectSocket)
		: ConnectSocket(ConnectSocket) {}

	struct Webpacket {
		Webpacket()
		{
			text = new char[WEB_BUFFER_SIZE];
		}
		~Webpacket()
		{
			delete[] text;
		}
		char* text;
	};

	bool send(const char* text, int len) {
		return ::send(ConnectSocket, text, len, 0);
	}

	int recv(Webpacket& wp) {
		return ::recv(ConnectSocket, wp.text, WEB_BUFFER_SIZE, 0);
	}

private:
	SOCKET& ConnectSocket;
};

namespace {
	void sendQuit(SOCKET socket) {
		SocketExchange exch(socket);
		exch.sendFileData("", 1u, 1u);
	}
}

class SocketClient {
public:
	SocketClient() 
		: exch(ConnectSocket) {
		ZeroMemory(&hints, sizeof(addrinfo));
	}

	~SocketClient() {
		if(isOpen) sendQuit(ConnectSocket);
	}

	int open(const char* hostname, const char* port = "27015") {
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		iResult = getaddrinfo(hostname, port, &hints, &result);
		if (iResult != 0) {
			freeaddrinfo(result);
			return 1;
		}
		ConnectSocket = INVALID_SOCKET;
		ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			freeaddrinfo(result);
			return 2;
		}
		iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			return 3;
		}
		freeaddrinfo(result);
		if (ConnectSocket == INVALID_SOCKET) {
			return 4;
		}
		SocketClient::isOpen = true;
		return 0;
	}

	int close() {
		if (isOpen) {
			isOpen = false;
			ZeroMemory(&hints, sizeof(addrinfo));
			return closesocket(ConnectSocket);
		}
		return 0;
	}

	bool isOpened() const {
		return isOpen;
	}

	int getWSAError() {
		return WSAGetLastError();
	}

	SocketExchange exch;

private:
	bool isOpen = false;
	int iResult = 0;
	SOCKET ConnectSocket = 0;
	struct addrinfo *result = 0,
					hints;
};

class SocketServer {
public:
	typedef void*(*SS_PROCESS_FUNC)(SOCKET&); // SS = SocketServer
	SocketServer() {
		ZeroMemory(&hints, sizeof(hints));
	}

	int open(const char* port = "27015") {
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
		iResult = getaddrinfo(NULL, port, &hints, &result);

		if (iResult != 0) return 1;
		ListenSocket = INVALID_SOCKET;
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) return 2;
		iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR) return 3;
		freeaddrinfo(result);
		return 0;
	}

	bool listen() {
		if (::listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(ListenSocket);
			return false;
		}
		return true;
	}

	SOCKET accept() {
		SOCKET ClientSocket = INVALID_SOCKET;
		ClientSocket = ::accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			closesocket(ListenSocket);
			return INVALID_SOCKET;
		}
		return ClientSocket;
	}

	int closeClient(SOCKET client) {
		closesocket(client);
		return 0;
	}

	void closeServer() {
		closesocket(ListenSocket);
	}

	SS_PROCESS_FUNC rFunc;

	bool close() {
		ZeroMemory(&hints, sizeof(hints));
	}

private:
	SOCKET ListenSocket = 0;
	int iResult = 0;
	struct addrinfo *result = nullptr, hints;
};

namespace WebClient {
	bool BasicInit() {
#ifdef _WIN32
		WSAData wsData;
		int result;
		result = WSAStartup(MAKEWORD(2, 2), &wsData);
		return result == 0;
#else
		return 0;
#endif
	}

	bool BasicDestroy() {
#ifdef _WIN32
		WSACleanup();
#endif // _WIN32
		return true;
	}

	static bool sentConnectedRequest = false;

	void processConnectPacket(SocketExchange& exch) {
		if (!sentConnectedRequest) {
			exch.sendFileData("", 0, 1);
		}
	}

	bool sendIsConnectedRequest(SocketExchange& exch) {
		sentConnectedRequest = true;
		return exch.sendFileData("", 0, 1);
	}
}