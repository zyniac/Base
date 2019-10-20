#pragma once

#include <CommandLib.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <iostream>

#include <jsapi.h>

#include "JSEngine.h"
#include "JSEnvBuilder.h"
#include "JSConsole.h"
#include "JSAccount.h"
#include <thread>

static JSClassOps global_ops = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	JS_GlobalObjectTraceHook
};

static JSClass StdJSClass = {
	"global",
	JSCLASS_GLOBAL_FLAGS,
	&global_ops
};

class RunCommand : public Command
{
public:
	RunCommand()
		: Command("run") {}

	static int BuildJS(JSContext* cx, std::string& fileName)
	{
		namespace jsenv = JSEngine::Environment;
		jsenv::EnvironmentInformation ei = jsenv::CreateEnvironment(cx);
		if (cx == nullptr)
		{
			std::cout << "Context status has changed to invalid." << std::endl;
			return 1;
		}
		if (!jsenv::AddEnvironmentStandardClasses(&ei))
		{
			JSEngine::Environment::EnvironmentCleanup(&ei);
			std::cout << "Javascript environment could not add standard classes." << std::endl;
			return 2;
		}
		if (!JSEngine::Console::Init(&ei))
		{
			JSEngine::Environment::EnvironmentCleanup(&ei);
			std::cout << "Cannot init console" << std::endl;
			return 4;
		}
		if (!JSEngine::Account::Init(&ei))
		{
			JSEngine::Environment::EnvironmentCleanup(&ei);
			std::cout << "Cannot init Account" << std::endl;
			return 5;
		}
		jsenv::SetErrorReporter(&ei, jsenv::Error::StdErrorHandler);
		std::cout << "<--------------------------------------------------------------------------------------------------->" << std::endl;
		if (!jsenv::CompileEnvironment(&ei, fileName.c_str()))
		{
			std::cout << "|-----------------------------------|--------------------------------------------------------------->" << std::endl;
			std::cout << "|Error found. Environment destroyed.|" << std::endl;
			std::cout << "|-----------------------------------|" << std::endl;
			return 3;
		}
		std::cout << "<--------------------------------------------------------------------------------------------------->" << std::endl;
		
		JSEngine::Environment::EnvironmentCleanup(&ei); // Freeing unused working memory
		return 0;
	}

	static void RunThread(Argument runArg)
	{
		namespace jsenv = JSEngine::Environment;
		struct stat info;
		std::string& fileName = runArg.getArgRef()[0];
		stat(fileName.c_str(), &info);
		if (info.st_mode & S_IFREG)
		{
			JSContext* cx = JS_NewContext(8L * 1024 * 1024);
			if (cx == nullptr)
			{
				std::cout << "Javascript evironment could not be created." << std::endl;
				return;
			}
			if (BuildJS(cx, fileName) != 0) // New function to destroy objects before canvas
			{
				JS_DestroyContext(cx);
				return;
			}
			//jsenv::DestroyEnvironment(ei);
			JS_DestroyContext(cx);
			return;
		}
		else
		{
			std::cout << "File does not exist" << std::endl;
		}
	}

	void call(std::vector<std::string> args)
	{
		ArgumentList arguments(args);
		Argument runArg = arguments.findBoth("p", "path");

		if (runArg.validation == Argument::ArgumentValidation::VALID)
		{
			if (runArg.getArgRef().size() == 1)
			{
				std::thread runtime(RunThread, runArg);
				runtime.join();
			}
			else
			{
				std::cout << "Parameter is missing" << std::endl;
			}
		}
		else
		{
			std::cout << "Run Argument is missing" << std::endl;
		}
	}
};