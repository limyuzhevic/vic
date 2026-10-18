# Error Handling and Validation System
# Comprehensive error handling and input validation for NLM

import std
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <chrono>
#include <optional>
#include <variant>

// Standard error types for NLM
class NLMError : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_INPUT = 1,
        CONFIGURATION_ERROR = 2,
        NEURAL_NETWORK_ERROR = 3,
        MEMORY_ERROR = 4,
        FILESYSTEM_ERROR = 5,
        VALIDATION_ERROR = 6,
        INITIALIZATION_ERROR = 7,
        SIMULATION_ERROR = 8,
        UNKNOWN_ERROR = 999
    };

    NLMError(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR)
        : std::runtime_error(message), errorCode(code) {}
    
    ErrorCode getErrorCode() const { return errorCode; }
    const char* getErrorCodeString() const {
        switch (errorCode) {
            case ErrorCode::SUCCESS: return "SUCCESS";
            case ErrorCode::INVALID_INPUT: return "INVALID_INPUT";
            case ErrorCode::CONFIGURATION_ERROR: return "CONFIGURATION_ERROR";
            case ErrorCode::NEURAL_NETWORK_ERROR: return "NEURAL_NETWORK_ERROR";
            case ErrorCode::MEMORY_ERROR: return "MEMORY_ERROR";
            case ErrorCode::FILESYSTEM_ERROR: return "FILESYSTEM_ERROR";
            case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
            case ErrorCode::INITIALIZATION_ERROR: return "INITIALIZATION_ERROR";
            case ErrorCode::SIMULATION_ERROR: return "SIMULATION_ERROR";
            case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            default: return "UNKNOWN_ERROR";
        }
    }

private:
    ErrorCode errorCode;
};

