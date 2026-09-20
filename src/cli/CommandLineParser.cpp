#include "CommandLineParser.h"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

struct CommandLineParser::Impl {
    std::unordered_map<std::string, CLIOption> options;
    std::unordered_map<char, std::string> shortToLongMap;
    std::unordered_map<std::string, CLIValue> parsedOptions;
    std::string commandName;
    std::vector<CLIValue> commandArgs;
    std::vector<std::string> errors;
    std::vector<CLICommand> commands;
    std::ostream* errorStream;
    std::ostream* outputStream;
    
    Impl() : errorStream(nullptr), outputStream(nullptr) {}
    
    ~Impl() {
        if (errorStream == &std::cerr) errorStream = nullptr;
        if (outputStream == &std::cout) outputStream = nullptr;
    }
};

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()) {
    pImpl->errorStream = &std::cerr;
    pImpl->outputStream = &std::cout;
}

CommandLineParser::~CommandLineParser() = default;

bool CommandLineParser::parse(int argc, char** argv) {
    reset();
    
    if (argc < 1) {
        pImpl->errors.push_back("No arguments provided");
        return false;
    }
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Check for help flag first
    bool showHelp = false;
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            break;
        }
    }
    
    if (showHelp) {
        (*pImpl->outputStream) << getHelp();
        return false;
    }
    
    bool parseResult = parseOptions(args);
    if (parseResult) {
        parseResult = parseCommand(args);
    }
    
    return parseResult;
}

template<typename T>
T CommandLineParser::getOption(const std::string& name) const {
    auto it = pImpl->parsedOptions.find(name);
    if (it == pImpl->parsedOptions.end()) {
        throw CLIParseError("Option not found: " + name);
    }
    
    const CLIValue& value = it->second;
    if (!std::holds_alternative<T>(value)) {
        throw CLIParseError("Type mismatch for option: " + name);
    }
    
    return std::get<T>(value);
}

template<typename T>
T CommandLineParser::getOptionOr(const std::string& name, const T& defaultValue) const {
    auto it = pImpl->parsedOptions.find(name);
    if (it == pImpl->parsedOptions.end()) {
        return defaultValue;
    }
    
    const CLIValue& value = it->second;
    if (!std::holds_alternative<T>(value)) {
        return defaultValue;
    }
    
    return std::get<T>(value);
}

bool CommandLineParser::hasOption(const std::string& name) const {
    return pImpl->parsedOptions.find(name) != pImpl->parsedOptions.end();
}

std::vector<CLIValue> CommandLineParser::getCommandArgs(const std::string& commandName) const {
    if (pImpl->commandName == commandName) {
        return pImpl->commandArgs;
    }
    return std::vector<CLIValue>();
}

std::string CommandLineParser::getCommandName() const {
    return pImpl->commandName;
}

std::string CommandLineParser::getHelp() const {
    std::ostringstream oss;
    
    oss << "=== NLM CLI Help ===\n\n";
    
    if (!pImpl->commands.empty()) {
        oss << "Commands:\n";
        for (const auto& cmd : pImpl->commands) {
            oss << "  " << cmd.name;
            if (!cmd.description.empty()) {
                oss << " - " << cmd.description;
            }
            oss << "\n";
            
            if (!cmd.argumentNames.empty()) {
                for (size_t i = 0; i < cmd.argumentNames.size(); ++i) {
                    oss << "    " << cmd.argumentNames[i];
                    if (i < cmd.argumentDescriptions.size() && !cmd.argumentDescriptions[i].empty()) {
                        oss << " - " << cmd.argumentDescriptions[i];
                    }
                    oss << "\n";
                }
            }
        }
        oss << "\n";
    }
    
    if (!pImpl->options.empty()) {
        oss << "Options:\n";
        for (const auto& pair : pImpl->options) {
            const auto& option = pair.second;
            oss << "  ";
            if (option.shortName) {
                oss << "-" << option.shortName << ", ";
            }
            oss << "--" << option.longName;
            
            if (option.type != "auto" && option.type != "string" && 
                option.type != "bool" && option.type != "int" && option.type != "float") {
                oss << "=" << option.type;
            }
            
            if (!option.description.empty()) {
                oss << " - " << option.description;
            }
            
            if (option.required) {
                oss << " (required)";
            }
            
            oss << "\n";
        }
        oss << "\n";
    }
    
    oss << "Usage examples:\n";
    oss << "  nlm --config=config.cfg run --steps=100\n";
    oss << "  nlm --speed=2.0 --debug run\n";
    oss << "  nlm save --output=checkpoint.bin\n";
    oss << "  nlm inject --neuron-id=5 --current=10.0\n\n";
    
    oss << "For more information, visit: https://github.com/your-repo/nlm\n";
    
    return oss.str();
}

bool CommandLineParser::validate() const {
    bool valid = true;
    
    for (const auto& pair : pImpl->options) {
        const auto& option = pair.second;
        if (option.required && pImpl->parsedOptions.find(pair.first) == pImpl->parsedOptions.end()) {
            pImpl->errors.push_back("Required option missing: " + option.longName);
            valid = false;
        }
    }
    
    return valid;
}

