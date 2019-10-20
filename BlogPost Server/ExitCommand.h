#pragma once

#include <CommandLib.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

class ExitCommand : public Command {
public:
	ExitCommand()
		: Command("exit") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);
		Argument timeArgument = arguments.findBoth("t", "time");
		if (timeArgument.validation == Argument::ArgumentValidation::VALID) {
			if (timeArgument.getArgRef().size() == 1) {
				int number = std::stoi(timeArgument.getArgRef()[0]);
				std::cout << "Waiting " << number << " seconds..." << std::endl;
				std::chrono::seconds timespan(number);
				std::this_thread::sleep_for(timespan);
				exit(0);
			}
			else {
				std::cout << "Arguments don't match" << std::endl;
				return;
			}
		}
		exit(0);
	}
};