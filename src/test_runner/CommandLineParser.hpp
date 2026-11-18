#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "src/core/Config/Config.hpp"

namespace nlm {

// Command line option definition
struct CommandLineOption {
    std::string longName;
    std::string shortName;
    std::string description;
    std::string defaultValue;
    bool required;
    std::vector<std::string> valueTypes;
};

// Command line argument parser
class CommandLineParser {
public:
    CommandLineParser() {}
    
    // Add an option
    void addOption(const std::string& longName, const std::string& shortName = "",
                   const std::string& description = "", const std::string& defaultValue = "",
                   bool required = false, const std::vector<std::string>& valueTypes = {"string"}) {
        CommandLineOption option;
        option.longName = longName;
        option.shortName = shortName;
        option.description = description;
        option.defaultValue = defaultValue;
        option.required = required;
        option.valueTypes = valueTypes;
        
        options[longName] = option;
        if (!shortName.empty()) {
            shortOptions[shortName] = longName;
        }
    }
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        return parseArguments(args);
    }
    
    // Parse from argument vector (for testing)
    bool parse(const std::vector<std::string>& args) {
        return parseArguments(args);
    }
    
    // Get option value
    template<typename T>
    T getOption(const std::string& name, const T& defaultValue) const {
        auto it = parsedValues.find(name);
        if (it == parsedValues.end()) {
            return defaultValue;
        }
        return convertValue<T>(it->second);
    }
    
    // Get option value with string type
    std::string getStringOption(const std::string& name, const std::string& defaultValue = "") const {
        return getOption<std::string>(name, defaultValue);
    }
    
    // Get option value with int type
    int getIntOption(const std::string& name, int defaultValue = 0) const {
        return getOption<int>(name, defaultValue);
    }
    
    // Get option value with float type
    float getFloatOption(const std::string& name, float defaultValue = 0.0f) const {
        return getOption<float>(name, defaultValue);
    }
    
    // Get option value with bool type
    bool getBoolOption(const std::string& name, bool defaultValue = false) const {
        return getOption<bool>(name, defaultValue);
    }
    
    // Check if option was provided
    bool hasOption(const std::string& name) const {
        return parsedValues.find(name) != parsedValues.end();
    }
    
    // Print help
    void printHelp() const {
        std::cout << "NLM (Neural Learning Machine) - Command Line Interface\n";
        std::cout << "================================================================\n\n";
        
        std::cout << "Usage:\n";
        std::cout << "  nlm [OPTIONS] [CONFIG_FILE]\n\n";
        
        std::cout << "Options:\n";
        for (const auto& pair : options) {
            const CommandLineOption& option = pair.second;
            std::cout << "  ";
            if (!option.shortName.empty()) {
                std::cout << "-" << option.shortName << ", ";
            }
            std::cout << "--" << option.longName;
            
            if (!option.defaultValue.empty()) {
                std::cout << "=" << option.defaultValue;
            }
            
            std::cout << "\n";
            std::cout << "      " << option.description << "\n";
            
            if (option.required) {
                std::cout << "      Required\n";
            }
        }
        
        std::cout << "\nConfig file:\n";
        std::cout << "  CONFIG_FILE     Load configuration from file\n\n";
        
        std::cout << "Common examples:\n";
        std::cout << "  nlm --help                       Show this help\n";
        std::cout << "  nlm --test-mode --neuron-count 500\n";
        std::cout << "  nlm --batch-mode --output results.json config.cfg\n";
    }
    
    // Get all parsed values
    const std::map<std::string, std::string>& getParsedValues() const {
        return parsedValues;
    }
    
private:
    std::map<std::string, CommandLineOption> options;
    std::map<std::string, std::string> shortOptions;
    std::map<std::string, std::string> parsedValues;
    std::string configFile;
    
