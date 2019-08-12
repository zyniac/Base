#pragma once
#include "CommandLib.h"
#include <iostream>

class TestCommand : public Command {
public:
	TestCommand()
		: Command("test") {}

	int test = 0;

	void call(std::vector<std::string> args) override {
		ArgumentList argList(args);
		if (argList.noCll()) {
			std::cout << "Command has false spelling." << std::endl;
		}
		else {
			for (Argument arg : argList.getArgumentsRef()) {
				std::string argString = "";
				bool firstArg = true;
				for (std::string str : arg.getArgRef()) {
					if (!firstArg) argString += " and ";
					firstArg = false;
					argString += str;
				}
				if (arg.getArgRef().size() == 0) argString = "Nothing";
				std::cout << "Arg: " << arg.get() << "\nArguments from Argument: " << argString << "\nWritten Out: " << (arg.isWrittenOut() ? "yes" : "no") << std::endl;
			}
		}
	}
};