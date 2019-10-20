#pragma once

#include <jsapi.h>
#include "JSEnvBuilder.h"
#include <string>
#include <ConsoleHandler.h>

#define JSCONSOLE_PREFIX "JS -> "
#define JSCONSOLE_CHARACTER ""

namespace JSEngine
{
	namespace Console
	{
		static JSClassOps ConsoleOps =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr // Replace with finalize function
		};

		static JSClass Console =
		{
			"console",
			JSCLASS_HAS_PRIVATE,
			&ConsoleOps
		};

		bool ConsoleConstruct(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, JS_NewObjectForConstructor(cx, &Console, args));

			if (!obj) return false;

			// Add functions (log)

			args.rval().setObject(*obj);
			return false; // Console does not have a constructor. (const Console = new console(); is prohibited.)
		}

		bool ConsoleLog(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() > 0)
			{
				std::string output;
				for (unsigned int i = 0; i < args.length(); i++)
				{
					std::string msg(std::move(JS_EncodeString(cx, args[i].toString())));
					if(args[i].isString())
						output += msg;
				}
				std::cout << JSCONSOLE_CHARACTER << JSCONSOLE_PREFIX << (output.size() > 0 ? output : "-") << std::endl;
			}
			else
			{
				std::cout << JSCONSOLE_CHARACTER << JSCONSOLE_PREFIX << "-" << std::endl;
			}
			return true;
		}

		bool ConsoleWriteLine(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() > 0)
			{
				std::string output;
				for (unsigned int i = 0; i < args.length(); i++)
				{
					std::string msg(std::move(JS_EncodeString(cx, args[i].toString())));
					if (args[i].isString())
						output += msg;
				}
				std::cout << JSCONSOLE_CHARACTER << (output.size() > 0 ? output : "-") << std::endl;
			}
			else
			{
				std::cout << JSCONSOLE_CHARACTER << "-" << std::endl;
			}
			return true;
		}

		bool ConsoleClear(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() != 0) return false;
			system("cls");
			return true;
		}

		bool ConsoleGetLine(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			std::cout << JSCONSOLE_CHARACTER;
			if (args.length() == 1 && args[0].isString())
			{
				std::string msg(std::move(JS_EncodeString(cx, args[0].toString())));
				std::cout << msg;
				std::cout.flush();
			}
			std::string input = ConsoleHandler::getLineInput();
			std::cout << std::endl;
			args.rval().setString(JS_NewStringCopyN(cx, input.c_str(), input.size()));
			return true;
		}

		bool ConsoleWrite(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			std::string output;
			for (unsigned int i = 0; i < args.length(); i++)
			{
				std::string msg(std::move(JS_EncodeString(cx, args[i].toString())));
				if (args[i].isString())
					output += msg;
			}
			std::cout << (output.size() > 0 ? output : "-");
			std::cout.flush();
			return true;
		}

		bool ConsoleGetLineCensored(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			std::cout << JSCONSOLE_CHARACTER;
			if (args.length() == 1 && args[0].isString())
			{
				std::string msg(std::move(JS_EncodeString(cx, args[0].toString())));
				std::cout << msg;
				std::cout.flush();
			}
			std::string input = ConsoleHandler::getLineInputPassword();
			std::cout << std::endl;
			args.rval().setString(JS_NewStringCopyN(cx, input.c_str(), input.size()));
			return true;
		}

		bool Init(Environment::EnvironmentInformation* ei)
		{
			static JSFunctionSpec cnfn[] =
			{
				{"log", ConsoleLog, 0, 0},
				{"writeLine", ConsoleWriteLine, 0, 0},
				{"clear", ConsoleClear, 0, 0},
				{"getLine", ConsoleGetLine, 0, 0},
				{"getLineCensored", ConsoleGetLineCensored, 0, 0},
				{"write", ConsoleWrite, 0, 0},
				JS_FS_END
			};

			JS::RootedObject ConsoleBuild(ei->cx, JS_InitClass(ei->cx, ei->global, nullptr, &Console, &ConsoleConstruct, 0, nullptr, nullptr, nullptr, cnfn));
			return ConsoleBuild != nullptr;
		}
	}
}