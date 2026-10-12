// NLM Command Line Interface Implementation
// Implements the CLI parser and builder functionality

#include "CLI.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace nlm {

// CLIParser implementation
bool CLIParser::parse(int argc, char* argv[]) {
    // Build usage string
    buildUsageString();
    
    // Parse arguments
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Handle special cases
    if (handleSpecialArgs(args)) {
        return true; // Help or version requested
    }
    
    // Find command
    std::string commandName;
    if (!args.empty()) {
        commandName = args[0];
        // Check if it's a command
        if (commands_.find(commandName) != commands_.end()) {
            currentCommand_ = commandName;
            args.erase(args.begin()); // Remove command name
        } else {
            // No command provided
            printErrorAndExit("No command provided. Use --help for available commands.");
            return false;
        }
    } else {
        printErrorAndExit("No command provided. Use --help for available commands.");
        return false;
    }
    
    // Parse command arguments
    parsedArgs_ = parseCommandArguments(args, commands_[commandName]);
    
    return true;
}

void CLIParser::buildUsageString() {
    for (auto& pair : commands_) {
        auto& cmd = pair.second;
        std::string usage = "";
        
        for (const auto& arg : cmd.arguments) {
            usage += " [";
            if (arg.required) {
                usage += "--" + arg.name;
            } else {
                usage += "[--" + arg.name + " VALUE]";
            }
            usage += "]";
        }
        
        cmd.usage = usage;
    }
}

bool CLIParser::handleSpecialArgs(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            helpRequested_ = true;
            return true;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return true;
        }
    }
    return false;
}

std::map<std::string, std::string> CLIParser::parseCommandArguments(const std::vector<std::string>& args, const CLICommand& command) {
    std::map<std::string, std::string> parsedArgs;
    
    // Merge global and command-specific arguments
    std::map<std::string, CLIArgument> allArgs = globalArgs_;
    for (const auto& arg : command.arguments) {
        allArgs[arg.name] = arg;
    }
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        // Check if it's an option (--name)
        if (arg.substr(0, 2) == "--") {
            std::string argName = arg.substr(2);
            auto argIt = allArgs.find(argName);
            
            if (argIt == allArgs.end()) {
                printErrorAndExit("Unknown option: --" + argName);
            }
            
            const CLIArgument& cliArg = argIt->second;
            
            // Check if it has a value (next argument)
            if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                // Has value
                std::string value = args[i + 1];
                i++; // Skip the value
                
                // Validate value
                if (!validateArgument(argName, value, cliArg)) {
                    printErrorAndExit("Invalid value for option --" + argName + ": " + value);
                }
                
                parsedArgs[argName] = value;
            } else if (cliArg.type == ArgumentType::Bool) {
                // Boolean flag
                parsedArgs[argName] = "true";
            } else if (cliArg.required) {
                // Required argument without value
                printErrorAndExit("Missing value for required option: --" + argName);
            }
            // Optional argument without value - skip (use default)
        } else if (arg.substr(0, 1) == "-") {
            // Short option (-x)
            std::string shortArg = arg.substr(1);
            
            // Check global arguments for short options
            bool found = false;
            for (const auto& pair : globalArgs_) {
                if (pair.second.shortName == shortArg[0]) {
                    std::string argName = pair.second.name;
                    auto argIt = allArgs.find(argName);
                    
                    if (argIt != allArgs.end()) {
                        const CLIArgument& cliArg = argIt->second;
                        
                        if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                            // Has value
                            std::string value = args[i + 1];
                            i++; // Skip the value
                            
                            if (!validateArgument(argName, value, cliArg)) {
                                printErrorAndExit("Invalid value for option -" + shortArg + ": " + value);
                            }
                            
                            parsedArgs[argName] = value;
                        } else if (cliArg.type == ArgumentType::Bool) {
                            // Boolean flag
                            parsedArgs[argName] = "true";
                        }
                        
                        found = true;
                        break;
                    }
                }
            }
            
            if (!found) {
                printErrorAndExit("Unknown option: -" + shortArg);
            }
        } else {
            // Positional argument
            printErrorAndExit("Positional arguments not supported. Use options like --argument VALUE.");
        }
    }
    
    return parsedArgs;
}

bool CLIParser::validateArgument(const std::string& argName, const std::string& value, const CLIArgument& arg) const {
    // Check against valid values if specified
    if (!arg.validValues.empty()) {
        bool found = false;
        for (const auto& validValue : arg.validValues) {
            if (value == validValue) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    // Use custom validator if specified
    if (arg.validator) {
        if (!arg.validator(value)) {
            return false;
        }
    }
    
    // Type validation
    switch (arg.type) {
        case ArgumentType::Int:
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                return false;
            }
        case ArgumentType::Double:
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        case ArgumentType::Bool:
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            return lowerValue == "true" || lowerValue == "1" || 
                   lowerValue == "yes" || lowerValue == "on" ||
                   lowerValue == "false" || lowerValue == "0" || 
                   lowerValue == "no" || lowerValue == "off";
        case ArgumentType::File:
            // Check if file exists
            std::ifstream file(value);
            return file.good();
        case ArgumentType::Directory:
            // Check if directory exists (simplified)
            return !value.empty();
        default:
            return !value.empty(); // Just check if not empty
    }
}

} // namespace nlm