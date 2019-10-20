#pragma once

#include "CommandLib.h"
#include "Configurable.h"
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
#include "extern/rlutil.h"

namespace {
	bool isValid(Argument& argument) {
		return argument.validation == Argument::ArgumentValidation::VALID;
	}
}
namespace ConfigData {

	ConfigObject dict;

	inline bool fExists(const char* path) {
		struct stat buffer;
		return stat(path, &buffer) != 0;
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
		dict.setType("object").setWriteable(false);
		dict["user"].setWriteable(false).setReadable(false).setType("object").setDescription("User Object for setting usual user stuff up")
			["username"].setValue("none");

		(dict["user"]["password"] = "-").setDescription("The Password Variable. It gets hashed and safed (Not decryptable exept planty years of bruteforce)").setReadable(false).setType("hash");
		dict["server"].setWriteable(false).setReadable(false).setType("object")["default"]
			.setWriteable(false).setReadable(false).setType("object")["autopost"].setType("bool") = "false";
		dict["server"]["default"]["notepassword"].setType("bool") = "false";
		dict["server"]["default"]["extra"].setReadable(false).setWriteable(false).setType("object")["debug"]
					.setReadable(false).setWriteable(false).setType("object")["server"].setType("bool") = "false";
		dict["server"]["default"]["extra"]["debug"]["client"].setType("bool") = "false";
		(dict["security"].setWriteable(false).setReadable(false).setType("object")["note_pc"].setType("bool") = "false").setWriteable(false);
		dict["settings"].setWriteable(false).setReadable("false").setType("object")["overwrite_on_new_version"].setType("bool") = "true";
		dict["settings"]["custom_commandline"].setType("bool")
			.setDescription("Custom Commandline in Blogpost Console") = "true";
		ConfigData::setStdObject(dict["command"])["push"]["default"]["fill_rest_config"].setType("bool").setDescription("Set to true if rest of command should be filled by config") = "false";
		dict["command"]["push"]["custom"]["connectTo"].setDescription("Standard connect if no connect argument is defined") = "none";
		dict["command"]["default"].setDescription("General Stuff that can be set to all commands")["fill_rest_config"]
			.setDescription("Set to true if rest of command should be filled by config").setType("boolean") = "false";
		setStdObject(dict["command"]["default"]);
		ConfigData::setStdObject(dict["command"]["push"]);
		ConfigData::setStdObject(dict["command"]["push"]["default"]);
		ConfigData::setStdObject(dict["command"]["push"]["custom"]);
		dict["command"].setDescription("Command Object to change standard command settings")["push"].setDescription("Push Command for releasing posts")["default"].setDescription("Standard settings to change");
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

	ConfigObject highlightPath(std::string& path) {
		std::vector<std::string> split = _split(path, ".");
		ConfigObject cfgObj;
		bool valid = true;
		std::string np = "";
		for (int i = 0; i < split.size(); i++) {
			np += "." + split[i];
			cfgObj = Configurable::getPath(dict, np.substr(1));
			if (valid && cfgObj.validation == ConfigObject::Validation::VALID) {
				rlutil::setColor(rlutil::LIGHTRED);
			}
			else {
				if (!valid) {
					rlutil::setColor(rlutil::RED);
				}
				else {
					rlutil::setColor(rlutil::MAGENTA);
				}
				valid = false;
			}
			printf((split[i]).c_str());
			rlutil::setColor(rlutil::GREY);
			printf(".");
		}
		printf("\b ");
		return cfgObj;
	}

	void highlightValue(std::string& value, ConfigObject& cfgObj) {
		if (cfgObj.getName() != "-") { // Path is already written
			if (!cfgObj.isWriteable()) {
				rlutil::setColor(rlutil::RED);
				printf("w~");
				rlutil::setColor(rlutil::GREY);
				printf((value + " ").c_str());
				return;
			}
			if (cfgObj.getType() == "string") {
				rlutil::setColor(rlutil::GREY);
				printf((value + " ").c_str());
			}
			else if(cfgObj.getType() == "bool") {
				if (value == "true" || value == "1" || value == "TRUE") {
					rlutil::setColor(rlutil::LIGHTRED);
					printf((value + " ").c_str());
				}
				else if (value == "false" || value == "0" || value == "FALSE") {
					rlutil::setColor(rlutil::LIGHTRED);
					printf((value + " ").c_str());
				}
				else {
					rlutil::setColor(rlutil::LIGHTMAGENTA);
					printf((value + " ").c_str());
				}
			}
			else if (cfgObj.getType() == "hash") {
				rlutil::setColor(rlutil::GREY);
				std::string star = "";
				for (int i = 0; i < value.size(); i++) {
					star += "*";
				}
				printf((star + " ").c_str());
			}
			else {
				rlutil::setColor(rlutil::GREEN);
				printf("~");
				rlutil::setColor(rlutil::GREY);
				printf((value + " ").c_str());
			}
		}
		else {
			rlutil::setColor(rlutil::GREY);
			printf((value + " ").c_str());
		}
	}

	void printNormal(std::string& out) {
		rlutil::setColor(rlutil::GREY);
		printf((out + " ").c_str());
	}
}

class ConfigCommand : public Command {
public:
	ConfigCommand()
		: Command("config")
	{
		ConfigData::Init();
	}

