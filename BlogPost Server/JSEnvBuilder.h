#pragma once

#include <jsapi.h>
#include <fstream>
#include <iostream>

namespace JSEngine {
	namespace Object
	{
		static JSClassOps GlobalClassOps = // https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey/JSAPI_reference/JSClass
		{
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

		static JSClass GlobalClass =
		{
			"global",
			JSCLASS_GLOBAL_FLAGS,
			&GlobalClassOps
		};
	}

	namespace Environment
	{
		/*
		EnvironmentInformation is an object that
		does pass information to worker functions.
		It should be created by calling CreateContext.
		*/
		struct EnvironmentInformation
		{
			EnvironmentInformation(JSContext* cx)
				: cx(cx), autoRequest(cx),
				global(cx, JS_NewGlobalObject(cx, &Object::GlobalClass, nullptr, JS::FireOnNewGlobalHook, options)),
				ac(cx, global) {}

			JSContext* cx;
			JS::RootedObject global;
			JSAutoRequest autoRequest;
			JS::CompartmentOptions options;
			JSAutoCompartment ac;
		};

		EnvironmentInformation CreateEnvironment(JSContext* cx)
		{
			if (!cx) return nullptr;
			if (!JS::InitSelfHostedCode(cx)) return nullptr;
			return {cx};
		}

		// Adds Classes like Math, Date and so on which are
		// in every browser and JavaScript environment
		bool AddEnvironmentStandardClasses(EnvironmentInformation* ei)
		{
			return JS_InitStandardClasses(ei->cx, ei->global);
		}

		namespace Error
		{
			void StdErrorHandler(JSContext* cx, JSErrorReport* report)
			{
				std::cout << "JSError ->\nFile: " << (report->filename ? report->filename : "[no filename]") << "\nLine: "
					<< static_cast<unsigned int>(report->lineno) << "\n" << report->exnType << std::endl;
			}
		}

		// No check if file exists
		// Returns nullptr if compiled successfully
		// Clean pointer if object received.
		bool CompileEnvironment(EnvironmentInformation* ei, const char* path)
		{
			std::ifstream file(path);
			int lines = std::count(std::istreambuf_iterator<char>(file),
				std::istreambuf_iterator<char>(), '\n');

			file.seekg(0, std::ios::beg);

			std::string scriptSrc((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			file.close();
			JS::CompileOptions opts(ei->cx);
			opts.setFileAndLine(path, lines);
			JS::RootedScript jsscript(ei->cx);
			if (!JS_CompileScript(ei->cx, scriptSrc.c_str(), scriptSrc.size(), opts, &jsscript)) return false;
			if (!JS_ExecuteScript(ei->cx, jsscript)) return false;
			JS_MaybeGC(ei->cx);
			return true;
		}

		void SetErrorReporter(EnvironmentInformation* ei, JS::WarningReporter reporter)
		{
			JS::SetWarningReporter(ei->cx, reporter);
		}

		void EnvironmentCleanup(EnvironmentInformation* ei)
		{
			
		}

	}
}