// Configuration management tools base class
//
// This file provides the base class and common functionality for configuration CLI tools.

#pragma once

#include "tools/CLITool.hpp"
#include <sstream>

namespace nlm {

class ConfigTool : public CLITool {
public:
    ConfigTool() = default;
    ~ConfigTool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override {
        return CommandType::CONFIG;
    }
    virtual std::string getCategory() const override {
        return "Configuration Management";
    }
    
    // Configuration-specific helper methods
    bool validateConfig(const Config& config) const;
    bool exportConfig(const std::string& outputFile, const Config& config, 
                     const std::string& format = "json") const;
    bool mergeConfigs(const std::vector<std::string>& configFiles, Config& resultConfig) const;
    bool compareConfigs(const Config& config1, const Config& config2) const;
    
    // Configuration preset helpers
    bool applyPreset(Config& config, const std::string& presetName) const;
    std::vector<std::string> getAvailablePresets() const;
    
    // Configuration validation helpers
    bool validateConfigAgainstSchema(const Config& config) const;
    bool validateConfigValues(const Config& config) const;
    
protected:
    // Configuration-specific implementations
    virtual bool validateConfigImpl(const Config& config) const;
    virtual bool exportConfigImpl(const std::string& outputFile, const Config& config,
                                 const std::string& format) const;
    virtual bool mergeConfigsImpl(const std::vector<std::string>& configFiles, Config& resultConfig) const;
    virtual bool compareConfigsImpl(const Config& config1, const Config& config2) const;
};

} // namespace nlm