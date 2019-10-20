#pragma once

#include <iostream>
#include "Configurable.h"

namespace ConnectionManager {
	SocketClient sc;

	typedef void(*RECEIVE_FUNCTION)(unsigned int, std::string);
	typedef void(*RECEIVE_CO_FUNCTION)(ConfigObject co);

	std::vector<RECEIVE_CO_FUNCTION> recvCOFunc;

	void clientNetRecv() {
		while (true) {
			std::pair<unsigned int, std::vector<char>> data = sc.exch.recvFileData();
			std::string str(data.second.begin(), data.second.end());
			switch (data.first) {
			case 0: // Server connection lost
				if (sc.isOpened()) {
					std::cout << "Server connection lost." << std::endl;
					sc.close();
				}
				return;
			case 1: // Remote disconnect
				std::cout << "Server disconnected." << std::endl;
				sc.close();
				return;
			case 2:
				std::cout << "Ping received." << std::endl;
				break;
			case 10: // ConfigObject packet
				std::stringstream strstream;
				strstream << str;
				ConfigObject cfgObj;
				Configurable::StreamToObj(cfgObj, strstream);
				if(cfgObj.get().count("recv:message") > 0) {
					int count = 1;
					while (cfgObj["recv:message"].get().count(std::to_string(count).c_str()) > 0) {
						rlutil::setColor(rlutil::GREY);
						std::cout << cfgObj["recv:message"][std::to_string(count).c_str()].getValue() << std::endl;
						count++;
					}
				}
				for (RECEIVE_CO_FUNCTION fRecv : recvCOFunc) {
					fRecv(cfgObj);
				}
				break;
			}
		}
	}

	void registerReceiveCOFunction(RECEIVE_CO_FUNCTION f) {
		recvCOFunc.push_back(f);
	}

	void unregisterReceiveCOFunction(RECEIVE_CO_FUNCTION f) {
		auto iterator = std::find(recvCOFunc.begin(), recvCOFunc.end(), f);
		if (iterator != recvCOFunc.end()) {
			recvCOFunc.erase(iterator);
		}
	}

	void threadRecv() {
		std::thread recvServer(clientNetRecv);
		recvServer.detach();
	}
}