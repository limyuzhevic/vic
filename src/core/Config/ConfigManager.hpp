// NLM Configuration Manager
// Provides unified configuration management with command-line argument parsing,
// file loading, validation, and easy access to configuration values.

#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <iostream>

namespace nlm {

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Load configuration from file and merge with defaults
    bool loadConfig(const std::string& configFile);
    
    // Initialize default configuration values
    void initializeDefaults();
    
    // Reset to factory defaults
    void resetToDefaults();
    
    // Parse command line arguments and apply to configuration
    bool parseCommandLine(int argc, char* argv[]);
    
    // Get configuration value (with type safety)
    template<typename T>
    T getValue(const std::string& key) const {
        return config_->getOr<T>(key, T{});
    }
    
    // Get configuration value with default
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Set configuration value
    template<typename T>
    void setValue(const std::string& key, const T& value, 
                  ConfigSource source = ConfigSource::Default) {
        config_->set(key, value, source);
    }
    
    // Apply command-line arguments to configuration
    void applyCommandLineArgs(int argc, char* argv[]);
    
    // Validate configuration
    bool validateConfig() const;
    
    // Get configuration summary for logging
    std::string getConfigSummary() const;
    
    // Check if configuration is fully initialized
    bool isInitialized() const { return config_ != nullptr; }
    
    // Get the underlying configuration object
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Print configuration help
    static void printConfigHelp(const char* programName);
    
    // Print configuration summary to output stream
    void printConfigSummary(std::ostream& out = std::cout) const;
    
    // Check for command-line help flags
    static bool shouldShowHelp(int argc, char* argv[]);
    
    // Check for specific command-line options
    static bool hasOption(int argc, char* argv[], const std::string& option);
    
    // Get value of command-line option if present
    static std::string getOptionValue(int argc, char* argv[], 
                                     const std::string& option,
                                     const std::string& defaultValue = "");
    
    // Parse numeric command-line option
    template<typename T>
    static T getOptionNumeric(int argc, char* argv[], 
                             const std::string& option,
                             T defaultValue) {
        if (hasOption(argc, argv, option)) {
            size_t pos = std::string("--").size();
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg.find(option + "=") == 0) {
                    std::string valueStr = arg.substr(pos + option.size() + 1);
                    try {
                        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                            return static_cast<T>(std::stoll(valueStr));
                        } else if constexpr (std::is_floating_point_v<T>) {
                            return static_cast<T>(std::stod(valueStr));
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            return valueStr;
                        }
                    } catch (...) {
                        // Fall back to next argument for boolean flags
                    }
                } else if (arg == option && i + 1 < argc) {
                    // Handle --option value format (for boolean flags)
                    std::string valueStr = argv[++i];
                    if constexpr (std::is_same_v<T, bool>) {
                        return valueStr == "1" || valueStr == "true" || valueStr == "yes";
                    }
                }
            }
        }
        return defaultValue;
    }
    
    // Parse command-line option with argument
    static std::string getOptionWithArg(int argc, char* argv[], 
                                       const std::string& option,
                                       const std::string& defaultValue = "");
    
    // Apply file configuration
    bool applyFileConfig(const std::string& configFile);
    
    // Apply system defaults
    void applySystemDefaults();
    
    // Save configuration to file
    bool saveConfig(const std::string& configFile) const;
    
    // Get configuration file list
    static std::vector<std::string> getAvailableConfigFiles();
    
    // Check if config file exists
    static bool configFileExists(const std::string& configFile);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fileLoaded;
        size_t commandLineApplied;
        size_t defaultsApplied;
        float memoryUsage;
    };
    
    ConfigStats getStats() const;

private:
    std::shared_ptr<Config> config_;
    bool initialized_;
    std::vector<std::string> loadedConfigFiles;
    
    // Default configuration values
    void setDefaultValues();
    
    // Validate individual configuration values
    bool validateEntry(const std::string& key, const std::string& value) const;
    
    // Get option value helper
    static std::string getOptionValueHelper(int argc, char* argv[], 
                                           const std::string& option,
                                           const std::string& defaultValue = "");
};

} // namespace nlm