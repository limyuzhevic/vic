// Command-line argument parser for NLM
// Provides enhanced CLI with validation and help

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Command-line argument definition
struct CLIOption {
    std::string name;
    std::string description;
    std::string defaultValue;
    bool required;
    std::string type;  // "string", "int", "float", "bool"
    std::string validation;  // Validation pattern or constraint
    
    CLIOption(const std::string& n, const std::string& desc, const std::string& def = "", 
             bool req = false, const std::string& typeName = "string",
             const std::string& validationPattern = "")
        : name(n), description(desc), defaultValue(def), required(req), 
          type(typeName), validation(validationPattern) {}
};

// Command-line argument parser
class CLIParser {
private:
    struct ParsedArg {
        std::string name;
        std::string value;
        bool isSet;
    };
    
    std::vector<CLIOption> options;
    std::map<std::string, std::string> parsedArgs;
    std::vector<std::string> positionalArgs;
    bool helpRequested;
    bool versionRequested;
    
    // Built-in option types
    static constexpr const char* HELP_FLAG = "--help";
    static constexpr const char* VERSION_FLAG = "--version";
    static constexpr const char* CONFIG_FLAG = "--config";
    static constexpr const char* LOG_LEVEL_FLAG = "--log-level";
    
public:
    CLIParser() : helpRequested(false), versionRequested(false) {
        // Add built-in options
        addOption(HELP_FLAG, "Show help message and exit", "", false, "flag");
        addOption(VERSION_FLAG, "Show version information and exit", "", false, "flag");
        addOption(CONFIG_FLAG, "Load configuration from file", "", false, "string");
        addOption("--log-level", "Set logging level (DEBUG, INFO, WARN, ERROR)", "INFO", false, "string");
    }
    
    // Add an option
    void addOption(const std::string& name, const std::string& description,
                   const std::string& defaultValue = "", bool required = false,
                   const std::string& type = "string",
                   const std::string& validation = "") {
        options.emplace_back(name, description, defaultValue, required, type, validation);
    }
    
    // Parse command line arguments
    bool parse(int argc, char** argv) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                // Handle built-in flags
                if (arg == HELP_FLAG) {
                    helpRequested = true;
                    continue;
                }
                
                if (arg == VERSION_FLAG) {
                    versionRequested = true;
                    continue;
                }
                
