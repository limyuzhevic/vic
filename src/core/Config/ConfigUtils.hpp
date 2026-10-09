// Core configuration utilities for NLM Python API
// Comprehensive configuration management and analysis

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cmath>
#include <limits>
#include <numeric>
#include <queue>
#include <set>
#include <regex>
#include <iomanip>

#include "Config.hpp"
#include "NLMExceptions.hpp"
#include "Types.hpp"

namespace nlm {

// ==============================================================================
// CONFIGURATION PRESETS
// ==============================================================================

// Task-specific configuration presets
class TaskPresets {
public:
    // RL (Reinforcement Learning) presets
    static Config createRLBasicPreset(const std::string& description = "Basic RL configuration for simple tasks");
    static Config createRLAdvancedPreset(const std::string& description = "Advanced RL configuration for complex tasks");
    
    // Developmental presets
    static Config createDevelopmentalPreset(const std::string& stage, double startAge, double duration);
    static Config createCriticalPeriodPreset(const std::string& description = "Critical period configuration for sensitive learning");
    static Config createMaturationPreset(const std::string& description = "Maturation phase configuration for advanced development");
    
    // Perception presets
    static Config createPerceptionPreset(const std::string& modality, size_t inputSize);
    static Config createVisionPreset(const std::string& description = "Vision perception configuration for visual tasks");
    static Config createAudioPreset(const std::string& description = "Audio perception configuration for auditory tasks");
    
    // Motor presets
    static Config createMotorPreset(const std::string& actionSpace, size_t outputSize);
    static Config createMovementPreset(const std::string& description = "Movement motor configuration for embodied agents");
    static Config createManipulationPreset(const std::string& description = "Manipulation motor configuration for fine motor tasks");
    
    // Cognitive presets
    static Config createCognitivePreset(const std::string& cognitiveModule, const std::string& complexity);
    static Config createMemoryPreset(const std::string& memoryType, size_t capacity);
    static Config createPlanningPreset(const std::string& planningType, size_t horizon);
    
    // Research domain presets
    static Config createNeurosciencePreset(const std::string& focusArea, size_t neuronCount);
    static Config createRoboticsPreset(const std::string& robotType, const std::string& sensorySuite);
    static Config createAIPreset(const std::string& aiTask, const std::string& learningApproach);
    static Config createPsychologyPreset(const std::string& behavioralModel, size_t subjectCount);
    
    // Performance optimization presets
    static Config createSpeedPreset(const std::string& description = "Speed-optimized configuration for real-time applications");
    static Config createAccuracyPreset(const std::string& description = "Accuracy-optimized configuration for high-precision tasks");
    static Config createBalancePreset(const std::string& description = "Balanced configuration for general-purpose applications");
    
    // Custom configuration templates
    static Config createCustomTemplate(const std::string& name, const std::map<std::string, std::string>& parameters);
    static Config createExperimentTemplate(const std::string& experimentType, const std::vector<std::string>& conditions);
    
    // List all available presets
    static std::vector<std::string> getAvailablePresets();
    static std::map<std::string, std::string> getPresetMetadata(const std::string& presetName);
};

// Network size presets
class NetworkSizePresets {
public:
    // Small networks (ideal for testing and embedded systems)
    static Config createSmallPreset(const std::string& description = "Small network configuration for quick testing");
    static Config createMinimalPreset(const std::string& description = "Minimal network configuration for resource-constrained environments");
    
    // Medium networks (balanced for most research applications)
    static Config createMediumPreset(const std::string& description = "Medium network configuration for balanced performance");
    static Config createStandardPreset(const std::string& description = "Standard network configuration for general research");
    
    // Large networks (for comprehensive experiments)
    static Config createLargePreset(const std::string& description = "Large network configuration for detailed simulations");
    static Config createExtensivePreset(const std::string& description = "Extensive network configuration for large-scale research");
    
