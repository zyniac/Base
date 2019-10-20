#pragma once
#include <map>
#include <jsapi.h>
#include "JSEnvBuilder.h"
#include <thread>
#include "JSConsole.h"
#include "JSAccount.h"
#include "JSWebServer.h"
#include <filesystem>

#define PLUGIN_FOLDER "plugins"

namespace JSEngine
{
	namespace Launcher
	{
		namespace
		{
			bool _endsWith(const std::string& str, const std::string& end) {
				return str.substr(str.length() - end.length(), str.length()) == end;
			}
		}

		std::map<const char*, Environment::EnvironmentInformation*> envMap;
		namespace threads
		{
			int resultRP = 0;

			void RPFunc2(JSContext* cx, std::string path)
			{
				namespace jsenv = JSEngine::Environment;
				Environment::EnvironmentInformation* ei = new Environment::EnvironmentInformation(cx);
				envMap[path.c_str()] = ei;
				if (cx == nullptr)
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "Javascript Context status changed to invalid" << std::endl;
					envMap.erase(path.c_str());
					return;
				}
				if (!jsenv::AddEnvironmentStandardClasses(ei))
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "Standard Environment classes could not init in context" << std::endl;
					Environment::EnvironmentCleanup(ei);
					envMap.erase(path.c_str());
					return;
				}
				if (!JSEngine::Console::Init(ei))
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "Console (Blogpost Library) had problems at initiation" << std::endl;
					Environment::EnvironmentCleanup(ei);
					envMap.erase(path.c_str());
					return;
				}
				if (!JSEngine::Account::Init(ei))
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "Account (Blogpost Library) had problems at initiation" << std::endl;
					Environment::EnvironmentCleanup(ei);
					envMap.erase(path.c_str());
					return;
				}
				if (!JSEngine::WebServer::Init(ei))
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "WebServer (Blogpost Library) had problems at initiation" << std::endl;
					Environment::EnvironmentCleanup(ei);
					envMap.erase(path.c_str());
					return;
				}
				jsenv::SetErrorReporter(ei, jsenv::Error::StdErrorHandler);
				if (!jsenv::CompileEnvironment(ei, path.c_str()))
				{
					std::cout << "Environment creation failed for plugin " << path << std::endl;
					std::cout << "Compile Error. Type or Runtime error detected" << std::endl;
					Environment::EnvironmentCleanup(ei);
					envMap.erase(path.c_str());
					return;
				}
			}

			void RPFunc1(std::string path)
			{
				JSContext* cx = JS_NewContext(8L * 1024 * 1024);
				if (cx == nullptr)
				{
					std::cout << "Failed at creating context" << std::endl;
					return;
				}
				if (!JS::InitSelfHostedCode(cx))
				{
					std::cout << "Failed at creating context hoster" << std::endl;
					return;
				}
				RPFunc2(cx, path);
				return;
			}

			void DPFunc(Environment::EnvironmentInformation* ei, const char* path)
			{
				Environment::EnvironmentCleanup(ei);
				delete ei;
				envMap.erase(path);
			}
		}

		void RunPublicEnvironment(std::string path)
		{
			std::thread rpThread(threads::RPFunc1, path);
			rpThread.detach();
		}

		void DestroyPublicEnvironments()
		{
			for (std::map<const char*, Environment::EnvironmentInformation*>::iterator itr = envMap.begin(); itr != envMap.end(); itr++)
			{
				JSContext* cx = itr->second->cx;
				threads::DPFunc(itr->second, itr->first);
				JS_DestroyContext(cx);
			}
		}

		void RunPlugins()
		{
			namespace fs = std::filesystem;
			
			if (fs::is_directory(PLUGIN_FOLDER))
			{
				bool containsIncompatible = false;
				for (const auto& itr : fs::directory_iterator(PLUGIN_FOLDER))
				{
					if (_endsWith(itr.path().string(), ".js") || _endsWith(itr.path().string(), ".jsz"))
					{
						RunPublicEnvironment(itr.path().relative_path().lexically_normal().string());
					}
					else containsIncompatible = true;
				}
				if (containsIncompatible)
				{
					std::cout << "Please remove the non valid files out of the plugins folder" << std::endl;
				}
			}
			else
			{
				std::cout << "No plugin folder found. (This should not trigger, please contact a Zyniac developer)" << std::endl;
			}
		}
	}
}