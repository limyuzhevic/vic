#include "CLIParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <filesystem>

namespace nlm {

ArgumentParser::ArgumentParser(int argc, char** argv) : argc(argc), argv(argv), 
    helpRequested(false), versionRequested(false), validationMode(false) {
}

void ArgumentParser::addArgument(const std::string& name, const std::string& description,
                                ArgType type, const std::string& defaultValue, 
                                bool required, const std::vector<std::string>& enumValues) {
    ArgumentDef def;
    def.name = name;
    def.description = description;
    def.type = type;
    def.defaultValue = defaultValue;
    def.required = required;
    def.enumValues = enumValues;
    def.hidden = false;
    
    // Add help text based on type
    switch (type) {
        case ArgType::FLAG:
            def.helpText = "Enable this flag";
            break;
        case ArgType::VALUE:
            def.helpText = "Specify a value";
            break;
        case ArgType::STRING:
            def.helpText = "String value";
            break;
        case ArgType::INTEGER:
            def.helpText = "Integer value";
            break;
        case ArgType::FLOAT:
            def.helpText = "Floating-point value";
            break;
        case ArgType::BOOLEAN:
            def.helpText = "Boolean value (true/false)";
            break;
        case ArgType::ENUM:
            std::string enumStr = "Enum value, one of: ";
            for (size_t i = 0; i < enumValues.size(); ++i) {
                if (i > 0) enumStr += ", ";
                enumStr += enumValues[i];
            }
            def.helpText = enumStr;
            break;
        case ArgType::VECTOR:
            def.helpText = "Comma-separated list of values";
            break;
        case ArgType::FILE_PATH:
            def.helpText = "File path";
            break;
        case ArgType::DIRECTORY_PATH:
            def.helpText = "Directory path";
            break;
        case ArgType::OUTPUT_FORMAT:
            def.helpText = "Output format (text, json, yaml, csv)";
            break;
        case ArgType::LOG_LEVEL:
            def.helpText = "Log level (trace, debug, info, warn, error, fatal)";
            break;
        case ArgType::RANGE:
            def.helpText = "Value within range";
            break;
        default:
            def.helpText = "Argument";
    }
    
    argumentDefs[name] = def;
}

void ArgumentParser::addFlag(const std::string& name, const std::string& description, bool defaultValue) {
    ArgumentDef def;
    def.name = "--" + name;
    def.description = description;
    def.type = ArgType::FLAG;
    def.defaultValue = defaultValue ? "true" : "false";
    def.hidden = false;
    def.helpText = "Flag option";
    
    argumentDefs[def.name] = def;
}

void ArgumentParser::addOption(const std::string& name, const std::string& description, ArgType type,
                              const std::string& defaultValue, bool required) {
    std::string flagName = "--" + name;
    addArgument(flagName, description, type, defaultValue, required);
}

void ArgumentParser::addCommand(const std::string& name, const std::string& description,
                               const std::vector<ArgumentDef>& arguments, bool hidden) {
    CommandDef cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.arguments = arguments;
    cmd.hidden = hidden;
    cmd.handler = nullptr;
    
    commandDefs[name] = cmd;
}

bool ArgumentParser::parse() {
    parseArgs(argc, argv);
    processCommandLineArgs();
    return validate();
}

bool ArgumentParser::parse(const std::string& inputString) {
    // For script mode parsing
    std::stringstream ss(inputString);
    std::string line;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, line)) {
        // Simple tokenization (could be improved)
        size_t pos = 0;
        while ((pos = line.find(' ')) != std::string::npos) {
            std::string token = line.substr(0, pos);
            if (!token.empty()) {
                tokens.push_back(token);
            }
            line.erase(0, pos + 1);
        }
        if (!line.empty()) {
            tokens.push_back(line);
        }
    }
    
    // Convert to char** format
    char** scriptArgs = new char*[tokens.size() + 1];
    for (size_t i = 0; i < tokens.size(); ++i) {
        scriptArgs[i] = new char[tokens[i].size() + 1];
        strcpy(scriptArgs[i], tokens[i].c_str());
    }
    scriptArgs[tokens.size()] = nullptr;
    
    argc = tokens.size();
    argv = scriptArgs;
    
    bool result = parse();
    
    // Cleanup
    for (size_t i = 0; i < tokens.size(); ++i) {
        delete[] scriptArgs[i];
    }
    delete[] scriptArgs;
    
    return result;
}

void ArgumentParser::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for help/version flags
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version") {
            versionRequested = true;
        } else if (arg == "--validation") {
            validationMode = true;
        } else if (arg.find('=') != std::string::npos) {
            parseEqualsFormat(arg);
        } else if (arg.find(' ') != std::string::npos && i + 1 < argc && argv[i + 1][0] != '-') {
            parseSpaceFormat(arg);
            i++; // Skip the next argument
        } else if (arg.substr(0, 2) == "--") {
            parseEqualsFormat("--" + arg.substr(2));
        } else {
            args[arg] = "";
        }
    }
}