    // Expert networks (state-of-the-art for benchmarking)
    static Config createExpertPreset(const std::string& description = "Expert network configuration for advanced research");
    static Config createBenchmarkPreset(const std::string& description = "Benchmark network configuration for performance testing");
    
    // Custom network size configuration
    static Config createCustomSizePreset(size_t neuronCount, size_t synapseCount, double connectivity);
    
    // Network complexity analysis
    static std::string analyzeNetworkSize(const Config& config);
    static std::map<std::string, double> getNetworkMetrics(const Config& config);
};

// ==============================================================================
// CONFIGURATION MANAGEMENT UTILITIES
// ==============================================================================

// Configuration comparison utilities
class ConfigurationComparator {
public:
    // Compare two configurations
    static bool compareConfigurations(const Config& config1, const Config& config2,
                                   std::vector<std::string>& differences,
                                   std::map<std::string, std::pair<ConfigValue, ConfigValue>>& valueChanges);
    
    // Compare configurations for equality
    static bool areConfigurationsEqual(const Config& config1, const Config& config2);
    static double calculateConfigurationSimilarity(const Config& config1, const Config& config2);
    
    // Find differences in specific keys
    static std::vector<std::string> getKeyDifferences(const Config& config1, const Config& config2);
    static std::map<std::string, ConfigValue> getChangedValues(const Config& config1, const Config& config2);
    
    // Configuration comparison with detailed analysis
    static std::string generateComparisonReport(const Config& config1, const Config& config2);
    static std::vector<std::map<std::string, std::string>> getDetailedDifferences(const Config& config1, const Config& config2);
};

// Configuration merging utilities
class ConfigurationMerger {
public:
    // Merge multiple configurations
    static Config mergeConfigurations(const std::vector<Config>& configs,
                                    const std::string& mergeStrategy = "overwrite",
                                    bool resolveConflicts = true);
    
    // Merge two configurations
    static Config mergeTwoConfigurations(const Config& config1, const Config& config2,
                                        const std::string& mergeStrategy = "overwrite");
    
    // Resolve configuration conflicts
    static Config resolveConflicts(const Config& baseConfig, const Config& overrideConfig);
    static Config smartMerge(const Config& config1, const Config& config2);
    
    // Merge with precedence
    static Config mergeWithPrecedence(const std::vector<std::pair<Config, int>>& configsWithPriority);
    
    // Validate merged configuration
    static bool validateMergedConfiguration(const Config& mergedConfig, std::string& errorMessage);
    static std::string generateMergeReport(const std::vector<Config>& configs);
};

// Configuration inheritance utilities
class ConfigurationInheritanceManager {
public:
    // Set up parent-child configuration relationships
    static void establishInheritance(const std::string& childKey, const Config& parentConfig,
                                   const std::string& inheritanceType = "deep");
    static Config resolveInheritance(const std::string& configKey);
    
    // Get inherited configuration
    static Config getInheritedConfiguration(const std::string& configKey);
    static std::vector<std::string> getInheritanceChain(const std::string& configKey);
    
    // Configure with inheritance
    static Config createInheritableConfig(const std::string& name, const Config& baseConfig);
    static bool updateInheritedConfig(const std::string& configKey, const std::map<std::string, ConfigValue>& updates);
    
    // Inheritance validation
    static bool validateInheritanceTree(const std::map<std::string, Config>& configTree);
    static std::string generateInheritanceReport(const std::map<std::string, Config>& configTree);
};

// Configuration validation utilities
class ConfigurationValidator {
public:
    // Validate configuration against multiple schemas
    static bool validateConfiguration(const Config& config, const std::string& schema = "default");
    static std::vector<std::string> getValidationErrors(const Config& config);
    static std::vector<std::string> getValidationWarnings(const Config& config);
    
    // Schema validation
    static bool validateAgainstSchema(const Config& config, const std::string& schemaName);
    static Config getSchemaDefaults(const std::string& schemaName);
    static std::string getAvailableSchemas();
    
    // Advanced validation
    static bool validateConfigurationConstraints(const Config& config);
    static std::vector<std::string> getConstraintViolations(const Config& config);
    
