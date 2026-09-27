// ==============================================================================
// Configuration Debugger
// ==============================================================================
#ifndef CONFIGURATIONDEBUGGER_HPP
#define CONFIGURATIONDEBUGGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <unordered_set>
#include "../config/OptimizedConfig.hpp"

namespace nlm {
namespace debug {

/**
 * @class ConfigurationDebugger
 * @brief Configuration debugging and validation tool for NLM
 * 
 * Provides tools for configuration debugging including validation, optimization,
 * comparison, and history tracking.
 */
class ConfigurationDebugger {
public:
    /**
     * @enum ConfigurationIssueType
     * @brief Type of configuration issue
     */
    enum class ConfigurationIssueType {
        VALIDATION_ERROR,
        MISSING_VALUE,
        INVALID_VALUE,
        OUT_OF_RANGE,
        CONFLICTING_VALUES,
        PERFORMANCE_ISSUE,
        SECURITY_VIOLATION,
        COMPATIBILITY_ISSUE,
        DEPRECATION_WARNING,
        OPTIMIZATION_SUGGESTION
    };

    /**
     * @struct ConfigurationIssue
     * @brief Configuration issue
     */
    struct ConfigurationIssue {
        ConfigurationIssueType type;
        std::string description;
        std::string key;
        std::string value;
        std::string expectedValue;
        double severity;
        std::chrono::steady_clock::time_point detectedTime;
        std::vector<std::string> suggestions;
        std::map<std::string, std::string> metadata;
    };

    /**
     * @struct ConfigurationVersion
     * @brief Configuration version
     */
    struct ConfigurationVersion {
        std::string version;
        std::chrono::steady_clock::time_point timestamp;
        std::string description;
        std::map<std::string, std::string> changes;
        std::string author;
    };

    ConfigurationDebugger();
    ~ConfigurationDebugger();

    /**
     * @brief Start configuration debugging
     * @param config Configuration reference
     */
    void startDebugging(Config& config);

    /**
     * @brief Stop configuration debugging
     */
    void stopDebugging();

    /**
     * @brief Validate configuration
     * @param config Configuration to validate
     * @return Vector of configuration issues
     */
    std::vector<ConfigurationIssue> validateConfiguration(const Config& config);

    /**
     * @brief Get configuration issues
     * @return Vector of configuration issues
     */
    std::vector<ConfigurationIssue> getConfigurationIssues() const;

    /**
     * @brief Get critical configuration issues
     * @return Vector of critical issues
     */
    std::vector<ConfigurationIssue> getCriticalIssues() const;

    /**
     * @brief Get warnings
     * @return Vector of warnings
     */
    std::vector<ConfigurationIssue> getWarnings() const;

    /**
     * @brief Get suggestions
     * @return Vector of suggestions
     */
    std::vector<std::string> getSuggestions() const;

    /**
     * @brief Get optimization recommendations
     * @param config Configuration to analyze
     * @return Vector of optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations(const Config& config);

    /**
     * @brief Get configuration statistics
     * @param config Configuration to analyze
     * @return Configuration statistics
     */
    struct ConfigurationStatistics {
        size_t totalKeys;
        size_t validKeys;
        size_t invalidKeys;
        double validationScore;
        double optimizationScore;
        std::chrono::steady_clock::time_point lastValidation;
        std::chrono::steady_clock::duration averageValidationTime;
    };

    ConfigurationStatistics getConfigurationStatistics(const Config& config) const;

    /**
     * @brief Check for conflicts
     * @param config Configuration to check
     * @return Vector of conflicting configurations
     */
    std::vector<std::pair<std::string, std::string>> checkForConflicts(const Config& config);

    /**
     * @brief Check for dependencies
     * @param config Configuration to check
     * @return Vector of missing dependencies
     */
    std::vector<std::string> checkForDependencies(const Config& config);

    /**
     * @brief Check for security issues
     * @param config Configuration to check
     * @return Vector of security issues
     */
    std::vector<ConfigurationIssue> checkForSecurityIssues(const Config& config);

    /**
     * @brief Check for performance issues
     * @param config Configuration to check
     * @return Vector of performance issues
     */
    std::vector<ConfigurationIssue> checkForPerformanceIssues(const Config& config);

    /**
     * @brief Get configuration history
     * @return Vector of configuration versions
     */
    std::vector<ConfigurationVersion> getConfigurationHistory() const;

    /**
     * @brief Get configuration version
     * @return Configuration version
     */
    ConfigurationVersion getConfigurationVersion() const;

    /**
     * @brief Restore configuration from history
     * @param version Configuration version
     * @param config Configuration to restore
     * @return true if restoration was successful
     */
    bool restoreConfiguration(const std::string& version, Config& config);

    /**
     * @brief Export configuration
     * @param config Configuration to export
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportConfiguration(const Config& config, const std::string& filename);

    /**
     * @brief Import configuration
     * @param config Configuration to populate
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importConfiguration(Config& config, const std::string& filename);

    /**
     * @brief Generate configuration report
     * @return Configuration report
     */
    struct ConfigurationReport {
        size_t totalIssues;
        size_t criticalIssues;
        size_t warnings;
        size_t suggestions;
        double overallScore;
        std::vector<ConfigurationIssue> issues;
        std::chrono::steady_clock::time_point reportTime;
    };

