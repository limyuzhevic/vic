// NLM Command Line Interface (CLI) Framework
// Comprehensive CLI system with help/usage documentation, command parsing,
// and scripting support for the NLM neural simulation project

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Command line argument types
enum class ArgumentType {
    String,
    Int,
    Double,
    Bool,
    File,
    Directory,
    Enum,
    Custom
};

// Command line option structure
struct CLIArgument {
    std::string name;
    char shortName;
    std::string description;
    ArgumentType type;
    std::string defaultValue;
    bool required;
    std::vector<std::string> validValues;
    std::function<bool(const std::string&)> validator;
    std::string helpText;
    
    CLIArgument() : shortName(' '), type(ArgumentType::String), required(false) {}
    
    CLIArgument(const std::string& name, char shortName, const std::string& description,
               ArgumentType type, const std::string& defaultValue = "",
               bool required = false, const std::vector<std::string>& validValues = {})
        : name(name), shortName(shortName), description(description), 
          type(type), defaultValue(defaultValue), required(required),
          validValues(validValues) {}
};

// Command structure
struct CLICommand {
    std::string name;
    std::string description;
    std::vector<CLIArgument> arguments;
    std::function<int(const std::map<std::string, std::string>&)> handler;
    std::string usage;
    std::string examples;
    
    CLICommand() : handler(nullptr) {}
    
    CLICommand(const std::string& name, const std::string& description)
        : name(name), description(description), handler(nullptr) {}
};

// Command line parser class
class CLIParser {
public:
    CLIParser(const std::string& programName, const std::string& version)
        : programName_(programName), version_(version), helpRequested_(false) {}
    
    // Add a command to the parser
    void addCommand(const CLICommand& command) {
        commands_[command.name] = command;
    }
    