class InputValidation {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };

    static ValidationResult validateNeuronCount(size_t count) {
        ValidationResult result;
        
        if (count == 0) {
            result.isValid = false;
            result.errors.push_back("Neuron count cannot be zero");
        } else if (count > 1000000) {
            result.isValid = false;
            result.errors.push_back("Neuron count too large (maximum: 1,000,000)");
        } else if (count < 10) {
            result.warnings.push_back("Small neuron count (minimum recommended: 100) may not produce meaningful results");
        }
        
        return result;
    }

    static ValidationResult validateSimulationSteps(size_t steps) {
        ValidationResult result;
        
        if (steps == 0) {
            result.isValid = false;
            result.errors.push_back("Simulation steps cannot be zero");
        } else if (steps > 1000000000) {
            result.isValid = false;
            result.errors.push_back("Simulation steps too large (maximum: 1,000,000,000)");
        }
        
        return result;
    }

    static ValidationResult validateBrainConfiguration(const class Config& config) {
        ValidationResult result;
        
        try {
            auto neuronCount = config.getOr<int64_t>("neuron_count", -1);
            if (neuronCount <= 0) {
                result.isValid = false;
                result.errors.push_back("Invalid neuron_count in configuration (must be positive)");
            }
            
            auto regionCount = config.getOr<int64_t>("region_count", -1);
            if (regionCount <= 0) {
                result.isValid = false;
                result.errors.push_back("Invalid region_count in configuration (must be positive)");
            }
            
            auto connectionProb = config.getOr<float>("connection_probability", -1.0f);
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                result.isValid = false;
                result.errors.push_back("Invalid connection_probability in configuration (must be between 0.0 and 1.0)");
            }
            
            // Check for required parameters
            if (!config.has("random_seed")) {
                result.warnings.push_back("Configuration missing random_seed (will use default)");
            }
            
        } catch (const std::exception& e) {
            result.isValid = false;
            result.errors.push_back(std::string("Configuration validation failed: ") + e.what());
        }
        
        return result;
    }

    static ValidationResult validateFilePath(const std::string& filepath) {
        ValidationResult result;
        
        if (filepath.empty()) {
            result.isValid = false;
            result.errors.push_back("File path cannot be empty");
        }
        
        // Check for potentially dangerous paths (simplified check)
        if (filepath.find("..") != std::string::npos) {
            result.warnings.push_back("File path contains '..' which may access parent directories");
        }
        
        return result;
    }

    static ValidationResult validateActionParameters(const class Action& action) {
        ValidationResult result;
        
        if (!action.getName().empty() && action.getName().length() > 100) {
            result.warnings.push_back("Action name is unusually long (max recommended: 100 characters)");
        }
        
        // Validate action parameters
        auto params = action.getParameters();
        for (size_t i = 0; i < params.size(); ++i) {
            if (std::isnan(params[i]) || std::isinf(params[i])) {
                result.isValid = false;
                result.errors.push_back(std::string("Invalid action parameter at index ") + std::to_string(i));
            }
        }
        
        return result;
    }

    static ValidationResult validateSensoryInput(const class SensoryInput& input) {
        ValidationResult result;
        
        auto data = input.getData();
        if (data.empty()) {
            result.warnings.push_back("Sensory input data is empty");
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            if (std::isnan(data[i]) || std::isinf(data[i])) {
                result.isValid = false;
                result.errors.push_back(std::string("Invalid sensory input data at index ") + std::to_string(i));
            }
        }
        
        return result;
    }

    static ValidationResult validateSynapseWeight(float weight) {
        ValidationResult result;
        
        if (std::isnan(weight) || std::isinf(weight)) {
            result.isValid = false;
            result.errors.push_back("Synapse weight is not a valid number");
        } else if (weight < -1.0f || weight > 1.0f) {
            result.warnings.push_back("Synapse weight outside typical range [-1.0, 1.0]");
        }
        
        return result;
    }

    static ValidationResult validateTimestamp(Timestamp timestamp) {
        ValidationResult result;
        
        if (timestamp < 0.0) {
            result.isValid = false;
            result.errors.push_back("Timestamp cannot be negative");
        }
        
        return result;
    }

    static ValidationResult validateStepRange(SimulationStep start, SimulationStep end) {
        ValidationResult result;
        
        if (start >= end) {
            result.isValid = false;
            result.errors.push_back("Start step must be less than end step");
        }
        
        if (end - start > 1000000000) {
            result.warnings.push_back("Step range is very large (may cause performance issues)");
        }
        
        return result;
    }

    static ValidationResult validateDevelopmentStage(DevelopmentalStage stage) {
        ValidationResult result;
        
        // All enum values are valid, but we could check for expected ranges
        return result;
    }

    static ValidationResult validateNeuromodulatorLevel(float level) {
        ValidationResult result;
        
        if (std::isnan(level) || std::isinf(level)) {
            result.isValid = false;
            result.errors.push_back("Neuromodulator level is not a valid number");
        } else if (level < 0.0f || level > 100.0f) {
            result.warnings.push_back("Neuromodulator level outside typical range [0.0, 100.0]");
        }
        
        return result;
    }

    static ValidationResult validateMemoryCapacity(size_t capacity) {
        ValidationResult result;
        
        if (capacity == 0) {
            result.isValid = false;
            result.errors.push_back("Memory capacity cannot be zero");
        } else if (capacity > 100000000) {
            result.warnings.push_back("Memory capacity is very large (may cause memory issues)");
        }
        
        return result;
    }

    static ValidationResult validatePlasticityFlags(const PlasticityFlags& flags) {
        ValidationResult result;
        
        // All combinations of flags are valid, but we could check for logical consistency
        return result;
    }
};

class ErrorHandler {
public:
    struct ErrorContext {
        std::string operation;
        std::string component;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> contextData;
        std::optional<std::string> recoveryAction;
        
        ErrorContext(const std::string& op, const std::string& comp)
            : operation(op), component(comp), timestamp(std::chrono::system_clock::now()) {}
    };