    // Validation with suggestions
    static std::pair<bool, std::vector<std::string>> validateWithSuggestions(const Config& config);
    static std::string generateValidationReport(const Config& config);
};

// Configuration optimization utilities
class ConfigurationOptimizer {
public:
    // Optimize configuration for specific goals
    static Config optimizeForSpeed(const Config& config, double targetSpeed = 1.0);
    static Config optimizeForAccuracy(const Config& config, double targetAccuracy = 0.95);
    static Config optimizeForBalance(const Config& config, double targetScore = 0.8);
    
    // Automated optimization based on performance goals
    static Config autoOptimize(const Config& baseConfig, const std::string& goal,
                             const std::map<std::string, double>& constraints = {});
    static std::map<std::string, Config> generateOptimizationVariants(const Config& config);
    
    // Parameter optimization
    static Config optimizeParameters(const Config& config, const std::vector<std::string>& parameterNames);
    static std::vector<Config> performParameterSweep(const Config& baseConfig, const std::string& parameter,
                                                   const std::vector<double>& values);
    
    // Optimization validation
    static bool validateOptimizedConfig(const Config& optimizedConfig, const Config& originalConfig);
    static std::string generateOptimizationReport(const Config& originalConfig, const Config& optimizedConfig);
};

// ==============================================================================
// CONFIGURATION ANALYSIS TOOLS
// ==============================================================================

// Configuration complexity analysis
class ConfigurationComplexityAnalyzer {
public:
    // Measure configuration complexity
    static double calculateConfigurationComplexity(const Config& config);
    static std::string analyzeComplexityFactors(const Config& config);
    static std::map<std::string, double> getComplexityBreakdown(const Config& config);
    
    // Complexity metrics
    static double calculateParameterInterdependency(const Config& config);
    static double calculateConfigurationSize(const Config& config);
    static double calculateValueVariability(const Config& config);
    static size_t estimateProcessingTime(const Config& config);
    
    // Complexity assessment
    static std::string classifyComplexity(const Config& config);
    static bool isComplexityAcceptable(const Config& config, double threshold = 0.7);
    static std::string getComplexityRecommendations(const Config& config);
};

// Configuration dependency analysis
class ConfigurationDependencyAnalyzer {
public:
    // Identify configuration dependencies
    static std::map<std::string, std::vector<std::string>> analyzeDependencies(const Config& config);
    static std::vector<std::string> getDependentKeys(const Config& config);
    static std::vector<std::string> getIndependentKeys(const Config& config);
    
    // Dependency graph analysis
    static std::string generateDependencyGraph(const Config& config);
    static std::vector<std::vector<std::string>> getDependencyClusters(const Config& config);
    static std::map<std::string, std::vector<std::string>> getDependencyMatrix(const Config& config);
    
    // Dependency impact analysis
    static std::map<std::string, double> calculateDependencyImpact(const Config& config);
    static std::string getDependencyReport(const Config& config);
};

// Configuration sensitivity analysis
class ConfigurationSensitivityAnalyzer {
public:
    // Analyze parameter sensitivity
    static std::map<std::string, double> analyzeParameterSensitivity(const Config& config);
    static double calculateParameterSensitivity(const Config& config, const std::string& parameter);
    static std::vector<std::string> getSensitiveParameters(const Config& config, double threshold = 0.5);
    static std::vector<std::string> getInsensitiveParameters(const Config& config, double threshold = 0.5);
    
    // Sensitivity to changes
    static std::map<std::string, std::vector<double>> analyzeSensitivityToChanges(const Config& config);
    static double predictSensitivity(const Config& config, const std::string& parameter, double changeAmount);
    static std::string getSensitivityReport(const Config& config);
};

// Configuration impact analysis
class ConfigurationImpactAnalyzer {
public:
    // Predict impact of parameter changes
    static std::map<std::string, double> analyzeParameterImpact(const Config& config, const std::string& parameter, double changeAmount);
    static std::vector<std::map<std::string, double>> analyzeImpactScenario(const Config& baseConfig, const std::map<std::string, double>& parameterChanges);
    static std::string predictImpact(const Config& config, const std::string& parameter, double changeAmount);
    