    ConfigurationReport generateConfigurationReport();

    /**
     * @brief Generate configuration comparison report
     * @param config1 First configuration
     * @param config2 Second configuration
     * @return Comparison report
     */
    struct ConfigurationComparisonReport {
        std::string comparisonType;
        size_t totalDifferences;
        size_t totalSimilarities;
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
        double compatibilityScore;
    };

    ConfigurationComparisonReport generateConfigurationComparisonReport(const Config& config1, const Config& config2);

    /**
     * @brief Validate configuration against schema
     * @param config Configuration to validate
     * @param schemaFile Schema file path
     * @return Vector of validation errors
     */
    std::vector<std::string> validateConfigurationAgainstSchema(const Config& config, const std::string& schemaFile);

    /**
     * @brief Validate configuration against template
     * @param config Configuration to validate
     * @param templateFile Template file path
     * @return Vector of validation errors
     */
    std::vector<std::string> validateConfigurationAgainstTemplate(const Config& config, const std::string& templateFile);

    /**
     * @brief Generate configuration template
     * @param config Configuration to template
     * @param filename Output file path
     * @return true if template was generated
     */
    bool generateConfigurationTemplate(const Config& config, const std::string& filename);

    /**
     * @brief Merge configurations
     * @param config1 First configuration
     * @param config2 Second configuration
     * @return Merged configuration
     */
    Config mergeConfigurations(const Config& config1, const Config& config2);

    /**
     * @brief Generate configuration debugging visualization
     * @param filename Output file path
     */
    void generateConfigurationDebuggingVisualization(const std::string& filename);

    /**
     * @brief Generate configuration validation visualization
     * @param filename Output file path
     */
    void generateConfigurationValidationVisualization(const std::string& filename);

    /**
     * @brief Generate configuration comparison visualization
     * @param filename Output file path
     */
    void generateConfigurationComparisonVisualization(const std::string& filename);

    /**
     * @brief Get available schemas
     * @return Vector of schema file paths
     */
    std::vector<std::string> getAvailableSchemas() const;

    /**
     * @brief Get available templates
     * @return Vector of template file paths
     */
    std::vector<std::string> getAvailableTemplates() const;

    /**
     * @brief Format configuration issue for display
     * @param issue Configuration issue
     * @return Formatted string
     */
    std::string formatConfigurationIssue(const ConfigurationIssue& issue) const;

    /**
     * @brief Format configuration statistics for display
     * @param stats Configuration statistics
     * @return Formatted string
     */
    std::string formatConfigurationStatistics(const ConfigurationStatistics& stats) const;

protected:
    /**
     * @brief Update configuration validation
     */
    void updateConfigurationValidation();

    /**
     * @brief Check configuration conflicts
     */
    void checkConfigurationConflicts();

    /**
     * @brief Check configuration dependencies
     */
    void checkConfigurationDependencies();

    /**
     * @brief Check configuration security
     */
    void checkConfigurationSecurity();

    /**
     * @brief Check configuration performance
     */
    void checkConfigurationPerformance();

    /**
     * @brief Generate configuration report
     */
    void generateConfigurationReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create validation visualization
     * @param filename Output file path
     */
    void createValidationVisualization(const std::string& filename);

    /**
     * @brief Create comparison visualization
     * @param filename Output file path
     */
    void createComparisonVisualization(const std::string& filename);

    /**
     * @brief Calculate configuration score
     * @param config Configuration
     * @return Configuration score
     */
    double calculateConfigurationScore(const Config& config) const;

    /**
     * @brief Calculate validation score
     * @param issues Configuration issues
     * @return Validation score
     */
    double calculateValidationScore(const std::vector<ConfigurationIssue>& issues) const;

    /**
     * @brief Calculate optimization score
     * @param config Configuration
     * @return Optimization score
     */
    double calculateOptimizationScore(const Config& config) const;

    /**
     * @brief Generate error report
     * @param issues Configuration issues
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(const std::vector<ConfigurationIssue>& issues) const;

    /**
     * @brief Generate warning report
     * @param issues Configuration issues
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(const std::vector<ConfigurationIssue>& issues) const;

    /**
     * @brief Generate suggestion report
     * @param issues Configuration issues
     * @return Suggestion report
     */
    std::vector<std::string> generateSuggestionReport(const std::vector<ConfigurationIssue>& issues) const;

    /**
     * @brief Generate conflict report
     * @param conflicts Configuration conflicts
     * @return Conflict report
     */
    std::vector<std::string> generateConflictReport(const std::vector<std::pair<std::string, std::string>>& conflicts) const;

    /**
     * @brief Generate dependency report
     * @param dependencies Missing dependencies
     * @return Dependency report
     */
    std::vector<std::string> generateDependencyReport(const std::vector<std::string>& dependencies) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // CONFIGURATIONDEBUGGER_HPP
