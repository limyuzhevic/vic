#pragma once

#include "Config.hpp"
#include "ConfigSchema.hpp"
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace nlm {

// Configuration comparison
class ConfigComparator {
public:
    // Compare two configurations
    static bool configsEqual(const Config& config1, const Config& config2);
    
    // Compare configurations with tolerance
    static bool configsEqual(const Config& config1, const Config& config2, 
                            double tolerance);
    
    // Get keys that are in config1 but not config2
    static std::vector<std::string> getAddedKeys(const Config& config1, const Config& config2);
    
    // Get keys that are in config2 but not config1
    static std::vector<std::string> getRemovedKeys(const Config& config1, const Config& config2);
    
    // Get keys with different values
    static std::vector<std::string> getChangedKeys(const Config& config1, const Config& config2, double tolerance = 0.0);
    
    // Get all differences
    static struct DifferenceReport {
        std::vector<std::string> addedKeys;
        std::vector<std::string> removedKeys;
        std::vector<std::string> changedKeys;
        
        std::string toString() const;
    } getDifferenceReport(const Config& config1, const Config& config2, double tolerance = 0.0);
};

// Configuration merging
class ConfigMerger {
public:
    // Merge two configurations
    static Config mergeConfigs(const Config& config1, const Config& config2);
    
    // Merge configuration with source preference
    static Config mergeConfigs(const Config& config1, const Config& config2,
                               ConfigSource preference);
    
    // Apply configuration to another configuration
    static void applyConfig(Config& target, const Config& source,
                           ConfigSource sourceType = ConfigSource::Runtime);
    
    // Merge multiple configurations
    static Config mergeConfigs(const std::vector<Config>& configs);
    
    // Create configuration with defaults, then apply overrides
    static Config createWithDefaults(const Config& defaults, const Config& overrides);
    
    // Create configuration with schema defaults, then apply overrides
    static Config createWithSchemaDefaults(const ConfigSchema& schema, const Config& overrides);
};

// Configuration diffing
class ConfigDiffer {
public:
    // Generate diff between two configurations
    static std::string generateDiff(const Config& config1, const Config& config2);
    
    // Generate human-readable diff
    static std::string generateHumanReadableDiff(const Config& config1, const Config& config2);
    
    // Generate JSON diff
    static std::string generateJsonDiff(const Config& config1, const Config& config2);
    
    // Generate unified diff format
    static std::string generateUnifiedDiff(const Config& config1, const Config& config2);
    
    // Check if configurations are compatible
    static bool areCompatible(const Config& config1, const Config& config2);
    
    // Get compatibility issues
    static std::vector<std::string> getCompatibilityIssues(const Config& config1, const Config& config2);
};

// Configuration validation utilities
class ConfigValidatorUtils {
public:
    // Check if configuration is valid according to schema
    static bool validateAgainstSchema(const Config& config, const ConfigSchema& schema);
    
    // Get validation errors for configuration
    static std::vector<std::string> getValidationErrors(const Config& config, const ConfigSchema& schema);
    
    // Get validation warnings for configuration
    static std::vector<std::string> getValidationWarnings(const Config& config, const ConfigSchema& schema);
    
    // Sanitize configuration (fix invalid values)
    static Config sanitizeConfig(const Config& config, const ConfigSchema& schema);
    
    // Get configuration health score
    static double getConfigHealthScore(const Config& config, const ConfigSchema& schema);
    
    // Check for configuration issues
    static struct ConfigIssues {
        std::vector<std::string> missingRequired;
        std::vector<std::string> invalidValues;
        std::vector<std::string> potentialIssues;
        
        bool hasIssues() const {
            return !missingRequired.empty() || !invalidValues.empty() || !potentialIssues.empty();
        }
        
        std::string toString() const;
    } getConfigIssues(const Config& config, const ConfigSchema& schema);
};

// Configuration import/export utilities
class ConfigIOUtils {
public:
    // Export configuration to JSON string
    static std::string exportToJson(const Config& config);
    
    // Export configuration to YAML string
    static std::string exportToYaml(const Config& config);
    
    // Export configuration toINI string
    static std::string exportToIni(const Config& config);
    
    // Import configuration from JSON string
    static Config importFromJson(const std::string& jsonStr);
    
    // Import configuration from YAML string
    static Config importFromYaml(const std::string& yamlStr);
    
    // Import configuration from INI string
    static Config importFromIni(const std::string& iniStr);
    
    // Export configuration to file
    static bool exportToFile(const Config& config, const std::string& filepath);
    
    // Import configuration from file
    static Config importFromFile(const std::string& filepath);
    
    // Generate configuration template
    static std::string generateTemplate(const ConfigSchema& schema, const std::string& format = "json");
    
    // Merge external configuration
    static Config mergeExternalConfig(const Config& baseConfig, const std::string& filepath);
    
    // Create configuration from command line
    static Config createFromCommandLine(int argc, char** argv);
};

// Configuration preset utilities
class ConfigPresetUtils {
public:
    // Get preset for scenario
    static Preset getPresetForScenario(const std::string& scenario);
    
    // Get all available preset names
    static std::vector<std::string> getAvailablePresets();
    
    // Create configuration from preset name
    static Config createFromPreset(const std::string& name);
    
    // Get preset recommendations
    static std::vector<Preset> getPresetRecommendations(const std::string& useCase);
    
    // Validate preset against schema
    static bool validatePreset(const Preset& preset, const ConfigSchema& schema);
    
    // Get preset comparison
    static std::string comparePresets(const std::string& preset1, const std::string& preset2);
};

// Configuration statistics
class ConfigStats {
public:
    // Get configuration statistics
    static struct ConfigStatistics {
        size_t totalEntries;
        size_t sourceCounts[3]; // Default, File, CommandLine, Runtime
        std::map<std::string, size_t> typeCounts;
        size_t uniqueKeys;
        
        double getUtilizationRate() const {
            if (totalEntries == 0) return 0.0;
            return static_cast<double>(uniqueKeys) / static_cast<double>(totalEntries);
        }
        
        std::string toString() const;
    } getStatistics(const Config& config);
    
    // Get configuration health indicators
    static struct HealthIndicators {
        double schemaCompliance; // 0-1
        double validationScore; // 0-1
        double completenessScore; // 0-1
        double consistencyScore; // 0-1
        
        double getOverallScore() const {
            return (schemaCompliance + validationScore + completenessScore + consistencyScore) / 4.0;
        }
        
        std::string toString() const;
    } getHealthIndicators(const Config& config, const ConfigSchema& schema);
    
    // Compare configuration with preset
    static double getPresetSimilarity(const Config& config, const Preset& preset);
    
    // Get configuration entropy
    static double getConfigurationEntropy(const Config& config);
};

} // namespace nlm