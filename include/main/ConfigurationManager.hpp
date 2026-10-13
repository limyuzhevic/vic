// ==============================================================================
// Configuration Manager - Handles all configuration operations
// ==============================================================================

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Load configuration from command line arguments
    bool load(int argc, char** argv);
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void set(const std::string& key, const std::string& value);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Print configuration summary
    void printSummary() const;
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Check if should run benchmarks
    bool shouldRunBenchmarks() const;
    
    // Check if should run experiments
    bool shouldRunExperiments() const;
    
    // Get experiment mode
    std::string getExperimentMode() const { return experimentMode_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm