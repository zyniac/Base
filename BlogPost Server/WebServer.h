#pragma once

#include <WebClient.h>
#include <thread>
#include <future>
#include <iostream>
#include <CommandLib.h>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <iterator>
#include <algorithm>
//#include "SSLUtil.h"
#include "DocumentRecognition.h"
#include <chrono>
#include <filesystem>

#include <jsapi.h>
#include "JSWebServer.h"

//#include <boost/iostreams/device/mapped_file.hpp>
//#include <boost/iostreams/stream.hpp>

namespace fs = std::filesystem;

#define WEBFILE_SUBPATH "web/"
#define WEBFILE_SUBPATH_LEN 3
#define WEB_SPECIALCODES "./web_specialcodes/"
#define WEB_404_NOT_FOUND "404.html"
#define WEB_403_FORBIDDEN "403.html"

// TODO make open ssl acceptor when file exists

namespace {
	int exists(const char* path) // 0 If Nothing in FileSystem exists, 1 if directory, 2 if File
	{
		struct stat info;
		stat(path, &info);
		if (info.st_mode & S_IFDIR) return 1;
		if (info.st_mode & S_IFREG) return 2;
		else return 0;
	}
}

namespace WebServer {
	void JSEngineBlock(std::string realpath, std::promise<bool> ret, std::promise<std::string> content, std::promise<std::string> mime)
	{
		if (JSEngine::Extern::WebServerCallMap.count(realpath))
		{
			std::pair<JSContext*, JSFunction*>& pair = JSEngine::Extern::WebServerCallMap[realpath];
			JS::RootedObject eventObj(pair.first, JSEngine::Extern::createWebEventObject(pair.first));
			JS::RootedFunction func(pair.first, pair.second);
			JS::RootedValue rval(pair.first);
			JS::AutoValueArray<1> args(pair.first);
			args[0].setObject(*eventObj);
			JS_CallFunction(pair.first, nullptr, func, args, &rval);

			JSEngine::WebServer::WebEventData* wed = reinterpret_cast<JSEngine::WebServer::WebEventData*>(JS_GetPrivate(eventObj));
			
			content.set_value(wed->content);
			mime.set_value(wed->mime);
			if (rval.isBoolean())
			{
				ret.set_value(rval.toBoolean());
			}
			else
			{
				ret.set_value(true);
			}
		}
		else ret.set_value(false);
	}

	void readInBytes(const char* path, HTTPSendInformation& info) {
		std::FILE* fp;
		fopen_s(&fp, path, "rb");
		if (fp)
		{
			std::fseek(fp, 0, SEEK_END);
			info.data.resize(std::ftell(fp));
			std::rewind(fp);
			std::fread(&info.data[0], 1, info.data.size(), fp);
			std::fclose(fp);
		}
	}

	void GetWebFile(std::string realpath, HTTPSendInformation& info) {
		if (realpath.find("/../") != std::string::npos) { // Exploit to get into computer filesystem (user uses uses extra software for this)
			info.status = 403;
			info.statusMessage = "Forbidden";
			info.setDataC("HTTP/1.1 403 Forbidden\r\n\r\nDon't use exploits. Your IP will be safed and reported after several trys. (If this is an error, please contact the developer to report this bug)");
			info.fullPath = "403.html";
			return;
		}

		if (exists(realpath.c_str()) == 2) { // 200 explicit file caller
			info.status = 200;
			info.statusMessage = "OK";
			readInBytes(realpath.c_str(), info);
			info.fullPath = realpath;

			return;
		}
		else if (exists(realpath.c_str()) == 1) { // 200 implicit index.html caller
			std::string fileString = (realpath + "/index.html");
			if (exists(fileString.c_str()) == 2) {
				info.status = 200;
				info.statusMessage = "OK";
				readInBytes(fileString.c_str(), info);
				info.fullPath = fileString;

				return;
			}
			else { // More exact error message
				std::string path = (std::string(WEB_SPECIALCODES) + WEB_404_NOT_FOUND);
				if (exists(path.c_str())) {
					info.status = 404;
					info.statusMessage = "Not Found";
					readInBytes(path.c_str(), info);
					info.fullPath = "404.html";
					return;
				}
				else {
					info.status = 404;
					info.statusMessage = "Not Found";
					info.setDataC("Site in Folder not found");
					info.fullPath = "404.code";
					return;
				}
			}
		}
		else { // 404 Not found
			std::string path = (std::string(WEB_SPECIALCODES) + WEB_404_NOT_FOUND);
			if (exists(path.c_str())) {
				info.status = 404;
				info.statusMessage = "Not Found";
				readInBytes(path.c_str(), info);
				info.fullPath = "404.html";
				return;
			}
			else {
				info.status = 404;
				info.statusMessage = "Not Found";
				info.setDataC("<h1 style='font-family: Arial; position: fixed; left: 50%; top: 50%; transform: translate(-50%, -50%);'>Site not found.</h1>");
				info.fullPath = "404.code";
				return;
			}
		}
	}

