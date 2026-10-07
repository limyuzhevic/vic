//=============================================================
// ADVANCED CONFIGURATION SYSTEM - Phase 2 Implementation
//=============================================================

#ifndef CONFIG_ADVANCED_HPP
#define CONFIG_ADVANCED_HPP

#include "Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace nlm {

class JSONParser;
class YAMLParser;

class AdvancedConfig : public Config {
public:
    AdvancedConfig();
    ~AdvancedConfig() override;
    
    // File format support
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromTOML(const std::string& filepath);
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Validation and schema
    bool validateSchema() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Merge configurations
    bool mergeFromFile(const std::string& filepath);
    bool mergeFromString(const std::string& content);
    
    // Get preset configurations
    static std::shared_ptr<AdvancedConfig> createDefaultConfig();
    static std::shared_ptr<AdvancedConfig> createMinimalConfig();
    static std::shared_ptr<AdvancedConfig> createResearchConfig();
    static std::shared_ptr<AdvancedConfig> createRealTimeConfig();
    
    // Advanced features
    void setAutoSave(const std::string& directory, int intervalSteps);
    void setConfigWatcher(const std::string& filepath, std::function<void()> callback);
    std::string getConfigSummary() const;
    
    // Type conversion utilities
    static std::string toString(const ConfigValue& value);
    static bool fromString(const std::string& str, ConfigValue& value);
    
    // Statistics
    size_t getEntryCount() const;
    std::map<std::string, int> getTypeDistribution() const;
    double getMemoryUsage() const;
    
    // Configuration templates
    struct ConfigurationTemplate {
        std::string name;
        std::string description;
        std::map<std::string, std::string> defaults;
        std::vector<std::string> requiredKeys;
    };
    
    static std::vector<ConfigurationTemplate> getAvailableTemplates();
    
    // Export/import utilities
    bool exportToXML(const std::string& filepath) const;
    bool importFromXML(const std::string& filepath);
    
    // Batch operations
    void batchSet(const std::map<std::string, std::string>& values);
    std::map<std::string, std::string> batchGet(const std::vector<std::string>& keys) const;
    
protected:
    // Override base methods for enhanced functionality
    bool loadFromFile(const std::string& filepath) override;
    bool saveToFile(const std::string& filepath) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced configuration management utilities
namespace ConfigUtils {
    // Configuration validation
    bool validateKeyFormat(const std::string& key);
    bool validateValueType(const std::string& key, const ConfigValue& value);
    
    // Configuration merging
    void mergeConfigs(Config& target, const Config& source);
    bool isSubset(const Config& subset, const Config& superset);
    
    // Preset configurations
    std::map<std::string, std::string> getBrainDefaults();
    std::map<std::string, std::string> getPlasticityDefaults();
    std::map<std::string, std::string> getMemoryDefaults();
    std::map<std::string, std::string> getNeuromodulationDefaults();
    
    // Export/import utilities
    std::string configToString(const Config& config, const std::string& format = "key=value");
    bool configFromString(Config& config, const std::string& content, const std::string& format = "key=value");
    
    // Statistics and analysis
    std::string analyzeConfig(const Config& config);
    size_t countActiveEntries(const Config& config);
    std::vector<std::string> findUnusedEntries(const Config& config);
}

} // namespace nlm

#endif // CONFIG_ADVANCED_HPP
