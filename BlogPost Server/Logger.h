#pragma once

#include <WebClient.h>
#include "RequestLib.h"
#include <Configurable.h>
#include <algorithm>
#include <Encryptor.h>
#include <map>
#include "RankSystem.h"
#include <algorithm>
#include <Packet.h>

namespace Logger {
	ConfigObject LoggDict;
	std::map<std::string, std::string> userRankDict;
	std::map<SOCKET, std::string> loggedSockets;

	uint64_t getUserPermissions(std::string user) {
		if (userRankDict.count(user.c_str()) > 0) {
			return RankSystem::getRankPermissions(userRankDict[user.c_str()].c_str());
		}
		else return 0;
	}

	std::string getUserDisplayName(std::string name) {
		return static_cast<char>(std::toupper(name[0])) + name.substr(1);
	}

	bool userExists(const char* user) {
		return LoggDict.get().count(user) > 0;
	}

	std::pair<bool, std::string> getUser(SOCKET client) {
		if (loggedSockets.count(client)) {
			return std::pair<bool, std::string>(true, loggedSockets[client]);
		}
		else return std::pair<bool, std::string>(false, "");
	}

	SOCKET getSocket(std::string user) {
		for (std::pair<SOCKET, std::string> pair : loggedSockets) {
			if (pair.second == user) return pair.first;
		}
		return INVALID_SOCKET;
	}

	std::pair<bool, std::string> getUserRank(const char* user) {
		if (userRankDict.count(user) > 0) {
			return std::pair<bool, std::string>(true, userRankDict[user]);
		}
		else return std::pair<bool, std::string>(false, "");
	}

	void setRank(const char* user, const char* rank) {
		userRankDict[user] = rank;
	}

	void initStdRanks() {
		RankSystem::addRank("admin", RankSystem::PERMISSION_ALL);
		RankSystem::addRank("member", RankSystem::PERMISSION_POST_BLOG);
		RankSystem::addRank("connector", RankSystem::PERMISSION_NONE);
	}

	bool registerUser(const char* username, const char* hash) {
		if (LoggDict.get().count(username) <= 0) {
			LoggDict[username].setValue(username)["hash"] = hash;
			LoggDict[username]["id"] = hash;
			return true;
		}
		else return false;
	}

	bool isLogged(std::string username) {
		for (std::pair<SOCKET, std::string> pair : loggedSockets) {
			if (pair.second == username)
				return true;
		}
		return false;
	}

	bool isLogged(SOCKET client) {
		return loggedSockets.count(client) > 0;
	}

	void logout(SOCKET client) {
		loggedSockets.erase(client);
	}

	bool checkUser(const char* username, const char* hash) {
		if (!isLogged(username)) {
			if (LoggDict.get().count(username) > 0) {
				if (LoggDict[username]["hash"].getValue() == hash) {
					return true;
				}
			}
		}
		return false;
	}

	bool eraseUser(const char* username)
	{
		if (userExists(username))
		{
			Logger::LoggDict.get().erase(username);
			SOCKET clientLogoff = Logger::getSocket(username);
			if (clientLogoff != INVALID_SOCKET)
				loggedSockets.erase(clientLogoff);
			std::pair<bool, std::string> rankPair = Logger::getUserRank(username);
			if (rankPair.first)
				Logger::userRankDict.erase(username);
			return true;
		}
		else return false;
	}
}

namespace UserLogger {
	void RegisterRequest(SOCKET client, ConfigObject& cfgObj, ConfigObject& earned) {
		if (cfgObj.get().count("username") > 0) {
			if (cfgObj.get().count("hash") > 0) {
				std::string username = cfgObj["username"].getValue();
				if (username.size() < 6) {
					earned["logger:user:register"].setValue("error")["message"].setValue("Username too short");
					return;
				}
				std::transform(username.begin(), username.end(), username.begin(), std::tolower);
				if (Logger::registerUser(username.c_str(), cfgObj["hash"].getValue().c_str())) {
					earned["logger:user:register"].setValue("1");
					Logger::loggedSockets[client] = username;
					std::cout << "Client registered as " << username << "." << std::endl;
				}
				else {
					earned["logger:user:register"].setValue("error")["message"].setValue("Username already taken");
				}
			}
		}
	}

