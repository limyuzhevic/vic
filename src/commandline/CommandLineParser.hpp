#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

namespace nlm {

// Simple command-line argument parser for NLM
class CommandLineParser {
public:
    struct Option {
        std::string name;
        std::string description;
        char shortOpt;
        bool hasValue;
        std::string defaultValue;
        bool required;
    };
    
    CommandLineParser() {}
    
    void addOption(const std::string& name, const std::string& description, 
                   char shortOpt = 0, bool hasValue = false, 
                   const std::string& defaultValue = "", bool required = false) {
        options[name] = {name, description, shortOpt, hasValue, defaultValue, required};
    }
    
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --option=value format
            if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                std::string optName = arg.substr(2, arg.find("=") - 2);
                std::string optValue = arg.substr(arg.find("=") + 1);
                parsedArgs[optName] = optValue;
            }
            // Handle --option value format
            else if (arg.find("--") == 0) {
                std::string optName = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    parsedArgs[optName] = argv[++i];
                }
            }
            // Handle -o value format
            else if (arg[0] == '-') {
                char optChar = arg[1];
                for (auto& pair : options) {
                    if (pair.second.shortOpt == optChar) {
                        std::string optName = pair.first;
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            parsedArgs[optName] = argv[++i];
                        }
                        break;
                    }
                }
            }
        }
        
        return validate();
    }
    
    bool hasArg(const std::string& name) const {
        return parsedArgs.find(name) != parsedArgs.end();
    }
    
    std::string getArg(const std::string& name, const std::string& defaultValue = "") const {
        auto it = parsedArgs.find(name);
        if (it != parsedArgs.end()) {
            return it->second;
        }
        
        // Check if we have a default value for this option
        auto optIt = options.find(name);
        if (optIt != options.end() && !optIt->second.defaultValue.empty()) {
            return optIt->second.defaultValue;
        }
        
        return defaultValue;
    }
    
    int getArgInt(const std::string& name, int defaultValue = 0) const {
        std::string str = getArg(name);
        try {
            return std::stoi(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    float getArgFloat(const std::string& name, float defaultValue = 0.0f) const {
        std::string str = getArg(name);
        try {
            return std::stof(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    double getArgDouble(const std::string& name, double defaultValue = 0.0) const {
        std::string str = getArg(name);
        try {
            return std::stod(str);
        } catch (...) {
            return defaultValue;
        }
    }
    
    bool getArgBool(const std::string& name, bool defaultValue = false) const {
        std::string str = getArg(name);
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return (str == "true" || str == "yes" || str == "1" || str == "on");
    }
    
    const std::map<std::string, std::string>& getAllArgs() const {
        return parsedArgs;
    }
    
    void printHelp() const {
        std::cout << "=== NLM Phase 2: Real Neural Computation ===\n\n";
        std::cout << "USAGE:\n  nlm [OPTIONS]\n\n";
        std::cout << "OPTIONS:\n";
        
        for (const auto& pair : options) {
            const auto& opt = pair.second;
            std::string optName = "  --" + opt.name;
            if (opt.shortOpt) {
                optName += " (-" + std::string(1, opt.shortOpt) + ")";
            }
            if (opt.hasValue) {
                optName += "=<value>";
            }
            optName += ";\n";
            
            std::cout << optName;
            std::cout << "    " << opt.description << "\n";
            if (!opt.defaultValue.empty()) {
                std::cout << "    Default: " << opt.defaultValue << "\n";
            }
        }
        
        std::cout << "\nEXAMPLES:\n";
        std::cout << "  nlm --help                          Show this help message\n";
        std::cout << "  nlm --version                      Show version information\n";
        std::cout << "  nlm --neuron_count=2000 --region_count=2\n";
        std::cout << "  nlm --enable_stdp --enable_hebbian\n";
    }
    
private:
    bool validate() {
        // Check required options
        for (const auto& pair : options) {
            const auto& opt = pair.second;
            if (opt.required && !hasArg(opt.name)) {
                std::cerr << "Error: Required option --" << opt.name << " is missing.\n";
                return false;
            }
        }
        return true;
    }
    
    std::map<std::string, std::string> parsedArgs;
    std::map<std::string, Option> options;
};

} // namespace nlm
