#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include "../Logger/Logger.hpp"

namespace nlm {

/**
 * Command line argument parser with enhanced functionality
 */
class CommandLineParser {
public:
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        bool hasArgument;
        std::string defaultValue;
        bool required;
        std::function<void(const std::string&)> callback;
    };
    
    CommandLineParser();
    ~CommandLineParser();
    
    // Add option
    void addOption(const std::string& longName, char shortName, 
                   const std::string& description, bool hasArgument = false,
                   const std::string& defaultValue = "", bool required = false);
    
    // Add callback for option
    void setCallback(const std::string& longName, 
                    std::function<void(const std::string&)> callback);
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]);
    
    // Get option value
    std::string getOption(const std::string& longName) const;
    
    // Check if option was provided
    bool hasOption(const std::string& longName) const;
    
    // Print help
    void printHelp() const;
    
    // Get unknown arguments (for forward compatibility)
    const std::vector<std::string>& getUnknownArgs() const { return unknownArgs_; }
    
    // Check if parsing was successful
    bool isValid() const { return valid_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<std::string> unknownArgs_;
    bool valid_;
};

/**
 * Configuration manager with enhanced features
 */
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line
    bool loadFromCommandLine(int argc, char* argv[]);
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get all keys
    std::vector<std::string> getAllKeys() const;
    
    // Print configuration
    void printConfiguration() const;
    
    // Reset to default values
    void resetToDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

/**
 * Enhanced logging with file rotation and levels
 */
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Initialize with configuration
    bool initialize(const ConfigurationManager& config);
    
    // Get log level from string
    static LogLevel getLogLevelFromString(const std::string& levelStr);
    
    // Get log level as string
    static std::string getLogLevelAsString(LogLevel level);
    
    // Format log message with timestamp
    static std::string formatLogMessage(LogLevel level, const std::string& message,
                                        const std::string& file = "", int line = 0,
                                        const std::string& function = "");
    
    // Check if log level should be logged
    static bool shouldLog(LogLevel level, LogLevel threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
