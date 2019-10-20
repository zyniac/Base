#pragma once

#include <map>
#include <string>
#include <WebClient.h>

namespace RequestLib {
	typedef void(*WORKER_FUNCTION)(SOCKET, ConfigObject&, ConfigObject&);

	std::map <std::string, WORKER_FUNCTION> requestMap;

	void registerFunction(std::string str, WORKER_FUNCTION pFunc) {
		requestMap[str] = pFunc;
	}

	WORKER_FUNCTION getRequest(std::string search) {
		if (requestMap.count(search) > 0) {
			return requestMap[search];
		}
		else return nullptr;
	}

	template<class F, class... Args>
	void exeWorkerFunction(F&& f, Args&&... args) {
		auto wf = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		wf();
	}

	void processCfgObj(ConfigObject cfgObj, SOCKET socket) {
		ConfigObject earned;
		earned.setName("return");
		for (std::pair<std::string, ConfigObject> cfg : cfgObj.get()) {
			WORKER_FUNCTION processFunc = getRequest(cfg.second.getName());
			if (processFunc != nullptr) {
				try {
					processFunc(socket, cfg.second, earned);
				}
				catch (...) {
					std::cout << "Function failed." << std::endl;
					SocketExchange exch(socket);
					ConfigObject cfgO;
					cfgO.setName("error").setValue("*");
					Configurable::netSend(cfgO, exch);
				}
			}
			else {
				std::cout << "No Function for Request found. - " << cfg.second.getName() << std::endl;
			}
		}

		SocketExchange exch(socket);
		Configurable::netSend(earned, exch);
	}
}