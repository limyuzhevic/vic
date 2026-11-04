#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <chrono>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;
class Brain;

// Enhanced configuration validation
struct ValidationResult {
    enum class Status {
        Valid,
        Warning,
        Invalid,
        Critical
    };
    
    Status status;
    std::vector<std::string> messages;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> details;
    double confidence;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationResult() : status(Status::Valid), confidence(1.0), timestamp(std::chrono::system_clock::now()) {}
    
    bool isValid() const { return status == Status::Valid || status == Status::Warning; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    std::string getSummary() const;
    std::string getFullReport() const;
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string warningMessage;
    bool isCritical;
    
    ValidationRule() : isCritical(false) {}
    ValidationRule(const std::string& k, const std::function<bool(const ConfigValue&)>& v, 
                   const std::string& err, const std::string& warn = "", bool critical = false)
        : key(k), validator(v), errorMessage(err), warningMessage(warn), isCritical(critical) {}
};

// Enhanced configuration class with validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format) with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with backup
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Enhanced configuration methods
    
    // Validate entire configuration
    ValidationResult validate() const;
    
    // Validate specific section (e.g., "brain", "simulation", "memory")
    ValidationResult validateSection(const std::string& section) const;
    
    // Validate with custom rules
    ValidationResult validateWithRules(const std::vector<ValidationRule>& rules) const;
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule);
    void addDefaultValidationRules();
    
    // Configuration health check
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        size_t totalAccesses;
        size_t cacheHits;
        double averageAccessTime;
        std::chrono::system_clock::time_point lastAccessed;
        size_t memoryUsage;
        
        PerformanceMetrics() : totalAccesses(0), cacheHits(0), averageAccessTime(0.0), 
                               lastAccessed(std::chrono::system_clock::now()), memoryUsage(0) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Configuration diffing and merging
    std::string diff(const Config& other) const;
    bool merge(const Config& other, bool overwrite = true);
    
    // Configuration backup and recovery
    bool createBackup(const std::string& backupPath) const;
    bool restoreFromBackup(const std::string& backupPath);
    std::vector<std::string> getBackupFiles() const;
    
    // Configuration evolution tracking
    void trackConfigurationChange(const std::string& changeDescription);
    std::vector<std::string> getConfigurationHistory() const;
    
    // Configuration compression and optimization
    bool compress();
    bool optimize();
    
    // Configuration validation for specific systems
    ValidationResult validateBrainConfiguration() const;
    ValidationResult validateSimulationConfiguration() const;
    ValidationResult validateMemoryConfiguration() const;
    ValidationResult validateNeuromodulationConfiguration() const;
    
    // Configuration recommendations
    std::vector<std::string> getRecommendations() const;
    
    // Configuration export/import
    std::string exportConfiguration() const;
    bool importConfiguration(const std::string& configString);
    
    // Configuration integrity check
    bool checkIntegrity() const;
    std::string calculateChecksum() const;
    
    // Configuration recovery helper
    bool recoverFromCorruption(const std::string& corruptionPath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    static bool validateString(const ConfigValue& value);
    static bool validateNumber(const ConfigValue& value);
    static bool validateBoolean(const ConfigValue& value);
    static bool validateVector(const ConfigValue& value);
    static bool validatePositive(const ConfigValue& value, double min = 0.0);
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateFilePath(const ConfigValue& value, bool forWriting = false);
    
    // Internal validation helpers
    static std::vector<std::string> getSystemValidationErrors(const Config& config);
    static std::vector<std::string> getParameterValidationWarnings(const Config& config);
    static std::string generateConfigurationSummary(const Config& config);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
};

// Configuration comparison utilities
namespace ConfigValidation {
    
    // Compare two configurations
    struct ComparisonResult {
        bool isIdentical;
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
        double similarityScore;
        
        ComparisonResult() : isIdentical(false), similarityScore(0.0) {}
    };
    
    ComparisonResult compare(const Config& config1, const Config& config2);
    
    // Validation for specific system types
    ValidationResult validateBrainConfig(const Config& config);
    ValidationResult validateSimulationConfig(const Config& config);
    ValidationResult validateMemoryConfig(const Config& config);
    ValidationResult validateNeuromodulationConfig(const Config& config);
    
    // Configuration fixers
    Config fixConfiguration(const Config& config);
    std::vector<std::string> getConfigurationFixes(const Config& config);
    
    // Configuration scenarios
    Config createScenarioConfig(const std::string& scenario);
    std::vector<std::string> getAvailableScenarios();
}

// Diagnostic utilities
namespace Diagnostic {
    
    // Brain diagnostics
    struct BrainDiagnostics {
        std::string brainHealth;
        std::vector<std::string> issues;
        std::vector<std::string> warnings;
        double performanceScore;
        std::chrono::system_clock::time_point diagnosisTime;
        
        BrainDiagnostics() : performanceScore(0.0), diagnosisTime(std::chrono::system_clock::now()) {}
    };
    
    BrainDiagnostics diagnoseBrain(const Brain& brain);
    std::string getBrainHealthReport(const Brain& brain);
    std::vector<std::string> getBrainRecommendations(const Brain& brain);
    
    // System diagnostics
    struct SystemDiagnostics {
        std::string overallHealth;
        std::vector<std::string> systemIssues;
        std::vector<std::string> systemWarnings;
        double systemHealthScore;
        std::chrono::system_clock::time_point lastCheck;
        
        SystemDiagnostics() : systemHealthScore(0.0), lastCheck(std::chrono::system_clock::now()) {}
    };
    
    SystemDiagnostics diagnoseSystem(const Config& config, const Brain& brain);
    std::string getSystemHealthReport(const Config& config, const Brain& brain);
    std::vector<std::string> getSystemRecommendations(const Config& config, const Brain& brain);
    