    // Comprehensive impact analysis
    static std::map<std::string, std::map<std::string, double>> analyzeAllParameterImpacts(const Config& config);
    static std::string generateImpactReport(const Config& config);
    static std::vector<std::string> getHighImpactParameters(const Config& config, double threshold = 0.3);
};

// Configuration optimization analysis
class ConfigurationOptimizationAnalyzer {
public:
    // Find optimal parameter combinations
    static std::vector<Config> findOptimalCombinations(const Config& baseConfig, const std::vector<std::string>& parameterNames);
    static Config findSingleOptimal(const Config& baseConfig, const std::string& parameter);
    static std::vector<Config> performOptimizationSearch(const Config& config, const std::string& goal);
    
    // Multi-objective optimization
    static std::vector<Config> paretoOptimize(const Config& config, const std::vector<std::string>& objectives);
    static Config findParetoOptimal(const Config& config, const std::string& objective1, const std::string& objective2);
    static std::string getParetoFront(const Config& config, const std::vector<std::string>& objectives);
    
    // Optimization analysis
    static std::map<std::string, double> analyzeOptimizationPotential(const Config& config);
    static std::string getOptimizationRecommendations(const Config& config);
    static std::string generateOptimizationAnalysis(const Config& config);
};

// ==============================================================================
// CONFIGURATION EXPORT/IMPORT UTILITIES
// ==============================================================================

// Configuration export to multiple formats
class ConfigurationExporter {
public:
    // Export to JSON format
    static bool exportToJSON(const Config& config, const std::string& filepath);
    static bool exportToYAML(const Config& config, const std::string& filepath);
    static bool exportToXML(const Config& config, const std::string& filepath);
    static bool exportToTOML(const Config& config, const std::string& filepath);
    
    // Export to multiple formats
    static std::map<std::string, std::string> exportToMultipleFormats(const Config& config,
                                                                      const std::vector<std::string>& formats);
    static std::string exportToString(const Config& config, const std::string& format = "json");
    
    // Export with metadata
    static bool exportWithMetadata(const Config& config, const std::string& filepath,
                                  const std::map<std::string, std::string>& metadata);
    static std::string generateExportReport(const Config& config, const std::string& format);
};

// Configuration import from multiple formats
class ConfigurationImporter {
public:
    // Import from JSON format
    static Config importFromJSON(const std::string& filepath);
    static Config importFromYAML(const std::string& filepath);
    static Config importFromXML(const std::string& filepath);
    static Config importFromTOML(const std::string& filepath);
    
    // Import from multiple formats
    static Config importFromFormat(const std::string& filepath, const std::string& format);
    static std::map<std::string, Config> importMultipleFiles(const std::vector<std::string>& filepaths);
    
    // Import with validation
    static std::pair<Config, std::vector<std::string>> importWithValidation(const std::string& filepath, const std::string& format);
    static bool validateImportFile(const std::string& filepath, const std::string& format);
};

// Configuration validation export
class ConfigurationValidationExporter {
public:
    // Export validation results
    static bool exportValidationResults(const Config& config, const std::string& filepath);
    static std::string generateValidationReport(const Config& config);
    static std::vector<std::string> exportValidationErrors(const Config& config);
    static std::vector<std::string> exportValidationWarnings(const Config& config);
    
    // Validation export in multiple formats
    static std::map<std::string, std::string> exportValidationToFormats(const Config& config, const std::vector<std::string>& formats);
    static std::string generateDetailedValidationReport(const Config& config);
};

// Configuration diff analysis
class ConfigurationDiffAnalyzer {
public:
    // Show differences between configurations
    static std::vector<std::string> analyzeDiff(const Config& config1, const Config& config2);
    static std::string generateDiffReport(const Config& config1, const Config& config2);
    static std::map<std::string, std::pair<ConfigValue, ConfigValue>> getDiffDetails(const Config& config1, const Config& config2);
    
