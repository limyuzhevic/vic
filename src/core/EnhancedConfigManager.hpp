#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class EnhancedConfigManager {
public:
    EnhancedConfigManager();
    ~EnhancedConfigManager() = default;
    
    // Load configuration from various sources
    bool loadConfig(const std::string& configFile = "");
    bool loadFromCLI(const class CLIParser& cli);
    bool loadFromDefaults();
    
    // Save configuration
    bool saveConfig(const std::string& configFile) const;
    bool exportConfig(const std::string& outputFile, const std::string& format = "json") const;
    
    // Get configuration value
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void setValue(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration
    bool validateConfig() const;
    std::string getValidationErrors() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Get config object
    std::shared_ptr<const Config> getConfig() const { return config_; }
    std::shared_ptr<Config> getConfig() { return config_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> validationErrors_;
    
    void initializeDefaults();
    bool validateNeuronConfig() const;
    bool validatePlasticityConfig() const;
    bool validateNeuromodulationConfig() const;
    bool validateDevelopmentConfig() const;
};

} // namespace nlm