#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>

#include "../Geometry/Vec.hpp"

class ArgumentsParser {
    struct Argument {
        std::string value;
        std::string defaultValue;
        std::string description;
    };

    std::map<std::string, Argument> arguments;
public:
    void AddArgument(const std::string &key, const std::string description, const std::string defaultValue);
    bool ParseArguments(int argc, const char **argv);
    void Help();

    bool IsSet(const std::string &key);

    std::string Get(const std::string &key);
    int GetInteger(const std::string &key);
    Vec GetVec(const std::string &key);
};

// добавление обрабатываемого аргумента
void ArgumentsParser::AddArgument(const std::string &key, const std::string description, const std::string defaultValue) {
    arguments[key] = { "", defaultValue, description };
}

// парсинг аргументов
bool ArgumentsParser::ParseArguments(int argc, const char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string key = argv[i];

        if (arguments.find(key) == arguments.end()) {
            std::cout << "unknown argument '" << key << "'" << std::endl;
            return false;
        }

        if (i == argc - 1) {
            std::cout << "invalid value argument '" << key << "'" << std::endl;
            return false;
        }

        arguments[key].value = argv[++i];
    }

    return true;
}

// вывод сообщения о помощи
void ArgumentsParser::Help() {
    std::cout << "Usage: ./rt arguments" << std::endl;
    std::cout << "Arguments:" << std::endl;

    for (auto i = arguments.begin(); i != arguments.end(); i++) {
        Argument arg = i->second;
        std::string defaultValue = arg.defaultValue == "" ? "" : "(default: '" + arg.defaultValue + "')";
        std::cout << "  " << std::setw(10) << std::left << i->first << " - " << arg.description << " " << defaultValue << std::endl;
    }

    std::cout << "  --help     - print this message" << std::endl;
}

bool ArgumentsParser::IsSet(const std::string &key) {
    return arguments[key].value != "";
}

// получение аргумента
std::string ArgumentsParser::Get(const std::string &key) {
    return IsSet(key) ? arguments[key].value : arguments[key].defaultValue;
}

int ArgumentsParser::GetInteger(const std::string &key) {
    return std::stoi(Get(key));
}

Vec ArgumentsParser::GetVec(const std::string &key) {
    std::stringstream ss(Get(key));
    Vec vec;
    ss >> vec;
    return vec;
}
