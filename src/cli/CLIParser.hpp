// CLI Parser - Argument parsing for NLM CLI

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <algorithm>

class CLIParser {
public:
    CLIParser(const std::vector<std::string>& args) : argv(args) {}
    
    // Parse --key=value and -key value formats
    std::string getString(const std::string& longOpt, const std::string& shortOpt = "", 
                         const std::string& defaultValue = "") const {
        std::string result = defaultValue;
        
        // Check --key=value format
        std::string longPrefix = "--" + longOpt;
        if (longOpt.empty() && !shortOpt.empty()) {
            longPrefix = "-" + shortOpt;
        }
        
        for (size_t i = 0; i < argv.size(); ++i) {
            std::string arg = argv[i];
            
            // Check for --key=value format
            if (longOpt.empty() && !shortOpt.empty()) {
                if (arg == "-" + shortOpt && i + 1 < argv.size()) {
                    result = argv[++i];
                    break;
                }
            } else {
                if (arg.find(longPrefix + "=") == 0) {
                    result = arg.substr(longPrefix.size() + 1);
                    break;
                }
            }
        }
        
        // Check for -key value format
        if (!longOpt.empty() && !shortOpt.empty()) {
            std::string shortPrefix = "-" + shortOpt;
            for (size_t i = 0; i < argv.size(); ++i) {
                if (argv[i] == shortPrefix && i + 1 < argv.size()) {
                    result = argv[++i];
                    break;
                }
            }
        }
        
        return result;
    }
    
    int getInt(const std::string& longOpt, const std::string& shortOpt = "", 
              int defaultValue = 0) const {
        std::string value = getString(longOpt, shortOpt, std::to_string(defaultValue));
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    double getDouble(const std::string& longOpt, const std::string& shortOpt = "", 
                     double defaultValue = 0.0) const {
        std::string value = getString(longOpt, shortOpt, std::to_string(defaultValue));
        try {
            return std::stod(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    bool getBool(const std::string& longOpt, const std::string& shortOpt = "", 
                 bool defaultValue = false) const {
        std::string value = getString(longOpt, shortOpt, defaultValue ? "true" : "false");
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes" || value == "on";
    }
    
    bool has(const std::string& longOpt, const std::string& shortOpt = "") const {
        std::string longPrefix = "--" + longOpt;
        std::string shortPrefix = "-" + shortOpt;
        
        for (const auto& arg : argv) {
            if (arg == longPrefix || arg == shortPrefix) {
                return true;
            }
        }
        return false;
    }
    
    // Get positional arguments
    std::vector<std::string> getPositionalArgs(size_t start = 0) const {
        std::vector<std::string> result;
        for (size_t i = start; i < argv.size(); ++i) {
            std::string arg = argv[i];
            
            // Skip known flags
            if ((arg.find("--") == 0 || arg.find("-") == 0) && 
                (arg == "--help" || arg == "--version" || 
                 arg == "-h" || arg == "-v")) {
                continue;
            }
            
            result.push_back(arg);
        }
        return result;
    }
    
    // Print help for command
    void printHelp(const std::string& description, 
                   const std::vector<std::pair<std::string, std::string>>& options,
                   const std::string& examples = "") const {
        std::cout << "Usage: nlm <command> [options]" << std::endl;
        std::cout << std::endl;
        
        if (!description.empty()) {
            std::cout << description << std::endl;
            std::cout << std::endl;
        }
        
        if (!options.empty()) {
            std::cout << "Options:" << std::endl;
            for (const auto& opt : options) {
                std::cout << "  " << std::left << std::setw(20) << opt.first;
                if (!opt.second.empty()) {
                    std::cout << "  " << opt.second;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (!examples.empty()) {
            std::cout << "Examples:" << std::endl;
            std::cout << examples << std::endl;
        }
    }
    
private:
    std::vector<std::string> argv;
};
