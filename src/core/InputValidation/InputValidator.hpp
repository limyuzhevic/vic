// InputValidator.hpp - Input validation utilities for NLM
// This file defines comprehensive input validation utilities with error handling

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "Error.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string path;
    std::map<std::string, std::string> metadata;
    
    ValidationResult() : valid(true), errorMessage(""), path("") {}
    ValidationResult(bool v, const std::string& error = "", const std::string& p = "")
        : valid(v), errorMessage(error), path(p) {}
    
    ValidationResult(const ValidationResult&) = default;
    ValidationResult& operator=(const ValidationResult&) = default;
    ValidationResult(ValidationResult&&) = default;
    ValidationResult& operator=(ValidationResult&&) = default;
};

// Input validation utilities
class InputValidator {
public:
    // Basic type validation
    static ValidationResult validateString(const std::string& key, const std::string& value, 
                                           const std::string& pattern = "");
    static ValidationResult validateInt(const std::string& key, int64_t value, 
                                        int64_t min = std::numeric_limits<int64_t>::lowest(),
                                        int64_t max = std::numeric_limits<int64_t>::max());
    static ValidationResult validateFloat(const std::string& key, float value,
                                          float min = -std::numeric_limits<float>::max(),
                                          float max = std::numeric_limits<float>::max());
    static ValidationResult validateDouble(const std::string& key, double value,
                                           double min = -std::numeric_limits<double>::max(),
                                           double max = std::numeric_limits<double>::max());
    static ValidationResult validateBool(const std::string& key, bool value);
    static ValidationResult validateChar(const std::string& key, char value);
    
    // Container validation
    static ValidationResult validateStringVector(const std::string& key, 
                                                 const std::vector<std::string>& values,
                                                 size_t maxSize = 1000);
    static ValidationResult validateIntVector(const std::string& key,
                                              const std::vector<int64_t>& values,
                                              size_t maxSize = 1000);
    static ValidationResult validateFloatVector(const std::string& key,
                                                const std::vector<float>& values,
                                                size_t maxSize = 1000);
    
    // Complex type validation
    static ValidationResult validateCoordinates(const std::string& key, float x, float y,
                                                 float maxX = 1000.0f, float maxY = 1000.0f);
    static ValidationResult validateRange(const std::string& key, float value,
                                          float min, float max);
    static ValidationResult validatePattern(const std::string& key, const std::string& value,
                                            const std::string& pattern);
    static ValidationResult validateChoice(const std::string& key, const std::string& value,
                                           const std::vector<std::string>& choices);
    
    // Special validation for neural system
    static ValidationResult validateNeuronCount(const std::string& key, size_t count);
    static ValidationResult validateRegionCount(const std::string& key, size_t count);
    static ValidationResult validateConnectionProbability(const std::string& key, float prob);
    static ValidationResult validateTimestep(const std::string& key, float timestep);
    static ValidationResult validateMemoryCapacity(const std::string& key, size_t capacity);
    static ValidationResult validateDevelopmentStage(const std::string& key, 
                                                      const std::string& stageStr);
    
    // File and path validation
    static ValidationResult validateFilePath(const std::string& key, const std::string& filepath,
                                             bool mustExist = false);
    static ValidationResult validateDirectoryPath(const std::string& key, const std::string& dirpath,
                                                  bool mustExist = false);
    static ValidationResult validateURL(const std::string& key, const std::string& url);
    
    // Network and connection validation
    static ValidationResult validatePort(const std::string& key, int port);
    static ValidationResult validateHost(const std::string& key, const std::string& host);
    static ValidationResult validateProtocol(const std::string& key, const std::string& protocol);
    
    // State and system validation
    static ValidationResult validateSystemState(const std::string& key, 
                                                 const std::map<std::string, std::string>& state);
    static ValidationResult validateConfiguration(const std::string& key,
                                                  const std::map<std::string, std::string>& config);
    static ValidationResult validateSimulationParameters(const std::string& key,
                                                        double timestep, size_t steps);
    
    // Safety and constraint validation
    static ValidationResult validateMemoryLimits(const std::string& key, size_t minMemory, size_t maxMemory);
    static ValidationResult validateCPUConstraints(const std::string& key, float cpuLimit);
    static ValidationResult validateNetworkConstraints(const std::string& key, size_t bandwidth);
    static ValidationResult validateTimeConstraints(const std::string& key, double maxTime);
    
    // Utility validation
    static ValidationResult validateNotEmpty(const std::string& key, const std::string& value);
    static ValidationResult validateLength(const std::string& key, const std::string& value,
                                           size_t minLength, size_t maxLength);
    static ValidationResult validateFormat(const std::string& key, const std::string& value,
                                           const std::string& format);
    static ValidationResult validateSemantic(const std::string& key, const std::string& value);
    
