#pragma once

#include <CommandLib.h>
#include <Configurable.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cereal/types/map.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include "Encryptor.h"
#include <chrono>

namespace {
	bool isValid(Argument& argument) {
		return argument.validation == Argument::ArgumentValidation::VALID;
	}
}

namespace ConfigData {

	ConfigObject dict;

	inline bool fExists(const char* path) {
		struct stat buffer;
		return stat(path, &buffer) == 0;
	}

	ConfigObject& setStdObject(ConfigObject& object) {
		object.setWriteable(false);
		object.setReadable(false);
		object.setType("object");
		return object;
	}

	void Init() {
		if (/*fExists("ud.bin")*/false) {
			std::ifstream is("ud.bin", std::ios::binary);
			Configurable::StreamToObj(dict, is);
			is.close();
		}
		else {
			
		}
		dict.setName("confdict:v0.0.1");
		dict["clientCommunicator"].setType("object").setReadable(false).setWriteable(false)
			["activated"].setType("bool").setValue("true").setDescription("By closing the connector, this server will get invisible if a client wants to connect to it");
		dict["settings"].setType("object")["custom_commandline"].setType("bool").setDescription("Custom Command Linter built for Blogpost") = "false";
		dict["WebServer"].setType("object").setReadable(false).setWriteable(false)["fastSend"].setType("bool")
			.setDescription("This loads the content in ram and can send more than 1000 times faster, but the content is not getting reloaded") = "false";
		std::ofstream os("ud.bin", std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(dict);
		os.close();
	}
	void Load() {

	}
	void Save() {

	}
	bool isInvalid(const ConfigObject& co) {
		return co.validation == ConfigObject::Validation::INVALID;
	}
}

class ConfigCommand : public Command {
public:
	ConfigCommand()
		: Command("config")
	{
		ConfigData::Init();
	}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);
		if (!arguments.noCll()) {
			Argument setArg = arguments.findBoth("s", "set");
			Argument getArg = arguments.findBoth("g", "get");
			Argument allArg = arguments.findBoth("a", "all");
			Argument valueArg = arguments.findBoth("v", "value");
			Argument pathArg = arguments.findBoth("p", "path");
			if (isValid(setArg)) {
				if (!isValid(getArg)) {
					if (pathArg.validation == Argument::ArgumentValidation::VALID) {
						if (pathArg.getArgRef().size() == 1) {
							ConfigObject& co = Configurable::getPath(ConfigData::dict, pathArg.getArgRef()[0]);
							if (co.validation != ConfigObject::Validation::INVALID) {
								if (valueArg.validation == Argument::ArgumentValidation::VALID) {
									if (valueArg.getArgRef().size() > 0) {
										if (co.isWriteable()) {
											bool foundType = true;
											if (co.getType() == "string") {
												std::string str = "";
												for (std::string s : valueArg.getArgRef()) {
													str += " " + s;
												}
												str = str.substr(1);
												co.setValue(str);
											}
											else if (co.getType() == "bool") {
												std::string& str = valueArg.getArgRef()[0];
												if (str == "true" || str == "TRUE" || str == "1") {
													co.setValue("true");
												}
												else if(str == "FALSE" || str == "0" || str == "false") {
													co.setValue("false");
												}
												else {
													std::cout << "Input is not a boolean (true or false value)" << std::endl;
													foundType = false;
												}
											}
											else if (co.getType() == "hash") {
												std::string str = "";
												for (std::string s : valueArg.getArgRef()) {
													str += " " + s;
												}
												str = str.substr(1);
												co.setValue(str);
												std::chrono::time_point<std::chrono::system_clock> start, end;
												start = std::chrono::system_clock::now();
												std::string hash = Encryptor::hashEncrypt(str);
												end = std::chrono::system_clock::now();
												int elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
												std::cout << "Hash created. Took " << elapsed << " microseconds." << std::endl;
												co.setValue(hash);
											}
											else {
												std::cout << "Cannot change value" << std::endl;
												foundType = false;
											}
											if (foundType) {
												Argument nosaveArg = arguments.find("nosave", true);
												if (nosaveArg.validation == Argument::ArgumentValidation::VALID)
													std::cout << "Value set!" << std::endl;
												else {
													std::ofstream os("ud.bin", std::ios::binary);
													Configurable::ObjToStream(ConfigData::dict, os);
													os.close();
													std::cout << "Value set and saved!" << std::endl;
												}
											}
										}
										else {
											std::cout << "Variable cannot be changed" << std::endl;
										}
									}
									else {
										std::cout << "Value Argument need parameter" << std::endl;
										return;
									}
								}
								else {
									std::cout << "Value (-v) Argument is missing" << std::endl;
									return;
								}
							}
							else {
								std::cout << "Path not found" << std::endl;
								return;
							}
						}
						else {
							std::cout << "No Valid Path argument given on path" << std::endl;
						}
					}
					else {
						std::cout << "Set Argument needs a path (-p)" << std::endl;
					}
					return;
				}
				else {
					std::cout << "Please choose set or get" << std::endl;
					return;
				}
			}
			if (isValid(getArg)) {
				if (pathArg.validation == Argument::ArgumentValidation::VALID || allArg.validation == Argument::ArgumentValidation::VALID) {
					if (pathArg.getArgRef().size() == 1 || allArg.validation == Argument::ArgumentValidation::VALID) {
						ConfigObject& cfgObj = pathArg.getArgRef().size() == 0 ? ConfigData::dict
												: Configurable::getPath(ConfigData::dict, pathArg.getArgRef()[0]);
						if (cfgObj.validation != ConfigObject::Validation::INVALID) {
							Argument nameArg = arguments.find("name", true);
							Argument descriptionArg = arguments.find("description", true);
							Argument typeArg = arguments.find("type", true);
							Argument valueArg = arguments.find("value", true);
							Argument readArg = arguments.find("readable", true);
							Argument writeArg = arguments.find("writeable", true);
							Argument allVarArg = arguments.find("varall", true);
							unsigned int displayTypes = 0;
							if (isValid(nameArg))
								displayTypes |= CONFIG_GET_NAME;
							if (isValid(descriptionArg))
								displayTypes |= CONFIG_GET_DESCRIPTION;
							if (isValid(typeArg))
								displayTypes |= CONFIG_GET_TYPE;
							if (isValid(valueArg))
								displayTypes |= CONFIG_GET_VALUE;
							if (isValid(readArg))
								displayTypes |= CONFIG_GET_READABLE;
							if (isValid(writeArg))
								displayTypes |= CONFIG_GET_WRITEABLE;
							if (isValid(allVarArg))
								displayTypes |= CONFIG_GET_ALL;

							if (allArg.validation == Argument::ArgumentValidation::VALID) {
								if (displayTypes) {
									Configurable::ListConfigObjects(cfgObj, displayTypes);
								}
								else {
									Configurable::ListConfigObjects(cfgObj, CONFIG_GET_NAME | CONFIG_GET_DESCRIPTION);
								}
							}
							else {
								if (displayTypes) {
									Configurable::OutputConfigObject(cfgObj, displayTypes);
								}
								else {
									Configurable::OutputConfigObject(cfgObj, CONFIG_GET_NAME | CONFIG_GET_DESCRIPTION);
								}
							}
							return;
						}
						else {
							std::cout << "Path not found" << std::endl;
						}
					}
					else {
						std::cout << "Path needs one argument" << std::endl;
					}
				}
				else {
					std::cout << "Path argument is missing" << std::endl;
				}
				return;
			}
		}


		else {
			std::cout << "Arguments not written right, please try again" << std::endl;
		}
	}
};