// Improved error handling and validation header
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    Configuration,
    Initialization,
    Simulation,
    Memory,
    Plasticity,
    Development,
    Cognitive,
    System
};

// Enhanced exception class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::System)
        : std::runtime_error(message), category_(category) {}
    
    ErrorCategory getCategory() const { return category_; }
    
    static NLMError configError(const std::string& message) {
        return NLMError(message, ErrorCategory::Configuration);
    }
    
    static NLMError initError(const std::string& message) {
        return NLMError(message, ErrorCategory::Initialization);
    }
    
    static NLMError simulationError(const std::string& message) {
        return NLMError(message, ErrorCategory::Simulation);
    }
    
    static NLMError memoryError(const std::string& message) {
        return NLMError(message, ErrorCategory::Memory);
    }
    
    static NLMError plasticityError(const std::string& message) {
        return NLMError(message, ErrorCategory::Plasticity);
    }
    
    static NLMError developmentError(const std::string& message) {
        return NLMError(message, ErrorCategory::Development);
    }
    
    static NLMError cognitiveError(const std::string& message) {
        return NLMError(message, ErrorCategory::Cognitive);
    }

private:
    ErrorCategory category_;
};

// Validation utilities
class ValidationUtils {
public:
    // Validate configuration values
    static void validateConfigValue(const std::string& key, const std::string& value,
                                   const std::string& type = "string") {
        if (value.empty() && key.find("optional") == std::string::npos) {
            throw NLMError::configError("Empty value for required configuration key: " + key);
        }
        
        if (type == "int") {
            try {
                std::stoi(value);
            } catch (...) {
                throw NLMError::configError("Invalid integer value for key \"" + key + "\": \" + value + "\");
            }
        } else if (type == "double") {
            try {
                std::stod(value);
            } catch (...) {
                throw NLMError::configError("Invalid double value for key \"" + key + "\": \" + value + "\");
            }
        } else if (type == "bool") {
            std::string lower = value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            if (lower != "true" && lower != "false" && lower != "1" && lower != "0" &&
                lower != "yes" && lower != "no" && lower != "on" && lower != "off") {
                throw NLMError::configError("Invalid boolean value for key \"" + key + "\": \" + value + "\");
            }
        }
    }
    
    // Validate simulation parameters
    static void validateSimulationParams(double timestep, size_t neuronCount,
                                         size_t regionCount, double connectionProbability) {
        if (timestep <= 0.0) {
            throw NLMError::configError("Invalid simulation timestep: must be positive, got " + std::to_string(timestep));
        }
        
        if (neuronCount == 0) {
            throw NLMError::configError("Invalid neuron count: must be positive, got " + std::to_string(neuronCount));
        }
        
        if (regionCount == 0) {
            throw NLMError::configError("Invalid region count: must be positive, got " + std::to_string(regionCount));
        }
        
        if (connectionProbability < 0.0 || connectionProbability > 1.0) {
            throw NLMError::configError("Invalid connection probability: must be between 0 and 1, got " + std::to_string(connectionProbability));
        }
    }
    
    // Validate plasticity parameters
    static void validatePlasticityParams(float stdpLtpWeight, float stdpLtdWeight,
                                         float stdpTimeConstant, float synaptogenesisRate,
                                         float pruningRate) {
        if (stdpLtpWeight <= 0.0) {
            throw NLMError::configError("Invalid STDP LTP weight: must be positive, got " + std::to_string(stdpLtpWeight));
        }
        
        if (stdpLtdWeight <= 0.0) {
            throw NLMError::configError("Invalid STDP LTD weight: must be positive, got " + std::to_string(stdpLtdWeight));
        }
        
        if (stdpTimeConstant <= 0.0) {
            throw NLMError::configError("Invalid STDP time constant: must be positive, got " + std::to_string(stdpTimeConstant));
        }
        
        if (synaptogenesisRate < 0.0 || synaptogenesisRate > 1.0) {
            throw NLMError::configError("Invalid synaptogenesis rate: must be between 0 and 1, got " + std::to_string(synaptogenesisRate));
        }
        
        if (pruningRate < 0.0 || pruningRate > 1.0) {
            throw NLMError::configError("Invalid pruning rate: must be between 0 and 1, got " + std::to_string(pruningRate));
        }
        
        if (stdpLtdWeight >= stdpLtpWeight) {
            throw NLMError::configError("Invalid STDP parameters: LTD weight must be less than LTP weight");
        }
    }
    
    // Validate neuromodulation parameters
    static void validateNeuromodulationParams(float dopamineBaseline, float curiosityRate,
                                               float noveltyThreshold, float rewardDiscount) {
        if (dopamineBaseline < -1.0 || dopamineBaseline > 1.0) {
            throw NLMError::configError("Invalid dopamine baseline: must be between -1 and 1, got " + std::to_string(dopamineBaseline));
        }
        
        if (curiosityRate < 0.0 || curiosityRate > 1.0) {
            throw NLMError::configError("Invalid curiosity rate: must be between 0 and 1, got " + std::to_string(curiosityRate));
        }
        
        if (noveltyThreshold < 0.0 || noveltyThreshold > 1.0) {
            throw NLMError::configError("Invalid novelty threshold: must be between 0 and 1, got " + std::to_string(noveltyThreshold));
        }
        
        if (rewardDiscount < 0.0 || rewardDiscount > 1.0) {
            throw NLMError::configError("Invalid reward discount factor: must be between 0 and 1, got " + std::to_string(rewardDiscount));
        }
    }
    
    // Calculate statistics safely
    static double calculateMean(const std::vector<float>& values) {
        if (values.empty()) {
            return 0.0;
        }
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    static double calculateStdDev(const std::vector<float>& values, double mean) {
        if (values.size() < 2) {
            return 0.0;
        }
        
        double sumSquares = std::accumulate(values.begin(), values.end(), 0.0,
            [mean](double acc, float val) {
                return acc + (val - mean) * (val - mean);
            });
        
        return std::sqrt(sumSquares / (values.size() - 1));
    }
    
    // Validate vector sizes
    static void validateVectorSize(const std::vector<float>& vec, size_t expectedSize,
                                   const std::string& context = "vector") {
        if (vec.size() != expectedSize) {
            throw NLMError::simulationError("Invalid " + context + " size: expected " + std::to_string(expectedSize) +
                                             ", got " + std::to_string(vec.size()));
        }
    }
    
    // Safe string conversion
    static std::string safeToString(double value, int precision = 6) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    // Validate neuron ID
    static void validateNeuronId(uint64_t neuronId, uint64_t maxNeurons) {
        if (neuronId >= maxNeurons) {
            throw NLMError::simulationError("Invalid neuron ID: " + std::to_string(neuronId) +
                                             ", must be less than " + std::to_string(maxNeurons));
        }
    }
    
    // Validate region ID
    static void validateRegionId(uint64_t regionId, uint64_t maxRegions) {
        if (regionId >= maxRegions) {
            throw NLMError::simulationError("Invalid region ID: " + std::to_string(regionId) +
                                             ", must be less than " + std::to_string(maxRegions));
        }
    }
    
    // Validate concentration (value between 0 and 1)
    static void validateConcentration(float value, const std::string& name = "value") {
        if (value < 0.0 || value > 1.0) {
            throw NLMError::configError("Invalid " + name + ": must be between 0 and 1, got " + std::to_string(value));
        }
    }
};

// Enhanced error reporting utilities
class ErrorReporting {
public:
    // Format error message with context
    static std::string formatError(const NLMError& error, const std::string& context = "") {
        std::ostringstream oss;
        oss << "[NLM Error]";
        
        if (!context.empty()) {
            oss << " " << context;
        }
        
        oss << ": " << error.what();
        
        switch (error.getCategory()) {
            case ErrorCategory::Configuration:
                oss << " (Configuration Error)";
                break;
            case ErrorCategory::Initialization:
                oss << " (Initialization Error)";
                break;
            case ErrorCategory::Simulation:
                oss << " (Simulation Error)";
                break;
            case ErrorCategory::Memory:
                oss << " (Memory Error)";
                break;
            case ErrorCategory::Plasticity:
                oss << " (Plasticity Error)";
                break;
            case ErrorCategory::Development:
                oss << " (Development Error)";
                break;
            case ErrorCategory::Cognitive:
                oss << " (Cognitive Error)";
                break;
            case ErrorCategory::System:
                oss << " (System Error)";
                break;
        }
        
        return oss.str();
    }
    
    // Print error with stack trace (simplified)
    static void reportError(const NLMError& error, const std::string& context = "") {
        std::string formatted = formatError(error, context);
        std::cerr << formatted << std::endl;
        
        // For critical errors, also print to stderr
        if (error.getCategory() == ErrorCategory::System) {
            std::cerr << "CRITICAL: System error occurred. See logs for details." << std::endl;
        }
    }
    
    // Report warning
    static void reportWarning(const std::string& message, const std::string& context = "") {
        std::ostringstream oss;
        oss << "[NLM Warning]";
        if (!context.empty()) oss << " " << context;
        oss << ": " << message;
        
        std::cerr << oss.str() << std::endl;
    }
    
    // Report info
    static void reportInfo(const std::string& message, const std::string& context = "") {
        std::ostringstream oss;
        oss << "[NLM Info]";
        if (!context.empty()) oss << " " << context;
        oss << ": " << message;
        
        std::cout << oss.str() << std::endl;
    }
    
    // Report debug
    static void reportDebug(const std::string& message, const std::string& context = "") {
        // Only output if verbose/debug mode is enabled
        // This is a placeholder - actual implementation would check logging level
        if (true) { // Replace with actual debug flag check
            std::ostringstream oss;
            oss << "[NLM Debug]";
            if (!context.empty()) oss << " " << context;
            oss << ": " << message;
            
            std::cout << oss.str() << std::endl;
        }
    }
};

// Safe operation wrapper
class SafeOperation {
public:
    // Safe configuration loading with validation
    static bool safeLoadConfig(const std::string& filepath, std::vector<std::string>& errors) {
        try {
            // Implementation would go here
            // For now, return true as placeholder
            return true;
        } catch (const NLMError& e) {
            errors.push_back(ErrorReporting::formatError(e, "Config loading"));
            return false;
        } catch (const std::exception& e) {
            errors.push_back(std::string("Unexpected error: ") + e.what());
            return false;
        }
    }
    
    // Safe simulation step
    static bool safeStep(Brain& brain, SimulationStep step, Timestamp time,
                        std::vector<std::string>& errors) {
        try {
            brain.step(step, time);
            return true;
        } catch (const NLMError& e) {
            errors.push_back(ErrorReporting::formatError(e, "Simulation step"));
            return false;
        } catch (const std::exception& e) {
            errors.push_back(std::string("Unexpected error: ") + e.what());
            return false;
        }
    }
    
    // Safe plasticity update
    static bool safeUpdatePlasticity(Brain& brain, std::vector<std::string>& errors) {
        try {
            brain.updatePlasticity();
            return true;
        } catch (const NLMError& e) {
            errors.push_back(ErrorReporting::formatError(e, "Plasticity update"));
            return false;
        } catch (const std::exception& e) {
            errors.push_back(std::string("Unexpected error: ") + e.what());
            return false;
        }
    }
    
    // Safe memory operation
    static bool safeMemoryOperation(const std::function<void()>& operation,
                                   std::vector<std::string>& errors,
                                   const std::string& operationName = "Memory operation") {
        try {
            operation();
            return true;
        } catch (const NLMError& e) {
            errors.push_back(ErrorReporting::formatError(e, operationName));
            return false;
        } catch (const std::exception& e) {
            errors.push_back(std::string("Unexpected error in " + operationName + ": ") + e.what());
            return false;
        }
    }
};

} // namespace nlm

// Convenience error handling macros
#define NLM_VALIDATE(condition, message) \
    if (!(condition)) { \
        throw nlm::NLMError::configError(message); \
    }

#define NLM_VALIDATE_OP(operation, condition, message) \
    if (!(operation)) { \
        throw nlm::NLMError::simulationError(message); \
    }

#define NLM_TRY(operation, errorHandler) \
    try { \
        operation; \
    } catch (const nlm::NLMError& e) { \
        errorHandler(e); \
    } catch (const std::exception& e) { \
        errorHandler(nlm::NLMError(std::string("Unexpected error: ") + e.what())); \
    }

#define NLM_SAFE_OP(operation, resultVar, errorList) \
    resultVar = nlm::SafeOperation::safeMemoryOperation([&]() { operation; }, errorList)

// Error reporting macros
#define NLM_REPORT_ERROR(error) nlm::ErrorReporting::reportError(error, __func__)
#define NLM_REPORT_WARNING(message) nlm::ErrorReporting::reportWarning(message, __func__)
#define NLM_REPORT_INFO(message) nlm::ErrorReporting::reportInfo(message, __func__)
#define NLM_REPORT_DEBUG(message) nlm::ErrorReporting::reportDebug(message, __func__)

// Validation macros
#define NLM_VALIDATE_CONFIG(value, type) nlm::ValidationUtils::validateConfigValue(#value, #type)
#define NLM_VALIDATE_PARAMS(...) nlm::ValidationUtils::validateSimulationParams(__VA_ARGS__)
#define NLM_VALIDATE_PLATISTICS(...) nlm::ValidationUtils::validatePlasticityParams(__VA_ARGS__)
#define NLM_VALIDATE_CONCENTRATION(value) nlm::ValidationUtils::validateConcentration(value)

// Unit testing utilities
class UnitTestHelper {
public:
    // Test assertion with detailed error reporting
    static void assertTrue(bool condition, const std::string& testName, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Test failed: " + testName + " - " + message);
        }
    }
    
    // Test equality
    template<typename T>
    static void assertEqual(const T& expected, const T& actual, const std::string& testName,
                           const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error("Test failed: " + testName + " - Expected: " + std::to_string(expected) +
                                     ", Actual: " + std::to_string(actual) + " - " + message);
        }
    }
    
    // Test approximate equality for floating point
    static void assertApproxEqual(double expected, double actual, double tolerance,
                                 const std::string& testName, const std::string& message = "") {
        if (std::abs(expected - actual) > tolerance) {
            throw std::runtime_error("Test failed: " + testName + " - Expected: " + std::to_string(expected) +
                                     ", Actual: " + std::to_string(actual) + ", Tolerance: " +
                                     std::to_string(tolerance) + " - " + message);
        }
    }
    
    // Test within range
    static void assertInRange(double value, double min, double max,
                             const std::string& testName, const std::string& message = "") {
        if (value < min || value > max) {
            throw std::runtime_error("Test failed: " + testName + " - Value " + std::to_string(value) +
                                     " is not within range [" + std::to_string(min) + ", " +
                                     std::to_string(max) + "] - " + message);
        }
    }
    
    // Test non-negative
    static void assertNonNegative(double value, const std::string& testName,
                                 const std::string& message = "") {
        if (value < 0.0) {
            throw std::runtime_error("Test failed: " + testName + " - Value " + std::to_string(value) +
                                     " is negative - " + message);
        }
    }
    
    // Test positive
    static void assertPositive(double value, const std::string& testName,
                              const std::string& message = "") {
        if (value <= 0.0) {
            throw std::runtime_error("Test failed: " + testName + " - Value " + std::to_string(value) +
                                     " is not positive - " + message);
        }
    }
    
    // Test not empty
    static void assertNotEmpty(const std::vector<float>& vec, const std::string& testName,
                               const std::string& message = "") {
        if (vec.empty()) {
            throw std::runtime_error("Test failed: " + testName + " - Vector is empty - " + message);
        }
    }
};