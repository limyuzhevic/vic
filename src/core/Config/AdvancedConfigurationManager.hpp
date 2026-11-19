// Enhanced Configuration Manager for Advanced Users
// This provides advanced configuration features including JSON/YAML support,
// validation, and enhanced user control
// Simplified version without external dependencies for compatibility

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <sstream>

using json = std::string;  // Simplified for compatibility

namespace nlm {

// Forward declaration
class Config;

// Advanced configuration validation rules
struct ValidationRule {
    std::string parameter;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool warningOnly;
};

// Configuration export/import utilities
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Load from JSON file (simplified implementation)
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file (simplified implementation)
    bool loadFromYAML(const std::string& filepath);
    
    // Export to JSON
    bool exportToJSON(const std::string& filepath) const;
    
    // Export to YAML
    bool exportToYAML(const std::string& filepath) const;
    
    // Configuration validation with detailed reports
    bool validateConfiguration(std::vector<std::string>& errors,
                               std::vector<std::string>& warnings) const;
    
    // Add validation rule
    void addValidationRule(const std::string& param,
                           const std::function<bool(const ConfigValue&)>& validator,
                           const std::string& errorMsg,
                           bool warningOnly = false);
    
    // Get validation report
    std::string getValidationReport() const;
    
    // Configuration optimization
    bool optimizeConfiguration();
    
    // Configuration compression
    bool compressConfiguration();
    
    // Configuration backup
    bool createBackup(const std::string& backupPath = "") const;
    
    // Configuration comparison
    bool compareConfigurations(const ConfigurationManager& other,
                              std::vector<std::string>& differences) const;
    
    // Get configuration statistics
    std::string getConfigurationStats() const;
    
    // Get all parameter descriptions
    std::unordered_map<std::string, std::string> getAllParameterDescriptions() const;
    
    // Set parameter description
    void setParameterDescription(const std::string& key,
                                 const std::string& description);
    
    // Get default value for parameter
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const;
    
    // Add default value
    void addDefaultValue(const std::string& key, const ConfigValue& value,
                        const std::string& description = "");
    
    // Reset to defaults
    void resetToDefaults();
    
    // Merge configurations
    bool mergeConfiguration(const ConfigurationManager& other);
    
    // Validate configuration schema
    bool validateSchema(const json& schema) const;
    
    // Set raw parameter (forward to underlying Config)
    void setParameter(const std::string& key, const ConfigValue& value);
    
    // Get raw parameter (forward to underlying Config)
    std::optional<ConfigValue> getParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration export/import utilities
class ConfigurationTemplate {
public:
    ConfigurationTemplate(const std::string& name);
    ~ConfigurationTemplate();
    
    // Add parameter set
    void addParameterSet(const std::string& brainType,
                        const std::unordered_map<std::string, ConfigValue>& parameters);
    
    // Get parameter set
    std::unordered_map<std::string, ConfigValue> getParameterSet(
        const std::string& brainType) const;
    
    // List all templates
    std::vector<std::string> listTemplates() const;
    
    // Save template
    bool saveTemplate(const std::string& filepath) const;
    
    // Load template
    bool loadTemplate(const std::string& filepath);
    
    // Create custom template
    bool createCustomTemplate(const std::string& name,
                              const std::string& description);
    
    // Export template to JSON
    bool exportTemplateToJSON(const std::string& filepath) const;
    
