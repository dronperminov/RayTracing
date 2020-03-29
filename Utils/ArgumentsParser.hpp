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
        std::string shortKey;
    };

    std::map<std::string, Argument> arguments;
    std::map<std::string, std::string> shortToFull;

    bool IsShortKey(const std::string& key);
public:
    void AddArgument(const std::string &key, const std::string description, const std::string defaultValue, const std::string& shortKey = "");
    bool ParseArguments(int argc, const char **argv);
    void Help();

    bool IsSet(const std::string &key);

    std::string Get(const std::string &key);
    int GetInteger(const std::string &key);
    Vec GetVec(const std::string &key);
};

bool ArgumentsParser::IsShortKey(const std::string& key) {
    return shortToFull.find(key) != shortToFull.end();
}

// добавление обрабатываемого аргумента
void ArgumentsParser::AddArgument(const std::string &key, const std::string description, const std::string defaultValue, const std::string& shortKey) {
    arguments[key] = { "", defaultValue, description, shortKey };

    if (IsShortKey(shortKey))
        throw std::runtime_error("short key '" + shortKey + "' already in use");

    if (shortKey != "")
        shortToFull[shortKey] = key;
}

// парсинг аргументов
bool ArgumentsParser::ParseArguments(int argc, const char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string key = argv[i];

        if (arguments.find(key) == arguments.end() && !IsShortKey(key)) {
            std::cout << "unknown argument '" << key << "'" << std::endl;
            return false;
        }

        if (i == argc - 1) {
            std::cout << "invalid value argument '" << key << "'" << std::endl;
            return false;
        }

        if (IsShortKey(key))
            key = shortToFull[key];

        arguments[key].value = argv[++i];
    }

    return true;
}

// вывод сообщения о помощи
void ArgumentsParser::Help() {
    std::cout << "Usage: ./rt arguments" << std::endl;
    std::cout << "Arguments:" << std::endl;

    std::cout << "  --help                   Print this message" << std::endl;

    for (auto i = arguments.begin(); i != arguments.end(); i++) {
        Argument arg = i->second;
        std::string shortKey = arg.shortKey == "" ? "" : " (" + arg.shortKey + ")";
        std::string defaultValue = arg.defaultValue == "" ? "" : "(default: '" + arg.defaultValue + "')";
        std::cout << "  " << std::setw(25) << std::left << (i->first + shortKey) << arg.description << " " << defaultValue << std::endl;
    }
}

bool ArgumentsParser::IsSet(const std::string &key) {
    return arguments[IsShortKey(key) ? shortToFull[key] : key].value != "";
}

// получение аргумента
std::string ArgumentsParser::Get(const std::string &key) {
    std::string argKey = IsShortKey(key) ? shortToFull[key] : key;
    return IsSet(key) ? arguments[argKey].value : arguments[argKey].defaultValue;
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