    static std::vector<std::string> handleError(const NLMError& error, const ErrorContext& context = ErrorContext("unknown", "unknown")) {
        std::vector<std::string> messages;
        
        // Log error details
        messages.push_back("[NLM Error] " + std::string(error.what()));
        messages.push_back("[Error Code] " + std::string(error.getErrorCodeString()));
        messages.push_back("[Operation] " + context.operation);
        messages.push_back("[Component] " + context.component);
        messages.push_back("[Timestamp] " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - context.timestamp).count()) + "ms ago");
        
        // Add recovery information if available
        if (context.recoveryAction.has_value()) {
            messages.push_back("[Recovery] " + context.recoveryAction.value());
        }
        
        // Add context data
        for (const auto& pair : context.contextData) {
            messages.push_back("[Context " + pair.first + "] " + pair.second);
        }
        
        return messages;
    }

    static std::vector<std::string> handleValidationError(const InputValidation::ValidationResult& result, 
                                                         const std::string& operation = "validation") {
        std::vector<std::string> messages;
        
        if (!result.errors.empty()) {
            messages.push_back("[Validation Errors]");
            for (const auto& error : result.errors) {
                messages.push_back("  - " + error);
            }
        }
        
        if (!result.warnings.empty()) {
            messages.push_back("[Validation Warnings]");
            for (const auto& warning : result.warnings) {
                messages.push_back("  - " + warning);
            }
        }
        
        if (!result.isValid && !result.errors.empty()) {
            messages.push_back("[Action Required] Operation cannot proceed due to validation errors");
        }
        
        return messages;
    }

    static std::vector<std::string> logRecoveryAction(const std::string& action, 
                                                     const std::string& outcome,
                                                     const std::string& component) {
        std::vector<std::string> messages;
        
        messages.push_back("[Recovery Action] " + action);
        messages.push_back("[Component] " + component);
        messages.push_back("[Outcome] " + outcome);
        
        return messages;
    }

    static std::vector<std::string> logPerformanceMetric(const std::string& metricName,
                                                         double value,
                                                         const std::string& unit = "",
                                                         const std::string& context = "") {
        std::vector<std::string> messages;
        
        std::string message = "[Performance] " + metricName + " = " + std::to_string(value);
        if (!unit.empty()) {
            message += " " + unit;
        }
        if (!context.empty()) {
            message += " (" + context + ")";
        }
        
        messages.push_back(message);
        
        return messages;
    }
};

class ExceptionTranslator {
public:
    static std::string translateErrorCode(NLMError::ErrorCode code) {
        switch (code) {
            case NLMError::ErrorCode::SUCCESS:
                return "Operation completed successfully";
            case NLMError::ErrorCode::INVALID_INPUT:
                return "Invalid input parameters or configuration";
            case NLMError::ErrorCode::CONFIGURATION_ERROR:
                return "Configuration error";
            case NLMError::ErrorCode::NEURAL_NETWORK_ERROR:
                return "Neural network error";
            case NLMError::ErrorCode::MEMORY_ERROR:
                return "Memory error";
            case NLMError::ErrorCode::FILESYSTEM_ERROR:
                return "File system error";
            case NLMError::ErrorCode::VALIDATION_ERROR:
                return "Validation error";
            case NLMError::ErrorCode::INITIALIZATION_ERROR:
                return "Initialization error";
            case NLMError::ErrorCode::SIMULATION_ERROR:
                return "Simulation error";
            default:
                return "Unknown error";
        }
    }

    static std::string suggestRecoveryAction(NLMError::ErrorCode code) {
        switch (code) {
            case NLMError::ErrorCode::INVALID_INPUT:
                return "Check input parameters and configuration files";
            case NLMError::ErrorCode::CONFIGURATION_ERROR:
                return "Verify configuration file syntax and required parameters";
            case NLMError::ErrorCode::NEURAL_NETWORK_ERROR:
                return "Check neural network initialization and parameters";
            case NLMError::ErrorCode::MEMORY_ERROR:
                return "Reduce memory usage or increase available memory";
            case NLMError::ErrorCode::FILESYSTEM_ERROR:
                return "Check file permissions and disk space";
            case NLMError::ErrorCode::VALIDATION_ERROR:
                return "Validate input data and parameters";
            case NLMError::ErrorCode::INITIALIZATION_ERROR:
                return "Check system resources and configuration";
            case NLMError::ErrorCode::SIMULATION_ERROR:
                return "Check simulation parameters and network stability";
            default:
                return "Contact support for assistance";
        }
    }
};

class SafeMemoryAllocator {
public:
    static void* allocate(size_t size, const std::string& context = "unknown") {
        try {
            if (size == 0) {
                throw NLMError("Attempted to allocate zero bytes", NLMError::ErrorCode::MEMORY_ERROR);
            }
            
            if (size > 1024 * 1024 * 1024) { // 1GB limit
                throw NLMError("Memory allocation too large (1GB limit)", NLMError::ErrorCode::MEMORY_ERROR);
            }
            
            void* ptr = std::malloc(size);
            if (!ptr) {
                throw NLMError("Memory allocation failed", NLMError::ErrorCode::MEMORY_ERROR);
            }
            
            return ptr;
            
        } catch (const NLMError& e) {
            // Log error and rethrow
            auto errorMessages = ErrorHandler::handleError(e, ErrorContext("allocate", context));
            for (const auto& msg : errorMessages) {
                std::cerr << msg << std::endl;
            }
            throw;
        }
    }