    // Detailed diff analysis
    static std::vector<std::map<std::string, std::string>> analyzeDetailedDiff(const Config& config1, const Config& config2);
    static std::string generateUnifiedDiff(const Config& config1, const Config& config2);
    static std::string generateSideBySideDiff(const Config& config1, const Config& config2);
};

// Configuration backup and restore utilities
class ConfigurationBackupManager {
public:
    // Backup configuration
    static bool createBackup(const Config& config, const std::string& backupPath);
    static std::string createTimestampedBackup(const Config& config, const std::string& basePath);
    static std::vector<std::string> listBackups(const std::string& backupDirectory);
    
    // Restore configuration
    static Config restoreBackup(const std::string& backupPath);
    static bool restoreToConfig(Config& config, const std::string& backupPath);
    static bool restoreLatestBackup(Config& config, const std::string& backupDirectory);
    
    // Backup management
    static bool deleteBackup(const std::string& backupPath);
    static bool cleanupOldBackups(const std::string& backupDirectory, int keepCount = 10);
    static std::string getBackupInfo(const std::string& backupPath);
};

// ==============================================================================
// ADVANCED CONFIGURATION FEATURES
// ==============================================================================

// Configuration versioning
class ConfigurationVersionManager {
public:
    // Track configuration changes over time
    static std::string createVersion(const Config& config, const std::string& description);
    static Config getVersion(const std::string& versionId);
    static std::vector<std::string> getVersionHistory();
    static std::map<std::string, std::string> getVersionInfo(const std::string& versionId);
    
    // Version comparison and diff
    static std::string compareVersions(const std::string& version1, const std::string& version2);
    static Config getDifference(const std::string& version1, const std::string& version2);
    static bool isVersionCompatible(const std::string& version1, const std::string& version2);
    
    // Version branching and merging
    static std::string createBranch(const Config& baseVersion, const std::string& branchName);
    static bool mergeVersions(const std::string& version1, const std::string& version2, const std::string& targetVersion);
    static std::vector<std::string> getBranches();
};

// Configuration comparison with diff visualization
class ConfigurationComparisonVisualizer {
public:
    // Generate diff visualization
    static std::string generateDiffVisualization(const Config& config1, const Config& config2);
    static std::string generateSideBySideComparison(const Config& config1, const Config& config2);
    static std::string generateTreeComparison(const Config& config1, const Config& config2);
    
    // Visual diff formats
    static std::string generateJSONDiff(const Config& config1, const Config& config2);
    static std::string generateHTMLDiff(const Config& config1, const Config& config2);
    static std::string generateGraphVizDiff(const Config& config1, const Config& config2);
    
    // Diff export
    static bool exportDiffVisualization(const Config& config1, const Config& config2, const std::string& filepath);
    static std::string getDiffStatistics(const Config& config1, const Config& config2);
};

// Configuration templating
class ConfigurationTemplateManager {
public:
    // Configuration templating for reusable experiment setups
    static std::string createTemplate(const Config& config, const std::string& templateName);
    static Config applyTemplate(const std::string& templateName, const std::map<std::string, ConfigValue>& overrides = {});
    static std::vector<std::string> getAvailableTemplates();
    static std::map<std::string, std::string> getTemplateInfo(const std::string& templateName);
    
    // Template management
    static bool updateTemplate(const std::string& templateName, const Config& newConfig);
    static bool deleteTemplate(const std::string& templateName);
    static std::vector<Config> listTemplateVariants(const std::string& templateName);
    
    // Template validation
    static bool validateTemplate(const std::string& templateName);
    static std::string getTemplateValidationReport(const std::string& templateName);
};

// Configuration sharing utilities
class ConfigurationSharingManager {
public:
    // Share configurations between users
    static std::string shareConfiguration(const Config& config, const std::string& userId, const std::string& permissionLevel = "read");
    static Config retrieveSharedConfiguration(const std::string& shareId);
    static std::vector<std::string> getSharedConfigurations(const std::string& userId);
    static bool revokeShare(const std::string& shareId);
    
