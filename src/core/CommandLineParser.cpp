#include "CommandLineParser.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <map>

namespace nlm {

struct CommandLineParser::Impl {
    std::vector<Argument> arguments;
    std::map<std::string, std::string> parsedValues;
    std::map<std::string, std::vector<std::string>> parsedVectors;
    std::vector<std::string> validationErrors;
    bool parsed;
    
    Impl() : parsed(false) {}
};

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()) {}

CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::addArgument(const std::string& key, const std::string& shorthand, 
                                   const std::string& description, const std::string& type,
                                   const std::string& category, bool required, 
                                   const std::string& defaultValue) {
    Argument arg;
    arg.key = key;
    arg.shorthand = shorthand;
    arg.description = description;
    arg.type = type;
    arg.category = category;
    arg.required = required;
    arg.defaultValue = defaultValue;
    
    pImpl->arguments.push_back(arg);
}

void CommandLineParser::addFlag(const std::string& key, const std::string& description, 
                               const std::string& category) {
    Argument arg;
    arg.key = key;
    arg.description = description;
    arg.type = "bool";
    arg.category = category;
    arg.required = false;
    
    pImpl->arguments.push_back(arg);
}

void CommandLineParser::addVectorArgument(const std::string& key, const std::string& description,
                                         const std::string& category, size_t minSize, size_t maxSize) {
    Argument arg;
    arg.key = key;
    arg.description = description;
    arg.type = "vector";
    arg.category = category;
    arg.required = false;
    
    pImpl->arguments.push_back(arg);
}

bool CommandLineParser::parse(int argc, char** argv) {
    pImpl->validationErrors.clear();
    
    // Check for required arguments
    for (const auto& arg : pImpl->arguments) {
        if (arg.required && !hasArgument(argv, argc, arg.key) && !hasArgument(argv, argc, arg.shorthand)) {
            pImpl->validationErrors.push_back("Required argument --" + arg.key + " is missing");
        }
    }
    
    if (!pImpl->validationErrors.empty()) {
        return false;
    }
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check if it's a flag (--key)
        if (arg.find('=') == std::string::npos && (arg.find("--") == 0 || (arg.find('-') == 0 && arg.size() > 1))) {
            // Flag
            std::string key = arg;
            if (arg.find("--") == 0) {
                key = arg.substr(2);
            } else {
                key = arg.substr(1);
            }
            
            // Find argument definition
            auto argDef = findArgument(key);
            if (argDef && argDef->type == "bool") {
                pImpl->parsedValues[key] = "true";
            }
        } else if (arg.find('=') != std::string::npos) {
            // Key=value pair
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Handle shorthand --key=value format
            if (key.find("--") == 0) {
                key = key.substr(2);
            }
            
            pImpl->parsedValues[key] = value;
        }
    }
    
    // Apply defaults for missing required arguments
    for (const auto& arg : pImpl->arguments) {
        if (arg.required && pImpl->parsedValues.find(arg.key) == pImpl->parsedValues.end()) {
            pImpl->parsedValues[arg.key] = arg.defaultValue;
        }
    }
    
    pImpl->parsed = true;
    return true;
}

std::string CommandLineParser::getString(const std::string& key) const {
    auto it = pImpl->parsedValues.find(key);
    if (it != pImpl->parsedValues.end()) {
        return it->second;
    }
    
    // Check for shorthand
    for (const auto& shorthand : findShorthands(key)) {
        auto shorthandIt = pImpl->parsedValues.find(shorthand);
        if (shorthandIt != pImpl->parsedValues.end()) {
            return shorthandIt->second;
        }
    }
    
    return "";
}

int CommandLineParser::getInt(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

double CommandLineParser::getDouble(const std::string& key) const {
    std::string value = getString(key);
    try {
        return std::stod(value);
    } catch (const std::exception&) {
        return 0.0;
    }
}

bool CommandLineParser::getBool(const std::string& key) const {
    std::string value = getString(key);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

std::vector<std::string> CommandLineParser::getStringVector(const std::string& key) const {
    auto it = pImpl->parsedVectors.find(key);
    if (it != pImpl->parsedVectors.end()) {
        return it->second;
    }
    
    // Parse vector from string
    std::string value = getString(key);
    std::vector<std::string> vector;
    if (!value.empty()) {
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ',')) {
            vector.push_back(item);
        }
    }
    
    return vector;
}

std::vector<int> CommandLineParser::getIntVector(const std::string& key) const {
    std::vector<std::string> stringVector = getStringVector(key);
    std::vector<int> intVector;
    
    for (const auto& str : stringVector) {
        try {
            intVector.push_back(std::stoi(str));
        } catch (const std::exception&) {
            intVector.push_back(0);
        }
    }
    
    return intVector;
}

std::vector<double> CommandLineParser::getDoubleVector(const std::string& key) const {
    std::vector<std::string> stringVector = getStringVector(key);
    std::vector<double> doubleVector;
    
    for (const auto& str : stringVector) {
        try {
            doubleVector.push_back(std::stod(str));
        } catch (const std::exception&) {
            doubleVector.push_back(0.0);
        }
    }
    
    return doubleVector;
}