void ArgumentParser::parseEqualsFormat(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        std::string key = arg.substr(0, pos);
        std::string value = arg.substr(pos + 1);
        
        // Handle vector arguments
        if (key.find("[]") != std::string::npos) {
            key = key.substr(0, key.find("[]"));
            std::vector<std::string> values;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ',')) {
                values.push_back(trim(item));
            }
            vectorArgs[key] = values;
        } else {
            args[key] = value;
        }
    }
}

void ArgumentParser::parseSpaceFormat(const std::string& arg) {
    size_t pos = arg.find(' ');
    if (pos != std::string::npos) {
        std::string key = arg.substr(0, pos);
        std::string value = arg.substr(pos + 1);
        
        if (key.find("[]") != std::string::npos) {
            key = key.substr(0, key.find("[]"));
            std::vector<std::string> values;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ',')) {
                values.push_back(trim(item));
            }
            vectorArgs[key] = values;
        } else {
            args[key] = value;
        }
    }
}

void ArgumentParser::processCommandLineArgs() {
    // Check for subcommand
    for (const auto& [key, value] : args) {
        if (commandDefs.find(key) != commandDefs.end()) {
            currentCommand = key;
            // Extract command arguments (simplified)
            break;
        }
    }
}

bool ArgumentParser::validate() const {
    errors.clear();
    
    // Check required arguments
    for (const auto& [name, def] : argumentDefs) {
        if (def.required && !hasArg(name)) {
            errors.push_back("Required argument missing: " + name);
        }
    }
    
    // Validate argument values
    for (const auto& [name, value] : args) {
        auto it = argumentDefs.find(name);
        if (it != argumentDefs.end()) {
            if (!validateArgument(it->second, value)) {
                errors.push_back("Invalid value for argument " + name + ": " + value);
            }
        }
    }
    
    return errors.empty();
}

bool ArgumentParser::validateArgument(const ArgumentDef& def, const std::string& value) const {
    if (def.validator) {
        return def.validator(value);
    }
    
    // Default validation based on type
    switch (def.type) {
        case ArgType::INTEGER: {
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                return false;
            }
        }
        case ArgType::FLOAT: {
            try {
                std::stof(value);
                return true;
            } catch (...) {
                return false;
            }
        }
        case ArgType::BOOLEAN: {
            return value == "true" || value == "false" || value == "1" || value == "0";
        }
        case ArgType::ENUM: {
            return std::find(def.enumValues.begin(), def.enumValues.end(), value) != def.enumValues.end();
        }
        case ArgType::RANGE: {
            try {
                double val = std::stod(value);
                return val >= def.range.first && val <= def.range.second;
            } catch (...) {
                return false;
            }
        }
        case ArgType::FILE_PATH: {
            return std::filesystem::exists(value);
        }
        case ArgType::DIRECTORY_PATH: {
            std::filesystem::path path(value);
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        }
        default:
            return true;
    }
}

bool ArgumentParser::hasArg(const std::string& name) const {
    return args.find(name) != args.end();
}