    // Add a global argument
    void addGlobalArgument(const CLIArgument& arg) {
        globalArgs_[arg.name] = arg;
    }
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]) {
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
    
    // Get the current command
    std::string getCurrentCommand() const {
        return currentCommand_;
    }
    
    // Get parsed arguments
    std::map<std::string, std::string> getParsedArgs() const {
        return parsedArgs_;
    }
    
    // Get global argument value
    std::string getGlobalArg(const std::string& name) const {
        auto it = globalArgs_.find(name);
        if (it != globalArgs_.end()) {
            auto parsedIt = parsedArgs_.find(name);
            if (parsedIt != parsedArgs_.end()) {
                return parsedIt->second;
            }
        }
        return "";
    }
    
    // Check if an argument was provided
    bool hasArg(const std::string& name) const {
        return parsedArgs_.find(name) != parsedArgs_.end();
    }
    
    // Get argument value
    std::string getArg(const std::string& name) const {
        auto it = parsedArgs_.find(name);
        if (it != parsedArgs_.end()) {
            return it->second;
        }
        return "";
    }
    
    // Get integer argument value
    int getIntArg(const std::string& name) const {
        std::string value = getArg(name);
        if (value.empty()) {
            throw std::runtime_error("Missing required argument: " + name);
        }
        try {
            return std::stoi(value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid integer value for argument " + name + ": " + e.what());
        }
    }
    
    // Get double argument value
    double getDoubleArg(const std::string& name) const {
        std::string value = getArg(name);
        if (value.empty()) {
            throw std::runtime_error("Missing required argument: " + name);
        }
        try {
            return std::stod(value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid double value for argument " + name + ": " + e.what());
        }
    }
    
    // Get boolean argument value
    bool getBoolArg(const std::string& name) const {
        std::string value = getArg(name);
        if (value.empty()) {
            throw std::runtime_error("Missing required argument: " + name);
        }
        // Convert common boolean representations
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        
        if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on") {
            return true;
        } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no" || lowerValue == "off") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean value for argument " + name + ": " + value);
        }
    }
    
    // Print help message
    void printHelp() const {
        std::cout << "Usage: " << programName_ << " <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        
        for (const auto& pair : commands_) {
            const auto& cmd = pair.second;
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Global Options:" << std::endl;
        for (const auto& pair : globalArgs_) {
            const auto& arg = pair.second;
            std::cout << "  --" << arg.name << (arg.shortName != ' ' ? ", -" + std::string(1, arg.shortName) : "") << " " << arg.description << std::endl;
            if (!arg.defaultValue.empty()) {
                std::cout << "      (default: " << arg.defaultValue << ")" << std::endl;
            }
            if (!arg.helpText.empty()) {
                std::cout << "      " << arg.helpText << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Use " << programName_ << " <command> --help for command-specific help." << std::endl;
        std::cout << std::endl;
        std::cout << "Version: " << version_ << std::endl;
    }
    
    // Print command help
    void printCommandHelp(const std::string& commandName) const {
        auto it = commands_.find(commandName);
        if (it == commands_.end()) {
            std::cout << "Error: Unknown command: " << commandName << std::endl;
            return;
        }
        
        const auto& cmd = it->second;
        
        std::cout << "Usage: " << programName_ << " " << cmd.name << cmd.usage << std::endl;
        std::cout << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        
        for (const auto& arg : cmd.arguments) {
            std::cout << "  --" << arg.name << (arg.shortName != ' ' ? ", -" + std::string(1, arg.shortName) : "") << " " << arg.description << std::endl;
            if (!arg.defaultValue.empty()) {
                std::cout << "      (default: " << arg.defaultValue << ")" << std::endl;
            }
            if (!arg.helpText.empty()) {
                std::cout << "      " << arg.helpText << std::endl;
            }
        }
        
        if (!cmd.examples.empty()) {
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << cmd.examples << std::endl;
        }
    }
    
    // Print version
    void printVersion() const {
        std::cout << programName_ << " version " << version_ << std::endl;
    }
    
    // Print error and exit
    void printErrorAndExit(const std::string& error) const {
        std::cerr << "Error: " << error << std::endl;
        std::cerr << "Use --help for usage information." << std::endl;
        std::exit(1);
    }
    
    // Get current command usage
    std::string getCurrentCommandUsage() const {
        auto it = commands_.find(currentCommand_);
        if (it != commands_.end()) {
            return it->second.usage;
        }
        return "";
    }
    
    // Get all command names
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    // Build usage string for all commands
    void buildUsageString() {
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
    
    // Handle special arguments (--help, --version)
    bool handleSpecialArgs(const std::vector<std::string>& args) {
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
    
    // Parse command arguments
    std::map<std::string, std::string> parseCommandArguments(const std::vector<std::string>& args, const CLICommand& command) {
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
    
    // Validate argument value
    bool validateArgument(const std::string& argName, const std::string& value, const CLIArgument& arg) const {
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
    
    std::string programName_;
    std::string version_;
    std::map<std::string, CLICommand> commands_;
    std::map<std::string, CLIArgument> globalArgs_;
    std::string currentCommand_;
    std::map<std::string, std::string> parsedArgs_;
    bool helpRequested_;
};

// Command line interface builder class
class CLIBuilder {
public:
    CLIBuilder(const std::string& programName, const std::string& version) {
        parser_ = std::make_unique<CLIParser>(programName, version);
    }
    
    // Add a command
    CLIBuilder& addCommand(const CLICommand& command) {
        parser_->addCommand(command);
        return *this;
    }
    
    // Add global argument
    CLIBuilder& addGlobalArgument(const CLIArgument& arg) {
        parser_->addGlobalArgument(arg);
        return *this;
    }
    
    // Add config file argument
    CLIBuilder& addConfigArgument() {
        parser_->addGlobalArgument(CLIArgument("config", 'c', 
            "Configuration file path", ArgumentType::File, 
            ConfigConstants::DEFAULT_CONFIG_FILE, false));
        return *this;
    }
    
    // Add help argument
    CLIBuilder& addHelpArgument() {
        parser_->addGlobalArgument(CLIArgument("help", 'h', 
            "Show help message", ArgumentType::Bool, "false", false));
        parser_->addGlobalArgument(CLIArgument("version", 'v', 
            "Show version information", ArgumentType::Bool, "false", false));
        return *this;
    }
    
    // Add log level argument
    CLIBuilder& addLogLevelArgument() {
        parser_->addGlobalArgument(CLIArgument("log_level", 'l', 
            "Log level (debug, info, warning, error, critical)", 
            ArgumentType::Enum, "info", false,
            {"debug", "info", "warning", "error", "critical"}));
        return *this;
    }
    
    // Add simulation steps argument
    CLIBuilder& addSimulationStepsArgument() {
        parser_->addGlobalArgument(CLIArgument("steps", 's', 
            "Number of simulation steps", ArgumentType::Int, "1000", false));
        return *this;
    }
    
    // Add random seed argument
    CLIBuilder& addRandomSeedArgument() {
        parser_->addGlobalArgument(CLIArgument("seed", 'r', 
            "Random seed for reproducible results", ArgumentType::Int, 
            std::to_string(ConfigConstants::DEFAULT_RANDOM_SEED), false));
        return *this;
    }
    
    // Add world size argument
    CLIBuilder& addWorldSizeArgument() {
        parser_->addGlobalArgument(CLIArgument("world_size", 'w', 
            "World size (width height)", ArgumentType::Custom, "20 20", false));
        return *this;
    }
    
    // Add config file argument with validation
    CLIBuilder& addConfigArgumentWithValidation() {
        auto configValidator = [](const std::string& value) {
            // Check if file can be opened for reading
            std::ifstream file(value);
            return file.good();
        };
        
        parser_->addGlobalArgument(CLIArgument("config", 'c', 
            "Configuration file path", ArgumentType::File, 
            ConfigConstants::DEFAULT_CONFIG_FILE, false,
            {}, configValidator));
        return *this;
    }
    
    // Add verbose output argument
    CLIBuilder& addVerboseArgument() {
        parser_->addGlobalArgument(CLIArgument("verbose", 'v', 
            "Enable verbose output", ArgumentType::Bool, "false", false));
        return *this;
    }
    
    // Add output file argument
    CLIBuilder& addOutputArgument() {
        parser_->addGlobalArgument(CLIArgument("output", 'o', 
            "Output file path", ArgumentType::File, "", false));
        return *this;
    }
    
    // Build and return the parser
    std::unique_ptr<CLIParser> build() {
        return std::move(parser_);
    }
    
private:
    std::unique_ptr<CLIParser> parser_;
};

// Convenience function to create a simple CLI command
CLICommand createCommand(const std::string& name, const std::string& description,
                         const std::vector<CLIArgument>& arguments,
                         std::function<int(const std::map<std::string, std::string>&)> handler) {
    CLICommand cmd(name, description);
    cmd.arguments = arguments;
    cmd.handler = handler;
    return cmd;
}

// Convenience function for simple commands with one argument
CLICommand createSimpleCommand(const std::string& name, const std::string& description,
                               const CLIArgument& argument,
                               std::function<int(const std::map<std::string, std::string>&)> handler) {
    return createCommand(name, description, {argument}, handler);
}

} // namespace nlm