#pragma once

#include <iostream>
#include <string>
#include <vector>
#ifndef __SERVER
#include "CommandHandler.h"
#endif
#include <conio.h>
#include "extern/rlutil.h"
#include "WebClient.h"

namespace ConsoleHandler {
	std::string getLineInput() {
		char character;
		std::string out;
		int charsWritten = 0;
		rlutil::setCursorVisibility(10);
		while ((character = _getch()) != 13) {
			if (character == 8) {
				if (charsWritten > 0) {
					printf("\b \b");
					out.erase(out.end() - 1);
					charsWritten--;
					continue;
				}
				else continue;
			}
			charsWritten++;
			std::cout << character;
			out += character;
		}
		//rlutil::setCursorVisibility(0);
		return out;
	}

	std::string getLineInputPassword() {
		char character;
		std::string out;
		int charsWritten = 0;
		rlutil::setCursorVisibility(10);
		while ((character = _getch()) != 13) {
			if (character == 8) {
				if (charsWritten > 0) {
					printf("\b \b");
					out.erase(out.end() - 1);
					charsWritten--;
					continue;
				}
				else continue;
			}
			charsWritten++;
			std::cout << '*';
			out += character;
		}
		return out;
	}

	BOOL WINAPI CtrlHandler(DWORD ctrlType) {
		switch (ctrlType) {
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			std::cout << "Called for shutdown. Blogpost is shutting down..." << std::endl;
			return true;
		default:
			return FALSE;
		}
	}
#ifndef __SERVER
	std::string getLineCommandHighlightConsole(const CommandHandler& hCmd) {
		std::string input;
		static std::vector<std::string> commandsExecutedBefore;
		std::vector<std::string> args;
		std::string eraseLine = "";
		int countLeftShifted = 0;
		int countUpShifted = 0;
		Command* cmd = nullptr;
		char c;
		rlutil::setCursorVisibility(10);
		while ((c = _getch()) != 13) {
			rlutil::setCursorVisibility(0);
			if (c == 8) {
				if (countLeftShifted < 1) {
					if (input.size() < 2) {
						input = "";
					}
					else input.erase(input.end() - 1);
				}
				else if (countLeftShifted < input.size()) {
					input.erase(input.end() - countLeftShifted - 1);
				}
			}
			else if (c == -32) {
				c = _getch();
				if (c == 75) {
					if (countLeftShifted < input.size()) {
						countLeftShifted++;
					}
				}
				else if (c == 77) {
					if (countLeftShifted > 0) {
						countLeftShifted--;
					}
				}
				else if (c == 72) {
					if (countUpShifted < commandsExecutedBefore.size()) {
						countUpShifted++;
						if (input.size() > commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size()) {
							std::string clear;
							for (int i = 0; i < input.size() - commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size(); i++) {
								clear += "\b";
							}
							for (int i = 0; i < input.size() - commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size(); i++) {
								clear += " ";
							}
							printf(clear.c_str());
						}
						input = *(commandsExecutedBefore.end() - countUpShifted);
					}
				}
				else if (c == 80) {
					if (countUpShifted > 1) {
						countUpShifted--;
						if (input.size() > commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size()) {
							std::string clear;
							for (int i = 0; i < input.size() - commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size(); i++) {
								clear += "\b";
							}
							for (int i = 0; i < input.size() - commandsExecutedBefore[commandsExecutedBefore.size() - countUpShifted].size(); i++) {
								clear += " ";
							}
							printf(clear.c_str());
						}
						input = *(commandsExecutedBefore.end() - countUpShifted);
					}
				}
			}
			else if (c == 3) {
				CtrlHandler(CTRL_C_EVENT);
				exit(0);
			}
			else {
				if (countLeftShifted < 1) {
					input += c;
				}
				else {
					input.insert(input.end() - countLeftShifted, c);
				}
			}
			args = _split(input, " ");
			int count = 0;
			printf("\r");
			std::vector<std::string> arguments(args.begin() + 1, args.end());
			bool handleByCommand = false;
			if (cmd != nullptr)
				handleByCommand = false;
			for (int i = 0; i < args.size(); i++) {
				std::string& arg = args[i];
				if (count > 0) {
					if (cmd != nullptr && cmd->highlightArgs(arguments)) {
						break;
					}
					if (_startsWith(arg, "--")) {
						rlutil::setColor(rlutil::LIGHTBLUE);
					}
					else if (_startsWith(arg, "-")) {
						rlutil::setColor(rlutil::LIGHTBLUE);
					}
					else if (arg == "true" || arg == "false" || arg == "TRUE" || arg == "FALSE") {
						rlutil::setColor(rlutil::LIGHTRED);
					}
					else {
						rlutil::setColor(rlutil::GREY);
					}
				}
				else {
					cmd = hCmd.getCommand(arg.c_str());
					if (cmd != nullptr) {
						rlutil::setColor(rlutil::YELLOW);
					}
					else {
						rlutil::setColor(rlutil::GREY);
					}
				}
				if ((args.size() - 1 > count) && !handleByCommand) {
					printf((arg + " ").c_str());
				}
				else printf(arg.c_str());
				++count;
			}
			printf("    \b\b\b\b");
			for (int i = 0; i < countLeftShifted; i++) {
				printf("\b");
			}
			rlutil::setCursorVisibility(10);
		}
		rlutil::setCursorVisibility(0);
		rlutil::setColor(rlutil::GREY);
		if(commandsExecutedBefore.size() == 0 || *(commandsExecutedBefore.end() - 1) != input)
			commandsExecutedBefore.emplace_back(input);
		std::cout << std::endl;
		return input;
	}
#endif
	void RegisterConsoleHandler() {
		SetConsoleCtrlHandler(CtrlHandler, true);
	}
}