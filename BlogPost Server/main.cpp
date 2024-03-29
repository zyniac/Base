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


#define VERSION 1 // Release Version
#define __SERVER // BlogPost Server definition for extern blogpost files
#define NOMINMAX // Windows no min() max() define for moz spider monkey (windows replaces functions with non valid code)

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS // For Mozilla Spider Monkeys deprecated functions in CXX 17

// Commands
#include "CommandHandler.h"
#include "CommandBase.h"
#include "ExitCommand.h"
#include "ConfigCommand.h"
#include "ClearCommand.h"
#include "RankCommand.h"
#include "UserCommand.h"
#include "RunCommand.h"
//#include "GenerateCommand.h"

// Utils
#include <WebClient.h>
#include <thread>
#include "RequestLib.h"
#include "BPServer.h"
#include "WebServer.h"

// JS Engine
#include "JSEngine.h"
#include "JSLauncher.h"

// Folders
#define WEB_FOLDER_NAME "web"
#define USERDATA_FOLDER_NAME "userdata"
#define CONFIG_FOLDER_NAME "configs"

// Adding filesystem to project
#if __has_include("filesystem")
#include <filesystem>
#elif __has_include("experimental/filesystem")
#include <experimental/filesystem>
#endif

namespace fs = std::filesystem;

int main(int argc, char** argv) {
	// Initialzing commands
	CommandHandler hCmd;
	hCmd.registerCommand(new ExitCommand);
	hCmd.registerCommand(new ConfigCommand);
	hCmd.registerCommand(new ClearCommand);
	hCmd.registerCommand(new RankCommand);
	hCmd.registerCommand(new UserCommand);
	hCmd.registerCommand(new RunCommand);
	//hCmd.registerCommand(new GenerateCommand);

	// File Initialisation
	if (!fs::is_directory(WEB_FOLDER_NAME)) {
		fs::create_directory(WEB_FOLDER_NAME);
	}
	if (!fs::is_directory(USERDATA_FOLDER_NAME)) {
		fs::create_directory(USERDATA_FOLDER_NAME);
	}
	if (!fs::is_directory(WEB_SPECIALCODES)) {
		fs::create_directory(WEB_SPECIALCODES);
	}
	if (!fs::is_directory(CONFIG_FOLDER_NAME)) {
		fs::create_directory(CONFIG_FOLDER_NAME);
	}
	if (!fs::is_directory(PLUGIN_FOLDER))
	{
		fs::create_directory(PLUGIN_FOLDER);
	}
	// System Initialisation
	JSEngine::BasicInit();
	WebClient::BasicInit(); // WebClient initializing eg. on windows winsock
	//SSLUtil::EasyInit(); // Init OpenSSL
	std::thread serveThr(serve); // BlogPost Server Thread
	std::thread webThr(WebServer::Process); // Web Server Thread

	JSEngine::Launcher::RunPlugins(); // js/jsz plugin loader

	// Keeping main thread alive / main process of program
	RunCmdBase(hCmd, argc, argv);

	// System Shutdown
	JSEngine::BasicDestroy();
	WebClient::BasicDestroy(); // Shutdown all netcode
	serveThr.~thread(); // Shutdown BlogPost Server
	webThr.~thread(); // Shutdown WebServer
	return 0;
}