// Configuration system for NLM
#pragma once

#include <string>
#include <map>
#include <memory>
#include "Types.hpp"

namespace nlm {

enum class ConfigSource {
    Default,
    File,
    CommandLine
};

class Config {
public:
    Config();
    ~Config() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Load configuration from command line arguments
    void loadFromArgs(int argc, char** argv);
    
    // Set a configuration value
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source = ConfigSource::Default);
    
    // Get a configuration value with default
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Get a configuration value (throws if not found)
    template<typename T>
    T get(const std::string& key) const;
    
    // Check if a key exists
    bool hasKey(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Print configuration to log
    void printConfig() const;
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const;
    
private:
    std::map<std::string, std::pair<void*, ConfigSource>> values_;
    std::map<std::string, std::string> stringValues_;
    std::map<std::string, double> doubleValues_;
    std::map<std::string, int64_t> intValues_;
    std::map<std::string, float> floatValues_;
    std::map<std::string, bool> boolValues_;
    
    void* getRawValue(const std::string& key) const;
    template<typename T>
    void setRawValue(const std::string& key, T value, ConfigSource source);
    
    // Parse command line arguments
    void parseArgs(int argc, char** argv);
    std::string parseConfigArg(const std::string& arg) const;
};

} // namespace nlm