    // Composite validation
    static ValidationResult validateAll(const std::string& key,
                                        const std::map<std::string, std::string>& values,
                                        const std::vector<std::function<ValidationResult(const std::string&, const std::string&)>>& validators);
    
    // Error recovery
    static ValidationResult attemptRecovery(const std::string& operation,
                                             const std::map<std::string, std::string>& context,
                                             float maxRetries = 3.0f);
    
    // Validation statistics
    static struct ValidationStats {
        uint64_t totalValidations;
        uint64_t successfulValidations;
        uint64_t failedValidations;
        std::map<std::string, uint64_t> failuresByType;
        std::chrono::system_clock::time_point lastValidationTime;
        double averageValidationTime;
        
        ValidationStats() : totalValidations(0), successfulValidations(0), failedValidations(0),
                           averageValidationTime(0.0) {
            lastValidationTime = std::chrono::system_clock::now();
        }
    } getValidationStats();
    
    // Configuration
    static void configure(const std::map<std::string, std::string>& config);
    static std::map<std::string, std::string> getConfiguration();
    
    // Error handling
    static void setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter);
    static std::shared_ptr<IErrorReporter> getErrorReporter();
    static void setStrictMode(bool strict);
    static bool isStrictMode();
    
    // Utility functions
    static std::string getValidationErrorType(ValidationResult& result);
    static bool isValueWithinTolerance(double value, double expected, double tolerance);
    static std::vector<std::string> getFailedKeys(const std::map<std::string, std::string>& values,
                                                  const std::vector<std::function<ValidationResult(const std::string&, const std::string&)>>& validators);
    static std::map<std::string, std::string> getValidatedValues(const std::map<std::string, std::string>& values,
                                                                 const std::vector<std::function<ValidationResult(const std::string&, const std::string&)>>& validators);
    
    // Statistical validation
    static ValidationResult validateStatisticalDistribution(const std::string& key,
                                                           const std::vector<double>& values,
                                                           double expectedMean,
                                                           double expectedStdDev,
                                                           double confidence = 0.95);
    static ValidationResult validateTimeSeries(const std::string& key,
                                               const std::vector<double>& values,
                                               double maxChangeRate = 0.1,
                                               double minValue = std::numeric_limits<double>::lowest(),
                                               double maxValue = std::numeric_limits<double>::max());
    
    // Neural network validation
    static ValidationResult validateNeuralArchitecture(const std::string& key,
                                                      size_t inputSize,
                                                      size_t outputSize,
                                                      const std::vector<size_t>& hiddenLayers);
    static ValidationResult validateTrainingParameters(const std::string& key,
                                                       double learningRate,
                                                       double momentum,
                                                       size_t batchSize,
                                                       size_t epochs);
    
    // World simulation validation
    static ValidationResult validateWorldBounds(const std::string& key, float width, float height);
    static ValidationResult validateAgentParameters(const std::string& key, float maxSpeed, float turnSpeed);
    static ValidationResult validateEnvironmentParameters(const std::string& key, float energyDecay, float resourceGain);
    
    // Development and plasticity validation
    static ValidationResult validateDevelopmentParameters(const std::string& key,
                                                         double criticalPeriodStart,
                                                         double criticalPeriodEnd,
                                                         double maturationRate);
    static ValidationResult validatePlasticityParameters(const std::string& key,
                                                         float ltpWeight,
                                                         float ltdWeight,
                                                         float stdpTau);
    
    // Memory system validation
    static ValidationResult validateMemorySystem(const std::string& key,
                                                 size_t workingMemoryCapacity,
                                                 size_t episodicMemoryCapacity,
                                                 size_t associativeMemoryCapacity);
    static ValidationResult validateMemoryParameters(const std::string& key,
                                                     double decayRate,
                                                     double consolidationRate,
                                                     double replayInterval);
    
    // Prediction and cognition validation
    static ValidationResult validatePredictionParameters(const std::string& key,
                                                        double predictionHorizon,
                                                        double predictionAccuracy);
    static ValidationResult validateCognitionParameters(const std::string& key,
                                                       size_t planningDepth,
                                                       float attentionStrength);
    
    // Neuromodulation validation
    static ValidationResult validateNeuromodulationParameters(const std::string& key,
                                                              double dopamineLevel,
                                                              double curiosityLevel,
                                                              double noveltyLevel);
    static ValidationResult validateRewardParameters(const std::string& key,
                                                     double rewardMagnitude,
                                                     double rewardDecay);
    
    // Checkpoint and persistence validation
    static ValidationResult validateCheckpointParameters(const std::string& key,
                                                        const std::string& filepath,
                                                        size_t maxCheckpoints,
                                                        double compressionLevel);
    static ValidationResult validatePersistenceParameters(const std::string& key,
                                                          const std::string& backupPath,
                                                          bool enableBackup,
                                                          size_t backupInterval);
    
    // Test helper functions
    static ValidationResult createTestValidation(const std::string& key, const std::string& value,
                                                 bool expectedValid = true);
    static bool runValidationSuite(const std::string& suiteName);
    static void resetValidationStats();
    
    // Validation reporting
    static std::string getValidationReport();
    static std::string getValidationSummary();
    static std::string getValidationDetails();
    static std::string getValidationHistory();
    
    // Real-time validation monitoring
    static bool isValidationHealthy();
    static std::string getValidationHealthReport();
    static uint64_t getCurrentValidationCount();
    static double getCurrentValidationFailureRate();
};

