#pragma once

#include "Logger.h"

#include <vector>

#include <Configurable.h>
#include <WebClient.h>

#include "RequestLib.h"
#include <thread>
#include <atomic>
#include <future>

#include "ConfigCommand.h"

namespace {
	SocketServer ss;
}

void serveClientRequests(SOCKET client) {
	SocketExchange exch(client);
	std::pair<int, std::vector<char>> data;
	while (true) {
		std::pair<unsigned int, std::vector<char>> data = exch.recvFileData();
		switch (data.first) {
		case false:
			if (Logger::isLogged(client)) {
				std::cout << Logger::getUserDisplayName(Logger::getUser(client).second) << " disconnected." << std::endl;
				Logger::logout(client);
			}
			else {
				std::cout << "Unknown client disconnected." << std::endl;
			}
			ss.closeClient(client);
			return;
		case 1: // Connection Pack (needed for remote logoff)
			std::cout << "Unknown client is calling for remote logoff." << std::endl;
			ss.closeClient(client);
			break;
		case 2: // Netcheck
		{
			std::pair<bool, std::string> pair = Logger::getUser(client);
			if (pair.first) {
				std::cout << "Ping from " << pair.second << "." << std::endl;
			}
			else {
				std::cout << "Ping from unknown client - " << client << std::endl;
			}
			exch.sendFileData("", 1, 2);
			break;
		}
		case 10: // Config Object Packet
		{
			{
				ConfigObject cfgObj;
				std::string str(data.second.begin(), data.second.end());
				std::stringstream strstream;
				strstream << str;
				Configurable::StreamToObj(cfgObj, strstream);
				std::thread worker(RequestLib::processCfgObj, cfgObj, client);
				worker.detach();
			}
		}
		break;
		default:
			std::cout << "Unknown Protocol (probably higher version), disconnecting client" << std::endl;
			ss.closeClient(client);
			break;
		}
	}
}

void serveClient(SOCKET client) {
	if (Configurable::getBool(ConfigData::dict["clientCommunicator"]["activated"])) {
		std::thread clientReq(serveClientRequests, client);
		clientReq.join();
	}
	else {
		std::cout << "Communicator off, blocking request..." << std::endl;
		ss.closeClient(client);
	}
}

void serve() {
	if (ss.open() == 0) {
		RankSystem::init();
		Logger::initStdRanks(); // TODO At end replace with file loader
		UserLogger::registerUtilities();
		SetConsoleTitleA("BlogPost Server - Running");
		std::cout << "BlogPost Server started syncrounously." << std::endl;
		while (ss.listen()) {
			SOCKET client = ss.accept();
			if (client != INVALID_SOCKET) {
				std::cout << "New Client connected." << std::endl;
				std::thread serveCl(serveClient, client);
				serveCl.detach();
			}
			else {
				SetConsoleTitleA("BlogPost Server - Closed");
				std::cout << "Client cannot connect because it is invalid." << std::endl;
			}
		}
	}
	else {
		std::cout << "Server had problems with starting, aborting..." << std::endl;
	}
}