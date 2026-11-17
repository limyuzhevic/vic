// Configuration file generation and validation utilities
// Provides enhanced configuration capabilities for NLM

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <json/json.h> // For JSON serialization if available

using namespace nlm;

namespace config {

    // Configuration schema validation
    class ConfigurationValidator {
    public:
        struct ValidationRule {
            std::string parameter;
            std::string type; // "int", "float", "string", "bool"
            std::string minValue;
            std::string maxValue;
            std::string defaultValue;
            std::string description;
            bool required;
        };
        
        static bool ValidateParameter(const std::string& name, const std::string& value);
        static bool ValidateFile(const std::string& filepath);
        static std::vector<std::string> GetValidationErrors(const std::shared_ptr<Config>& config);
    };

    // Configuration file generator
    class ConfigurationGenerator {
    public:
        static std::string GenerateDefaultConfig();
        static std::string GeneratePhaseConfig(const std::string& phase);
        static std::string GenerateBenchmarkConfig(int neuronCount, int steps);
        static std::string GenerateCustomConfig(const std::map<std::string, std::string>& customSettings);
        static bool SaveConfigToFile(const std::string& config, const std::string& filepath);
    };

    // Configuration migration utilities
    class ConfigurationMigrator {
    public:
        static std::shared_ptr<Config> MigrateFromLegacyFormat(const std::string& legacyConfig);
        static std::string MigrateToLegacyFormat(const std::shared_ptr<Config>& config);
        static std::string GetMigrationNotes();
    };

    // Configuration template system
    class ConfigurationTemplateSystem {
    public:
        struct Template {
            std::string name;
            std::string description;
            std::map<std::string, std::string> defaultValues;
            std::vector<std::string> tags;
        };
        
        static std::vector<Template> GetAvailableTemplates();
        static std::string ApplyTemplate(const std::string& templateName, 
                                        const std::map<std::string, std::string>& overrides);
        static bool ValidateTemplate(const Template& template);
    };

    // Configuration comparison and diff utilities
    class ConfigurationComparator {
    public:
        struct ComparisonResult {
            std::vector<std::string> differences;
            std::vector<std::string> similarities;
            double similarityScore; // 0-100
        };
        
        static ComparisonResult CompareConfigs(const std::shared_ptr<Config>& config1, 
                                             const std::shared_ptr<Config>& config2);
        static std::string GenerateDiffReport(const ComparisonResult& result);
    };

    // Configuration statistics and analytics
    class ConfigurationAnalytics {
    public:
        struct ConfigStats {
            size_t totalParameters;
            size_t validParameters;
            size_t missingParameters;
            std::map<std::string, std::string> parameterTypes;
            std::vector<std::string> unusedParameters;
        };
        
        static ConfigStats AnalyzeConfig(const std::shared_ptr<Config>& config);
        static std::string GenerateAnalyticsReport(const ConfigStats& stats);
    };

    // Export/Import utilities
    class ConfigurationExporter {
    public:
        static std::string ExportToJSON(const std::shared_ptr<Config>& config);
        static std::string ExportToINI(const std::shared_ptr<Config>& config);
        static std::string ExportToXML(const std::shared_ptr<Config>& config);
        static std::string ExportToYAML(const std::shared_ptr<Config>& config);
        
        static bool ImportFromJSON(const std::string& json, std::shared_ptr<Config>& config);
        static bool ImportFromINI(const std::string& ini, std::shared_ptr<Config>& config);
        static bool ImportFromXML(const std::string& xml, std::shared_ptr<Config>& config);
        static bool ImportFromYAML(const std::string& yaml, std::shared_ptr<Config>& config);
    };

    // Configuration history and versioning
    class ConfigurationHistory {
    public:
        struct ConfigVersion {
            int version;
            std::string timestamp;
            std::string author;
            std::string description;
            std::map<std::string, std::string> changes;
        };
        
        static void RecordConfig(const std::shared_ptr<Config>& config, 
                               const std::string& author, const std::string& description);
        static std::vector<ConfigVersion> GetHistory();
        static std::shared_ptr<Config> RestoreVersion(int version);
        static std::string GetHistoryReport();
    };

} // namespace config