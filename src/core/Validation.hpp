#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <cassert>
#include <regex>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// === NLM ERROR HIERARCHY ===

// Base NLM exception
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message, const std::string& context = "")
        : std::runtime_error(formatMessage(message, context)) {}
    
    virtual ~NLMException() = default;
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
protected:
    static std::string formatMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        oss << "NLM Error";
        if (!context.empty()) {
            oss << " (context: " << context << ")";
        }
        oss << ": " << message;
        return oss.str();
    }
};

// Configuration-related exceptions
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~ConfigurationException() = default;
};

// Brain-related exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~BrainException() = default;
};

// Memory-related exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~MemoryException() = default;
};

// Plasticity-related exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~PlasticityException() = default;
};

// Neuromodulation-related exceptions
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~NeuromodulationException() = default;
};

// Development-related exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~DevelopmentException() = default;
};

// World/Environment-related exceptions
class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~WorldException() = default;
};

// Experiment-related exceptions
class ExperimentException : public NLMException {
public:
    explicit ExperimentException(const std::string& message, const std::string& context = "")
        : NLMException(message, context) {}
    
    virtual ~ExperimentException() = default;
};

// === VALIDATION FUNCTIONS ===

// Configuration validation
class ConfigurationValidator {
public:
    static bool isValidKey(const std::string& key) {
        if (key.empty() || key.length() > 255) {
            NLM_LOG_ERROR("Invalid key length or empty: '" + key + "'");
            return false;
        }
        
        // Check for invalid characters
        static const std::regex invalidKeyChars("[\\s\\.#\\$\\@\\*\\?\\/\\|\\<\\>\\&\\^\\%\\=\\'\\\\]");
        if (std::regex_search(key, invalidKeyChars)) {
            NLM_LOG_ERROR("Key contains invalid characters: '" + key + "'");
            return false;
        }
        
        return true;
    }
    
    static bool isValidString(const std::string& value, size_t maxLength = 1024) {
        if (value.length() > maxLength) {
            NLM_LOG_ERROR("String value too long (max " + std::to_string(maxLength) + ", actual " + std::to_string(value.length()) + ")");
            return false;
        }
        return true;
    }
    
    static bool isValidNumber(double value, double min = -1e9, double max = 1e9) {
        if (std::isnan(value) || std::isinf(value)) {
            NLM_LOG_ERROR("Invalid number: NaN or infinite value");
            return false;
        }
        if (value < min || value > max) {
            NLM_LOG_ERROR("Number out of range [" + std::to_string(min) + ", " + std::to_string(max) + "], value: " + std::to_string(value));
            return false;
        }
        return true;
    }
    
    static bool isValidVector(const std::vector<double>& values, size_t maxSize = 1000) {
        if (values.size() > maxSize) {
            NLM_LOG_ERROR("Vector too large (max " + std::to_string(maxSize) + ", actual " + std::to_string(values.size()) + ")");
            return false;
        }
        for (size_t i = 0; i < values.size(); ++i) {
            if (!isValidNumber(values[i])) {
                NLM_LOG_ERROR("Invalid value at index " + std::to_string(i) + ": " + std::to_string(values[i]));
                return false;
            }
        }
        return true;
    }
    
    static bool validateConfigFile(const std::string& filepath) {
        if (filepath.empty()) {
            NLM_LOG_ERROR("Empty config filepath");
            return false;
        }
        
        // Check file extension for format detection
        std::string extension = getFileExtension(filepath);
        if (extension != "json" && extension != "yaml" && extension != "yml" && 
            extension != "cfg" && extension != "txt") {
            NLM_LOG_WARNING("Unknown config file extension: '" + extension + "' - may not be parsed correctly");
        }
        
        return true;
    }
    
private:
    static std::string getFileExtension(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            return filepath.substr(dotPos + 1);
        }
        return "";
    }
};