    // Shared configuration management
    static std::vector<std::string> getSharedConfigurationList();
    static std::map<std::string, std::string> getSharedConfigurationMetadata(const std::string& shareId);
    static bool updateSharedConfiguration(const std::string& shareId, const Config& newConfig);
    
    // Sharing permissions
    static bool setSharePermission(const std::string& shareId, const std::string& userId, const std::string& permissionLevel);
    static std::string getSharePermission(const std::string& shareId, const std::string& userId);
};

// Configuration marketplace
class ConfigurationMarketplace {
public:
    // Access to community configurations
    static std::vector<Config> browseCommunityConfigurations(const std::string& category = "all");
    static Config searchCommunityConfiguration(const std::string& query);
    static Config getFeaturedConfiguration();
    static std::vector<std::string> getPopularCategories();
    
    // Marketplace operations
    static bool purchaseConfiguration(const std::string& configId, const std::string& buyerId);
    static bool uploadConfiguration(const Config& config, const std::string& sellerId, const std::string& pricing = "free");
    static bool rateConfiguration(const std::string& configId, const std::string& userId, int rating);
    static std::vector<Config> getUserPurchases(const std::string& userId);
    
    // Marketplace statistics
    static std::map<std::string, int> getMarketplaceStatistics();
    static std::vector<std::string> getTopRatedConfigurations();
    static std::string getMarketplaceReport();
};

// ==============================================================================
// UTILITY CLASSES
// ==============================================================================

// Configuration template for common experiments
struct ExperimentTemplate {
    std::string name;
    std::string description;
    Config config;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point created;
    std::string createdBy;
    
    ExperimentTemplate() : created(std::chrono::system_clock::now()) {}
    ExperimentTemplate(const std::string& n, const std::string& desc, const Config& c)
        : name(n), description(desc), config(c), created(std::chrono::system_clock::now()) {}
};

// Configuration diff entry
struct ConfigurationDiff {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    std::string changeType; // "added", "removed", "modified"
    double significance;
    
    ConfigurationDiff() : significance(0.0) {}
    ConfigurationDiff(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal, const std::string& change)
        : key(k), oldValue(oldVal), newValue(newVal), changeType(change), significance(calculateSignificance(oldVal, newVal)) {}
    
private:
    static double calculateSignificance(const ConfigValue& oldVal, const ConfigValue& newVal) {
        if (std::holds_alternative<std::string>(oldVal) && std::holds_alternative<std::string>(newVal)) {
            return std::get<std::string>(oldVal) != std::get<std::string>(newVal) ? 1.0 : 0.0;
        } else if (std::holds_alternative<double>(oldVal) && std::holds_alternative<double>(newVal)) {
            return std::abs(std::get<double>(oldVal) - std::get<double>(newVal)) / std::max(std::abs(std::get<double>(oldVal)), 1.0);
        }
        return 1.0; // Default significance for other types
    }
};

// Configuration comparison result
struct ConfigurationComparisonResult {
    bool identical;
    double similarity;
    std::vector<ConfigurationDiff> differences;
    std::map<std::string, double> categoryScores;
    std::string summary;
    
    ConfigurationComparisonResult() : identical(true), similarity(1.0) {}
};

// Configuration optimization result
struct ConfigurationOptimizationResult {
    Config optimizedConfig;
    double improvementScore;
    std::map<std::string, double> parameterChanges;
    std::vector<std::string> optimizationSteps;
    std::string optimizationStrategy;
    
    ConfigurationOptimizationResult() : improvementScore(0.0) {}
    ConfigurationOptimizationResult(const Config& config, double score) : optimizedConfig(config), improvementScore(score) {}
};

// Configuration validation result
struct ConfigurationValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, bool> sectionValidity;
    std::string validationReport;
    