    // Performance diagnostics
    struct PerformanceDiagnostics {
        double simulationSpeed;
        size_t memoryUsage;
        size_t activeNeurons;
        float firingRate;
        double stabilityScore;
        std::chrono::system_clock::time_point measurementTime;
    };
    
    PerformanceDiagnostics measurePerformance(const Brain& brain);
    std::string getPerformanceReport(const PerformanceDiagnostics& diag);
    
    // Configuration diagnostics
    struct ConfigDiagnostics {
        bool isValid;
        std::vector<std::string> validationErrors;
        std::vector<std::string> validationWarnings;
        size_t totalKeys;
        size_t recommendedKeys;
        std::chrono::system_clock::time_point lastValidated;
    };
    
    ConfigDiagnostics diagnoseConfig(const Config& config);
    std::string getConfigDiagnosticReport(const ConfigDiagnostics& diag);
    
    // Comprehensive system diagnostics
    struct ComprehensiveDiagnostics {
        std::string timestamp;
        std::string systemId;
        ConfigDiagnostics configDiagnostics;
        BrainDiagnostics brainDiagnostics;
        PerformanceDiagnostics performanceDiagnostics;
        std::string overallHealth;
        double overallScore;
        std::vector<std::string> recommendations;
    };
    
    ComprehensiveDiagnostics runComprehensiveDiagnostics(const Config& config, const Brain& brain);
    std::string generateDiagnosticReport(const ComprehensiveDiagnostics& diag);
    std::vector<std::string> generateDiagnosticRecommendations(const ComprehensiveDiagnostics& diag);
    
    // Diagnostic helper functions
    std::string generateId();
    std::string getCurrentTimestamp();
    std::string formatDiagnostics(const std::string& title, const std::vector<std::string>& sections);
}

// Validation exception hierarchy
class ValidationException : public std::runtime_error {
public:
    enum class ErrorCode {
        InvalidValue,
        MissingRequired,
        OutOfRange,
        FileNotFound,
        ParseError,
        IntegrityError
    };
    
    ValidationException(const std::string& message, ErrorCode code = ErrorCode::InvalidValue)
        : std::runtime_error(message), code_(code) {}
    
    ErrorCode getCode() const { return code_; }
    std::string getCodeString() const;
    
private:
    ErrorCode code_;
};

class ConfigurationException : public ValidationException {
public:
    ConfigurationException(const std::string& message, ErrorCode code = ErrorCode::IntegrityError)
        : ValidationException(message, code) {}
};

class ValidationRuleException : public ValidationException {
public:
    ValidationRuleException(const std::string& message, ErrorCode code = ErrorCode::InvalidValue)
        : ValidationException(message, code) {}
};

// Configuration validation helpers
namespace ConfigHelpers {
    
    // Validation helpers
    bool isValidNeuronCount(int64_t count);
    bool isValidStepCount(int64_t steps);
    bool isValidTimestep(double timestep);
    bool isValidConnectionProbability(float prob);
    bool isValidRandomSeed(int64_t seed);
    
    // Configuration fixers
    int64_t getSafeNeuronCount(int64_t count);
    int64_t getSafeStepCount(int64_t steps);
    double getSafeTimestep(double timestep);
    float getSafeConnectionProbability(float prob);
    
    // Configuration validators
    bool validateBrainConfig(const Config& config);
    bool validateSimulationConfig(const Config& config);
    bool validateMemoryConfig(const Config& config);
    bool validateNeuromodulationConfig(const Config& config);
    
    // Configuration scenarios
    std::vector<std::string> getAvailableScenarios();
    Config getScenarioConfig(const std::string& scenario);
    
    // Configuration recommendations
    std::vector<std::string> getConfigRecommendations(const Config& config);
    std::string suggestConfigChanges(const Config& config);
}

// Configuration utilities
namespace ConfigUtils {
    
    // Configuration management
    std::string backupConfig(const Config& config, const std::string& path);
    Config loadConfigFromBackup(const std::string& path);
    std::vector<std::string> listBackups(const std::string& directory = ".");
    
    // Configuration comparison
    std::string diffConfigs(const Config& config1, const Config& config2);
    bool mergeConfigs(Config& config1, const Config& config2, bool overwrite = true);
    
    // Configuration optimization
    Config optimizeConfig(const Config& config);
    std::vector<std::string> getOptimizationSuggestions(const Config& config);
    
    // Configuration analysis
    std::string analyzeConfig(const Config& config);
    std::vector<std::string> getConfigAnalysis(const Config& config);
    
    // Configuration validation
    bool validateConfigFile(const std::string& filepath);
    std::vector<std::string> validateConfig(const Config& config);
    ValidationResult validateConfigWithRules(const Config& config, const std::vector<ValidationRule>& rules);
    
    // Configuration utilities
    std::string generateConfigId();
    std::string getConfigTimestamp();
    std::string formatConfig(const Config& config);
    Config parseConfigString(const std::string& configString);
    
    // Configuration recovery
    bool recoverConfig(const Config& config, const std::string& corruptionPath);
    std::vector<std::string> getRecoveryOptions(const Config& config);
    
    // Configuration performance
    struct ConfigPerformance {
        double loadTime;
        double saveTime;
        size_t memoryUsage;
        size_t diskUsage;
        std::chrono::system_clock::time_point lastAccess;
    };
    
    ConfigPerformance measureConfigPerformance(const Config& config);
    std::string formatPerformanceReport(const ConfigPerformance& perf);
    
    // Configuration monitoring
    void trackConfigAccess(const Config& config);
    size_t getConfigAccessCount(const Config& config);
    double getConfigAccessRate(const Config& config);
}

} // namespace nlm
