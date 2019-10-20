#pragma once

#include "CommandLib.h"
#include "ConfigCommand.h"
#include "Configurable.h"
#include <vector>
#include <string>
#include "Encryptor.h"
#include "WebClient.h"
#include <thread>
#include "ConnectionManager.h"
#include "Packet.h"

namespace PushCommandNS {
	void receiveRegisterCallback(ConfigObject co) {
		if (co.get().count("logger:user:register") > 0) {
			if (co["logger:user:register"].getValue() == "1") {
				std::cout << "Client registered successfully." << std::endl;
				ConnectionManager::unregisterReceiveCOFunction(receiveRegisterCallback);
			}
			else {
				std::cout << "Client register error: " << co["logger:user:register"]["message"].getValue() << std::endl;
				ConnectionManager::unregisterReceiveCOFunction(receiveRegisterCallback);
			}
		}
	}

	void receiveLoginCallback(ConfigObject co) {
		if (co.get().count("logger:user:login") > 0) {
			if (co["logger:user:login"].getValue() == "1") {
				std::cout << "Client logged in." << std::endl;
				ConnectionManager::unregisterReceiveCOFunction(receiveLoginCallback);
			}
			else {
				std::cout << "Failed to login to server." << std::endl;
				ConnectionManager::unregisterReceiveCOFunction(receiveLoginCallback);
			}
		}
	}
}

class PushCommand : public Command {
public:
	PushCommand()
		: Command("push") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);
		Argument openArg = arguments.findBoth("o", "open");
		Argument closeArg = arguments.findBoth("c", "close");
		Argument sendArg = arguments.find("s", false);
		Argument receiveArg = arguments.find("r", false);
		Argument registerArg = arguments.findBoth("rg", "register");
		Argument loginArg = arguments.findBoth("lg", "login");
		Argument pingArg = arguments.findBoth("p", "ping");
		Argument accountEraseArg = arguments.findBoth("ae", "accounterase");

		if (openArg.validation == Argument::ArgumentValidation::VALID) {
			if (openArg.getArgRef().size() == 1) {
				if (ConnectionManager::sc.isOpened())
				{
					std::cout << "Disconnect first." << std::endl;
					return;
				}
				int code = 0;
				if ((code = ConnectionManager::sc.open(openArg.getArgRef()[0].c_str())) == 0) {
					std::cout << "Client is now open." << std::endl;
					ConnectionManager::threadRecv(); // Activating receive manager in another thread
				}
				else {
					std::cout << "Client cannot open. (error code: " << code << ")" << std::endl;
				}
			}
			else {
				std::cout << "open argument must take an url." << std::endl;
			}
		}

		if (accountEraseArg.validation == Argument::ArgumentValidation::VALID) {
			if (ConnectionManager::sc.isOpened()) {
				ConfigObject erasePacket;
				ConfigObject packetList;
				std::cout << "Username: ";
				std::string username = ConsoleHandler::getLineInput();
				std::cout << std::endl;
				Packet::AccountErasePacket(erasePacket, username);
				PacketList::add(packetList, erasePacket);
				Configurable::netSend(packetList, ConnectionManager::sc.exch);
			}
		}

		if (registerArg.validation == Argument::ArgumentValidation::VALID) {
			if (ConnectionManager::sc.isOpened()) {
				std::cout << "Username: ";
				std::string username = ConsoleHandler::getLineInput();
				std::cout << std::endl << "Password: ";
				std::string hash = Encryptor::hashEncrypt(ConsoleHandler::getLineInputPassword());
				std::cout << std::endl;
				ConfigObject list;
				ConfigObject packet;
				Packet::RegisterPacket(packet, username, hash);
				PacketList::add(list, packet);
				ConnectionManager::registerReceiveCOFunction(&PushCommandNS::receiveRegisterCallback);
				Configurable::netSend(list, ConnectionManager::sc.exch);
			}
			else {
				std::cout << "Client is not connected to Blogpost Server" << std::endl;
			}
		}

		if (loginArg.validation == Argument::ArgumentValidation::VALID) {
			if (ConnectionManager::sc.isOpened()) {
				std::cout << "Username: ";
				std::string username = ConsoleHandler::getLineInput();
				std::cout << std::endl << "Password: ";
				std::string hash = Encryptor::hashEncrypt(ConsoleHandler::getLineInputPassword());
				std::cout << std::endl;
				ConfigObject list;
				ConfigObject packet;
				Packet::LoginPacket(packet, username, hash);
				PacketList::add(list, packet);
				ConnectionManager::registerReceiveCOFunction(&PushCommandNS::receiveLoginCallback);
				Configurable::netSend(list, ConnectionManager::sc.exch);
			}
			else {
				std::cout << "Client is not connected to Blogpost Server" << std::endl;
			}
		}

		if (closeArg.validation == Argument::ArgumentValidation::VALID) {
			if (ConnectionManager::sc.isOpened()) {
				if (ConnectionManager::sc.close() == 0) {
					std::cout << "Client closed." << std::endl;
				}
				else std::cout << "Client already closed." << std::endl;
			}
			else {
				std::cout << "Client already closed." << std::endl;
			}
		}

		if (pingArg.validation == Argument::ArgumentValidation::VALID) {
			if (!ConnectionManager::sc.isOpened()) {
				std::cout << "Can't find any connection." << std::endl;
			}
			else {
				if (ConnectionManager::sc.exch.sendFileData("", 1, 2)) { // Ping Server
					std::cout << "Ping sent." << std::endl;
				}
				else {
					std::cout << "Connection refused." << std::endl;
				}
			}
		}

		if (sendArg.validation == Argument::ArgumentValidation::VALID) {
			ConnectionManager::sc.open("127.0.0.1");
			if (Configurable::netSend(ConfigData::dict, ConnectionManager::sc.exch)) {
				std::cout << "Sent successfully." << std::endl;
			}
			else {
				std::cout << "Error while sending." << std::endl;
			}
			ConnectionManager::sc.close();
		}

		if (receiveArg.validation == Argument::ArgumentValidation::VALID) {
			SocketServer ss;
			if (ss.open()) {
				if (ss.listen()) {
					SOCKET Client = ss.accept();
					if (Client != INVALID_SOCKET) {
						SocketExchange exch(Client);
						ConfigObject cfgObj;
						Configurable::netRecv(cfgObj, exch);
						std::cout << "Object received!" << std::endl;
						Configurable::ListConfigObjects(cfgObj, CONFIG_GET_ALL);
						ss.closeClient(Client);
					}
				}
			}
			ss.closeServer();
			std::cout << "Something went wrong." << std::endl;
		}
	}
};