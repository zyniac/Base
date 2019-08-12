#include <iostream>
#include <vector>
#include <string>
#include "CommandHandler.h"
#include "TestCommand.h"
#include "CommandBase.h"

int main(int argc, char** argv) {
	CommandHandler hCmd;
	hCmd.registerCommand(new TestCommand);
	RunCmdBase(hCmd, argc, argv);
	return 0;
}