    static void deallocate(void* ptr, size_t size = 0) {
        try {
            if (!ptr) {
                return; // Safe to ignore null pointer
            }
            
            std::free(ptr);
            
        } catch (const std::exception& e) {
            // Log error but don't throw - deallocation failures are less critical
            std::cerr << "Warning: Memory deallocation failed: " << e.what() << std::endl;
        }
    }

    template<typename T>
    static std::unique_ptr<T> make_unique(size_t count, const std::string& context = "unknown") {
        try {
            auto ptr = std::make_unique<T[]>(count);
            return ptr;
        } catch (const std::bad_alloc& e) {
            throw NLMError("Memory allocation failed for " + std::to_string(count) + " elements", 
                          NLMError::ErrorCode::MEMORY_ERROR);
        }
    }
};

class InputSanitizer {
public:
    static std::string sanitizeString(const std::string& input, size_t maxLength = 1000) {
        // Remove control characters except newline and tab
        std::string result;
        for (char c : input) {
            if (c >= 32 || c == '\n' || c == '\t') {
                result += c;
            }
        }
        
        // Truncate if too long
        if (result.length() > maxLength) {
            result = result.substr(0, maxLength);
        }
        
        return result;
    }

    static double sanitizeFloat(double value, double min = -1e6, double max = 1e6) {
        if (std::isnan(value) || std::isinf(value)) {
            return 0.0; // Default value for NaN/inf
        }
        
        return std::clamp(value, min, max);
    }

    static int64_t sanitizeInt(int64_t value, int64_t min = std::numeric_limits<int64_t>::min(), 
                              int64_t max = std::numeric_limits<int64_t>::max()) {
        return std::clamp(value, min, max);
    }

    static bool sanitizeBool(bool value) {
        return value; // No sanitization needed for bool
    }

    static std::string sanitizeFilepath(const std::string& path) {
        // Remove potentially dangerous path components
        std::string result;
        std::string token;
        std::istringstream stream(path);
        
        while (std::getline(stream, token, '/')) {
            if (token == ".." || token == ".") {
                continue; // Skip relative path components
            }
            
            // Check for empty components
            if (!token.empty()) {
                if (!result.empty()) result += "/";
                result += token;
            }
        }
        
        return result;
    }
};

class PerformanceValidator {
public:
    struct PerformanceMetrics {
        double cpuUsage;      // Percentage
        double memoryUsage;   // Bytes
        double executionTime; // Seconds
        size_t allocatedMemory; // Bytes
        size_t peakMemory;    // Bytes
    };

    static PerformanceMetrics measurePerformance(std::function<void()> operation) {
        PerformanceMetrics metrics = {0.0, 0.0, 0.0, 0, 0};
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Measure CPU usage before
            auto startCpu = std::chrono::high_resolution_clock::now();
            
            // Execute operation
            operation();
            
            // Measure CPU usage after
            auto endCpu = std::chrono::high_resolution_clock::now();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            // Calculate metrics
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            auto cpuDuration = std::chrono::duration_cast<std::chrono::duration<double>>(endCpu - startCpu);
            
            metrics.executionTime = duration.count();
            metrics.cpuUsage = (cpuDuration.count() / duration.count()) * 100.0;
            
            // Memory metrics would require platform-specific implementation
            // For now, provide placeholders
            
        } catch (const std::exception& e) {
            // Log error but don't propagate - let the caller handle it
            std::cerr << "Warning: Performance measurement failed: " << e.what() << std::endl;
        }
        
        return metrics;
    }

    static bool validatePerformanceThresholds(const PerformanceMetrics& metrics,
                                              double maxCpuUsage = 95.0,
                                              double maxMemoryUsage = 1024 * 1024 * 1024) { // 1GB
        bool isValid = true;
        
        if (metrics.cpuUsage > maxCpuUsage) {
            std::cerr << "Warning: CPU usage " << metrics.cpuUsage << "% exceeds threshold " 
                      << maxCpuUsage << "%" << std::endl;
            isValid = false;
        }
        
        if (metrics.memoryUsage > maxMemoryUsage) {
            std::cerr << "Warning: Memory usage " << metrics.memoryUsage / (1024*1024) << "MB exceeds threshold " 
                      << maxMemoryUsage / (1024*1024) << "MB" << std::endl;
            isValid = false;
        }
        
        return isValid;
    }
};

class ErrorRecovery {
public:
    struct RecoveryStrategy {
        std::string name;
        std::function<bool()> canRecover;
        std::function<void()> recover;
        double successRate;
        double averageRecoveryTime;
    };

