#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>

namespace nlm {

class CommandLineParser {
public:
    struct Argument {
        std::string name;
        std::string value;
        std::string description;
        bool required;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
        std::function<bool(const std::string&)> validator;
    };

    CommandLineParser() = default;
    
    // Add a positional argument (like first argument without --)
    void addPositional(const std::string& name, const std::string& description) {
        Argument arg;
        arg.name = name;
        arg.description = description;
        arg.required = true;
        arg.allowedValues = {"config", "experiment", "help", "version"};
        positionalArgs_.push_back(arg);
    }
    
    // Add an optional argument with short option (e.g., -c)
    void addOption(char shortOpt, const std::string& longOpt, 
                   const std::string& description, bool required = false,
                   const std::string& defaultValue = "") {
        Argument arg;
        arg.name = longOpt.substr(2); // Remove -- prefix
        arg.value = defaultValue;
        arg.description = description;
        arg.required = required;
        arg.defaultValue = defaultValue;
        
        // Register short option mapping
        shortOptions_[shortOpt] = longOpt;
        options_[longOpt] = arg;
    }
    
    // Add an optional argument with long option only
    void addOption(const std::string& longOpt, const std::string& description,
                   bool required = false, const std::string& defaultValue = "") {
        Argument arg;
        arg.name = longOpt.substr(2); // Remove -- prefix
        arg.value = defaultValue;
        arg.description = description;
        arg.required = required;
        arg.defaultValue = defaultValue;
        options_[longOpt] = arg;
    }
    
    // Set validator for an option
    void setValidator(const std::string& opt, std::function<bool(const std::string&)> validator) {
        auto it = options_.find("--" + opt);
        if (it != options_.end()) {
            it->second.validator = validator;
        }
    }
    
    // Set allowed values for an option
    void setAllowedValues(const std::string& opt, const std::vector<std::string>& values) {
        auto it = options_.find("--" + opt);
        if (it != options_.end()) {
            it->second.allowedValues = values;
        }
    }
    
    // Parse command line arguments
    bool parse(int argc, char** argv) {
        errors_.clear();
        
        // Build help string
        std::stringstream helpStream;
        helpStream << "Usage: " << (argc > 0 ? argv[0] : "nlm") << " [OPTIONS] [ARGS]\n\n";
        helpStream << "Options:\n";
        
        for (const auto& pair : options_) {
            const auto& arg = pair.second;
            helpStream << "  --" << arg.name;
            if (!arg.defaultValue.empty()) {
                helpStream << "=" << arg.defaultValue;
            }
            helpStream << "\t" << arg.description;
            
            if (!arg.allowedValues.empty()) {
                helpStream << " (allowed: ";
                for (size_t i = 0; i < arg.allowedValues.size(); ++i) {
                    if (i > 0) helpStream << ", ";
                    helpStream << arg.allowedValues[i];
                }
                helpStream << ")";
            }
            helpStream << "\n";
        }
        
        for (const auto& pair : shortOptions_) {
            helpStream << "  -" << pair.first << " (--" << pair.second.substr(2) << ")\n";
        }
        
        help_ = helpStream.str();
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Check for --option=value or --option value
            if (arg.find("--") == 0) {
                size_t eqPos = arg.find('=');
                std::string optName;
                std::string optValue;
                
                if (eqPos != std::string::npos) {
                    optName = arg.substr(0, eqPos);
                    optValue = arg.substr(eqPos + 1);
                } else {
                    optName = arg;
                    if (i + 1 < argc) {
                        optValue = argv[++i];
                        // Skip if next argument is also an option
                        if (optValue.find("--") == 0 || (optValue.size() == 2 && optValue[0] == '-')) {
                            i--; // Put it back for next iteration
                        }
                    } else {
                        errors_.push_back("Option --" + optName.substr(2) + " requires a value");
                        return false;
                    }
                }
                
                optName = optName.substr(2); // Remove --
                setOptionValue(optName, optValue);
            } 
            else if (arg.size() == 2 && arg[0] == '-') { // -x
                char shortOpt = arg[1];
                auto it = shortOptions_.find(shortOpt);
                if (it != shortOptions_.end()) {
                    std::string longOpt = it->second;
                    std::string optName = longOpt.substr(2);
                    
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        setOptionValue(optName, argv[++i]);
                    } else {
                        errors_.push_back("Option -" + std::string(1, shortOpt) + " requires a value");
                        return false;
                    }
                } else {
                    errors_.push_back("Unknown option: -" + std::string(1, shortOpt));
                    return false;
                }
            }
            else { // Positional argument
                if (positionalArgs_.size() > 0) {
                    // Use first positional
                    positionalArgs_[0].value = arg;
                    positionalArgs_[0].required = false; // Now it's provided
                } else {
                    errors_.push_back("Unexpected positional argument: " + arg);
                    return false;
                }
            }
        }
        
