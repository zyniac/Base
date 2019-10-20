#pragma once
#include "CommandLib.h"
#include <vector>
#include <string>

class ClearCommand : public Command {
public:
	ClearCommand()
		: Command("clear") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);
		Argument arg = arguments.findBoth("i", "ignore");
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