#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace nlm {

// Forward declarations
class Error;
class ErrorReporter;
class ValidationEngine;

// Validation statistics
struct ValidationStatistics {
    size_t totalValidations = 0;
    size_t successfulValidations = 0;
    size_t failedValidations = 0;
    size_t recoveryAttempts = 0;
    size_t successfulRecoveries = 0;
    double averageRecoveryTime = 0.0;
    std::chrono::system_clock::time_point lastValidationTime;
    std::map<std::string, size_t> validationCounts;
    std::map<std::string, size_t> errorCounts;
    
    void recordValidation(bool success, const std::string& context = "");
    void recordRecovery(bool success, double recoveryTime);
    double getSuccessRate() const;
};

// Validation rule context
struct ValidationContext {
    std::string name;
    std::string description;
    std::vector<std::string> data;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    
    ValidationContext(const std::string& n, const std::string& desc, const std::vector<std::string>& d)
        : name(n), description(desc), data(d), timestamp(std::chrono::system_clock::now()), source("unknown") {}
};

// Validation result with enhanced information
struct EnhancedValidationResult {
    bool valid;
    std::vector<std::shared_ptr<Error>> errors;
    std::vector<std::shared_ptr<Error>> warnings;
    std::vector<std::shared_ptr<Error>> messages;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    double validationTime;
    bool recoveryApplied;
    
    EnhancedValidationResult() : valid(true), recoveryApplied(false), validationTime(0.0), timestamp(std::chrono::system_clock::now()) {}
    EnhancedValidationResult(bool v) : valid(v), recoveryApplied(false), validationTime(0.0), timestamp(std::chrono::system_clock::now()) {}
    
    void addError(std::shared_ptr<Error> error) { errors.push_back(error); }
    void addWarning(std::shared_ptr<Error> warning) { warnings.push_back(warning); }
    void addMessage(std::shared_ptr<Error> message) { messages.push_back(message); }
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    size_t getWarningCount(ErrorSeverity severity = ErrorSeverity::Warning) const;
};

// Validation configuration
struct ValidationConfig {
    bool enableAutoRecovery = true;
    bool enableDetailedReporting = true;
    bool enablePerformanceMonitoring = true;
    ErrorSeverity minimumSeverity = ErrorSeverity::Warning;
    size_t maxErrorHistory = 1000;
    double recoveryTimeout = 5.0;
    bool threadSafe = true;
    std::string defaultContext = "validation";
    std::map<std::string, std::string> customSettings;
    
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    std::string toString() const;
};

// Thread-safe error queue for asynchronous processing
class ErrorQueue {
public:
    ErrorQueue(size_t maxQueueSize = 1000);
    ~ErrorQueue();
    
    void push(std::shared_ptr<Error> error);
    bool tryPush(std::shared_ptr<Error> error);
    std::shared_ptr<Error> pop();
    bool tryPop(std::shared_ptr<Error>& error);
    size_t size() const;
    bool empty() const;
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Recovery strategy interface
class IRecoveryStrategy {
public:
    virtual ~IRecoveryStrategy() = default;
    virtual bool canRecover(const std::shared_ptr<Error>& error) const = 0;
    virtual bool recover(std::shared_ptr<Error>& error) = 0;
    virtual std::string getName() const = 0;
    virtual ErrorSeverity getRecommendedSeverity() const = 0;
};

// Default recovery strategies
class DefaultRecoveryStrategy : public IRecoveryStrategy {
public:
    bool canRecover(const std::shared_ptr<Error>& error) const override;
    bool recover(std::shared_ptr<Error>& error) override;
    std::string getName() const override { return "DefaultRecovery"; }
    ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Warning; }
};

class MemoryRecoveryStrategy : public IRecoveryStrategy {
public:
    bool canRecover(const std::shared_ptr<Error>& error) const override;
    bool recover(std::shared_ptr<Error>& error) override;
    std::string getName() const override { return "MemoryRecovery"; }
    ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Error; }
};

class FileRecoveryStrategy : public IRecoveryStrategy {
public:
    bool canRecover(const std::shared_ptr<Error>& error) const override;
    bool recover(std::shared_ptr<Error>& error) override;
    std::string getName() const override { return "FileRecovery"; }
    ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Error; }
};

class ConfigurationRecoveryStrategy : public IRecoveryStrategy {
public:
    bool canRecover(const std::shared_ptr<Error>& error) const override;
    bool recover(std::shared_ptr<Error>& error) override;
    std::string getName() const override { return "ConfigurationRecovery"; }
    ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Warning; }
};

// Validation engine with advanced features
class ValidationEngine {
public:
    ValidationEngine();
    ValidationEngine(const ValidationConfig& config);
    ~ValidationEngine();

    // Validation rule management
    void addRule(const ValidationRule& rule);
    void addRule(const std::string& name, const std::string& description,
                 std::function<bool(const std::vector<std::string>&)> validator,
                 ErrorType errorType = ErrorType::ValidationError,
                 ErrorSeverity severity = ErrorSeverity::Warning,
                 bool required = true);

    void removeRule(const std::string& name);
    void clearRules();
    bool hasRule(const std::string& name) const;
    ValidationRule* getRule(const std::string& name);