template<typename T>
T CommandLineParser::getOr(const std::string& key, const T& defaultValue) const {
    // This would need template specialization for different types
    // For now, returning defaultValue
    return defaultValue;
}

void CommandLineParser::printHelp(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]\n\n";
    
    // Group arguments by category
    std::map<std::string, std::vector<Argument>> categorizedArgs;
    for (const auto& arg : pImpl->arguments) {
        categorizedArgs[arg.category].push_back(arg);
    }
    
    for (const auto& pair : categorizedArgs) {
        std::cout << "Options - " << pair.first << ":\n";
        
        for (const auto& arg : pair.second) {
            std::cout << "  ";
            
            // Show shorthand if available
            if (!arg.shorthand.empty()) {
                std::cout << "-" << arg.shorthand << ", ";
            }
            
            std::cout << "--" << arg.key;
            
            if (arg.type == "bool") {
                std::cout << " (flag)";
            } else if (arg.type == "vector") {
                std::cout << " <value1,value2,...>";
            } else {
                std::cout << " <value>";
            }
            
            if (arg.required) {
                std::cout << " (required)";
            }
            
            if (!arg.defaultValue.empty()) {
                std::cout << " (default: " << arg.defaultValue << ")";
            }
            
            std::cout << "\n    " << arg.description << "\n";
        }
        
        std::cout << std::endl;
    }
}

std::string CommandLineParser::getUsageSummary() const {
    std::ostringstream oss;
    oss << "Usage: program [options]\n\n";
    
    for (const auto& arg : pImpl->arguments) {
        oss << "--" << arg.key;
        if (!arg.shorthand.empty()) {
            oss << "/-" << arg.shorthand;
        }
        oss << " " << arg.description << "\n";
    }
    
    return oss.str();
}

bool CommandLineParser::validate() const {
    return pImpl->validationErrors.empty();
}

std::vector<std::string> CommandLineParser::getValidationErrors() const {
    return pImpl->validationErrors;
}

void CommandLineParser::applyToConfig(EnhancedConfig& config) const {
    for (const auto& pair : pImpl->parsedValues) {
        // Try to determine type and set accordingly
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Try to find argument definition for this key
        auto argDef = findArgument(key);
        if (argDef) {
            // Convert based on type
            if (argDef->type == "bool") {
                bool boolVal = (value == "true" || value == "1" || value == "yes" || value == "on");
                config.setValidated<bool>(key, boolVal, ConfigSource::CommandLine, argDef->description);
            } else if (argDef->type == "int") {
                try {
                    int intVal = std::stoi(value);
                    config.setValidated<int>(key, intVal, ConfigSource::CommandLine, argDef->description);
                } catch (const std::exception&) {
                    config.setValidated<std::string>(key, value, ConfigSource::CommandLine, argDef->description);
                }
            } else if (argDef->type == "float") {
                try {
                    float floatVal = std::stof(value);
                    config.setValidated<float>(key, floatVal, ConfigSource::CommandLine, argDef->description);
                } catch (const std::exception&) {
                    config.setValidated<std::string>(key, value, ConfigSource::CommandLine, argDef->description);
                }
            } else {
                config.setValidated<std::string>(key, value, ConfigSource::CommandLine, argDef->description);
            }
        } else {
            // Default to string
            config.setValidated<std::string>(key, value, ConfigSource::CommandLine);
        }
    }
}

void CommandLineParser::loadFromEnvironment(const std::string& prefix) {
    // This would load environment variables into the argument parser
    // For now, just a placeholder
}

// Private helper methods
bool CommandLineParser::hasArgument(char** argv, int argc, const std::string& key) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--") == 0) {
            std::string argKey = arg.substr(2);
            if (argKey == key) {
                return true;
            }
        } else if (arg.find('-') == 0 && arg.size() > 1) {
            std::string argKey = arg.substr(1);
            if (argKey == key) {
                return true;
            }
        }
    }
    return false;
}

CommandLineParser::Argument* CommandLineParser::findArgument(const std::string& key) {
    for (auto& arg : pImpl->arguments) {
        if (arg.key == key) {
            return &arg;
        }
    }
    return nullptr;
}

std::vector<std::string> CommandLineParser::findShorthands(const std::string& key) const {
    std::vector<std::string> shorthands;
    for (const auto& arg : pImpl->arguments) {
        if (arg.key == key && !arg.shorthand.empty()) {
            shorthands.push_back(arg.shorthand);
        }
    }
    return shorthands;
}

std::string CommandLineParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

ConfigValue CommandLineParser::parseValue(const std::string& value) const {
    // Try to parse as different types
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    try {
        // Try integer
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            return intVal;
        }
        
        // Try float
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            return doubleVal;
        }
    } catch (const std::exception&) {}
    
    // Default to string
    return value;
}

} // namespace nlm