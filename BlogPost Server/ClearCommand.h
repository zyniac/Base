#pragma once
#include <CommandLib.h>
#include <vector>
#include <string>
#include "HTTP.h"

class ClearCommand : public Command {
public:
	ClearCommand()
		: Command("clear") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);
		Argument arg = arguments.findBoth("i", "ignore");
		Argument test = arguments.find("t", false);

		if (test.validation == Argument::ArgumentValidation::VALID) {
			if (test.getArgRef().size() == 1) {
				std::cout << HTTPNS::urlDecode(test.getArgRef()[0]) << std::endl;
				return;
			}
		}

		if (arg.validation == Argument::ArgumentValidation::VALID) {
			if (arg.getArgRef()[0] != "true" && arg.getArgRef()[0] != "1") {
				system("cls");
			}
		}
		else {
			system("cls");
		}
	}
};