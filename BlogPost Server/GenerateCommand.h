#pragma once

#include <CommandLib.h>
#include <vector>
#include <string>

#define EXTERN_BLOGPOST
#include <ConsoleHandler.h>
#include "SSLUtil.h"

class GenerateCommand : public Command {
public:
	GenerateCommand()
		: Command("generate") {}

	void call(std::vector<std::string> args) override {
		ArgumentList arguments(args);

		Argument sslArg = arguments.find("ssl", true);

		if (sslArg.validation == Argument::ArgumentValidation::VALID) {
			std::cout << "Country ID: ";
			std::string country = ConsoleHandler::getLineInput();
			if (country.size() != 2) {
				std::cout << "Country id invalid." << std::endl;
				return;
			}

			std::cout << std::endl << "Organisation: ";
			std::string organisation = ConsoleHandler::getLineInput();
			std::cout << std::endl << "URL: ";
			std::string url = ConsoleHandler::getLineInput();
			std::cout << std::endl << "Passphrase: ";
			std::string password = ConsoleHandler::getLineInputPassword();
			std::cout << std::endl;
			bool success = SSLUtil::GenerateCertificate(password.c_str(), "key.pem", "cert.pem", country.c_str(), organisation.c_str(), url.c_str());
			if (success) {
				std::cout << "Certificate generated successfully." << std::endl;
			}
			else {
				std::cout << "Error while generating." << std::endl;
			}
		}
	}
};