        // Check required options
        for (const auto& pair : options_) {
            const auto& arg = pair.second;
            if (arg.required && arg.value.empty() && arg.defaultValue.empty()) {
                errors_.push_back("Required option --" + arg.name + " is missing");
                return false;
            }
        }
        
        // Check required positional arguments
        for (const auto& arg : positionalArgs_) {
            if (arg.required && arg.value.empty()) {
                errors_.push_back("Required positional argument '" + arg.name + "' is missing");
                return false;
            }
        }
        
        return true;
    }
    
    // Get option value
    std::string getValue(const std::string& optName) const {
        auto it = options_.find("--" + optName);
        if (it != options_.end()) {
            return it->second.value.empty() ? it->second.defaultValue : it->second.value;
        }
        return "";
    }
    
    // Get option value as integer
    int getIntValue(const std::string& optName) const {
        std::string val = getValue(optName);
        try {
            return std::stoi(val);
        } catch (...) {
            return 0;
        }
    }
    
    // Get option value as double
    double getDoubleValue(const std::string& optName) const {
        std::string val = getValue(optName);
        try {
            return std::stod(val);
        } catch (...) {
            return 0.0;
        }
    }
    
    // Get option value as float
    float getFloatValue(const std::string& optName) const {
        std::string val = getValue(optName);
        try {
            return std::stof(val);
        } catch (...) {
            return 0.0f;
        }
    }
    
    // Get option value as boolean (true if "1", "true", "yes", "enable")
    bool getBoolValue(const std::string& optName) const {
        std::string val = getValue(optName);
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return (val == "1" || val == "true" || val == "yes" || val == "enable" || val == "on");
    }
    
    // Get positional argument value
    std::string getPositionalValue(size_t index = 0) const {
        if (index < positionalArgs_.size()) {
            return positionalArgs_[index].value;
        }
        return "";
    }
    
    // Check if option was specified
    bool hasOption(const std::string& optName) const {
        auto it = options_.find("--" + optName);
        return it != options_.end() && !it->second.value.empty();
    }
    
    // Print help message
    void printHelp() const {
        std::cout << help_ << std::endl;
    }
    
    // Get parsing errors
    const std::vector<std::string>& getErrors() const {
        return errors_;
    }
    
    // Validate all options
    bool validateAll() const {
        for (const auto& pair : options_) {
            const auto& arg = pair.second;
            if (!arg.validator.empty() && !arg.value.empty()) {
                if (!arg.validator(arg.value)) {
                    errors_.push_back("Invalid value for option --" + arg.name + ": " + arg.value);
                    return false;
                }
            }
            
            if (!arg.allowedValues.empty() && !arg.value.empty()) {
                bool found = false;
                for (const auto& allowed : arg.allowedValues) {
                    if (arg.value == allowed) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    errors_.push_back("Invalid value for option --" + arg.name + ": " + arg.value + ". Allowed values: ");
                    for (size_t i = 0; i < arg.allowedValues.size(); ++i) {
                        if (i > 0) errors_.back() += ", ";
                        errors_.back() += arg.allowedValues[i];
                    }
                    return false;
                }
            }
        }
        return true;
    }
    
    // Convert to string for logging
    std::string toString() const {
        std::stringstream ss;
        ss << "CommandLineParser:";
        for (const auto& pair : options_) {
            const auto& arg = pair.second;
            if (!arg.value.empty()) {
                ss << " --" << arg.name << "=" << arg.value;
            }
        }
        return ss.str();
    }

private:
    void setOptionValue(const std::string& optName, const std::string& optValue) {
        auto it = options_.find("--" + optName);
        if (it != options_.end()) {
            it->second.value = optValue;
        } else {
            // Register unknown option (maybe passed from command line)
            Argument arg;
            arg.name = optName;
            arg.value = optValue;
            arg.description = "Automatically registered option";
            arg.required = false;
            options_["--" + optName] = arg;
        }
    }
    
    std::map<std::string, Argument> options_;
    std::map<char, std::string> shortOptions_;
    std::vector<Argument> positionalArgs_;
    std::vector<std::string> errors_;
    std::string help_;
};

} // namespace nlm