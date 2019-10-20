#define VERSION 1
#define __CLIENT

#include "CommandHandler.h"
#include "TestCommand.h"
#include "CommandBase.h"
#include "ExitCommand.h"
#include "ConfigCommand.h"
#include "PushCommand.h"
#include "ClearCommand.h"
#include "WebClient.h"
#include "ConsoleHandler.h"


int main(int argc, char** argv) {

	CommandHandler hCmd;
	hCmd.registerCommand(new TestCommand);
	hCmd.registerCommand(new ExitCommand);
	hCmd.registerCommand(new ConfigCommand);
	hCmd.registerCommand(new PushCommand);
	hCmd.registerCommand(new ClearCommand);

	WebClient::BasicInit();
	RunCmdBase(hCmd, argc, argv);
	WebClient::BasicDestroy();
	return 0;
}