// Validation context for tracking validation state
struct ValidationContext {
    std::string operation;
    std::map<std::string, std::string> parameters;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
    
    ValidationContext() : success(false) {
        startTime = std::chrono::system_clock::now();
    }
    
    ValidationContext(const std::string& op) : operation(op), success(false) {
        startTime = std::chrono::system_clock::now();
    }
    
    void complete(bool success, const std::string& error = "") {
        this->success = success;
        errorMessage = error;
        endTime = std::chrono::system_clock::now();
    }
    
    double getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
};

// Validation manager for coordinating validation across components
class ValidationManager {
public:
    static ValidationManager& getInstance() {
        static ValidationManager instance;
        return instance;
    }
    
    // Register validators
    void registerValidator(const std::string& key, std::function<ValidationResult(const std::string&, const std::string&)> validator);
    void unregisterValidator(const std::string& key);
    std::vector<std::string> getRegisteredValidators() const;
    
    // Run validations
    ValidationResult validate(const std::string& key, const std::string& value);
    std::map<std::string, ValidationResult> validateAll(const std::map<std::string, std::string>& values);
    
    // Get validation statistics
    ValidationStats getStats() const;
    uint64_t getSuccessCount() const;
    uint64_t getFailureCount() const;
    double getSuccessRate() const;
    
    // Error handling
    void setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter);
    std::shared_ptr<IErrorReporter> getErrorReporter() const;
    
    // Configuration
    void configure(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getConfiguration() const;
    
    // Validation history
    void logValidation(const ValidationContext& context);
    std::vector<ValidationContext> getValidationHistory(size_t maxCount = 100) const;
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
    // Testing
    void reset();
    bool runValidationSuite(const std::string& suiteName);
    std::string generateValidationReport() const;
    
    // Real-time monitoring
    uint64_t getCurrentValidationCount() const;
    double getCurrentValidationFailureRate() const;
    std::chrono::system_clock::time_point getLastValidationTime() const;
    
private:
    ValidationManager() = default;
    ~ValidationManager() = default;
    ValidationManager(const ValidationManager&) = delete;
    ValidationManager& operator=(const ValidationManager&) = delete;
    
    std::map<std::string, std::function<ValidationResult(const std::string&, const std::string&)>> validators_;
    ValidationStats stats_;
    std::shared_ptr<IErrorReporter> errorReporter_;
    std::vector<ValidationContext> validationHistory_;
    std::mutex mutex_;
    
    bool isStrictMode_;
    std::map<std::string, std::string> config_;
    
    void updateStats(const ValidationResult& result);
    double calculateSuccessRate() const;
    
    static std::string getErrorMessage(const ValidationResult& result);
};

// Global validation functions
namespace Validation {
    // Initialize validation system
    void initialize();
    
    // Shutdown validation system
    void shutdown();
    
    // Register a validator
    void registerValidator(const std::string& key, std::function<ValidationResult(const std::string&, const std::string&)> validator);
    
    // Run validations
    ValidationResult validate(const std::string& key, const std::string& value);
    std::map<std::string, ValidationResult> validateAll(const std::map<std::string, std::string>& values);
    
    // Get statistics
    ValidationStats getStats();
    std::string getValidationReport();
    std::string getValidationSummary();
    std::string getValidationDetails();
    
    // Health and monitoring
    bool isHealthy();
    std::string getHealthReport();
    
    // Testing
    bool runValidationSuite(const std::string& suiteName);
    void reset();
    
    // Error handling
    void setErrorReporter(std::shared_ptr<IErrorReporter> errorReporter);
    std::shared_ptr<IErrorReporter> getErrorReporter();
    