	std::string GetRealPath(std::string path) {
		std::string realpath;

		if (_startsWith(path, "/")) // Adjustments
			realpath = WEBFILE_SUBPATH + HTTPNS::urlDecode(path.substr(1));
		else realpath = WEBFILE_SUBPATH + HTTPNS::urlDecode(path);

		int questPos = realpath.find("?");
		if (questPos != std::string::npos) {
			realpath = realpath.substr(0, questPos);
		}

		if (*(realpath.end() - 1) == '/') {
			realpath.erase(realpath.end() - 1);
		}

		return realpath;
	}

	void HandleUserRequest(SOCKET client) {
		WebExchange exch(client);
		WebExchange::Webpacket packet;
		while (true) {
			ZeroMemory(packet.text, WEB_BUFFER_SIZE);
			int n = exch.recv(packet);
			if (n <= 0) {
				ss.closeClient(client);
				return;
			}
			std::string httpVal(packet.text, n);
			HTTPReadProtocol http(httpVal);

			std::string realpath = GetRealPath(http.encodedPath);

			std::cout << realpath << std::endl;
			std::promise<bool> jsRet;
			std::promise<std::string> jsContent;
			std::promise<std::string> jsMime;
			auto jsRetVal = jsRet.get_future();
			auto jsContentRet = jsContent.get_future();
			auto jsMimeRet = jsMime.get_future();
			std::thread jsThread(&JSEngineBlock, realpath, std::move(jsRet), std::move(jsContent), std::move(jsMime)); // TODO Repair Thread

			HTTPSendInformation response;

			GetWebFile(realpath, response);

			std::pair<bool, std::string> pair = DocumentRecognition::getContentType(response.fullPath);
			std::string contentType;
			if (pair.first) {
				contentType = pair.second;
			}
			else {
				contentType = "text/plain";
			}
			HTTPWriteProtocol writer;
			writer.AddArgument("Server", "BlogPost");
			writer.AddArgument("Accept-Ranges", "bytes");
			bool keepSending = false;

			std::map<std::string, std::string>::iterator connectionItr = http.HTTPMap.find("Connection");
			if (connectionItr != http.HTTPMap.end()) {
				if (connectionItr->second == "keep-alive") {
					keepSending = true;
				}
			}

			if (keepSending) {
				writer.AddArgument("Connection", "Keep-Alive");
				writer.AddArgument("Keep-Alive", "timeout=5000, max=100");
			}
			else {
				writer.AddArgument("Connection", "close");
			}

			std::string data;

			if (contentType == "text/html") {
				writer.AddArgument("Cache-Control", "no-store");
			}
			else {
				writer.AddArgument("Cache-Control", "public");
			}

			jsThread.join();
			if (jsRetVal.get())
			{
				response.data = jsContentRet.get();
				contentType = jsMimeRet.get();
			}
			writer.AddArgument("Content-Type", (contentType + "; charset=utf-8").c_str());

			writer.AddArgument("Content-Length", std::to_string(response.data.size()).c_str());

			writer.Create(data, response);

			exch.send(data.c_str(), data.size());

			if (!keepSending) break;
		}
		ss.closeClient(client);
		return;
	}

	void HandleUser(SOCKET client) {
		std::thread handleUserReq(HandleUserRequest, client);
		handleUserReq.detach();
	}

	void Process() {
		SocketServer ss;
		DocumentRecognition::Init();
		//InitWebDir();
		//std::thread checkFile(CheckFileModifyThread);
		if (ss.open("80") == 0) {
			std::cout << "WebServer opened at port 80." << std::endl;
			while (ss.listen()) {
				SOCKET client = ss.accept();
				std::thread clientWorker(HandleUser, client);
				clientWorker.detach();
			}
		}
		else {
			std::cout << "WebServer error. Can't open connection." << std::endl;
		}
	}
}