	bool highlightArgs(std::vector<std::string>& args) override {
		ArgumentList arguments(args);
		std::string out;
		if (arguments.noCll()) {
			rlutil::setColor(rlutil::RED);
			for (int i = 0; i < args.size(); i++) {
				printf((args[i] + " ").c_str());
			}
		}
		else {
			ConfigObject cfgObj;
			cfgObj.setName("-");
			Argument getArg = arguments.findBoth("g", "get");
			Argument setArg = arguments.findBoth("s", "set");
			Argument pathArg = arguments.findBoth("p", "path");
			for (int i = 0; i < arguments.getArgumentsRef().size(); i++) {
				Argument& arg = arguments.getArgumentsRef()[i];
				if (arg.isWrittenOut()) {
					if (arg.get() == "value") {
						if (setArg.validation == Argument::ArgumentValidation::VALID &&
							pathArg.validation == Argument::ArgumentValidation::VALID &&
							getArg.validation == Argument::ArgumentValidation::INVALID) {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
						else {
							if (getArg.validation == Argument::ArgumentValidation::VALID &&
								setArg.validation == Argument::ArgumentValidation::INVALID) {
									rlutil::setColor(rlutil::LIGHTRED);
							}
							else {
								rlutil::setColor(rlutil::RED);
							}
						}
						printf(("--" + arg.get() + " ").c_str());
						std::string input = "";
						if (arg.getArgRef().size() > 0) {
							for (int i = 0; i < arg.getArgRef().size(); i++) {
								input += arg.getArgRef()[i] + " ";
							}
							if (input.size() > 0) {
								input.erase(input.end() - 1);
							}
							ConfigData::highlightValue(input, cfgObj);
						}
						continue;
					}
					else if (arg.get() == "get") {
						if (setArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::RED);
						}
						else {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
					}
					else if (arg.get() == "set") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::RED);
						}
						else {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
					}
					else if (arg.get() == "path") {
						if (pathArg.getArgRef().size() == 1 && pathArg.getArgRef()[0] != "") {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
						else {
							rlutil::setColor(rlutil::LIGHTMAGENTA);
						}
					}
					else if (arg.get() == "varall") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "name") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "readable") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "writeable") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "type") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "description") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else if (arg.get() == "nosave") {
						if (setArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTRED);
						}
						else rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					else {
						rlutil::setColor(rlutil::LIGHTMAGENTA);
					}
					printf(("--" + arg.get() + " ").c_str());
				}
				else {
					if (arg.get() == "v") {
						if (setArg.validation == Argument::ArgumentValidation::VALID
							&& pathArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
						else {
							rlutil::setColor(rlutil::LIGHTMAGENTA);
						}
						std::string input = "";
						printf(("-" + arg.get() + " ").c_str());
						if (arg.getArgRef().size() > 0) {
							for (int i = 0; i < arg.getArgRef().size(); i++) {
								input += arg.getArgRef()[i] + " ";
							}
							if (input.size() > 0) {
								input.erase(input.end() - 1);
							}
							ConfigData::highlightValue(input, cfgObj);
						}
						continue;
					}
					else if (arg.get() == "a") {
						rlutil::setColor(rlutil::LIGHTRED);
					}
					else if (arg.get() == "g") {
						if (setArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::RED);
						}
						else {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
					}
					else if (arg.get() == "s") {
						if (getArg.validation == Argument::ArgumentValidation::VALID) {
							rlutil::setColor(rlutil::RED);
						}
						else {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
					}
					else if (arg.get() == "p") {
						if (pathArg.getArgRef().size() == 1 && pathArg.getArgRef()[0] != "") {
							rlutil::setColor(rlutil::LIGHTBLUE);
						}
						else {
							rlutil::setColor(rlutil::LIGHTMAGENTA);
						}
					}
					else {
						rlutil::setColor(rlutil::MAGENTA);
					}
					printf(("-" + arg.get() + " ").c_str());
				}
				
				for (int y = 0; y < arg.getArgRef().size(); y++) {
					std::string argRef = arg.getArgRef()[y];
					if (y == 0) {
						if (arg.isWrittenOut()) {
							if (arg.get() == "path") {
								cfgObj = ConfigData::highlightPath(argRef);
							}
							else ConfigData::printNormal(argRef);
						}
						else {
							if (arg.get() == "p") {
								cfgObj = ConfigData::highlightPath(argRef);
							}
							else ConfigData::printNormal(argRef);
						}
					}
					else {
						ConfigData::printNormal(argRef);
					}
				}
			}

		}
		printf("\b");
		return true;
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