// Brain validation
class BrainValidator {
public:
    static bool isValidNeuronCount(size_t count, size_t min = 1, size_t max = 1000000) {
        if (count < min || count > max) {
            NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(count) + ". Must be between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    static bool isValidSynapseProbability(float prob) {
        if (prob < 0.0f || prob > 1.0f) {
            NLM_LOG_ERROR("Invalid synapse probability: " + std::to_string(prob) + ". Must be between 0.0 and 1.0");
            return false;
        }
        return true;
    }
    
    static bool isValidTimestep(double timestep, double min = 1e-6, double max = 1.0) {
        if (timestep <= 0.0 || timestep < min || timestep > max) {
            NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep) + ". Must be positive and between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    static bool isValidRegionId(const RegionId& regionId) {
        if (regionId.value == UINT64_MAX) {
            NLM_LOG_ERROR("Invalid region ID: sentinel value");
            return false;
        }
        return true;
    }
    
    static bool isValidNeuronId(const NeuronId& neuronId) {
        if (neuronId.value == UINT64_MAX) {
            NLM_LOG_ERROR("Invalid neuron ID: sentinel value");
            return false;
        }
        return true;
    }
    
    static bool isValidSynapseId(const SynapseId& synapseId) {
        if (synapseId.value == UINT64_MAX) {
            NLM_LOG_ERROR("Invalid synapse ID: sentinel value");
            return false;
        }
        return true;
    }
};

// Memory validation
class MemoryValidator {
public:
    static bool isValidMemoryCapacity(size_t capacity, size_t min = 1, size_t max = 1000000) {
        if (capacity < min || capacity > max) {
            NLM_LOG_ERROR("Invalid memory capacity: " + std::to_string(capacity) + ". Must be between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    static bool isValidNeuronVector(const std::vector<NeuronId>& neurons) {
        for (const auto& neuronId : neurons) {
            if (!BrainValidator::isValidNeuronId(neuronId)) {
                NLM_LOG_ERROR("Invalid neuron ID in vector");
                return false;
            }
        }
        return true;
    }
    
    static bool isValidValueVector(const std::vector<float>& values) {
        for (size_t i = 0; i < values.size(); ++i) {
            if (std::isnan(values[i]) || std::isinf(values[i])) {
                NLM_LOG_ERROR("Invalid value at index " + std::to_string(i) + ": " + std::to_string(values[i]));
                return false;
            }
        }
        return true;
    }
};

// Plasticity validation
class PlasticityValidator {
public:
    static bool isValidLearningRate(float rate) {
        if (rate < 0.0f || rate > 1.0f) {
            NLM_LOG_ERROR("Invalid learning rate: " + std::to_string(rate) + ". Must be between 0.0 and 1.0");
            return false;
        }
        return true;
    }
    
    static bool isValidWeight(float weight, float min = -1.0f, float max = 1.0f) {
        if (weight < min || weight > max) {
            NLM_LOG_ERROR("Invalid synaptic weight: " + std::to_string(weight) + ". Must be between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    static bool validatePlasticityFlags(const PlasticityFlags& flags) {
        if (flags.hebbian && flags.stdp && flags.reward_modulated) {
            NLM_LOG_WARNING("Multiple plasticity mechanisms enabled simultaneously");
        }
        return true;
    }
};

// Development validation
class DevelopmentValidator {
public:
    static bool isValidDevelopmentalStage(DevelopmentalStage stage) {
        if (static_cast<uint8_t>(stage) > static_cast<uint8_t>(DevelopmentalStage::Aging)) {
            NLM_LOG_ERROR("Invalid developmental stage: " + std::to_string(static_cast<int>(stage)));
            return false;
        }
        return true;
    }
    
    static bool isValidMaturationRate(float rate, float min = 0.0f, float max = 1.0f) {
        if (rate < min || rate > max) {
            NLM_LOG_ERROR("Invalid maturation rate: " + std::to_string(rate) + ". Must be between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
};

// World validation
class WorldValidator {
public:
    static bool isValidPosition(double x, double y, double min = -1000.0, double max = 1000.0) {
        if (x < min || x > max || y < min || y > max) {
            NLM_LOG_ERROR("Invalid position: (" + std::to_string(x) + ", " + std::to_string(y) + ") not in range [" + 
                         std::to_string(min) + ", " + std::to_string(max) + "]");
            return false;
        }
        return true;
    }
    
    static bool isValidDimension(int width, int height, int min = 1, int max = 10000) {
        if (width < min || width > max || height < min || height > max) {
            NLM_LOG_ERROR("Invalid dimension: (" + std::to_string(width) + "x" + std::to_string(height) + "). Must be between " + 
                         std::to_string(min) + "x" + std::to_string(min) + " and " + std::to_string(max) + "x" + std::to_string(max));
            return false;
        }
        return true;
    }
};

// Error handling utilities
class ErrorHandler {
public:
    template<typename T>
    static T handleOperation(const std::string& operationName, std::function<T()> operation,
                            const std::string& context = "") {
        try {
            T result = operation();
            NLM_LOG_INFO("Successfully " + operationName);
            return result;
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Operation failed: " + operationName + ". " + std::string(e.what()));
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Operation failed: " + operationName + ". " + std::string(e.what()));
            throw BrainException("Operation " + operationName + " failed: " + std::string(e.what()), context);
        } catch (...) {
            NLM_LOG_ERROR("Operation failed: " + operationName + ". Unknown error");
            throw BrainException("Operation " + operationName + " failed with unknown error", context);
        }
    }
    
    static void logValidationError(const std::string& operation, const std::string& error, const std::string& context = "") {
        std::ostringstream oss;
        oss << "Validation failed for " << operation << ": " << error;
        if (!context.empty()) {
            oss << " (context: " << context << ")";
        }
        NLM_LOG_ERROR(oss.str());
    }
    
    static void validateAndThrow(bool condition, const std::string& error, const std::string& context = "") {
        if (!condition) {
            logValidationError("operation", error, context);
            throw BrainException(error, context);
        }
    }
};

// Smart pointer validation
class PointerValidator {
public:
    template<typename T>
    static bool isNotNull(const T* ptr, const std::string& operation) {
        if (ptr == nullptr) {
            NLM_LOG_ERROR("Null pointer encountered in " + operation);
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isNotNull(std::shared_ptr<T> ptr, const std::string& operation) {
        if (!ptr) {
            NLM_LOG_ERROR("Null shared_ptr encountered in " + operation);
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isNotNull(std::unique_ptr<T> ptr, const std::string& operation) {
        if (!ptr) {
            NLM_LOG_ERROR("Null unique_ptr encountered in " + operation);
            return false;
        }
        return true;
    }
};

// Resource management validation
class ResourceValidator {
public:
    static bool isValidResourceLimit(size_t limit, size_t min = 1, size_t max = 1000000) {
        if (limit < min || limit > max) {
            NLM_LOG_ERROR("Invalid resource limit: " + std::to_string(limit) + ". Must be between " + 
                         std::to_string(min) + " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    static bool isValidResourceUsage(size_t usage, size_t limit) {
        if (usage > limit) {
            NLM_LOG_ERROR("Resource usage " + std::to_string(usage) + " exceeds limit " + std::to_string(limit));
            return false;
        }
        return true;
    }
};

// === VALIDATION MACROS ===

// Validate and throw on failure
#define VALIDATE(condition, error_msg) \
    do { \
        if (!(condition)) { \
            nlm::ErrorHandler::logValidationError("validation", std::string(error_msg)); \
            throw nlm::BrainException(std::string(error_msg)); \
        } \
    } while(0)

#define VALIDATE_PTR(ptr, op) VALIDATE(nlm::PointerValidator::isNotNull(ptr, op), "Null pointer in " #op)

#define VALIDATE_PTR_SHARED(ptr, op) VALIDATE(nlm::PointerValidator::isNotNull(ptr, op), "Null shared_ptr in " #op)

#define VALIDATE_PTR_UNIQUE(ptr, op) VALIDATE(nlm::PointerValidator::isNotNull(ptr, op), "Null unique_ptr in " #op)

#define VALIDATE_CONFIG(key, validator) \
    do { \
        if (!(validator(key))) { \
            nlm::ErrorHandler::logValidationError("config", "Invalid configuration key: '" + key + "'"); \
            throw nlm::ConfigurationException("Invalid configuration key: '" + key + "'"); \
        } \
    } while(0)

#define VALIDATE_STATEMENT(statement) \
    do { \
        if (!(statement)) { \
            nlm::ErrorHandler::logValidationError("state", "Statement failed: '" #statement "'"); \
            throw nlm::BrainException("State validation failed: '" #statement "'"); \
        } \
    } while(0)

// === VALIDATION FUNCTION PROTOTYPES ===

// Type-specific validation functions
bool validateNeuronId(const NeuronId& id);
bool validateSynapseId(const SynapseId& id);
bool validateRegionId(const RegionId& id);
bool validatePopulationId(const PopulationId& id);
bool validateNeurons(const std::vector<NeuronId>& neurons);
bool validateValues(const std::vector<float>& values);
bool validateWeights(const std::vector<float>& weights);

// Error handling function prototypes
void logError(const std::string& operation, const std::string& error, const std::string& context = "");
void throwIfError(bool condition, const std::string& error, const std::string& context = "");

// === VALIDATION IMPLEMENTATIONS ===

bool validateNeuronId(const NeuronId& id) {
    return BrainValidator::isValidNeuronId(id);
}

bool validateSynapseId(const SynapseId& id) {
    return BrainValidator::isValidSynapseId(id);
}

bool validateRegionId(const RegionId& id) {
    return BrainValidator::isValidRegionId(id);
}

bool validatePopulationId(const PopulationId& id) {
    if (id.value == UINT64_MAX) {
        NLM_LOG_ERROR("Invalid population ID: sentinel value");
        return false;
    }
    return true;
}

bool validateNeurons(const std::vector<NeuronId>& neurons) {
    return MemoryValidator::isValidNeuronVector(neurons);
}

bool validateValues(const std::vector<float>& values) {
    return MemoryValidator::isValidValueVector(values);
}

bool validateWeights(const std::vector<float>& weights) {
    return PlasticityValidator::isValidWeight(weights[0]);  // Simplified for single weight validation
}

void logError(const std::string& operation, const std::string& error, const std::string& context) {
    ErrorHandler::logValidationError(operation, error, context);
}

void throwIfError(bool condition, const std::string& error, const std::string& context) {
    ErrorHandler::validateAndThrow(condition, error, context);
}

// === VALIDATION ERROR TRACKING ===

// Global validation error tracking
class ValidationErrorTracker {
public:
    static void reset() {
        errorCount = 0;
        warningCount = 0;
        errors.clear();
        warnings.clear();
    }
    
    static void addError(const std::string& error, const std::string& context = "") {
        ++errorCount;
        std::ostringstream oss;
        oss << "Error " << errorCount << ": " << error;
        if (!context.empty()) {
            oss << " (context: " << context << ")";
        }
        errors.push_back(oss.str());
        NLM_LOG_ERROR(oss.str());
    }
    
    static void addWarning(const std::string& warning, const std::string& context = "") {
        ++warningCount;
        std::ostringstream oss;
        oss << "Warning " << warningCount << ": " << warning;
        if (!context.empty()) {
            oss << " (context: " << context << ")";
        }
        warnings.push_back(oss.str());
        NLM_LOG_WARNING(oss.str());
    }
    
    static size_t getErrorCount() { return errorCount; }
    static size_t getWarningCount() { return warningCount; }
    
    static std::vector<std::string> getErrors() { return errors; }
    static std::vector<std::string> getWarnings() { return warnings; }
    
    static bool hasErrors() { return errorCount > 0; }
    static bool hasWarnings() { return warningCount > 0; }
    
    static void report() {
        if (errorCount == 0 && warningCount == 0) {
            NLM_LOG_INFO("Validation completed successfully - 0 errors, 0 warnings");
            return;
        }
        
        NLM_LOG_INFO("Validation completed - " + std::to_string(errorCount) + " errors, " + 
                    std::to_string(warningCount) + " warnings");
        
        if (errorCount > 0) {
            NLM_LOG_ERROR("Errors encountered:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  " + error);
            }
        }
        
        if (warningCount > 0) {
            NLM_LOG_WARNING("Warnings encountered:");
            for (const auto& warning : warnings) {
                NLM_LOG_WARNING("  " + warning);
            }
        }
    }
    
private:
    static size_t errorCount;
    static size_t warningCount;
    static std::vector<std::string> errors;
    static std::vector<std::string> warnings;
};

// Initialize static members
size_t ValidationErrorTracker::errorCount = 0;
size_t ValidationErrorTracker::warningCount = 0;
std::vector<std::string> ValidationErrorTracker::errors;
std::vector<std::string> ValidationErrorTracker::warnings;

// === VALIDATION UTILITIES ===

// Macro for comprehensive error checking
#define NLM_VALIDATE_AND_THROW(condition, error_msg, context) \
    do { \
        if (!(condition)) { \
            nlm::ErrorHandler::validateAndThrow(condition, std::string(error_msg), std::string(context)); \
        } \
    } while(0)

// Macro for validation with error logging
#define NLM_VALIDATE_LOG(condition, error_msg, context) \
    do { \
        if (!(condition)) { \
            nlm::ErrorHandler::logValidationError("validation", std::string(error_msg), std::string(context)); \
        } \
    } while(0)

// Smart validation with automatic error tracking
#define NLM_VALIDATE_TRACKED(condition, error_msg, context) \
    do { \
        if (!(condition)) { \
            nlm::ValidationErrorTracker::addError(std::string(error_msg), std::string(context)); \
        } else { \
            nlm::ValidationErrorTracker::addWarning("Validation passed: " + std::string(error_msg), std::string(context)); \
        } \
    } while(0)

// Comprehensive validation macro
#define NLM_COMPOSITE_VALIDATE(...) \
    do { \
        nlm::ValidationErrorTracker::reset(); \
        __VA_ARGS__; \
        if (nlm::ValidationErrorTracker::hasErrors()) { \
            throw nlm::BrainException("Composite validation failed with " + \
                                     std::to_string(nlm::ValidationErrorTracker::getErrorCount()) + \
                                     " errors"); \
        } \
    } while(0)

// Validation context manager for nested validations
class ValidationContext {
public:
    ValidationContext(const std::string& contextName, bool enableTracking = true) 
        : contextName_(contextName), enabled_(enableTracking) {
        if (enabled_) {
            NLM_LOG_INFO("Entering validation context: " + contextName_);
        }
    }
    
    ~ValidationContext() {
        if (enabled_) {
            NLM_LOG_INFO("Exiting validation context: " + contextName_);
            ValidationErrorTracker::report();
        }
    }
    
    void addError(const std::string& error) {
        if (enabled_) {
            ValidationErrorTracker::addError(error, contextName_);
        }
    }
    
    void addWarning(const std::string& warning) {
        if (enabled_) {
            ValidationErrorTracker::addWarning(warning, contextName_);
        }
    }
    
    bool hasErrors() const {
        return ValidationErrorTracker::hasErrors();
    }
    
    size_t getErrorCount() const {
        return ValidationErrorTracker::getErrorCount();
    }
    
    void reset() {
        ValidationErrorTracker::reset();
    }
    
    void report() {
        ValidationErrorTracker::report();
    }
    
private:
    std::string contextName_;
    bool enabled_;
};

// === USAGE EXAMPLES (as comments) ===

// Example usage:
// 
// #include "Validation.hpp"
// 
// void myFunction() {
//     VALIDATE(config->has("neuron_count"), "Configuration missing neuron_count");
//     
//     VALIDATE_PTR(brain, "initialization");
//     
//     VALIDATE(brain->getTotalNeuronCount() > 0, "Brain must have neurons");
//     
//     VALIDATE_STATEMENT(brain->initialize());
//     
//     // Composite validation
//     NLM_COMPOSITE_VALIDATE(
//         VALIDATE(config->has("learning_rate"), "Missing learning rate"),
//         VALIDATE_PTR(brain, "brain initialization"),
//         VALIDATE(brain->getTotalNeuronCount() > 100, "Not enough neurons"),
//         VALIDATE(config->getOr<float>("learning_rate", 0.01f) > 0.0f, "Learning rate must be positive")
//     );
//     
//     NLM_LOG_INFO("Validation successful");
// }n
} // namespace nlm

// Global validation function for easy use
namespace nlm {
    inline void validateAndThrow(bool condition, const std::string& error, const std::string& context = "") {
        nlm::ErrorHandler::validateAndThrow(condition, error, context);
    }
    
    inline void logValidationError(const std::string& operation, const std::string& error, const std::string& context = "") {
        nlm::ErrorHandler::logValidationError(operation, error, context);
    }
}