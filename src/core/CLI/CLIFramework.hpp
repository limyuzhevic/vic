#pragma once

/**
 * NLM Command Line Interface Framework
 * Common utilities for all NLM demo executables
 */

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm {

class CLIArgument {
public:
    std::string name;
    std::string value;
    std::string description;
    bool required;
    
    CLIArgument(const std::string& n, const std::string& desc, bool req = false)
        : name(n), description(desc), required(req), value("") {}
};

class CLIParser {
public:
    std::vector<CLIArgument> arguments;
    std::map<std::string, std::string> parsedValues;
    std::string programName;
    
    CLIParser(const std::string& name) : programName(name) {}
    
    void addArgument(const std::string& name, const std::string& desc, bool required = false) {
        arguments.push_back(CLIArgument(name, desc, required));
    }
    
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg == "--version" || arg == "-v") {
                printVersion();
                return false;
            } else if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                std::string val = "";
                
                // Check if this is a key=value pair
                size_t eqPos = key.find("=");
                if (eqPos != std::string::npos) {
                    val = key.substr(eqPos + 1);
                    key = key.substr(0, eqPos);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // Next argument is the value
                    val = argv[++i];
                }
                
                parsedValues[key] = val;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
    
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const {
        auto it = parsedValues.find(key);
        return it != parsedValues.end() ? it->second : defaultValue;
    }
    
    int getIntValue(const std::string& key, int defaultValue = 0) const {
        std::string val = getValue(key);
        if (val.empty()) return defaultValue;
        try {
            return std::stoi(val);
        } catch (...) {
            return defaultValue;
        }
    }
    
    float getFloatValue(const std::string& key, float defaultValue = 0.0f) const {
        std::string val = getValue(key);
        if (val.empty()) return defaultValue;
        try {
            return std::stof(val);
        } catch (...) {
            return defaultValue;
        }
    }
    
    bool getBoolValue(const std::string& key, bool defaultValue = false) const {
        std::string val = getValue(key);
        if (val.empty()) return defaultValue;
        std::string lower = val;
        for (char& c : lower) c = std::tolower(c);
        return lower == "true" || lower == "1" || lower == "yes";
    }
    
    void printHelp() const {
        std::cout << programName << " - Neural Learning Machine Demo" << std::endl;
        std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
        
        std::cout << "Common options:" << std::endl;
        std::cout << "  -h, --help                    Show this help message and exit" << std::endl;
        std::cout << "  -v, --version                 Show version information and exit" << std::endl;
        std::cout << "  --config <file>               Load configuration from file" << std::endl;
        std::cout << std::endl;
        
        if (!arguments.empty()) {
            std::cout << "Program options:" << std::endl;
            for (const auto& arg : arguments) {
                std::cout << "  --" << arg.name << " <value>";
                if (arg.required) std::cout << " (required)";
                std::cout << std::endl;
                std::cout << "    " << arg.description << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    void printVersion() const {
        std::cout << programName << " version 1.0.0" << std::endl;
        std::cout << "Phase 6 Final Integration Demo" << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
    }
};

class ProgressReporter {
public:
    bool verbose;
    bool quiet;
    bool showProgress;
    std::chrono::steady_clock::time_point startTime;
    
    ProgressReporter(bool verbose = false, bool quiet = false, bool showProgress = true)
        : verbose(verbose), quiet(quiet), showProgress(showProgress), startTime(std::chrono::steady_clock::now()) {}
    
    void log(const std::string& message) const {
        if (!quiet) {
            std::cout << message << std::endl;
        }
    }
    
    void progress(int current, int total, const std::string& status = "") const {
        if (showProgress && !quiet) {
            float percentage = (total > 0) ? (100.0f * current / total) : 0.0f;
            std::cout << "\rProgress: " << current << "/" << total << " (" 
                     << std::fixed << std::setprecision(1) << percentage << "%) "
                     << status << std::flush;
            
            if (current >= total) {
                std::cout << std::endl;
            }
        }
    }
    
    void status(const std::string& message) const {
        if (verbose && !quiet) {
            std::cout << "[STATUS] " << message << std::endl;
        }
    }
    
    void error(const std::string& message) const {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    void success(const std::string& message) const {
        if (!quiet) {
            std::cout << "[SUCCESS] " << message << std::endl;
        }
    }
    
    void warning(const std::string& message) const {
        if (!quiet) {
            std::cout << "[WARNING] " << message << std::endl;
        }
    }
    
    double getElapsedTime() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        return duration.count() / 1000.0;
    }
};

class ConfigManager {
public:
    static std::shared_ptr<Config> createConfig(int argc, char** argv, const std::string& defaultConfigFile = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        
        // Load default config file if it exists
        if (config->loadFromFile(defaultConfigFile)) {
            std::cout << "Loaded configuration from: " << defaultConfigFile << std::endl;
        }
        
        // Load from command line arguments
        config->loadFromArgs(argc, argv);
        
        return config;
    }
    
    static void printConfigSummary(const Config& config) {
        std::cout << "\n=== Configuration Summary ===" << std::endl;
        for (const auto& key : config.getKeys()) {
            if (config.has(key)) {
                std::cout << "  " << key << " = ";
                
                auto val = config.get<std::string>(key);
                if (val) {
                    std::cout << *val;
                } else {
                    auto valInt = config.get<int64_t>(key);
                    if (valInt) {
                        std::cout << *valInt;
                    } else {
                        auto valDouble = config.get<double>(key);
                        if (valDouble) {
                            std::cout << *valDouble;
                        } else {
                            auto valFloat = config.get<float>(key);
                            if (valFloat) {
                                std::cout << *valFloat;
                            }
                        }
                    }
                }
                std::cout << std::endl;
            }
        }
        std::cout << "===========================" << std::endl;
    }
};

} // namespace nlm
