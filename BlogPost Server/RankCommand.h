#pragma once

#include <CommandLib.h>
#include <string>
#include <vector>
#include <iostream>
#include "Logger.h"
#include "RankSystem.h"

namespace {
	bool isValid(const Argument& arg) {
		return arg.validation == Argument::ArgumentValidation::VALID;
	}
}

class RankCommand : public Command {
public:
	RankCommand()
		: Command("rank") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);

		Argument rankArg = arguments.findBoth("r", "rank");
		Argument permissionArg = arguments.findBoth("p", "permission");
		Argument addArg = arguments.findBoth("a", "add");
		Argument removeArg = arguments.findBoth("rm", "remove");
		Argument valueArg = arguments.findBoth("v", "value");
		Argument showArg = arguments.findBoth("s", "show");

		if (isValid(showArg)) {
			if (RankSystem::rankDict.size() > 0) {
				for (std::pair<std::string, uint64_t> pair : RankSystem::rankDict) {
					std::cout << std::endl << "Rank: "  << pair.first << std::endl << "Permissions: " << RankSystem::parse(pair.second) << std::endl;
				}
			}
			else {
				std::cout << "No Ranks registered." << std::endl;
			}
			return;
		}

		if (isValid(rankArg) || isValid(permissionArg)) {
			if (isValid(addArg) && !isValid(removeArg)) {
				if (isValid(rankArg) && !isValid(permissionArg)) {
					// rank -a -r
					if (rankArg.getArgRef().size() == 1) {
						std::string rankStr = rankArg.getArgRef()[0];
						std::transform(rankStr.begin(), rankStr.end(), rankStr.begin(), std::tolower);
						RankSystem::addRank(rankStr.c_str(), RankSystem::PERMISSION_NONE);
						std::cout << "Rank added." << std::endl;
						return;
					}
					else {
						std::cout << "Rank parameter missing." << std::endl;
						return;
					}
				}
				else if (isValid(permissionArg) && isValid(rankArg)) {
					if (permissionArg.getArgRef().size() == 1) {
						if (rankArg.getArgRef().size() == 1) {
							uint64_t permission = RankSystem::parse(permissionArg.getArgRef()[0].c_str());
							if (permission != 0) {
								std::string rankStr = rankArg.getArgRef()[0];
								std::transform(rankStr.begin(), rankStr.end(), rankStr.begin(), std::tolower);
								if (RankSystem::addRankPermission(rankStr.c_str(), permission)) {
									std::cout << "Permission added." << std::endl;
									return;
								}
								else {
									std::cout << "Rank not found." << std::endl;
									return;
								}
							}
							else {
								std::cout << "Permission not found." << std::endl;
								return;
							}
						}
						else {
							std::cout << "Rank parameter is missing" << std::endl;
							return;
						}
					}
					else {
						std::cout << "Permission parameter is missing" << std::endl;
						return;
					}
				}
				else {
					std::cout << "Please choose the name or permission argument" << std::endl;
				}
			}
			else if (isValid(removeArg) && !isValid(addArg)) {
				if (isValid(rankArg) && !isValid(permissionArg)) {
					if (rankArg.getArgRef().size() == 1) {
						std::string rankStr = rankArg.getArgRef()[0];
						std::transform(rankStr.begin(), rankStr.end(), rankStr.begin(), std::tolower);
						if (RankSystem::removeRank(rankStr.c_str())) {
							std::cout << "Rank removed." << std::endl;
						}
						else {
							std::cout << "Rank doesn't exist." << std::endl;
						}
						return;
					}
					else {
						std::cout << "Rank parameter missing." << std::endl;
						return;
					}
				}
				else if (isValid(permissionArg) && isValid(rankArg)) {
					if (permissionArg.getArgRef().size() == 1) {
						if (rankArg.getArgRef().size() == 1) {
							uint64_t permission = RankSystem::parse(permissionArg.getArgRef()[0].c_str());
							if (permission != 0) {
								std::string rankStr = rankArg.getArgRef()[0];
								std::transform(rankStr.begin(), rankStr.end(), rankStr.begin(), std::tolower);
								if (RankSystem::removeRankPermission(rankStr.c_str(), permission)) {
									std::cout << "Permission removed." << std::endl;
									return;
								}
								else {
									std::cout << "Rank not found." << std::endl;
									return;
								}
							}
							else {
								std::cout << "Permission not found." << std::endl;
								return;
							}
						}
						else {
							std::cout << "Rank parameter is missing" << std::endl;
							return;
						}
					}
					else {
						std::cout << "Permission parameter is missing" << std::endl;
						return;
					}
				}
				else {
					std::cout << "Please choose the name or permission argument" << std::endl;
				}
			}
			else {
				std::cout << "Please choose to add or remove" << std::endl;
				return;
			}
		}
		else {
			std::cout << "Command needs a rank or permission argument" << std::endl;
			return;
		}
	}
};