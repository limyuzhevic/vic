#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <functional>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Configuration lifecycle
    bool initialize(const std::string& configFile = "");
    void update(double timestep);
    void shutdown();
    
    // Configuration loading and saving
    bool loadFromFile(const std::string& filepath);
    bool loadFromString(const std::string& yamlString);
    bool saveToFile(const std::string& filepath) const;
    std::string exportToYAML() const;
    bool importFromYAML(const std::string& yamlString);
    
    // Environment-based configuration
    void loadFromEnvironment();
    bool setEnvironmentVariable(const std::string& name, const std::string& value);
    std::string getEnvironmentVariable(const std::string& name) const;
    
    // Runtime configuration modifications
    void applyConfiguration(const Config& config);
    void scheduleConfigurationUpdate(const Config& config, double delay);
    void cancelScheduledUpdates();
    
    // Configuration inheritance
    bool inheritFrom(const std::string& baseConfigPath);
    void addInheritancePath(const std::string& path);
    void removeInheritancePath(const std::string& path);
    std::vector<std::string> getInheritanceChain() const;
    std::map<std::string, Config> getMergedConfiguration() const;
    
    // Schema validation
    bool validateConfiguration() const;
    bool addSchemaValidator(const std::string& name, std::function<bool(const Config&)> validator);
    std::vector<std::string> getValidationErrors() const;
    std::string getValidationReport() const;
    
    // Configuration access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::map<std::string, ConfigValue> getAllValues() const;
    
    // Configuration statistics
    size_t getVersion() const;
    std::chrono::system_clock::time_point getLastModified() const;
    std::string getConfigurationHash() const;
    
    // Configuration features
    void enableHotReloading(bool enable);
    bool isHotReloadingEnabled() const;
    std::chrono::system_clock::duration getReloadInterval() const;
    void setReloadInterval(std::chrono::system_clock::duration interval);
    
    // Plugin configuration
    bool loadPluginConfiguration(const std::string& pluginName);
    bool savePluginConfiguration(const std::string& pluginName, const Config& config);
    std::map<std::string, Config> getAllPluginConfigurations() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
};

} // namespace nlm
