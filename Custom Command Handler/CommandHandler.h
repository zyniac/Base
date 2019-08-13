#pragma once
#include "CommandLib.h"

class CommandHandler {
public:
	CommandHandler() {}
	CommandHandler(const CommandHandler&) = delete;

	~CommandHandler() {
		destroyCommands();
	}

	Command* getCommand(const char* name) const {
		for (Command* cmd : commandList) {
			if (name == cmd->getName())
				return cmd;
		}
		return nullptr;
	}

	void registerCommand(Command* command) {
		commandList.push_back(command);
	}

private:
	std::vector<Command*> commandList;

	void destroyCommands() {
		for (Command* cmd : commandList) {
			delete cmd;
		}
	}
};