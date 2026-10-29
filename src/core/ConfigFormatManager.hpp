// ConfigFormatManager.hpp - Multi-format configuration manager
// Supports INI, JSON, and YAML configuration formats with validation and inheritance

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

class ConfigFormatManager {
public:
    enum class Format {
        INI,
        JSON,
        YAML
    };
    
    enum class Option {
        Inheritance,
        Validation,
        Overwrite
    };
    
    static ConfigFormatManager& getInstance();
    
    // Disable copying
    ConfigFormatManager(const ConfigFormatManager&) = delete;
    ConfigFormatManager& operator=(const ConfigFormatManager&) = delete;
    
    // Configuration loading with format detection
    bool loadConfig(Config& config, const std::string& filepath, Format format = Format::INI);
    bool loadConfig(Config& config, const std::string& content, Format format = Format::INI);
    
    // Configuration export
    bool exportConfig(const Config& config, const std::string& filepath, Format format = Format::INI);
    std::string exportConfig(const Config& config, Format format = Format::INI);
    
    // Format detection
    static Format detectFormat(const std::string& filepath);
    static Format detectFormatFromContent(const std::string& content);
    
    // Validation
    bool validateConfig(const Config& config, const std::map<std::string, std::pair<double, double>>& ranges);
    bool validateParameter(const std::string& key, const ConfigValue& value, 
                          const std::pair<double, double>& range);
    
    // Inheritance support
    bool loadWithInheritance(Config& config, const std::string& baseFile, 
                           const std::vector<std::string>& overrideFiles);
    
    // Configuration comparison
    std::vector<std::string> diffConfigs(const Config& config1, const Config& config2);
    
    // Configuration validation for ranges
    static std::map<std::string, std::pair<double, double>> getDefaultParameterRanges();
    
    // Configuration templates
    bool saveConfigTemplate(const std::string& filepath, Format format = Format::JSON);
    
    // Advanced configuration operations
    std::vector<std::string> getAllKeys(const Config& config);
    bool mergeConfigs(Config& config, const Config& other, bool overwrite = true);
    
    // Configuration history
    void addConfigToHistory(const Config& config);
    std::vector<Config> getConfigHistory();
    void clearConfigHistory();
    
    // Format-specific operations
    static std::string formatValue(const ConfigValue& value, Format format);
    static std::optional<ConfigValue> parseValue(const std::string& str, Format format);
    
    // Performance optimization
    void enableCaching(bool enable);
    bool isCachingEnabled() const;
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration schema
    struct ParameterInfo {
        std::string key;
        std::string type;
        std::string description;
        std::pair<double, double> range;
        ConfigValue defaultValue;
        bool required;
        
        ParameterInfo() : required(false) {}
    };
    
    static std::vector<ParameterInfo> getParameterSchema();
    
private:
    ConfigFormatManager();
    ~ConfigFormatManager();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience functions
bool loadConfigFromJSON(Config& config, const std::string& filepath);
bool loadConfigFromYAML(Config& config, const std::string& filepath);
bool loadConfigFromINI(Config& config, const std::string& filepath);

bool exportConfigToJSON(const Config& config, const std::string& filepath);
bool exportConfigToYAML(const Config& config, const std::string& filepath);
bool exportConfigToINI(const Config& config, const std::string& filepath);

} // namespace nlm