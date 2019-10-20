#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <CommandLib.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <sys/stat.h>

#define DOCUMENT_RECOGNITION_FILE "configs/mime_types.json"

// Class is shit and needs to be recoded

namespace {
	bool _endsWith(const std::string& str, const std::string& end) {
		return str.substr(str.length() - end.length(), str.length()) == end;
	}
}

namespace DocumentRecognition {

	rapidjson::Document document;

	void addValue(rapidjson::Value value, const char* name, const char* val) {

	}

	bool loaded = false;

	std::pair<bool, std::string> getContentType(std::string str) {
		if (loaded) {
			std::vector<std::string> splits = _split(str, ".");
			const char* end = splits[splits.size() - 1].c_str();
			rapidjson::Value& val = document["MIME"];
			rapidjson::Value::ConstMemberIterator itr = val.FindMember(end);
			if (itr != val.MemberEnd()) {
				if (itr->value.IsString()) {
					return { true, itr->value.GetString() };
				}
			}
		}
		return { false, std::string() };
	}

	bool ReadJson() {
		using namespace rapidjson;
		struct stat info;
		stat(DOCUMENT_RECOGNITION_FILE, &info);
		if (info.st_mode & S_IFREG) {
			std::ifstream file(DOCUMENT_RECOGNITION_FILE);
			std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			document.Parse(content.c_str());
			if (document.IsObject()) {
				Value::ConstMemberIterator itr = document.FindMember("MIME");
				if (itr != document.MemberEnd()) {
					if (itr->value.IsObject()) {
						loaded = true;
						return true;
					}
				}
			}
		}
		return false;
	}

	void createDocumentBasicMIMES() {
		using namespace rapidjson;
		Value val(kObjectType);
		val.SetObject();
		Document::AllocatorType& allocator = document.GetAllocator();
		val.AddMember("png", Value("image/png").Move(), allocator);
		val.AddMember("html", Value("text/html").Move(), allocator);
		val.AddMember("css", Value("text/css").Move(), allocator);
		val.AddMember("js", Value("application/javascript").Move(), allocator);
		val.AddMember("jpg", Value("image/jpg").Move(), allocator);
		val.AddMember("jpeg", Value("image/jpeg").Move(), allocator);
		val.AddMember("json", Value("application/json").Move(), allocator);
		document.AddMember("MIME", val.Move(), allocator);
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		document.Accept(writer);
		std::ofstream file(DOCUMENT_RECOGNITION_FILE);
		file.write(buffer.GetString(), buffer.GetSize());
		file.flush();
		file.close();
		loaded = true;
	}

	void Init() {
		if (ReadJson()) {
			std::cout << "Web Document Recognition loaded successfully." << std::endl;
		}
		else {
			std::cout << DOCUMENT_RECOGNITION_FILE << " is getting created..." << std::endl;
			using namespace rapidjson;
			document.SetObject();
			createDocumentBasicMIMES();
		}
	}
}