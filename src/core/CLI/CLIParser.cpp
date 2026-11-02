#include "CLI/CLIParser.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLIParser::CLIParser(const std::string& programName) 
    : programName(programName), verbose(false) {
}

void CLIParser::addArgument(const std::string& name, const std::string& shortName,
                           const std::string& description, const std::string& defaultValue,
                           ArgType type, bool required, const std::vector<std::string>& choices) {
    ArgumentDef arg;
    arg.name = name;
    arg.shortName = shortName;
    arg.description = description;
    arg.type = type;
    arg.defaultValue = defaultValue;
    arg.choices = choices;
    arg.required = required;
    arg.callback = nullptr;
    
    arguments.push_back(arg);
    parsedValues[name] = defaultValue; // Initialize with default
}

bool CLIParser::parse(int argc, char** argv) {
    if (!handleHelpAndVersion(argc, argv)) {
        return false;
    }
    
    return parseArguments(argc, argv);
}

bool CLIParser::handleHelpAndVersion(int argc, char** argv) {
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        } else if (arg == "--verbose" || arg == "-V") {
            verbose = true;
        }
    }
    return true;
}

template<typename T>
T CLIParser::stringToValue(const std::string& str) const {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}

bool CLIParser::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for --name=value format
        size_t eqPos = arg.find('=');
        if (eqPos != std::string::npos) {
            std::string name = arg.substr(0, eqPos);
            std::string value = arg.substr(eqPos + 1);
            
            // Remove leading dashes
            if (name.size() > 1 && name[0] == '-' && name[1] == '-') {
                name = name.substr(2);
            } else if (name.size() > 1 && name[0] == '-') {
                name = name.substr(1);
            }
            
            // Find the argument definition
            for (auto& argDef : arguments) {
                if (argDef.name == name) {
                    parsedValues[name] = value;
                    if (argDef.callback) {
                        argDef.callback(value);
                    }
                    break;
                }
            }
        }
        // Check for --name value format
        else if (arg.size() > 1 && arg[0] == '-' && arg[1] == '-') {
            std::string name = arg.substr(2);
            
            // Find the argument definition
            for (auto& argDef : arguments) {
                if (argDef.name == name) {
                    // Check if next argument is a value (not a flag)
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        parsedValues[name] = argv[++i];
                        if (argDef.callback) {
                            argDef.callback(argv[i]);
                        }
                    }
                    break;
                }
            }
        }
        // Check for -n value format
        else if (arg.size() > 0 && arg[0] == '-') {
            std::string name = arg.substr(1);
            
            // Find the argument definition
            for (auto& argDef : arguments) {
                if (argDef.shortName == name) {
                    // Check if next argument is a value (not a flag)
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        parsedValues[argDef.name] = argv[++i];
                        if (argDef.callback) {
                            argDef.callback(argv[i]);
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return validateArguments();
}

template<typename T>
T CLIParser::getValue(const std::string& name, const T& defaultValue) const {
    auto it = parsedValues.find(name);
    if (it == parsedValues.end()) {
        return defaultValue;
    }
    
    return stringToValue<T>(it->second);
}

std::string CLIParser::getStringValue(const std::string& name, const std::string& defaultValue) const {
    auto it = parsedValues.find(name);
    if (it == parsedValues.end()) {
        return defaultValue;
    }
    
    return it->second;
}

bool CLIParser::hasArgument(const std::string& name) const {
    return parsedValues.find(name) != parsedValues.end() && 
           !parsedValues.at(name).empty();
}

std::vector<std::string> CLIParser::getArgumentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : parsedValues) {
        names.push_back(pair.first);
    }
    return names;
}

void CLIParser::printHelp() const {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& arg : arguments) {
        std::cout << "  ";
        if (!arg.shortName.empty()) {
            std::cout << "-" << arg.shortName << ", ";
        }
        std::cout << "--" << arg.name;
        
        if (arg.type == ArgType::CHOICE && !arg.choices.empty()) {
            std::cout << "=";
            bool first = true;
            for (const auto& choice : arg.choices) {
                if (!first) std::cout << ",";
                std::cout << choice;
                first = false;
            }
        } else if (arg.type == ArgType::STRING || arg.type == ArgType::INTEGER || 
                   arg.type == ArgType::FLOAT || arg.type == ArgType::BOOLEAN) {
            std::cout << "=<value>";
        }
        
        std::cout << std::endl;
        
        std::cout << "      " << arg.description;
        if (!arg.defaultValue.empty()) {
            std::cout << " (default: " << arg.defaultValue << ")";
        }
        if (arg.required) {
            std::cout << " (required)";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --experiment=phase3 --epochs=10" << std::endl;
    std::cout << "  " << programName << " -e phase4 -n 200 -c config.json" << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << programName << " version 0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
}

bool CLIParser::validateArguments() const {
    for (const auto& arg : arguments) {
        if (arg.required && !hasArgument(arg.name)) {
            std::cerr << "Error: Required argument --" << arg.name << " is missing." << std::endl;
            return false;
        }
        
        if (hasArgument(arg.name) && arg.type == ArgType::CHOICE) {
            const std::string& value = getStringValue(arg.name);
            if (std::find(arg.choices.begin(), arg.choices.end(), value) == arg.choices.end()) {
                std::cerr << "Error: Invalid value for --" << arg.name << "." << std::endl;
                std::cerr << "  Valid choices: ";
                for (size_t i = 0; i < arg.choices.size(); ++i) {
                    if (i > 0) std::cerr << ", ";
                    std::cerr << arg.choices[i];
                }
                std::cerr << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

} // namespace nlm