    // Validation execution
    EnhancedValidationResult validate(const std::vector<std::string>& data, const std::string& context = "");
    EnhancedValidationResult validate(const ValidationContext& context);
    EnhancedValidationResult validateConfiguration(const std::string& key, const std::string& value);
    EnhancedValidationResult validateSystemState();
    EnhancedValidationResult validateNeuralState(const std::string& neuronId = "");
    EnhancedValidationResult validateMemoryState();
    EnhancedValidationResult validatePlasticityState();

    // Error recovery
    bool attemptRecovery(const std::shared_ptr<Error>& error);
    bool attemptRecovery(const std::shared_ptr<Error>& error, const std::string& strategyName);
    bool autoRecover(const EnhancedValidationResult& result);

    // Recovery strategy management
    void addRecoveryStrategy(std::shared_ptr<IRecoveryStrategy> strategy);
    void removeRecoveryStrategy(const std::string& name);
    void clearRecoveryStrategies();
    std::shared_ptr<IRecoveryStrategy> getRecoveryStrategy(const std::string& name);

    // Configuration
    void setConfig(const ValidationConfig& config);
    const ValidationConfig& getConfig() const { return config_; }
    void configure(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;

    // Statistics and monitoring
    const ValidationStatistics& getStatistics() const { return stats_; }
    void resetStatistics();
    std::string getStatisticsReport() const;

    // Error reporting integration
    void setErrorReporter(std::shared_ptr<ErrorReporter> reporter);
    void setErrorLevel(ErrorSeverity level);

    // Thread safety
    void setThreadSafe(bool enabled) { config_.threadSafe = enabled; }
    bool isThreadSafe() const { return config_.threadSafe; }

    // Error history and analytics
    std::vector<std::shared_ptr<Error>> getRecentErrors(size_t count = 100) const;
    std::map<std::string, size_t> getErrorTypeDistribution() const;
    std::map<std::string, size_t> getErrorCategoryDistribution() const;
    std::map<std::string, double> getErrorFrequency() const;

    // Performance optimization
    void optimizeForSpeed();
    void optimizeForAccuracy();
    void enableBatchValidation(bool enabled) { batchValidationEnabled_ = enabled; }
    bool isBatchValidationEnabled() const { return batchValidationEnabled_; }

    // Validation scheduling
    void scheduleValidation(std::function<EnhancedValidationResult()> validationFunc, double intervalSeconds);
    void stopAllScheduledValidations();

    // Export/import validation rules
    bool exportRules(const std::string& filepath) const;
    bool importRules(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    ValidationConfig config_;
    ValidationStatistics stats_;
    std::map<std::string, std::shared_ptr<IRecoveryStrategy>> recoveryStrategies_;
    bool batchValidationEnabled_;

    EnhancedValidationResult validateAgainstRules(const ValidationContext& context);
    EnhancedValidationResult applyRecovery(const EnhancedValidationResult& result);
    void recordValidationResult(const EnhancedValidationResult& result, const std::string& context);
};

// Validation utilities
namespace ValidationUtils {
    // Data type conversion and validation
    bool isNumber(const std::string& str, double* value = nullptr);
    bool isInteger(const std::string& str, long* value = nullptr);
    bool isBoolean(const std::string& str, bool* value = nullptr);
    bool isValidName(const std::string& name);
    bool isValidPath(const std::string& path, bool writeAccess = false);

    // Range validation
    bool isInRange(double value, double min, double max, bool inclusive = true);
    bool isPositive(double value);
    bool isNonNegative(double value);
    bool isLessThan(double value, double max);
    bool isGreaterThan(double value, double min);

    // String validation
    bool hasMinimumLength(const std::string& str, size_t minLength);
    bool hasMaximumLength(const std::string& str, size_t maxLength);
    bool containsOnly(const std::string& str, const std::string& allowedChars);
    bool isValidIdentifier(const std::string& identifier);

    // Configuration validation
    bool isValidConfigKey(const std::string& key);
    bool isValidConfigValue(const std::string& key, const std::string& value);

    // System resource validation
    bool checkMemoryAvailability(size_t minBytes);
    bool checkFileDescriptorAvailability(size_t minDescriptors);
    bool checkNetworkAvailability();

    // Neural validation
    bool isValidNeuronId(const std::string& id);
    bool isValidSynapseId(const std::string& id);
    bool isValidRegionId(const std::string& id);

    // Performance validation
    bool checkPerformanceConstraints(const std::string& operation, double timeLimit);
    bool checkMemoryConstraints(size_t memoryLimit);

    // Validation helpers
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
}

// Validation macros
#define NLM_VALIDATE(config_key, validator_type, severity) \
    do { \
        std::string value = ValidationUtils::getConfigValue(config_key); \
        auto result = ValidationEngine::getGlobal().validateConfiguration(config_key, value); \
        if (!result.valid) { \
            ErrorReporter::getGlobal().report(result.errors[0]); \
        } \
    } while(0)

#define NLM_VALIDATE_BATCH(data, context) \
    do { \
        if (ValidationEngine::getGlobal().isBatchValidationEnabled()) { \
            auto result = ValidationEngine::getGlobal().validate(data, context); \
            if (!result.valid) { \
                ErrorReporter::getGlobal().report(result.errors[0]); \
            } \
        } \
    } while(0)

#define NLM_ATTEMPT_RECOVERY(error) \
    do { \
        if (ValidationEngine::getGlobal().attemptRecovery(error)) { \
            /* Recovery successful */ \
        } \
    } while(0)

// Global validation engine instance
ValidationEngine& getGlobalValidationEngine();
void setGlobalValidationEngine(ValidationEngine& engine);

} // namespace nlm