    ConfigurationValidationResult() : isValid(true) {}
    ConfigurationValidationResult(bool valid, const std::string& report) : isValid(valid), validationReport(report) {}
};

// ==============================================================================
// GLOBAL FUNCTIONS
// ==============================================================================

// Global configuration preset registry
class GlobalPresetRegistry {
public:
    static void registerPreset(const std::string& name, const Config& config, const std::string& description);
    static Config getPreset(const std::string& name);
    static bool isPresetAvailable(const std::string& name);
    static std::vector<std::string> getAllPresetNames();
    static std::map<std::string, std::string> getAllPresetMetadata();
    
    // Preset management
    static bool removePreset(const std::string& name);
    static bool updatePreset(const std::string& name, const Config& newConfig);
    static std::string exportAllPresets(const std::string& filepath);
    static bool importPresets(const std::string& filepath);
};

// Global configuration comparison utilities
namespace Comparison {
    inline ConfigurationComparisonResult compare(const Config& config1, const Config& config2) {
        ConfigurationComparisonResult result;
        result.identical = ConfigurationComparator::areConfigurationsEqual(config1, config2);
        result.similarity = ConfigurationComparator::calculateConfigurationSimilarity(config1, config2);
        result.differences = ConfigurationComparator::getDetailedDifferences(config1, config2);
        return result;
    }
    
    inline std::string generateReport(const Config& config1, const Config& config2) {
        return ConfigurationComparator::generateComparisonReport(config1, config2);
    }
}

// Global configuration optimization utilities
namespace Optimization {
    inline ConfigurationOptimizationResult optimize(const Config& config, const std::string& goal) {
        ConfigurationOptimizationResult result;
        if (goal == "speed") {
            result = { ConfigurationOptimizer::optimizeForSpeed(config), 0.0 }; // Would calculate improvement
        } else if (goal == "accuracy") {
            result = { ConfigurationOptimizer::optimizeForAccuracy(config), 0.0 };
        } else if (goal == "balance") {
            result = { ConfigurationOptimizer::optimizeForBalance(config), 0.0 };
        }
        return result;
    }
    
    inline std::vector<Config> findOptimalCombinations(const Config& config, const std::vector<std::string>& parameters) {
        return ConfigurationOptimizer::findOptimalCombinations(config, parameters);
    }
}

// Global configuration validation utilities
namespace Validation {
    inline ConfigurationValidationResult validate(const Config& config, const std::string& schema = "default") {
        ConfigurationValidationResult result;
        result.isValid = ConfigurationValidator::validateConfiguration(config, schema);
        result.errors = ConfigurationValidator::getValidationErrors(config);
        result.warnings = ConfigurationValidator::getValidationWarnings(config);
        result.validationReport = ConfigurationValidator::generateValidationReport(config);
        return result;
    }
    
    inline std::string getSchemaReport(const Config& config) {
        return ConfigurationValidator::generateValidationReport(config);
    }
}

// Global configuration export/import utilities
namespace ExportImport {
    inline bool exportConfig(const Config& config, const std::string& filepath, const std::string& format = "json") {
        if (format == "json") return ConfigurationExporter::exportToJSON(config, filepath);
        else if (format == "yaml") return ConfigurationExporter::exportToYAML(config, filepath);
        else if (format == "xml") return ConfigurationExporter::exportToXML(config, filepath);
        else if (format == "toml") return ConfigurationExporter::exportToTOML(config, filepath);
        return false;
    }
    
    inline Config importConfig(const std::string& filepath, const std::string& format = "json") {
        if (format == "json") return ConfigurationImporter::importFromJSON(filepath);
        else if (format == "yaml") return ConfigurationImporter::importFromYAML(filepath);
        else if (format == "xml") return ConfigurationImporter::importFromXML(filepath);
        else if (format == "toml") return ConfigurationImporter::importFromTOML(filepath);
        throw ConfigError("Unsupported import format: " + format);
    }
}

} // namespace nlm

#endif // NLM_CONFIG_UTILITIES_HPP