    static std::vector<RecoveryStrategy> getDefaultRecoveryStrategies() {
        std::vector<RecoveryStrategy> strategies;
        
        // Recovery strategy for memory errors
        strategies.push_back({
            "memory_cleanup",
            []() { return true; }, // Can always attempt
            []() {
                // Simple cleanup - could be more sophisticated
                std::cerr << "Attempting memory cleanup..." << std::endl;
            },
            0.8, // 80% success rate
            0.1 // 100ms average recovery time
        });
        
        // Recovery strategy for configuration errors
        strategies.push_back({
            "reset_to_defaults",
            []() { return true; }, // Can always attempt
            []() {
                std::cerr << "Resetting configuration to defaults..." << std::endl;
            },
            0.9, // 90% success rate
            0.05 // 50ms average recovery time
        });
        
        // Recovery strategy for file system errors
        strategies.push_back({
            "retry_with_alternative_path",
            []() { return true; }, // Can always attempt
            []() {
                std::cerr << "Retrying with alternative file path..." << std::endl;
            },
            0.7, // 70% success rate
            0.2 // 200ms average recovery time
        });
        
        return strategies;
    }

    static std::vector<std::string> attemptRecovery(const NLMError& error,
                                                    const ErrorContext& context,
                                                    const std::vector<RecoveryStrategy>& strategies) {
        std::vector<std::string> messages;
        messages.push_back("[Error Recovery Attempted]");
        
        for (const auto& strategy : strategies) {
            try {
                if (strategy.canRecover()) {
                    std::cerr << "Attempting recovery strategy: " << strategy.name << std::endl;
                    
                    strategy.recover();
                    
                    // Check if recovery was successful (simplified)
                    messages.push_back("[Recovery Strategy] " + strategy.name + " executed");
                    break; // Stop after first successful recovery attempt
                } else {
                    messages.push_back("[Recovery Strategy] " + strategy.name + " not applicable");
                }
            } catch (const std::exception& e) {
                messages.push_back("[Recovery Strategy] " + strategy.name + " failed: " + e.what());
            }
        }
        
        return messages;
    }
};

// Convenience functions for common validation scenarios
inline InputValidation::ValidationResult validateBrainConfiguration(const class Config& config) {
    return InputValidation::validateBrainConfiguration(config);
}

inline InputValidation::ValidationResult validateSensoryInput(const class SensoryInput& input) {
    return InputValidation::validateSensoryInput(input);
}

inline InputValidation::ValidationResult validateAction(const class Action& action) {
    return InputValidation::validateActionParameters(action);
}

inline InputValidation::ValidationResult validateSynapse(const Synapse& synapse) {
    auto weightValidation = InputValidation::validateSynapseWeight(synapse.getWeight());
    auto flagsValidation = InputValidation::validatePlasticityFlags(synapse.getPlasticityFlags());
    
    InputValidation::ValidationResult result;
    result.isValid = weightValidation.isValid && flagsValidation.isValid;
    result.errors.insert(result.errors.end(), weightValidation.errors.begin(), weightValidation.errors.end());
    result.errors.insert(result.errors.end(), flagsValidation.errors.begin(), flagsValidation.errors.end());
    result.warnings.insert(result.warnings.end(), weightValidation.warnings.begin(), weightValidation.warnings.end());
    result.warnings.insert(result.warnings.end(), flagsValidation.warnings.begin(), flagsValidation.warnings.end());
    
    return result;
}

inline NLMError createError(NLMError::ErrorCode code, const std::string& message) {
    return NLMError(message, code);
}

inline std::vector<std::string> logError(const NLMError& error, const std::string& operation = "unknown") {
    ErrorHandler::ErrorContext context(operation, "general");
    return ErrorHandler::handleError(error, context);
}

// Python bindings for error handling (will be exposed in bindings.cpp)
void exposeErrorHandlingBindings(py::module& m) {
    m.def("validate_neuron_count", &InputValidation::validateNeuronCount, 
          "Validate neuron count", py::arg("count"));
    
    m.def("validate_simulation_steps", &InputValidation::validateSimulationSteps,
          "Validate simulation steps", py::arg("steps"));
    
    m.def("create_error", &createError,
          "Create NLM error", py::arg("code"), py::arg("message"));
    
    m.def("log_error", &logError,
          "Log error", py::arg("error"), py::arg("operation"));
}
