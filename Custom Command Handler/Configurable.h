#pragma once
#include <map>
#include <string>
#include <sstream>

#include <cereal/types/map.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

#include "WebClient.h"

#define CONFIG_GET_VALUE 1
#define CONFIG_GET_WRITEABLE 1 << 1
#define CONFIG_GET_READABLE 1 << 2
#define CONFIG_GET_NAME 1 << 3
#define CONFIG_GET_DESCRIPTION 1 << 4
#define CONFIG_GET_TYPE 1 << 5
#define CONFIG_GET_ALL 0xFFFFFFFF

class ConfigObject {
public:
	enum class Validation {
		VALID, INVALID
	};

	ConfigObject() = default;

	ConfigObject(ConfigObject::Validation validation)
		: validation(validation) {}

	ConfigObject(std::string name)
		: name(name) {}

	ConfigObject& operator[](const char* string) {
		if (this->ConfigMap.count(string) <= 0) {
			this->ConfigMap.insert(std::pair<std::string, ConfigObject>(string, ConfigObject(string)));
		}
		return this->ConfigMap.find(string)->second;
	}

	std::map<std::string, ConfigObject>& get() {
		return this->ConfigMap;
	}

	ConfigObject& operator=(const char* string) {
		this->vvalue = string;
		return *this;
	}

	ConfigObject(const ConfigObject& cfgObj) {
		this->name = cfgObj.name;
		this->ConfigMap = cfgObj.ConfigMap;
		this->writeable = cfgObj.writeable;
		this->readable = cfgObj.readable;
		this->type = cfgObj.type;
		this->vvalue = cfgObj.vvalue;
		this->description = cfgObj.description;
	}

	ConfigObject& setName(const char* name) {
		this->name = name;
		return *this;
	}

	ConfigObject& setValue(std::string value) {
		this->vvalue = value;
		return *this;
	}

	std::string getValue() const {
		return this->vvalue;
	}

	std::string getName() const {
		return this->name;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar(ConfigMap, name, readable, writeable, vvalue, type, description);
	}

	ConfigObject& setWriteable(bool writeable) {
		this->writeable = writeable;
		return *this;
	}

	ConfigObject& setReadable(bool readable) {
		this->readable = readable;
		return *this;
	}

	bool isWriteable() const {
		return this->writeable;
	}

	bool isReadable() const {
		return this->readable;
	}

	bool isValid(std::string& str) const {
		return this->ConfigMap.count(str) > 0;
	}

	ConfigObject& setType(std::string type) {
		this->type = type;
		return *this;
	}

	std::string getType() const {
		return this->type;
	}

	operator bool() {
		return this->validation == Validation::VALID;
	}

	std::string getDescription() const {
		return this->description;
	}

	ConfigObject& setDescription(std::string description) {
		this->description = description;
		return *this;
	}

	Validation validation = Validation::VALID;
private:
	std::string name = "";
	std::string vvalue = "";
	bool writeable = true;
	bool readable = true;
	std::string type = "string";
	std::string description = "-";
	std::map<std::string, ConfigObject> ConfigMap;
};

namespace Configurable {
	ConfigObject& workPath(ConfigObject& cfgObj, std::vector<std::string>& str, uint64_t count) {
		static ConfigObject INVALID(ConfigObject::Validation::INVALID);
		if (count < str.size())
			if (cfgObj.isValid(str[count])) {
				++count;
				return workPath(cfgObj.get()[str[(count - 1)].c_str()], str, count);
			}
			else return INVALID;
		else return cfgObj;
	}
	ConfigObject& getPath(ConfigObject& obj, std::string str) {
		std::vector<std::string> splits = _split(str, ".");
		return workPath(obj, splits, 0);
	}

	static void OutputConfigObject(const ConfigObject& co, unsigned int configSettings, int width = 0) {
		std::string space = " ";
		for (int i = 0; i < width; i++) {
			space += "  |";
		}
		space += " ";
		std::cout << (width > 0 ? space.substr(0, space.size() - 1) : "") << "--------------" << std::endl;
		if (configSettings & CONFIG_GET_NAME) {
			std::cout << space << "Name        : " << co.getName() << std::endl;
		}
		if (configSettings & CONFIG_GET_DESCRIPTION) {
			std::cout << space << "Description : " << co.getDescription() << std::endl;
		}
		if (configSettings & CONFIG_GET_TYPE) {
			std::cout << space << "Type        : " << co.getType() << std::endl;
		}
		if (configSettings & CONFIG_GET_VALUE) {
			std::cout << space << "Value       : " << (co.isReadable() ? co.getValue() == "" ? "-" : co.getValue() : "-") << std::endl;
		}
		if (configSettings & CONFIG_GET_READABLE) {
			std::cout << space << "Readable    : " << (co.isReadable() ? "true" : "false") << std::endl;
		}
		if (configSettings & CONFIG_GET_WRITEABLE) {
			std::cout << space << "Writeable   : " << (co.isWriteable() ? "true" : "false") << std::endl;
		}
		std::cout << (width > 0 ? space.substr(0, space.size() - 1) : "") << "--------------" << std::endl;
	}

	static void ListConfigObjects(ConfigObject& co, unsigned int configSettings, int width = 0) {
		std::string space = " ";
		for (int i = 0; i < width + 1; i++) {
			space += "  |";
		}

		std::string spaceLast = space.substr(0, space.size() - 3);

		if (co.get().size() > 0) {
			std::cout << spaceLast << "->|" << std::endl;
			for (std::pair<std::string, ConfigObject> cfgObj : co.get()) {
				OutputConfigObject(cfgObj.second, configSettings, width + 1);
				ListConfigObjects(cfgObj.second, configSettings, width + 1);
			}
			std::cout << spaceLast << "<-|" << std::endl;
		}

	}

	std::string ObjToBytes(ConfigObject& cfgObj) {
		std::stringstream ss;
		cereal::BinaryOutputArchive archive(ss);
		archive(cfgObj);
		return ss.str();
	}

	void ObjToStream(ConfigObject& cfgObj, std::ostream& stream) {
		cereal::BinaryOutputArchive archive(stream);
		archive(cfgObj);
	}

	void StreamToObj(ConfigObject& cfgObj, std::istream& stream) {
		cereal::BinaryInputArchive archive(stream);
		archive(cfgObj);
	}

	bool netSend(ConfigObject& cfgObj, SocketExchange& exch) {
		std::string data(ObjToBytes(cfgObj));
		return exch.sendFileData(data.c_str(), data.size(), 10);
	}

	bool netRecv(ConfigObject& cfgObj, SocketExchange& exch) {
		std::pair<int, std::vector<char>> pair = exch.recvFileData();
		std::string str(pair.second.begin(), pair.second.end());
		if (pair.first == 10) {
			std::stringstream ss;
			ss << str;
			StreamToObj(cfgObj, ss);
			return true;
		}
		return false;
	}

	bool getBool(ConfigObject& cfgObj) {
		return cfgObj.getValue() == "true";
	}
}