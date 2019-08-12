#include <iostream>
#include <vector>
#include <string>
#include "CommandLib.h"
#include "CommandHandler.h"
#include "TestCommand.h"

#define EXIT_STRING "exit"
#define SUBCOMMAND_NF "Subcommand not found"
#define COMMAND_NF "Command not found"
#define ARGUMENT_INDICATOR "-"

int main(int argc, char** argv) {
	CommandHandler hCmd;
	hCmd.registerCommand(new TestCommand);

	Command* cmd;

	std::string input;

	if (argc == 1) {
		do
		{
			input = "";
			char c;
			while ((c = std::cin.get()) != '\n') {
				input += c;
			}
			std::vector<std::string> args = _split(input, " ");
			cmd = hCmd.getCommand(args[0].c_str());
			args.erase(args.begin());
			if (cmd != nullptr) {
				cmd->call(args);
			}
			else if (input != EXIT_STRING) {
				std::cout << COMMAND_NF << std::endl;
			}
		} while (input != EXIT_STRING);
	}
	else if (argc > 1) {
		std::vector<std::string> args(argv + 1, argv + argc);

		Command* cmd = hCmd.getCommand(args[0].c_str());
		if (cmd != nullptr) {
			args.erase(args.begin(), args.begin() + 1);
			cmd->call(args);
		}
		else {
			std::cout << SUBCOMMAND_NF << std::endl;
		}
	}
	return 0;
}