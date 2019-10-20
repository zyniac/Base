#pragma once

#include <CommandLib.h>
#include <vector>
#include <string>
#include <iostream>

#include "RankSystem.h"
#include "Logger.h"

class UserCommand : public Command {
public:
	UserCommand()
		: Command("user") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);

		Argument rankArg = arguments.findBoth("r", "rank");
		Argument userArg = arguments.findBoth("u", "user");
		Argument seeArg = arguments.findBoth("s", "see");

		if (seeArg.validation == Argument::ArgumentValidation::VALID && rankArg.validation != Argument::ArgumentValidation::VALID) {
			if (userArg.validation == Argument::ArgumentValidation::VALID) {
				if (userArg.getArgRef().size() > 0) {
					std::string username = "";
					for (int i = 0; i < userArg.getArgRef().size() - 1; i++) {
						username += userArg.getArgRef()[i] + " ";
					}
					username += userArg.getArgRef()[userArg.getArgRef().size() - 1];
					uint64_t permission = Logger::getUserPermissions(username);
					if (Logger::LoggDict.get().count(username.c_str()) > 0) {
						std::pair<bool, std::string> pair = Logger::getUserRank(username.c_str());
						if (pair.first) {
							std::cout << "User: " << username << std::endl << "Rank: " << pair.second << std::endl
								<< "Permissions: " << RankSystem::parse(permission) << std::endl;
						}
						else {
							std::cout << "User has no rank" << std::endl;
						}
					}
					else {
						std::cout << "User does not exist." << std::endl;
					}
				}
				else {
					std::cout << "Username parameter is missing." << std::endl;
				}
			}
			else {
				std::cout << "User argument is missing." << std::endl;
			}
			return;
		}

		if (rankArg.validation == Argument::ArgumentValidation::VALID) {
			if (rankArg.getArgRef().size() == 1) {
				if (userArg.validation == Argument::ArgumentValidation::VALID) {
					if (userArg.getArgRef().size() > 0) {
						std::string username = "";
						for (int i = 0; i < userArg.getArgRef().size() - 1; i++) {
							username += userArg.getArgRef()[i] + " ";
						}
						username += userArg.getArgRef()[userArg.getArgRef().size() - 1];
						std::transform(username.begin(), username.end(), username.begin(), std::tolower);
						if (Logger::LoggDict.get().count(username.c_str()) > 0) {
							std::string rankStr = rankArg.getArgRef()[0];
							std::transform(rankStr.begin(), rankStr.end(), rankStr.begin(), std::tolower);
							if (RankSystem::rankExists(rankStr.c_str())) {
								Logger::setRank(username.c_str(), rankStr.c_str());
								std::cout << "User rank set." << std::endl;
								SOCKET client = Logger::getSocket(username);
								if (client != INVALID_SOCKET) {
									SocketExchange exch(client);
									ConfigObject messagePacket;
									ConfigObject packetList;
									Packet::CreateMessagePacket(messagePacket);
									Packet::AddMessageToPacket(messagePacket, "Your rank has been updated to " + rankStr);
									PacketList::add(packetList, messagePacket);
									Configurable::netSend(packetList, exch);
								}
							}
							else {
								std::cout << "Rank does not exist." << std::endl;
							}
						}
						else {
							std::cout << "Username not found." << std::endl;
						}
					}
					else {
						std::cout << "User Parameter missing." << std::endl;
					}
				}
				else {
					std::cout << "User Argument missing." << std::endl;
				}
			}
			else {
				std::cout << "Rank parameter (rank name) missing." << std::endl;
			}
		}
		else {
			std::cout << "Rank Argument missing." << std::endl;
		}
	}
};