    // Import template from JSON
    bool importTemplateFromJSON(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration diff tool
class ConfigurationDiffTool {
public:
    ConfigurationDiffTool();
    ~ConfigurationDiffTool();
    
    // Generate diff between two configurations
    std::vector<std::string> generateDiff(const ConfigurationManager& config1,
                                          const ConfigurationManager& config2) const;
    
    // Show diff in human-readable format
    std::string showDiffHumanReadable(const ConfigurationManager& config1,
                                     const ConfigurationManager& config2) const;
    
    // Show diff in machine-readable format
    std::string showDiffMachineReadable(const ConfigurationManager& config1,
                                       const ConfigurationManager& config2) const;
    
    // Export diff to file
    bool exportDiffToFile(const std::string& filepath,
                         const ConfigurationManager& config1,
                         const ConfigurationManager& config2) const;
    
    // Compare configurations with detailed metrics
    struct ComparisonMetrics {
        int totalDifferences;
        int additions;
        int removals;
        int modifications;
        double similarityScore;
    };
    
    ComparisonMetrics compareWithMetrics(const ConfigurationManager& config1,
                                         const ConfigurationManager& config2) const;
    
    // Get detailed comparison report
    std::string getDetailedComparisonReport(const ConfigurationManager& config1,
                                           const ConfigurationManager& config2) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration performance analyzer
class ConfigurationPerformanceAnalyzer {
public:
    ConfigurationPerformanceAnalyzer();
    ~ConfigurationPerformanceAnalyzer();
    
    // Analyze configuration impact on performance
    struct PerformanceImpact {
        std::string parameter;
        std::string impact;  // "positive", "negative", "neutral"
        double magnitude;    // 0.0 to 1.0
        std::string description;
    };
    
    std::vector<PerformanceImpact> analyzePerformanceImpact(
        const ConfigurationManager& config) const;
    
    // Get performance recommendations
    std::vector<std::string> getPerformanceRecommendations(
        const ConfigurationManager& config) const;
    
    // Analyze memory usage
    struct MemoryAnalysis {
        size_t estimatedMemoryUsageMB;
        std::string analysis;
        std::vector<std::string> optimizations;
    };
    
    MemoryAnalysis analyzeMemoryUsage(const ConfigurationManager& config) const;
    
    // Analyze simulation speed
    struct SpeedAnalysis {
        double estimatedStepsPerSecond;
        std::string analysis;
        std::vector<std::string> recommendations;
    };
    
    SpeedAnalysis analyzeSimulationSpeed(const ConfigurationManager& config) const;
    
    // Get comprehensive performance report
    std::string getPerformanceReport(const ConfigurationManager& config) const;
    
    // Get optimization report
    std::string getOptimizationReport(const ConfigurationManager& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced visualization configuration
class AdvancedVisualizationConfig {
public:
    AdvancedVisualizationConfig();
    ~AdvancedVisualizationConfig();
    
    // Configure visualization settings
    void configureVisualization(const std::string& type, 
                                const std::unordered_map<std::string, ConfigValue>& settings);
    
    // Get visualization settings
    std::unordered_map<std::string, ConfigValue> getVisualizationSettings(
        const std::string& type) const;
    
    // List available visualization types
    std::vector<std::string> listAvailableVisualizationTypes() const;
    
    // Configure real-time visualization
    void enableRealTimeVisualization(double fps = 30.0);
    
    // Configure offline visualization
    void configureOfflineVisualization(const std::string& outputFormat,
                                       const std::string& outputPath);
    
    // Configure export settings
    void configureExportSettings(const std::string& format,
                                 const std::unordered_map<std::string, std::string>& options);
    
    // Get export capabilities
    std::vector<std::string> getExportFormats() const;
    
    // Configure data filtering
    void configureDataFiltering(const std::string& filterType,
                                const std::unordered_map<std::string, ConfigValue>& parameters);
    
    // Get filtering options
    std::vector<std::string> getFilteringOptions() const;
    
    // Configure annotation settings
    void configureAnnotations(bool enabled,
                              const std::unordered_map<std::string, std::string>& templates);
    
    // Get annotation status
    bool getAnnotationStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced configuration export/import
class ConfigurationExportImport {
public:
    ConfigurationExportImport();
    ~ConfigurationExportImport();
    
    // Export configuration with metadata
    bool exportWithMetadata(const std::string& filepath,
                           const std::string& description,
                           const std::string& author = "",
                           const std::unordered_map<std::string, std::string>& tags = {});
    
    // Import configuration with validation
    bool importWithValidation(const std::string& filepath,
                             bool validate = true,
                             bool backup = true);
    
    // Export configuration as archive
    bool exportAsArchive(const std::string& filepath,
                        const std::string& format = "zip",
                        bool includeLogs = false,
                        bool includeScripts = false);
    
    // Import configuration from archive
    bool importFromArchive(const std::string& filepath);
    
    // Export configuration as documentation
    bool exportAsDocumentation(const std::string& filepath,
                              const std::string& format = "markdown");
    
    // Export configuration as script
    bool exportAsScript(const std::string& filepath,
                       const std::string& language = "python");
    
    // Batch export configurations
    bool batchExport(const std::vector<std::string>& filepaths,
                    const std::string& format = "json");
    
    // Batch import configurations
    bool batchImport(const std::vector<std::string>& filepaths);
    
    // Get export history
    std::vector<std::string> getExportHistory() const;
    
    // Get import history
    std::vector<std::string> getImportHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm