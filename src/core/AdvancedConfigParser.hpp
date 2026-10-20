//! Advanced Configuration Parser for NLM
// Phase 2: Advanced Configuration Management
//
// This header provides the advanced configuration system for NLM that supports:
// - JSON configuration files (modern, structured)
// - YAML configuration files (human-readable)
// - JSON5 (JSON with comments and trailing commas)
// - Configuration validation and schema checking
// - Type-safe configuration values with proper conversion
// - Configuration inheritance and merging
// - Advanced configuration features for research

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <filesystem>
#include <vector>
#include <regex>
#include "nlohmann/json.hpp"
#include "Config.hpp"

namespace nlm {

struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::string> typeConstraints;
    std::map<std::string, std::string> defaultValues;
    std::vector<std::string> requiredKeys;
    std::map<std::string, std::vector<std::string>> keyGroups;
};

class AdvancedConfigParser {
private:
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> cacheTimestamps;
    bool validateOnLoad = true;
    bool allowInheritance = true;
    std::string backupDirectory = "backups";

public:
    AdvancedConfigParser();
    ~AdvancedConfigParser() = default;

    void setValidationEnabled(bool enabled);
    void setInheritanceEnabled(bool enabled);
    void setBackupDirectory(const std::string& directory);

    // File loading methods
    bool loadFromJSONFile(const std::string& filepath, Config& config);
    bool loadFromYAMLFile(const std::string& filepath, Config& config);
    bool loadFromJSON5File(const std::string& filepath, Config& config);

    // File saving methods
    bool saveToJSONFile(const std::string& filepath, const Config& config);
    bool saveToYAMLFile(const std::string& filepath, const Config& config);

    // Configuration caching
    bool isConfigurationCached(const std::string& filepath) const;

    // Backup management
    void createBackup(const std::string& filepath, const Config& config);

private:
    void initializeDefaultSchemas();

    nlohmann::json parseJSON(const std::string& content);
    nlohmann::json parseYAML(const std::string& content);
    std::string preprocessJSON5(const std::string& content);
    void validateJSONSchema(const nlohmann::json& jsonData, const std::string& filepath);
    void validateYAMLSchema(const nlohmann::json& yamlData, const std::string& filepath);
    void convertJSONToConfig(const nlohmann::json& jsonData, Config& config);
    void convertYAMLToConfig(const nlohmann::json& yamlData, Config& config);
    nlohmann::json convertConfigToJSON(const Config& config);
    nlohmann::json convertConfigToYAML(const Config& config);

    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
};

// Global parser instance management
extern AdvancedConfigParser* globalParser;

// Advanced configuration management functions
bool loadAdvancedConfig(const std::string& filepath, Config& config, const std::string& format = "auto");
bool saveAdvancedConfig(const std::string& filepath, const Config& config, const std::string& format = "json");
void initializeAdvancedConfigurationSystem();
void cleanupAdvancedConfigurationSystem();

// Configuration analysis and validation functions
std::map<std::string, std::string> analyzeConfiguration(const Config& config);
bool validateConfiguration(const Config& config);
std::string generateConfigurationReport(const Config& config);

} // namespace nlm