    bool parseArguments(const std::vector<std::string>& args) {
        for (size_t i = 0; i < args.size(); ++i) {
            std::string arg = args[i];
            
            // Check for help option
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            
            // Check for config file (last positional argument)
            if (i == args.size() - 1 && arg.find('.') != std::string::npos) {
                configFile = arg;
                continue;
            }
            
            // Check for option (--name value or -n value)
            if (arg.substr(0, 2) == "--") {
                std::string optionName = arg.substr(2);
                auto optionIt = options.find(optionName);
                
                if (optionIt == options.end()) {
                    // Check for short option
                    if (arg.length() == 2) {
                        char shortOpt = arg[1];
                        auto shortIt = shortOptions.find(std::string(1, shortOpt));
                        if (shortIt != shortOptions.end()) {
                            optionIt = options.find(shortIt->second);
                        }
                    }
                    
                    if (optionIt == options.end()) {
                        std::cerr << "Error: Unknown option --" << optionName << std::endl;
                        return false;
                    }
                }
                
                const CommandLineOption& option = optionIt->second;
                
                // Check if option requires a value
                if (option.valueTypes.size() == 0 || 
                    (option.valueTypes.size() > 0 && option.valueTypes[0] != "flag")) {
                    if (i + 1 >= args.size()) {
                        std::cerr << "Error: Option --" << optionName << " requires a value" << std::endl;
                        return false;
                    }
                    
                    std::string value = args[++i];
                    
                    // Convert value type
                    if (option.valueTypes.size() > 0) {
                        if (option.valueTypes[0] == "int") {
                            if (!isInteger(value)) {
                                std::cerr << "Error: Option --" << optionName << " expects an integer" << std::endl;
                                return false;
                            }
                        } else if (option.valueTypes[0] == "float") {
                            if (!isFloat(value)) {
                                std::cerr << "Error: Option --" << optionName << " expects a float" << std::endl;
                                return false;
                            }
                        } else if (option.valueTypes[0] == "bool") {
                            // For bool options, check if it's a flag or has value
                            if (value == "true" || value == "false" || value == "1" || value == "0") {
                                // Accept boolean string
                            } else {
                                std::cerr << "Error: Option --" << optionName << " expects a boolean (true/false)" << std::endl;
                                return false;
                            }
                        }
                    }
                    
                    parsedValues[optionName] = value;
                } else {
                    // Flag option (no value)
                    parsedValues[optionName] = "true";
                }
            } else if (arg.substr(0, 1) == "-") {
                // Handle short option
                std::string shortOption = arg.substr(1);
                auto optionIt = shortOptions.find(shortOption);
                
                if (optionIt == shortOptions.end()) {
                    std::cerr << "Error: Unknown option -" << shortOption << std::endl;
                    return false;
                }
                
                std::string optionName = optionIt->second;
                const CommandLineOption& option = options[optionName];
                
                if (option.valueTypes.size() == 0 || 
                    (option.valueTypes.size() > 0 && option.valueTypes[0] != "flag")) {
                    if (i + 1 >= args.size()) {
                        std::cerr << "Error: Option -" << shortOption << " requires a value" << std::endl;
                        return false;
                    }
                    
                    std::string value = args[++i];
                    
                    if (option.valueTypes.size() > 0) {
                        if (option.valueTypes[0] == "int") {
                            if (!isInteger(value)) {
                                std::cerr << "Error: Option -" << shortOption << " expects an integer" << std::endl;
                                return false;
                            }
                        } else if (option.valueTypes[0] == "float") {
                            if (!isFloat(value)) {
                                std::cerr << "Error: Option -" << shortOption << " expects a float" << std::endl;
                                return false;
                            }
                        }
                    }
                    
                    parsedValues[optionName] = value;
                } else {
                    parsedValues[optionName] = "true";
                }
            } else {
                std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
                return false;
            }
        }
        
        // Check required options
        for (const auto& pair : options) {
            if (pair.second.required && parsedValues.find(pair.first) == parsedValues.end()) {
                std::cerr << "Error: Required option --" << pair.first << " is missing" << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    bool isInteger(const std::string& str) const {
        if (str.empty()) return false;
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            start = 1;
        }
        return std::all_of(str.begin() + start, str.end(), ::isdigit);
    }
    
    bool isFloat(const std::string& str) const {
        if (str.empty()) return false;
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            start = 1;
        }
        
        bool hasDecimal = false;
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] == '.') {
                if (hasDecimal) return false;  // Multiple decimals not allowed
                hasDecimal = true;
            } else if (!std::isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }
    
    template<typename T>
    T convertValue(const std::string& str) const {
        std::istringstream iss(str);
        T value;
        if (std::is_same<T, int>::value) {
            iss >> value;
        } else if (std::is_same<T, float>::value) {
            iss >> value;
        } else if (std::is_same<T, double>::value) {
            double temp;
            iss >> temp;
            value = static_cast<double>(temp);
        } else if (std::is_same<T, bool>::value) {
            // Convert string to bool
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            value = (str == "true" || str == "1" || str == "yes" || str == "on");
        } else {
            value = str;
        }
        return value;
    }
    
    // Get config file path
    const std::string& getConfigFile() const {
        return configFile;
    }
};

} // namespace nlm