	void LoginRequest(SOCKET client, ConfigObject& cfgObj, ConfigObject& earned) {
		if (cfgObj.get().count("username") > 0) {
			if (cfgObj.get().count("hash") > 0) {
				std::string username = cfgObj["username"].getValue();
				std::transform(username.begin(), username.end(), username.begin(), std::tolower);
				if (Logger::checkUser(username.c_str(), cfgObj["hash"].getValue().c_str())) {
					earned["logger:user:login"].setValue("1");
					std::cout << "Client logged in as " << username << "." << std::endl;
					Logger::loggedSockets[client] = username;
					return;
				}
			}
		}
		earned["logger:user:login"].setValue("0");
	}

	void UserEraseRequest(SOCKET client, ConfigObject& cfgObj, ConfigObject& earned) {
		std::pair<bool, std::string> userPair = Logger::getUser(client);
		if (userPair.first) {
			std::string username = userPair.second;
			std::transform(username.begin(), username.end(), username.begin(), std::tolower);
			uint64_t permission = Logger::getUserPermissions(username);
			std::string username2 = cfgObj["username"].getValue();
			std::transform(username2.begin(), username2.end(), username2.begin(), std::tolower);
			if (permission & RankSystem::PERMISSION_MANAGE_ACCOUNTS) {
				if (Logger::userExists(username2.c_str())) {
					if (userPair.second != username2) {
						Logger::LoggDict.get().erase(username2);
						SOCKET clientLogoff = Logger::getSocket(username2);
						if (clientLogoff != INVALID_SOCKET) {
							ConfigObject messagePacket;
							ConfigObject packetList;
							Packet::CreateMessagePacket(messagePacket);
							Packet::AddMessageToPacket(messagePacket, "Your account has been deleted, please contact the owners for more details.");
							PacketList::add(packetList, messagePacket);
							SocketExchange exch(clientLogoff);
							Configurable::netSend(packetList, exch);
							Logger::loggedSockets.erase(clientLogoff);
						}
						std::pair<bool, std::string> rankPair = Logger::getUserRank(username2.c_str());
						if (rankPair.first) {
							Logger::userRankDict.erase(username2);
						}

						ConfigObject messagePacket;
						ConfigObject packetList;
						Packet::CreateMessagePacket(messagePacket);
						Packet::AddMessageToPacket(messagePacket, "Account has been deleted.");
						PacketList::add(earned, messagePacket);
						std::cout << "User Data erased." << std::endl;
					}
					else {
						ConfigObject messagePacket;
						Packet::CreateMessagePacket(messagePacket);
						Packet::AddMessageToPacket(messagePacket, "You cannot delete your own account. Sending message in Server log, to process action manually.");
						PacketList::add(earned, messagePacket);

						std::cout << userPair.second << " tried to delete his own account." << std::endl;
					}
				}
				else {
					ConfigObject messagePacket;
					Packet::CreateMessagePacket(messagePacket);
					Packet::AddMessageToPacket(messagePacket, "User does not exist: " + cfgObj["username"].getValue());
					PacketList::add(earned, messagePacket);
				}
			}
			else {
				ConfigObject messagePacket;
				Packet::CreateMessagePacket(messagePacket);
				Packet::AddMessageToPacket(messagePacket, "Not enough permissions!");
				PacketList::add(earned, messagePacket);

				std::cout << userPair.second << " tried to delete an account!" << std::endl;
			}
		}
		else {
			ConfigObject messagePacket;
			Packet::CreateMessagePacket(messagePacket);
			Packet::AddMessageToPacket(messagePacket, "You are not logged in!");
			PacketList::add(earned, messagePacket);
		}
	}

	void registerUtilities() {
		RequestLib::registerFunction("logger:user:register", RegisterRequest);
		RequestLib::registerFunction("logger:user:login", LoginRequest);
		RequestLib::registerFunction("logger:user:erase", UserEraseRequest);
	}
}