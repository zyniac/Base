#pragma once
#include <string>
#include <vector>

class Command {
public:
	Command(const char* name) {
		this->name = name;
	}

	virtual void call(std::vector<std::string> args) = 0;

	inline std::string getName() const {
		return this->name;
	}

private:
	std::string name;
};

class Argument {
public:
	Argument() = default;

	enum class ArgumentValidation {
		VALID, INVALID
	};

	ArgumentValidation validation = ArgumentValidation::VALID;

	Argument(std::string mainArg)
		: mainArg(mainArg) {}

	Argument(ArgumentValidation validation)
		: validation(validation) {}

	void set(std::string string) {
		mainArg = string;
	}

	void setWrittenOut(bool writtenOut) {
		this->writtenOut = writtenOut;
	}

	bool isWrittenOut() {
		return this->writtenOut;
	}

	void setArg(std::vector<std::string> lString) {
		argArg = lString;
	}

	std::string get() {
		return mainArg;
	}

	std::vector<std::string> getArg() {
		return argArg;
	}

	std::vector<std::string>& getArgRef() {
		return argArg;
	}
private:
	std::string mainArg;
	std::vector<std::string> argArg;
	bool writtenOut = false;
};

bool _startsWith(std::string string, std::string start) {
	return string.substr(0, start.size()) == start;
}

class ArgumentList {
public:
	ArgumentList(std::vector<std::string> lString) {
		this->processArguments(lString);
	}

	void processArguments(std::vector<std::string> lString) {
		for (unsigned int i = 0; i < lString.size(); i++) {
			if (_startsWith(lString[i], "--")) {
				arguments.push_back(Argument(lString[i].substr(2)));
				(arguments.end() - 1)->setWrittenOut(true);
			}
			else if (_startsWith(lString[i], "-")) {
				arguments.push_back(Argument(lString[i].substr(1)));
			}
			else if (arguments.size() > 0) {
				(arguments.end() - 1)->getArgRef().push_back(lString[i]);
			}
			else {
				this->firstArgNM = true;
				break;
			}
		}
	}

	Argument find(std::string argumentName, bool isWrittenOut) {
		for (auto argument : this->arguments) {
			if (argument.get() == argumentName && argument.isWrittenOut() == isWrittenOut) return argument;
		}
		return Argument(Argument::ArgumentValidation::INVALID);
	}

	std::vector<Argument> getArguments() {
		return this->arguments;
	}

	std::vector<Argument>& getArgumentsRef() {
		return this->arguments;
	}

	bool noCll() {
		return this->firstArgNM;
	}

private:
	std::vector<Argument> arguments;
	bool firstArgNM = false; // First Argument not a definer
};

std::vector<std::string> _split(std::string str, std::string del) // Function from high level programming languages like c# or js
{
	std::vector<std::string> splits;
	size_t pos;
	while ((pos = str.find(del)) != std::string::npos) {
		splits.push_back(str.substr(0, pos));
		str.erase(0, pos + del.length());
	}
	splits.push_back(str);
	return splits;
}