                // Handle long options (--key=value or --key value)
                if (arg.substr(0, 2) == "--") {
                    size_t pos = arg.find('=');
                    if (pos != std::string::npos) {
                        std::string key = arg.substr(0, pos);
                        std::string value = arg.substr(pos + 1);
                        if (!setArgument(key, value)) {
                            return false;
                        }
                    } else {
                        // Check if next argument exists
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            std::string key = arg;
                            std::string value = argv[++i];
                            if (!setArgument(key, value)) {
                                return false;
                            }
                        } else {
                            std::cerr << "Error: Option '" << arg << "' requires a value." << std::endl;
                            return false;
                        }
                    }
                } 
                // Handle short options (-k value)
                else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
                    std::string key = arg.substr(1);
                    if (key.size() == 1) {
                        // Single character option
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            std::string value = argv[++i];
                            if (!setArgument("-" + key, value)) {
                                return false;
                            }
                        } else {
                            std::cerr << "Error: Option '-" << key << "' requires a value." << std::endl;
                            return false;
                        }
                    } else {
                        // Multiple short options (-abc)
                        for (char c : key) {
                            std::string singleKey = "-" + std::string(1, c);
                            if (!setArgument(singleKey, "true")) {
                                return false;
                            }
                        }
                    }
                } else {
                    // Positional argument
                    positionalArgs.push_back(arg);
                }
            }
            
            // Validate required options
            return validateRequiredOptions();
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing command line arguments: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Check if help was requested
    bool isHelpRequested() const { return helpRequested; }
    
    // Check if version was requested
    bool isVersionRequested() const { return versionRequested; }
    
    // Get parsed argument value
    std::string getArgument(const std::string& name) const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            return it->second;
        }
        
        // Return default value if option exists
        for (const auto& option : options) {
            if (option.name == name) {
                return option.defaultValue;
            }
        }
        
        return ""; // Unknown option
    }
    
    // Check if argument was set
    bool hasArgument(const std::string& name) const {
        return parsedArgs.find(name) != parsedArgs.end();
    }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs; }
    
    // Generate help message
    std::string generateHelp(const std::string& programName) const {
        std::ostringstream oss;
        
        oss << programName << " - NLM (Neural Learning Machine)" << std::endl;
        oss << "Phase 2: Real Neural Computation" << std::endl;
        oss << std::endl;
        oss << "Usage: " << programName << " [options] [positional_args]" << std::endl;
        oss << std::endl;
        oss << "Options:" << std::endl;
        
        for (const auto& option : options) {
            oss << "  ";
            if (option.name.size() <= 6) {
                oss << option.name;
                // Pad to align descriptions
                size_t pad = 10 - option.name.size();
                for (size_t i = 0; i < pad; ++i) oss << " ";
            } else {
                oss << option.name << std::endl << "    ";
            }
            oss << option.description;
            
            if (!option.defaultValue.empty()) {
                oss << " (default: " << option.defaultValue << ")";
            }
            if (option.required) {
                oss << " (required)";
            }
            oss << std::endl;
        }
        
        oss << std::endl;
        oss << "Positional arguments:" << std::endl;
        oss << "  simulation_steps   Number of simulation steps to run" << std::endl;
        oss << "  output_file        Output file for results" << std::endl;
        oss << std::endl;
        oss << "Examples:" << std::endl;
        oss << "  " << programName << " --config=myconfig.cfg --neuron_count=1000" << std::endl;
        oss << "  " << programName << " -s 500 -o results.txt" << std::endl;
        oss << "  " << programName << " --help" << std::endl;
        
        return oss.str();
    }
    
    // Validate arguments against options
    bool validateArguments() const {
        for (const auto& [name, value] : parsedArgs) {
            // Find the option definition
            auto it = std::find_if(options.begin(), options.end(),
                [&name](const CLIOption& opt) { return opt.name == name; });
            
            if (it == options.end()) {
                std::cerr << "Warning: Unknown option '" << name << "'" << std::endl;
                continue;
            }
            
            const auto& option = *it;
            
            // Type validation
            if (!validateType(option, value)) {
                return false;
            }
            
            // Pattern validation
            if (!option.validation.empty() && !validatePattern(option, value)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Validate required options
    bool validateRequiredOptions() const {
        for (const auto& option : options) {
            if (option.required && !hasArgument(option.name)) {
                std::cerr << "Error: Required option '" << option.name << "' is missing." << std::endl;
                return false;
            }
        }
        return true;
    }
    
private:
    // Set an argument value
    bool setArgument(const std::string& name, const std::string& value) {
        // Normalize option name
        std::string normalizedName = name;
        
        // Check if option exists
        auto it = std::find_if(options.begin(), options.end(),
            [&normalizedName](const CLIOption& opt) { return opt.name == normalizedName; });n        
        if (it != options.end()) {
            // Valid option
            parsedArgs[normalizedName] = value;
            return true;
        } else {
            std::cerr << "Error: Unknown option '" << name << "'" << std::endl;
            return false;
        }
    }
    
    // Validate argument type
    bool validateType(const CLIOption& option, const std::string& value) const {
        if (option.type == "int") {
            try {
                std::stoi(value);
                return true;
            } catch (...) {
                std::cerr << "Error: Option '" << option.name << "' requires an integer value." << std::endl;
                return false;
            }
        } else if (option.type == "float") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                std::cerr << "Error: Option '" << option.name << "' requires a float value." << std::endl;
                return false;
            }
        } else if (option.type == "bool") {
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            return lowerValue == "true" || lowerValue == "false" || 
                   lowerValue == "1" || lowerValue == "0" ||
                   lowerValue == "yes" || lowerValue == "no";
        }
        return true; // string type always valid
    }
    
    // Validate argument against pattern
    bool validatePattern(const CLIOption& option, const std::string& value) const {
        if (option.validation.empty()) return true;
        
        // Simple pattern matching - could be enhanced with regex
        if (option.validation == "positive") {
            try {
                double val = std::stod(value);
                return val >= 0.0;
            } catch (...) { return false; }
        } else if (option.validation == "non-negative") {
            try {
                double val = std::stod(value);
                return val >= 0.0;
            } catch (...) { return false; }
        } else if (option.validation == "range") {
            // Format: "min,max"
            size_t pos = option.validation.find(',');
            if (pos == std::string::npos) return true;
            
            try {
                double min = std::stod(option.validation.substr(0, pos));
                double max = std::stod(option.validation.substr(pos + 1));
                double val = std::stod(value);
                return val >= min && val <= max;
            } catch (...) { return false; }
        }
        return true;
    }
};

} // namespace nlm