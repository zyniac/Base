#pragma once

#include "Configurable.h"

namespace Packet {
	void RegisterPacket(ConfigObject& cfgObj, std::string username, std::string hash) {
		cfgObj.setName("logger:user:register");
		cfgObj["username"].setValue(username);
		cfgObj["hash"].setValue(hash);
	}

	void LoginPacket(ConfigObject& cfgObj, std::string username, std::string hash) {
		cfgObj.setName("logger:user:login");
		cfgObj["username"].setValue(username);
		cfgObj["hash"].setValue(hash);
	}

	void AccountErasePacket(ConfigObject& cfgObj, std::string username) {
		cfgObj.setName("logger:user:erase");
		cfgObj["username"].setValue(username);
	}

	void CreateMessagePacket(ConfigObject& cfgObj) {
		cfgObj.setName("recv:message");
	}

	void AddMessageToPacket(ConfigObject& cfgObj, std::string message) {
		int count = 1;
		while (cfgObj.get().count(std::to_string(count).c_str()) > 0) {
			++count;
		}

		cfgObj[std::to_string(count).c_str()].setValue(message);
	}
}

namespace PacketList {
	void add(ConfigObject& list, ConfigObject& packet) {
		list.get()[packet.getName().c_str()] = packet;
	}
}