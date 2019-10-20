#pragma once

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <sys/stat.h>
#include <fstream>

using namespace rapidjson;
class ConfigGenerator
{
public:
	virtual bool checkValid()
	{
		return true;
	}

	virtual bool load(const char* path)
	{
		struct stat info;
		stat(path, &info);
		if (info.st_mode == S_IFREG) // Is file
		{
			std::ifstream file(path, std::ios::binary);
			std::string str((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
			document.Parse(str.c_str());
			return this->checkValid();
		}
		else return false;
	}

	Document& get()
	{
		return document;
	}

	virtual void save(const char* path) const
	{
		std::ofstream file(path, std::ios::binary);
		StringBuffer buffer;
		PrettyWriter<StringBuffer> writer(buffer);
		document.Accept(writer);
		file.write(buffer.GetString(), buffer.GetSize());
		file.flush();
		file.close();
	}

protected:
	Document document;
};

class WebConfig : public ConfigGenerator
{
public:
	bool checkValid() override
	{
		if (document.IsObject())
		{
			Value::ConstMemberIterator portItr = document.FindMember("Port");
			if (portItr != document.MemberEnd())
			{
				if (!portItr->value.IsInt()) return false;
				return true;
			}
		}
		return false;
	}
};

class BlogPostConfig : public ConfigGenerator
{
public:
	bool checkValid() override
	{
		if (document.IsObject())
		{
			Value::ConstMemberIterator roleIterator = document.FindMember("Role");
			if (roleIterator != document.MemberEnd())
			{
				return roleIterator->value.IsArray();
			}
		}
		return false;
	}
};