std::vector<std::string> CommandLineParser::getErrors() const {
    return pImpl->errors;
}

void CommandLineParser::reset() {
    pImpl->parsedOptions.clear();
    pImpl->commandName.clear();
    pImpl->commandArgs.clear();
    pImpl->errors.clear();
}

void CommandLineParser::setErrorStream(std::ostream* errorStream) {
    pImpl->errorStream = errorStream;
}

void CommandLineParser::setOutputStream(std::ostream* outputStream) {
    pImpl->outputStream = outputStream;
}

void CommandLineParser::addOption(const CLIOption& option) {
    pImpl->options[option.longName] = option;
    if (option.shortName != '\0') {
        pImpl->shortToLongMap[option.shortName] = option.longName;
    }
}

void CommandLineParser::addCommand(const CLICommand& command) {
    pImpl->commands.push_back(command);
}

bool CommandLineParser::parseOptions(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.substr(0, 2) == "--") {
            std::string optionName = arg.substr(2);
            CLIValue optionValue;
            
            size_t equalsPos = optionName.find('=');
            if (equalsPos != std::string::npos) {
                std::string valueStr = optionName.substr(equalsPos + 1);
                optionName = optionName.substr(0, equalsPos);
                
                auto optionIt = pImpl->options.find(optionName);
                if (optionIt == pImpl->options.end()) {
                    pImpl->errors.push_back("Unknown option: --" + optionName);
                    return false;
                }
                
                const auto& option = optionIt->second;
                optionValue = convertStringToCLIValue(valueStr, option.type);
            } else if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                std::string valueStr = args[++i];
                
                auto optionIt = pImpl->options.find(optionName);
                if (optionIt == pImpl->options.end()) {
                    pImpl->errors.push_back("Unknown option: --" + optionName);
                    return false;
                }
                
                const auto& option = optionIt->second;
                optionValue = convertStringToCLIValue(valueStr, option.type);
            } else {
                auto optionIt = pImpl->options.find(optionName);
                if (optionIt == pImpl->options.end()) {
                    pImpl->errors.push_back("Unknown option: --" + optionName);
                    return false;
                }
                
                const auto& option = optionIt->second;
                if (option.type == "bool") {
                    optionValue = true;
                } else {
                    optionValue = option.defaultValue;
                }
            }
            
            auto optionIt = pImpl->options.find(optionName);
            if (optionIt != pImpl->options.end() && validateOption(optionIt->second, optionValue)) {
                pImpl->parsedOptions[optionName] = optionValue;
            }
        } else if (arg.size() == 2 && arg[0] == '-' && arg[1] != '-') {
            char shortOpt = arg[1];
            auto shortIt = pImpl->shortToLongMap.find(shortOpt);
            if (shortIt == pImpl->shortToLongMap.end()) {
                pImpl->errors.push_back("Unknown short option: -" + std::string(1, shortOpt));
                return false;
            }
            
            std::string optionName = shortIt->second;
            CLIValue optionValue;
            
            auto optionIt = pImpl->options.find(optionName);
            if (optionIt != pImpl->options.end()) {
                const auto& option = optionIt->second;
                if (option.type == "bool") {
                    optionValue = true;
                } else {
                    optionValue = option.defaultValue;
                }
                pImpl->parsedOptions[optionName] = optionValue;
            }
        } else {
            pImpl->errors.push_back("Unrecognized argument: " + arg);
            return false;
        }
    }
    
    return pImpl->errors.empty();
}

bool CommandLineParser::parseCommand(const std::vector<std::string>& args) {
    for (const auto& cmd : pImpl->commands) {
        if (std::find(args.begin(), args.end(), cmd.name) != args.end()) {
            pImpl->commandName = cmd.name;
            
            auto cmdPos = std::find(args.begin(), args.end(), cmd.name);
            if (cmdPos != args.end()) {
                ++cmdPos;
                while (cmdPos != args.end() && (*cmdPos).substr(0, 2) != "--") {
                    pImpl->commandArgs.push_back(*cmdPos);
                    ++cmdPos;
                }
            }
            
            break;
        }
    }
    
    return true;
}

CLIValue CommandLineParser::convertStringToCLIValue(const std::string& valueStr, const std::string& type) const {
    if (type == "bool") {
        return valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on";
    } else if (type == "int") {
        return std::stoi(valueStr);
    } else if (type == "float") {
        return std::stod(valueStr);
    } else {
        return valueStr;
    }
}

bool CommandLineParser::validateOption(const CLIOption& option, const CLIValue& value) const {
    if (option.validator && !option.validator(value)) {
        pImpl->errors.push_back("Invalid value for option --" + option.longName);
        return false;
    }
    return true;
}

std::string CommandLineParser::formatValue(const CLIValue& value) const {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return "";
}

} // namespace nlm