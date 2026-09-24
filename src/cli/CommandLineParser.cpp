#include "CommandLineParser.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineParser::CommandLineParser() = default;

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::addCommand(std::unique_ptr<Command> command) {
    std::string name = command->getName();
    
    CommandInfo info;
    info.name = name;
    info.description = command->getDescription();
    info.usage = command->getUsage();
    info.help = command->getHelp();
    info.isAdvanced = command->isAdvanced();
    info.examples = command->getExamples();
    info.parameters = command->getParameters();
    
    // Store the command
    commands_.push_back(std::move(command));
    
    // Store in registry for help
    commandRegistry_[name] = info;
}

bool CommandLineParser::parse(int argc, char** argv, Config& config) {
    if (argc < 2) {
        std::cout << "No command provided. Use '--help' for usage information.\n";
        return false;
    }
    
    std::string firstArg(argv[1]);
    
    // Check for global flags
    if (hasHelpFlag(argc, argv)) {
        std::cout << getGlobalHelp() << std::endl;
        return true;
    }
    
    if (hasVersionFlag(argc, argv)) {
        std::cout << "NLM (熙然) - Neural Learning Machine v2.0 (Phase 2 Enhanced)\n";
        return true;
    }
    
    // Parse advanced arguments first
    if (!parseAdvancedArgs(argc, argv, config)) {
        return false;
    }
    
    // Find and execute the command
    std::string commandName = firstArg;
    if (commandName == "--config") {
        // Skip config flag for command parsing
        if (argc >= 3 && argv[2][0] != '-') {
            commandName = argv[2];
        } else {
            std::cout << "Error: No command specified after --config\n";
            return false;
        }
    }
    
    auto it = commandRegistry_.find(commandName);
    if (it == commandRegistry_.end()) {
        std::cout << "Unknown command: " << commandName << "\n";
        std::cout << "Use '--help' to see available commands.\n";
        return false;
    }
    
    const CommandInfo& cmdInfo = it->second;
    
    // Check if this is an advanced command and we're not in advanced mode
    if (cmdInfo.isAdvanced) {
        std::cout << "Advanced command: " << commandName << "\n";
        std::cout << "Use '\x1b[1m' + command name + '\x1b[0m' for advanced usage\n";
        return false;
    }
    
    // Parse remaining arguments as command arguments
    std::vector<std::string> cmdArgs;
    bool inCommandArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Skip global flags
        if (arg == "--help" || arg == "--version") continue;
        
        // Check if we've found the command
        if (!inCommandArgs && i == 1) {
            inCommandArgs = true;
            continue;
        }
        
        if (inCommandArgs && arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            // This is an advanced argument like --param=value
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            config.set(key, value, ConfigSource::CommandLine);
        } else if (inCommandArgs) {
            cmdArgs.push_back(arg);
        }
    }
    
    // Find the actual command (in case it was skipped)
    std::string actualCommand = commandName;
    if (commandName == "--config" && argc >= 3 && argv[2][0] != '-') {
        actualCommand = argv[2];
        inCommandArgs = true;
    }
    
    // Execute the command
    auto cmdIt = std::find_if(commands_.begin(), commands_.end(),
                             [&actualCommand](const std::unique_ptr<Command>& cmd) {
                                 return cmd->getName() == actualCommand;
                             });
    
    if (cmdIt == commands_.end()) {
        std::cout << "Error: Command not found.\n";
        return false;
    }
    
    return (*cmdIt)->execute(cmdArgs);
}

Command* CommandLineParser::getCommand(const std::string& name) {
    auto it = std::find_if(commands_.begin(), commands_.end(),
                         [&name](const std::unique_ptr<Command>& cmd) {
                             return cmd->getName() == name;
                         });
    
    if (it != commands_.end()) {
        return it->get();
    }
    return nullptr;
}

std::string CommandLineParser::getGlobalHelp() const {
    std::ostringstream oss;
    oss << "NLM (熙然) - Neural Learning Machine\n";
    oss << "Version 2.0 (Phase 2 Enhanced)\n\n";
    oss << "USAGE:\n";
    oss << "  nlm [OPTIONS] [COMMAND] [ARGS]...\n\n";
    oss << "COMMANDS:\n";
    
    // List basic commands
    for (const auto& pair : commandRegistry_) {
        const CommandInfo& info = pair.second;
        if (!info.isAdvanced) {
            oss << "  " << std::left << std::setw(15) << info.name 
                << "- " << info.description << "\n";
        }
    }
    
    oss << "\nADVANCED COMMANDS:\n";
    for (const auto& pair : commandRegistry_) {
        const CommandInfo& info = pair.second;
        if (info.isAdvanced) {
            oss << "  " << std::left << std::setw(15) << info.name 
                << "- " << info.description << "\n";
        }
    }
    
    oss << "\nGLOBAL OPTIONS:\n";
    oss << "  --help              Show this help message\n";
    oss << "  --version           Show version information\n";
    oss << "  --config=file       Load configuration from file\n";
    oss << "  -key value          Set configuration key-value pair\n\n";
    
    oss << "Use 'nlm [COMMAND] --help' for command-specific help.\n";
    
    return oss.str();
}

std::string CommandLineParser::getCommandHelp(const std::string& commandName) const {
    auto it = commandRegistry_.find(commandName);
    if (it == commandRegistry_.end()) {
        return "Command not found.\n";
    }
    
    return buildCommandHelp(it->second);
}

void CommandLineParser::listCommands() const {
    std::cout << "Available commands:\n";
    
    std::cout << "\nBASIC COMMANDS:\n";
    for (const auto& pair : commandRegistry_) {
        const CommandInfo& info = pair.second;
        if (!info.isAdvanced) {
            std::cout << "  " << info.name << " - " << info.description << "\n";
        }
    }
    
    std::cout << "\nADVANCED COMMANDS:\n";
    for (const auto& pair : commandRegistry_) {
        const CommandInfo& info = pair.second;
        if (info.isAdvanced) {
            std::cout << "  " << info.name << " - " << info.description << "\n";
        }
    }
}

bool CommandLineParser::hasHelpFlag(int argc, char** argv) const {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") return true;
    }
    return false;
}

bool CommandLineParser::hasVersionFlag(int argc, char** argv) const {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--version") return true;
    }
    return false;
}

bool CommandLineParser::parseAdvancedArgs(int argc, char** argv, Config& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                config.set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            config.set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool CommandLineParser::parseCommandArgs(const std::string& commandName, int argc, char** argv, Config& config) {
    // Implementation will go here
    return true;
}

std::string CommandLineParser::buildCommandHelp(const CommandInfo& cmdInfo) const {
    std::ostringstream oss;
    oss << "Command: " << cmdInfo.name << "\n";
    oss << "Description: " << cmdInfo.description << "\n";
    oss << "Usage: nlm " << cmdInfo.usage << "\n\n";
    oss << "Help:\n" << cmdInfo.help << "\n\n";
    
    if (!cmdInfo.examples.empty()) {
        oss << "Examples:\n";
        for (const auto& example : cmdInfo.examples) {
            oss << "  " << example << "\n";
        }
        oss << "\n";
    }
    
    if (!cmdInfo.parameters.empty()) {
        oss << "Parameters:\n";
        for (const auto& param : cmdInfo.parameters) {
            oss << "  --" << param.first << " - " << param.second << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CommandLineParser::buildGlobalHelp() const {
    std::ostringstream oss;
    oss << getGlobalHelp() << std::endl;
    return oss.str();
}

void CommandLineParser::displayError(const std::string& message) const {
    std::cout << "\x1b[31mERROR:\x1b[0m " << message << "\n";
}

} // namespace nlm