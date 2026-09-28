// NLM CLI Framework Implementation

#include "cli/CLI.hpp"
#include <iostream>
#include <algorithm>

using namespace nlm;

void CLI::addCommand(std::unique_ptr<CLICommand> command) {
    commands.push_back(std::move(command));
}

bool CLI::executeCommand(const std::string& commandName, 
                        const std::vector<std::string>& args) {
    auto it = std::find_if(commands.begin(), commands.end(),
                          [&commandName](const std::unique_ptr<CLICommand>& cmd) {
                              return cmd->getName() == commandName;
                          });
    
    if (it == commands.end()) {
        std::cerr << "Error: Unknown command: " << commandName << std::endl;
        return false;
    }
    
    return (*it)->execute(args);
}

void CLI::printHelp() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd->getName() << ": " << cmd->getDescription() << std::endl;
        std::cout << "    Usage: " << cmd->getUsage() << std::endl;
    }
}

CLI& CLI::getInstance() {
    static CLI instance;
    return instance;
}

std::vector<CLIArgumentParser::Argument> CLIArgumentParser::parseArguments(
    const std::vector<std::string>& args) {
    
    std::vector<Argument> parsedArgs;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.substr(0, 2) == "--") {
            std::string flag = arg.substr(2);
            
            // Check if this flag has a value
            if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                parsedArgs.push_back({flag, args[i + 1], true});
                i++; // Skip the next arg as it's the value
            } else {
                parsedArgs.push_back({flag, "", false});
            }
        }
    }
    
    return parsedArgs;
}

bool CLIArgumentParser::hasFlag(const std::vector<Argument>& args, 
                                const std::string& flag) {
    return std::any_of(args.begin(), args.end(),
                       [&flag](const Argument& arg) { return arg.name == flag; });
}

std::string CLIArgumentParser::getValue(const std::vector<Argument>& args,
                                        const std::string& flag,
                                        const std::string& defaultValue) {
    auto it = std::find_if(args.begin(), args.end(),
                          [&flag](const Argument& arg) { return arg.name == flag; });
    
    if (it != args.end() && it->hasValue) {
        return it->value;
    }
    
    return defaultValue;
}

int CLIArgumentParser::getIntValue(const std::vector<Argument>& args,
                                   const std::string& flag,
                                   int defaultValue) {
    std::string value = getValue(args, flag);
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

float CLIArgumentParser::getFloatValue(const std::vector<Argument>& args,
                                       const std::string& flag,
                                       float defaultValue) {
    std::string value = getValue(args, flag);
    try {
        return std::stof(value);
    } catch (...) {
        return defaultValue;
    }
}

double CLIArgumentParser::getDoubleValue(const std::vector<Argument>& args,
                                         const std::string& flag,
                                         double defaultValue) {
    std::string value = getValue(args, flag);
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}