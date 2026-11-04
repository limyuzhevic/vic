#pragma once

#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/AdvancedConfig.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class BrainConfig {
public:
    BrainConfig();
    ~BrainConfig();
    
    // Disable copying
    BrainConfig(const BrainConfig&) = delete;
    BrainConfig& operator=(const BrainConfig&) = delete;
    
    // Initialize with core brain
    bool initialize(BrainCore* core);
    
    // Configuration management
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool saveToFile(const std::string& filepath, const std::string& format = "yaml") const;
    
    // Advanced configuration
    std::shared_ptr<AdvancedConfig> getAdvancedConfig() { return advancedConfig; }
    void setAdvancedConfig(std::shared_ptr<AdvancedConfig> config) { advancedConfig = config; }
    
    // Configuration validation
    bool validate(std::vector<std::string>& errors) const;
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return core->getConfig(); }
    std::shared_ptr<Config> getCoreConfig() { return core->getConfig(); }
    
    // Configuration queries
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    float getConnectionProbability() const;
    double getSimulationTimestep() const;
    
    // Configuration validation rules
    void setValidationRules(const std::vector<std::string>& rules);
    bool validateConfiguration(const std::string& key, const ConfigValue& value) const;
    
    // Configuration presets
    bool loadDefaultConfig();
    bool loadQuickTestConfig();
    bool loadResearchConfig();
    
    // Configuration export/import
    std::string exportConfig() const;
    bool importConfig(const std::string& configString);
    
    // Configuration statistics
    size_t getConfigurationEntryCount() const;
    std::vector<std::string> getConfigurationKeys() const;
    
private:
    // Core brain reference
    BrainCore* core = nullptr;
    
    // Advanced configuration system
    std::shared_ptr<AdvancedConfig> advancedConfig;
    
    // Configuration schema
    std::vector<std::string> validationRules;
    
    // Helper methods
    bool validateNeuronCount(const ConfigValue& value) const;
    bool validateRegionCount(const ConfigValue& value) const;
    bool validateConnectionProbability(const ConfigValue& value) const;
    bool validateSimulationTimestep(const ConfigValue& value) const;
    bool validateSinaptogenesisRate(const ConfigValue& value) const;
    bool validatePruningRate(const ConfigValue& value) const;
    
    // Configuration presets
    bool createDefaultConfig();
    bool createQuickTestConfig();
    bool createResearchConfig();
    
    // Configuration export/import
    std::string configToString() const;
    bool stringToConfig(const std::string& configString);
    
    // Friend class
    friend class Brain;
};

} // namespace nlm