std::optional<std::string> ArgumentParser::getArg(const std::string& name) const {
    auto it = args.find(name);
    if (it != args.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<int> ArgumentParser::getInt(const std::string& name) const {
    auto value = getArg(name);
    if (value) {
        try {
            return std::stoi(*value);
        } catch (...) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::optional<float> ArgumentParser::getFloat(const std::string& name) const {
    auto value = getArg(name);
    if (value) {
        try {
            return std::stof(*value);
        } catch (...) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::optional<bool> ArgumentParser::getBool(const std::string& name) const {
    auto value = getArg(name);
    if (value) {
        std::string lower = *value;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "1") return true;
        if (lower == "false" || lower == "0") return false;
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> ArgumentParser::getVector(const std::string& name) const {
    auto it = vectorArgs.find(name);
    if (it != vectorArgs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string ArgumentParser::getArg(const std::string& name, const std::string& defaultValue) const {
    auto value = getArg(name);
    return value ? *value : defaultValue;
}

int ArgumentParser::getInt(const std::string& name, int defaultValue) const {
    auto value = getInt(name);
    return value ? *value : defaultValue;
}

float ArgumentParser::getFloat(const std::string& name, float defaultValue) const {
    auto value = getFloat(name);
    return value ? *value : defaultValue;
}

bool ArgumentParser::getBool(const std::string& name, bool defaultValue) const {
    auto value = getBool(name);
    return value ? *value : defaultValue;
}

std::map<std::string, std::string> ArgumentParser::getCommandArgs() const {
    std::map<std::string, std::string> cmdArgs;
    
    // Extract command arguments (simplified)
    // In a full implementation, this would parse based on command definition
    return cmdArgs;
}

void ArgumentParser::printHelp() const {
    std::cout << "NLM — Neural Learning Machine" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "" << std::endl;
    
    // Print usage
    std::cout << "Usage: nlm [OPTIONS] [COMMAND] [ARGS...]" << std::endl;
    std::cout << "" << std::endl;
    
    // Print available commands
    std::cout << "Commands:" << std::endl;
    for (const auto& [name, cmd] : commandDefs) {
        if (!cmd.hidden) {
            std::cout << "  " << name << "    " << cmd.description << std::endl;
        }
    }
    std::cout << "" << std::endl;
    
    // Print options
    std::cout << "Options:" << std::endl;
    for (const auto& [name, def] : argumentDefs) {
        if (!def.hidden) {
            std::string option = "  " + name;
            if (def.required) option += "*";
            option += std::string(50 - option.length(), ' ');
            
            std::cout << option << "  " << def.description << std::endl;
            
            if (!def.helpText.empty() && def.helpText != "Flag option" && 
                def.helpText != "Option flag" && def.helpText != "Enable this flag") {
                std::cout << "          " << def.helpText << std::endl;
            }
        }
    }
    std::cout << "" << std::endl;
    
    // Print examples
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --help                                    Show this help message" << std::endl;
    std::cout << "  nlm --config=config.json                      Load configuration from file" << std::endl;
    std::cout << "  nlm --run --brain.neurons=1000 --simulation.timestep=0.001" << std::endl;
    std::cout << "  nlm session save --path=session.json" << std::endl;
    std::cout << "  nlm session load --path=session.json" << std::endl;
    std::cout << "" << std::endl;
    
    // Print validation
    if (hasErrors()) {
        std::cout << "Validation Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
        std::cout << "" << std::endl;
    }
}

void ArgumentParser::printUsage(const std::string& command) const {
    std::cout << "Usage:" << std::endl;
    if (!currentCommand.empty()) {
        std::cout << "  " << currentCommand << " [OPTIONS]" << std::endl;
    } else {
        std::cout << "  nlm [OPTIONS] [COMMAND] [ARGS...]" << std::endl;
    }
}

void ArgumentParser::printArgumentHelp(const std::string& name) const {
    auto it = argumentDefs.find(name);
    if (it != argumentDefs.end()) {
        const auto& def = it->second;
        std::cout << "Argument: " << name << std::endl;
        std::cout << "  Description: " << def.description << std::endl;
        std::cout << "  Type: " << static_cast<int>(def.type) << std::endl;
        std::cout << "  Default: " << def.defaultValue << std::endl;
        if (!def.helpText.empty()) {
            std::cout << "  Help: " << def.helpText << std::endl;
        }
    }
}

void ArgumentParser::printVersion() const {
    std::cout << "NLM Version 0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with C++20" << std::endl;
}

bool ArgumentParser::hasShortOption(char c) const {
    for (const auto& [key, value] : args) {
        if (key.length() == 2 && key[0] == '-' && key[1] == c) {
            return true;
        }
    }
    return false;
}

bool ArgumentParser::hasLongOption(const std::string& name) const {
    std::string flagName = "--" + name;
    return hasArg(name);
}

std::string ArgumentParser::getPositionalArgument(size_t index) const {
    // Simplified implementation - in a real implementation,
    // this would track positional arguments separately
    return "";
}

bool ArgumentParser::loadScript(const std::string& scriptPath) {
    // Load script file and parse
    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        errors.push_back("Cannot open script file: " + scriptPath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parse(buffer.str());
}

bool ArgumentParser::executeScript(const std::string& scriptPath) {
    if (!loadScript(scriptPath)) {
        return false;
    }
    return validate();
}

bool ArgumentParser::loadConfigFile(const std::string& configPath) {
    // Load configuration file (JSON/YAML/XML)
    // This would parse the config file and add arguments
    std::ifstream file(configPath);
    if (!file.is_open()) {
        errors.push_back("Cannot open config file: " + configPath);
        return false;
    }
    
    // Implementation would parse different formats
    return true;
}

bool ArgumentParser::saveConfigFile(const std::string& configPath) const {
    // Save current arguments to config file
    std::ofstream file(configPath);
    if (!file.is_open()) {
        errors.push_back("Cannot write config file: " + configPath);
        return false;
    }
    
    // Implementation would serialize arguments
    return true;
}

void ArgumentParser::setProgressCallback(std::function<void(double)> callback) {
    // Set callback for progress updates
    // This would be used during long-running operations
}

std::string ArgumentParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string ArgumentParser::ltrim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    return str.substr(first);
}

std::string ArgumentParser::rtrim(const std::string& str) const {
    size_t last = str.find_last_not_of(" \t\n\r");
    if (last == std::string::npos) return "";
    return str.substr(0, last + 1);
}

} // namespace nlm