    // Configuration
    void configure(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getConfiguration();
    
    // Real-time monitoring
    uint64_t getCurrentValidationCount();
    double getCurrentValidationFailureRate();
    std::chrono::system_clock::time_point getLastValidationTime();
}

// Validation macros
#define VALIDATE(key, value, validator) \
    nlm::Validation::validate(key, value)

#define VALIDATE_ALL(values, validators) \
    nlm::Validation::validateAll(values)

#define VALIDATE_STRING(key, value) \
    nlm::InputValidator::validateString(key, value)

#define VALIDATE_INT(key, value, min, max) \
    nlm::InputValidator::validateInt(key, value, min, max)

#define VALIDATE_FLOAT(key, value, min, max) \
    nlm::InputValidator::validateFloat(key, value, min, max)

#define VALIDATE_DOUBLE(key, value, min, max) \
    nlm::InputValidator::validateDouble(key, value, min, max)

#define VALIDATE_BOOL(key, value) \
    nlm::InputValidator::validateBool(key, value)

#define VALIDATE_CHOICE(key, value, choices) \
    nlm::InputValidator::validateChoice(key, value, choices)

#define VALIDATE_RANGE(key, value, min, max) \
    nlm::InputValidator::validateRange(key, value, min, max)

#define VALIDATE_COORDINATES(key, x, y, maxX, maxY) \
    nlm::InputValidator::validateCoordinates(key, x, y, maxX, maxY)

#define VALIDATE_FILEPATH(key, filepath, mustExist) \
    nlm::InputValidator::validateFilePath(key, filepath, mustExist)

#define VALIDATE_DIRECTORY(key, dirpath, mustExist) \
    nlm::InputValidator::validateDirectoryPath(key, dirpath, mustExist)

#define VALIDATE_PORT(key, port) \
    nlm::InputValidator::validatePort(key, port)

#define VALIDATE_CHOICE_VAL(key, value, choices) \
    nlm::InputValidator::validateChoice(key, value, choices)

// Test validation helpers
inline ValidationResult createTestValidation(const std::string& key, const std::string& value, bool expectedValid = true) {
    ValidationResult result = nlm::InputValidator::validateString(key, value);
    if (result.valid != expectedValid) {
        result.valid = false;
        result.errorMessage = "Test validation failed: expected " + std::string(expectedValid ? "valid" : "invalid") + 
                              ", got " + std::string(result.valid ? "valid" : "invalid");
    }
    return result;
}

inline bool runValidationSuite(const std::string& suiteName) {
    return nlm::Validation::runValidationSuite(suiteName);
}

inline void resetValidationStats() {
    nlm::ValidationManager::getInstance().reset();
}

inline std::string getValidationReport() {
    return nlm::Validation::getValidationReport();
}

inline std::string getValidationSummary() {
    return nlm::Validation::getValidationSummary();
}

inline std::string getValidationDetails() {
    return nlm::Validation::getValidationDetails();
}

inline bool isValidationHealthy() {
    return nlm::Validation::isHealthy();
}

inline std::string getValidationHealthReport() {
    return nlm::Validation::getHealthReport();
}

inline uint64_t getCurrentValidationCount() {
    return nlm::Validation::getCurrentValidationCount();
}

inline double getCurrentValidationFailureRate() {
    return nlm::Validation::getCurrentValidationFailureRate();
}

} // namespace Validation

// Forward declarations for circular dependencies
class ErrorAwareLogger;
class ValidationAwareLogger;

} // namespace nlm

// Helper function for creating validation from error
inline nlm::ValidationResult createValidationFromError(const nlm::NLMError& error) {
    nlm::ValidationResult result(false, error.what(), error.getContext());
    return result;
}

// Validation utility functions
inline bool validateAll(const std::map<std::string, std::string>& values,
                        const std::vector<std::function<nlm::ValidationResult(const std::string&, const std::string&)>>& validators) {
    for (const auto& validator : validators) {
        for (const auto& pair : values) {
            nlm::ValidationResult result = validator(pair.first, pair.second);
            if (!result.valid) {
                return false;
            }
        }
    }
    return true;
}

inline std::vector<std::string> getFailedKeys(const std::map<std::string, std::string>& values,
                                              const std::vector<std::function<nlm::ValidationResult(const std::string&, const std::string&)>>& validators) {
    std::vector<std::string> failedKeys;
    for (const auto& validator : validators) {
        for (const auto& pair : values) {
            nlm::ValidationResult result = validator(pair.first, pair.second);
            if (!result.valid) {
                failedKeys.push_back(pair.first);
                break;
            }
        }
    }
    return failedKeys;
}

inline std::map<std::string, std::string> getValidatedValues(const std::map<std::string, std::string>& values,
                                                             const std::vector<std::function<nlm::ValidationResult(const std::string&, const std::string&)>>& validators) {
    std::map<std::string, std::string> validValues;
    for (const auto& validator : validators) {
        for (const auto& pair : values) {
            nlm::ValidationResult result = validator(pair.first, pair.second);
            if (result.valid) {
                validValues[pair.first] = pair.second;
            }
        }
    }
    return validValues;
}