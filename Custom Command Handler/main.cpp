/*
Copyright 2019 3l4ckL1ght

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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