#include "CLIFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>

namespace nlm {

CLIApplication::CLIApplication(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

CLIApplication::~CLIApplication() {}

void CLIApplication::addCommand(const std::string& name, const std::string& description,
                               CLICommandHandler handler) {
    commands_[name] = handler;
}

void CLIApplication::addOption(char shortOpt, const std::string& longOpt,
                              const std::string& description, CLIOptionValue defaultValue) {
    shortOptions_[shortOpt] = defaultValue;
    longOptions_[longOpt] = defaultValue;
    optionDescriptions_[longOpt] = description;
}

int CLIApplication::run(int argc, char** argv) {
    if (argc < 2) {
        printError("No command specified");
        printUsage(name_);
        return 1;
    }
    
    std::string commandName = argv[1];
    
    // Check for help/version commands
    if (commandName == "help" || commandName == "--help" || commandName == "-h") {
        std::cout << "Help not implemented yet" << std::endl;
        return 0;
    }
    
    if (commandName == "version" || commandName == "--version" || commandName == "-v") {
        std::cout << name_ << " version " << version_ << std::endl;
        return 0;
    }
    
    // Execute the command
    return executeCommand(commandName, argc - 1, argv + 1);
}

int CLIApplication::executeCommand(const std::string& commandName, int argc, char** argv) {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        printError("Unknown command: " + commandName);
        return 1;
    }
    
    try {
        return it->second(argc, argv);
    } catch (const std::exception& e) {
        printError(std::string("Command failed: ") + e.what());
        return 1;
    }
}

bool CLIApplication::parseOptions(int argc, char** argv, std::unordered_map<std::string, CLIOptionValue>& parsedOptions) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Long option
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parsedOptions[key] = value;
            } else {
                parsedOptions[arg.substr(2)] = true;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option
            std::string key(arg.substr(1));
            if (key.size() == 1) {
                // Single character option
                parsedOptions[key] = true;
            } else {
                // Combined short options like -abc
                for (char c : key) {
                    std::string shortKey(1, c);
                    parsedOptions[shortKey] = true;
                }
            }
        }
    }
    
    return true;
}

CLICommand::CLICommand(const std::string& name, const std::string& description,
                       CLICommandHandler handler)
    : name_(name), description_(description), handler_(handler) {}

CLIOption::CLIOption(char shortOpt, const std::string& longOpt,
                    const std::string& description, CLIOptionValue defaultValue)
    : shortOpt_(shortOpt), longOpt_(longOpt), description_(description),
      defaultValue_(defaultValue) {}

} // namespace nlm
