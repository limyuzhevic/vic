# Improved command-line interface for NLM
// Provides advanced options and user-friendly features

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace cli {

// Command option types
enum class OptionType {
    FLAG,
    VALUE,
    OPTIONAL_VALUE,
    MULTIPLE_VALUES
};

// Command option definition
struct CommandOption {
    std::string name;
    std::string shortName;
    std::string description;
    OptionType type;
    std::string defaultValue;
    std::string valueName;
    bool required;
    
    // Custom validation function
    std::function<bool(const std::string&)> validator;
    
    // Custom action function
    std::function<void(const std::string&)> action;
};

// Command help formatter
class HelpFormatter {
public:
    static std::string formatOptions(const std::vector<CommandOption>& options) {
        std::stringstream ss;
        ss << "Usage: nlm [OPTIONS] [EXPERIMENT...]" << std::endl;
        ss << std::endl;
        ss << "Options:" << std::endl;
        
        // Group options by type
        std::vector<CommandOption> flagOptions, valueOptions, optionalOptions;
        
        for (const auto& option : options) {
            if (option.type == OptionType::FLAG) {
                flagOptions.push_back(option);
            } else if (option.type == OptionType::VALUE) {
                valueOptions.push_back(option);
            } else {
                optionalOptions.push_back(option);
            }
        }
        
        // Format flag options
        if (!flagOptions.empty()) {
            ss << "  Flags:" << std::endl;
            for (const auto& option : flagOptions) {
                std::string flag = "--" + option.name;
                if (!option.shortName.empty()) {
                    flag += ", -" + option.shortName;
                }
                ss << "    " << flag << std::string(20 - flag.length(), ' ') << option.description << std::endl;
            }
            ss << std::endl;
        }
        
        // Format value options
        if (!valueOptions.empty()) {
            ss << "  Required Options:" << std::endl;
            for (const auto& option : valueOptions) {
                std::string opt = "--" + option.name + "=" + option.valueName;
                if (!option.shortName.empty()) {
                    opt += ", -" + option.shortName + " " + option.valueName;
                }
                ss << "    " << opt << std::string(30 - opt.length(), ' ') << option.description << std::endl;
            }
            ss << std::endl;
        }
        
        // Format optional options
        if (!optionalOptions.empty()) {
            ss << "  Optional Options:" << std::endl;
            for (const auto& option : optionalOptions) {
                std::string opt = "--" + option.name + "=" + option.valueName;
                if (!option.shortName.empty()) {
                    opt += ", -" + option.shortName + " " + option.valueName;
                }
                ss << "    " << opt << std::string(30 - opt.length(), ' ') << option.description << std::endl;
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string formatExperimentHelp() {
        std::stringstream ss;
        ss << "Available Experiments:" << std::endl;
        ss << std::endl;
        ss << "  connectivity    Basic neural connectivity test" << std::endl;
        ss << "  plasticity      Plasticity learning experiment" << std::endl;
        ss << "  stdp            STDP verification experiment" << std::endl;
        ss << std::endl;
        ss << "  --help          Show this help message" << std::endl;
        ss << "  --version       Show version information" << std::endl;
        ss << "  --list          List available experiments" << std::endl;
        ss << "  --config FILE   Configuration file (default: configs/default.cfg)" << std::endl;
        ss << std::endl;
        ss << "Examples:" << std::endl;
        ss << "  nlm                                    Run all experiments" << std::endl;
        ss << "  nlm connectivity                       Run connectivity experiment only" << std::endl;
        ss << "  nlm --config myconfig.cfg plasticity   Run with custom config" << std::endl;
        ss << "  nlm --help                             Show help" << std::endl;
        return ss.str();
    }
    
    static std::string formatVersionInfo() {
        std::stringstream ss;
        ss << "NLM Phase 2: Real Neural Computation" << std::endl;
        ss << "Version: 2.0.0" << std::endl;
        ss << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
        ss << "Architecture: C++20" << std::endl;
        ss << "License: MIT" << std::endl;
        return ss.str();
    }
};

// Advanced command-line parser
class CommandLineParserAdvanced {
public:
    CommandLineParserAdvanced(int argc, char** argv) 
        : argc(argc), argv(argv), helpRequested(false), versionRequested(false), listRequested(false) {}
    
    // Register a command option
    void registerOption(const CommandOption& option) {
        options.push_back(option);
        
        // Build lookup tables
        if (!option.shortName.empty()) {
            shortOptionMap[option.shortName] = &options.back();
        }
        longOptionMap[option.name] = &options.back();
        
        // For optional values, add to default options
        if (option.type == OptionType::OPTIONAL_VALUE) {
            optionalValueOptions.push_back(&options.back());
        }
    }
    
    // Parse command line arguments
    bool parse() {
        // First pass: parse options
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
                continue;
            }
            
            if (arg == "--version") {
                versionRequested = true;
                continue;
            }
            
            if (arg == "--list") {
                listRequested = true;
                continue;
            }
            
            // Check for option with value (--option=value or -o value)
            bool optionFound = false;
            
            // Try to find in long option map
            if (arg.find("--") == 0) {
                std::string optionName = arg.substr(2);
                size_t equalsPos = optionName.find('=');
                
                if (equalsPos != std::string::npos) {
                    // --option=value format
                    std::string optionKey = optionName.substr(0, equalsPos);
                    std::string optionValue = optionName.substr(equalsPos + 1);
                    
                    auto it = longOptionMap.find(optionKey);
                    if (it != longOptionMap.end()) {
                        setOptionValue(it->second, optionValue);
                        optionFound = true;
                    }
                } else {
                    // --option value format
                    auto it = longOptionMap.find(optionName);
                    if (it != longOptionMap.end()) {
                        optionFound = true;
                        auto option = it->second;
                        
                        if (option->type == OptionType::VALUE || option->type == OptionType::OPTIONAL_VALUE) {
                            if (i + 1 < argc && argv[i + 1][0] != '-') {
                                setOptionValue(option, argv[++i]);
                            } else {
                                std::cerr << "Error: Option --" << option->name << " requires a value" << std::endl;
                                return false;
                            }
                        } else {
                            std::cerr << "Error: Option --" << option->name << " does not take a value" << std::endl;
                            return false;
                        }
                    }
                }
            } 
            // Check for short option
            else if (arg.find('-') == 0 && arg.length() > 1 && arg[1] != '-') {
                std::string optionKey = arg.substr(1);
                auto it = shortOptionMap.find(optionKey);
                if (it != shortOptionMap.end()) {
                    optionFound = true;
                    auto option = it->second;
                    
                    if (option->type == OptionType::VALUE || option->type == OptionType::OPTIONAL_VALUE) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            setOptionValue(option, argv[++i]);
                        } else {
                            std::cerr << "Error: Option -" << optionKey << " requires a value" << std::endl;
                            return false;
                        }
                    } else {
                        // Trigger action for flag
                        if (option->action) {
                            option->action("");
                        }
                        optionFound = true;
                    }
                }
            }
            
            // If not an option, treat as experiment name
            if (!optionFound && arg[0] != '-') {
                experiments.push_back(arg);
            }
        }
        
        return true;
    }
    
    // Check if help was requested
    bool isHelpRequested() const { return helpRequested; }
    
    // Check if version was requested
    bool isVersionRequested() const { return versionRequested; }
    
    // Check if list was requested
    bool isListRequested() const { return listRequested; }
    
    // Get option value by name
    std::string getOptionValue(const std::string& name) const {
        auto it = longOptionMap.find(name);
        if (it != longOptionMap.end()) {
            return optionValues[it->second];
        }
        return "";
    }
    
    // Get experiment names
    const std::vector<std::string>& getExperiments() const { return experiments; }
    
    // Check if option was set
    bool hasOption(const std::string& name) const {
        auto it = longOptionMap.find(name);
        return it != longOptionMap.end() && optionValues.count(it->second) > 0;
    }
    
    // Get option by name
    const CommandOption* getOption(const std::string& name) const {
        auto it = longOptionMap.find(name);
        if (it != longOptionMap.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Print help
    void printHelp() const {
        std::cout << HelpFormatter::formatOptions(options) << std::endl;
        std::cout << HelpFormatter::formatExperimentHelp() << std::endl;
    }
    
    // Print version info
    void printVersion() const {
        std::cout << HelpFormatter::formatVersionInfo() << std::endl;
    }
    
    // Print available experiments
    void printAvailableExperiments() const {
        std::cout << "Available experiments:" << std::endl;
        for (const auto& experiment : availableExperiments) {
            std::cout << "  - " << experiment << std::endl;
        }
    }
    
    // Get option value with type conversion
    template<typename T>
    T getOptionValue(const std::string& name, const T& defaultValue) const {
        std::string strValue = getOptionValue(name);
        if (strValue.empty()) {
            return defaultValue;
        }
        // Simple conversion - in production, use robust parsing
        return T(); // Placeholder
    }
    
    // Add an experiment to the list of available experiments
    void addExperiment(const std::string& name, const std::string& description = "") {
        availableExperiments.push_back(name);
    }
    
    // Set custom experiment list
    void setExperiments(const std::vector<std::string>& exps) {
        experiments = exps;
    }
    
    // Validate all options
    bool validate() const {
        // Check required options
        for (const auto& option : options) {
            if (option.required && !hasOption(option.name)) {
                std::cerr << "Error: Required option --" << option.name << " is missing" << std::endl;
                return false;
            }
        }
        
        // Run custom validators
        for (const auto& option : options) {
            if (option.validator) {
                auto it = longOptionMap.find(option.name);
                if (it != longOptionMap.end() && optionValues.count(it->second)) {
                    if (!option.validator(optionValues[it->second])) {
                        std::cerr << "Error: Invalid value for option --" << option.name << std::endl;
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
    // Get execution time
    double getExecutionTime() const {
        if (startTime.time_since_epoch().count() > 0 && endTime.time_since_epoch().count() > 0) {
            auto duration = endTime - startTime;
            return std::chrono::duration<double>(duration).count() * 1000.0; // milliseconds
        }
        return 0.0;
    }
    
    // Start timing execution
    void startTiming() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    // End timing execution
    void endTiming() {
        endTime = std::chrono::high_resolution_clock::now();
    }
    
private:
    void setOptionValue(CommandOption* option, const std::string& value) {
        optionValues[option] = value;
        
        // Run custom action
        if (option->action) {
            option->action(value);
        }
        
        // Validate
        if (option->validator && !option->validator(value)) {
            throw std::runtime_error("Validation failed for option --" + option->name);
        }
    }
    
    int argc;
    char** argv;
    bool helpRequested;
    bool versionRequested;
    bool listRequested;
    std::vector<CommandOption> options;
    std::unordered_map<std::string, CommandOption*> shortOptionMap;
    std::unordered_map<std::string, CommandOption*> longOptionMap;
    std::unordered_map<CommandOption*, std::string> optionValues;
    std::vector<std::string> experiments;
    std::vector<std::string> availableExperiments;
    std::vector<CommandOption*> optionalValueOptions;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
};

} // namespace cli
} // namespace nlm