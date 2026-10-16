#pragma once

#include "../core/Error.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>

namespace nlm {
namespace Validation {

/**
 * @namespace Validation
 * @brief Validation utilities for NLM framework
 * 
 * Provides comprehensive validation functions for input parameters,
 * configuration values, memory operations, and system state.
 */

// Forward declarations for validation helpers
class ValidationHelper;

/**
 * @class ValidationMacros
 * @brief Preprocessor macros for common validation patterns
 * 
 * These macros provide consistent validation with detailed error messages
 * and optional logging for debugging and error reporting.
 */
class ValidationMacros {
public:
    // Input validation macros
    #define NLM_VALIDATE(cond, msg, ...) \
        if (!(cond)) { \
            throw nlm::InvalidArgumentException(msg, ##__VA_ARGS__); \
        }
    
    #define NLM_VALIDATE_WITH_SUGGESTION(cond, msg, suggestion) \
        if (!(cond)) { \
            throw nlm::InvalidArgumentException(msg).withSuggestion(suggestion); \
        }
    
    #define NLM_VALIDATE_PTR(ptr, msg) \
        if (!(ptr)) { \
            throw nlm::MemoryManagementException("Null pointer: " + std::string(msg)); \
        }
    
    #define NLM_VALIDATE_RANGE(value, min, max, msg) \
        if ((value) < (min) || (value) > (max)) { \
            std::ostringstream oss; \
            oss << "Value " << (value) << " out of range [" << (min) << ", " << (max) << "]: " << msg; \
            throw nlm::InvalidArgumentException(oss.str()); \
        }
    
    #define NLM_VALIDATE_POSITIVE(value, msg) \
        if ((value) <= 0) { \
            throw nlm::InvalidArgumentException("Value must be positive: " + std::string(msg)); \
        }
    
    #define NLM_VALIDATE_NON_NEGATIVE(value, msg) \
        if ((value) < 0) { \
            throw nlm::InvalidArgumentException("Value must be non-negative: " + std::string(msg)); \
        }
    
    #define NLM_VALIDATE_CONDITIONAL(cond, msg, suggestion) \
        if ((cond)) { \
            throw nlm::InvalidArgumentException(msg).withSuggestion(suggestion); \
        }
    
    // Resource validation macros
    #define NLM_VALIDATE_FILE_ACCESS(filepath, operation) \
        if (!nlm::Validation::validateFileAccess(filepath, operation)) { \
            throw nlm::ResourceAccessException("Cannot " + operation + " file: " + filepath); \
        }
    
    #define NLM_VALIDATE_MEMORY_ALLOC(size, type) \
        if (!nlm::Validation::validateMemoryAllocation(size, type)) { \
            throw nlm::MemoryManagementException("Memory allocation failed", size); \
        }
    
    #define NLM_VALIDATE_SYSTEM_STATE(state, operation) \
        if (!nlm::Validation::validateSystemState(state, operation)) { \
            throw nlm::SystemStateException("System state invalid for operation", operation); \
        }
};

/**
 * @class ValidationHelper
 * @brief Utility class for validation operations
 * 
 * Provides static helper functions for various validation scenarios
 * including parameter validation, file access validation, and memory validation.
 */
class ValidationHelper {
public:
    /**
     * @brief Validate numeric value range
     * @tparam T Numeric type (integral or floating-point)
     * @param value The value to validate
     * @param min Minimum allowed value (inclusive)
     * @param max Maximum allowed value (inclusive)
     * @param paramName Name of parameter for error messages
     * @return true if valid, false otherwise
     * @throws InvalidArgumentException if validation fails
     */
    template<typename T>
    static bool validateRange(const T& value, const T& min, const T& max,
                             const std::string& paramName = "value",
                             const std::string& context = "",
                             const std::string& suggestion = "") {
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << "Parameter '" << paramName << "': " << value
                << " is outside valid range [" << min << ", " << max << "]";
            if (!context.empty()) oss << " (context: " << context << ")";
            
            auto exception = nlm::InvalidArgumentException(oss.str());
            if (!suggestion.empty()) exception.withSuggestion(suggestion);
            throw exception;
        }
        return true;
    }
    
    /**
     * @brief Validate positive value
     * @tparam T Numeric type
     * @param value The value to validate
     * @param paramName Name of parameter for error messages
     * @return true if valid, false otherwise
     */
    template<typename T>
    static bool validatePositive(const T& value, const std::string& paramName = "value",
                                const std::string& context = "") {
        if (value <= 0) {
            std::ostringstream oss;
            oss << "Parameter '" << paramName << "': " << value << " must be positive";
            if (!context.empty()) oss << " (context: " << context << ")";
            throw nlm::InvalidArgumentException(oss.str())
                .withSuggestion("Parameter must be greater than zero");
        }
        return true;
    }
    
    /**
     * @brief Validate non-negative value
     * @tparam T Numeric type
     * @param value The value to validate
     * @param paramName Name of parameter for error messages
     * @return true if valid, false otherwise
     */
    template<typename T>
    static bool validateNonNegative(const T& value, const std::string& paramName = "value",
                                   const std::string& context = "") {
        if (value < 0) {
            std::ostringstream oss;
            oss << "Parameter '" << paramName << "': " << value << " must be non-negative";
            if (!context.empty()) oss << " (context: " << context << ")";
            throw nlm::InvalidArgumentException(oss.str())
                .withSuggestion("Parameter must be greater than or equal to zero");
        }
        return true;
    }
    
    /**
     * @brief Validate file access
     * @param filepath Path to file
     * @param operation Operation to perform (e.g., "read", "write")
     * @param context Additional context for error messages
     * @return true if file can be accessed, false otherwise
     */
    static bool validateFileAccess(const std::string& filepath, 
                                  const std::string& operation,
                                  const std::string& context = "") {
        std::ifstream file(filepath);
        bool accessible = file.good();
        file.close();
        
        if (!accessible) {
            NLM_LOG_WARNING("File access validation failed for " + filepath +
                           " during " + operation + ". " + context);
        }
        
        return accessible;
    }
    
    /**
     * @brief Validate memory allocation
     * @param size_bytes Number of bytes to allocate
     * @param type Type description for error messages
     * @return true if memory can be allocated, false otherwise
     */
    static bool validateMemoryAllocation(size_t size_bytes, 
                                        const std::string& type = "data") {
        // Conservative check - assuming minimum allocation size
        if (size_bytes == 0) return true;
        
        // Check if size is reasonable (not excessively large)
        constexpr size_t maxReasonableAllocation = 1024 * 1024 * 1024; // 1GB
        if (size_bytes > maxReasonableAllocation) {
            NLM_LOG_WARNING("Memory allocation check: Requested " + 
                           std::to_string(size_bytes) + " bytes for " + type +
                           " exceeds reasonable limit");
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Validate pointer is not null
     * @param ptr Pointer to validate
     * @param paramName Name of parameter for error messages
     * @return true if pointer is not null, throws otherwise
     */
    template<typename T>
    static T* validateNotNull(T* ptr, const std::string& paramName = "pointer",
                             const std::string& context = "") {
        if (!ptr) {
            std::ostringstream oss;
            oss << "Parameter '" << paramName << "': null pointer";
            if (!context.empty()) oss << " (context: " << context << ")";
            throw nlm::MemoryManagementException(oss.str())
                .withSuggestion("Check if pointer was properly initialized");
        }
        return ptr;
    }
    
    /**
     * @brief Validate neuron ID is valid
     * @param neuronId The neuron ID to validate
     * @param operation Description of operation being performed
     * @return true if valid, throws otherwise
     */
    static bool validateNeuronId(NeuronId neuronId, const std::string& operation = "operation") {
        if (neuronId.value == UINT64_MAX || neuronId.value == 0) {
            throw nlm::InvalidArgumentException(
                "Invalid neuron ID for " + operation + ": " + 
                std::to_string(neuronId.value),
                "neuronId"
            ).withSuggestion("Check if neuron exists in the current region");
        }
        return true;
    }
    
    /**
     * @brief Validate synapse parameters
     * @param weight Synaptic weight
     * @param delay Delay in steps
     * @param type Synapse type
     * @return true if valid, throws otherwise
     */
    static bool validateSynapseParameters(float weight, Delay delay, 
                                          SynapseType type,
                                          const std::string& context = "") {
        validateRange(weight, -10.0f, 10.0f, "synapseWeight", context,
                     "Synaptic weights should typically be in range [-10, 10]");
        
        validateRange(delay, 0u, 10000u, "synapseDelay", context,
                     "Synaptic delay should be reasonable (0-10000 steps)");
        
        return true;
    }
    
    /**
     * @brief Validate neuron state
     * @param membranePotential Membrane potential
     * @param firingState Firing state
     * @param context Additional context
     * @return true if valid, throws otherwise
     */
    static bool validateNeuronState(float membranePotential, 
                                   FiringState firingState,
                                   const std::string& context = "") {
        // Check membrane potential range (reasonable bounds)
        validateRange(membranePotential, -100.0f, 100.0f, "membranePotential", context,
                     "Membrane potential should be in reasonable range [-100, 100] mV");
        
        // Check firing state enum
        if (static_cast<uint8_t>(firingState) > static_cast<uint8_t>(FiringState::Inhibited)) {
            throw nlm::InvalidArgumentException(
                "Invalid firing state value: " + std::to_string(static_cast<uint8_t>(firingState)),
                "firingState"
            ).withSuggestion("Ensure firing state is a valid FiringState enum value");
        }
        
        return true;
    }
    
    /**
     * @brief Validate configuration key
     * @param key Configuration key to validate
     * @param required Whether key is required
     * @return true if valid, throws otherwise
     */
    static bool validateConfigKey(const std::string& key, bool required = true,
                                 const std::string& context = "") {
        if (key.empty()) {
            if (required) {
                throw nlm::ConfigurationException("Configuration key cannot be empty", key);
            }
            return false;
        }
        
        // Check for valid key characters
        bool validChars = true;
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                validChars = false;
                break;
            }
        }
        
        if (!validChars) {
            throw nlm::ConfigurationException(
                "Invalid configuration key format: '" + key + "'",
                key
            ).withSuggestion("Use alphanumeric characters, underscores, hyphens, or periods");
        }
        
        return true;
    }
    
    /**
     * @brief Validate system initialization state
     * @param isInitialized Current initialization state
     * @param operation Operation to validate
     * @return true if system is properly initialized
     */
    static bool validateSystemState(bool isInitialized, const std::string& operation,
                                   const std::string& context = "") {
        if (!isInitialized) {
            throw nlm::SystemStateException(
                "System not initialized for operation: " + operation,
                operation
            ).withSuggestion("Call initialize() method before performing operations");
        }
        return true;
    }
    
    /**
     * @brief Validate plasticity parameters
     * @param ltp_weight Long-term potentiation weight
     * @param ltd_weight Long-term depression weight
     * @param tau Time constant
     * @return true if valid, throws otherwise
     */
    static bool validatePlasticityParameters(float ltp_weight, float ltd_weight,
                                           float tau, const std::string& context = "") {
        validateRange(ltp_weight, 0.0f, 0.1f, "stdp_ltp_weight", context,
                     "LTP weight should be positive and reasonable");
        
        validateRange(ltd_weight, 0.0f, 0.2f, "stdp_ltd_weight", context,
                     "LTD weight should be positive and reasonable");
        
        validateRange(tau, 1.0f, 100.0f, "stdp_tau", context,
                     "STDP time constant should be reasonable (1-100 ms)");
        
        // Check that LTD weight is slightly larger than LTP weight (typical STDP property)
        if (ltd_weight <= ltp_weight) {
            NLM_LOG_WARNING("STDP parameter check: LTD weight (" + std::to_string(ltd_weight) +
                           ") should be greater than LTP weight (" + std::to_string(ltp_weight) +
                           "). This may cause unexpected learning behavior.");
        }
        
        return true;
    }
    
    /**
     * @brief Validate timestep parameters
     * @param timestep Timestep duration
     * @param minStepsPerSecond Minimum steps per second
     * @return true if valid, throws otherwise
     */
    static bool validateTimestep(double timestep, double minStepsPerSecond = 1000.0) {
        validateRange(timestep, 1e-6, 0.1, "simulation_timestep",
                     "Timestep should be small enough for numerical stability",
                     "Try decreasing timestep or increasing minStepsPerSecond");
        
        if (1.0 / timestep < minStepsPerSecond) {
            throw nlm::ConfigurationException(
                "Timestep " + std::to_string(timestep) +
                " results in insufficient simulation speed (< " +
                std::to_string(minStepsPerSecond) + " Hz)",
                "simulation_timestep"
            ).withSuggestion("Increase minStepsPerSecond or decrease timestep");
        }
        
        return true;
    }
    
    /**
     * @brief Validate neuron population parameters
     * @param neuronCount Number of neurons
     * @param regionCount Number of regions
     * @param populationSize Size of each population
     * @return true if valid, throws otherwise
     */
    static bool validatePopulationParameters(size_t neuronCount, size_t regionCount,
                                            size_t populationSize) {
        validatePositive(neuronCount, "neuron_count", "Brain initialization");
        validatePositive(regionCount, "region_count", "Brain initialization");
        validatePositive(populationSize, "population_size", "Brain initialization");
        
        if (neuronCount < regionCount) {
            throw nlm::ConfigurationException(
                "Insufficient neurons per region: " + std::to_string(neuronCount) +
                " neurons for " + std::to_string(regionCount) + " regions",
                "neuron_count"
            ).withSuggestion("Increase neuron_count or decrease region_count");
        }
        
        if (populationSize > neuronCount) {
            throw nlm::ConfigurationException(
                "Population size exceeds total neuron count: " + std::to_string(populationSize) +
                " > " + std::to_string(neuronCount),
                "population_size"
            ).withSuggestion("Reduce population size or increase neuron_count");
        }
        
        return true;
    }
    
    /**
     * @brief Validate Python binding data types
     * @param pyTypeName Python type name
     * @param expectedTypes Vector of expected types
     * @return true if valid, throws otherwise
     */
    static bool validatePythonType(const std::string& pyTypeName,
                                  const std::vector<std::string>& expectedTypes,
                                  const std::string& context = "") {
        bool typeValid = std::find(expectedTypes.begin(), expectedTypes.end(), pyTypeName) !=
                        expectedTypes.end();
        
        if (!typeValid) {
            throw nlm::PythonBindingException(
                "Python type '" + pyTypeName + "' not supported in " + context,
                pyTypeName
            ).withSuggestion("Convert to one of expected types: " + 
                            (expectedTypes.empty() ? "none" : 
                             std::accumulate(expectedTypes.begin(), expectedTypes.end(), std::string(),
                                            [](const std::string& a, const std::string& b) {
                                                return a.empty() ? b : a + ", " + b;
                                            }));
        }
        
        return true;
    }
    
    /**
     * @brief Safe resource cleanup with error handling
     * @tparam T Type of resource
     * @param resource Resource to clean up
     * @param cleanupAction Function to perform cleanup
     * @return true if cleanup completed, throws on critical errors
     */
    template<typename T, typename CleanupFunc>
    static bool safeCleanup(T* resource, const std::string& resourceName,
                           CleanupFunc cleanupAction) {
        if (!resource) {
            NLM_LOG_DEBUG("Cleanup skipped for null " + resourceName);
            return true;
        }
        
        try {
            cleanupAction(resource);
            NLM_LOG_DEBUG("Successfully cleaned up " + resourceName);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error cleaning up " + resourceName + ": " + e.what());
            // Don't throw on cleanup failures to avoid double-exception issues
            return false;
        }
    }
    
    /**
     * @brief Validate neural connectivity parameters
     * @param connectionProb Probability of connection
     * @param weightMean Mean synaptic weight
     * @param weightStdDev Standard deviation of synaptic weight
     * @return true if valid, throws otherwise
     */
    static bool validateConnectivityParameters(float connectionProb, float weightMean,
                                              float weightStdDev) {
        validateRange(connectionProb, 0.0f, 1.0f, "connection_probability",
                     "Connection probability should be between 0 and 1");
        
        validateRange(weightMean, -10.0f, 10.0f, "synapseWeightMean",
                     "Mean synaptic weight should be reasonable");
        
        validateRange(weightStdDev, 0.0f, 5.0f, "synapseWeightStdDev",
                     "Standard deviation of synaptic weight should be reasonable");
        
        return true;
    }
    
    /**
     * @brief Validate neuromodulation signal levels
     * @param dopamineLevel Dopamine level
     * @param noveltyLevel Novelty level
     * @param curiosityLevel Curiosity level
     * @return true if valid, throws otherwise
     */
    static bool validateNeuromodulationLevels(float dopamineLevel, float noveltyLevel,
                                             float curiosityLevel) {
        validateRange(dopamineLevel, 0.0f, 10.0f, "dopamineLevel",
                     "Dopamine level should be in reasonable range");
        
        validateRange(noveltyLevel, 0.0f, 10.0f, "noveltyLevel",
                     "Novelty level should be in reasonable range");
        
        validateRange(curiosityLevel, 0.0f, 10.0f, "curiosityLevel",
                     "Curiosity level should be in reasonable range");
        
        return true;
    }
};

} // namespace Validation
} // namespace nlm

// Convenience validation functions (to be used throughout codebase)
namespace nlm {
    // Safe memory operations
    template<typename T>
    T* safeAllocate(size_t count, const std::string& context = "") {
        NLM_VALIDATE_MEMORY_ALLOC(count * sizeof(T), "memory allocation");
        return static_cast<T*>(operator new(count * sizeof(T)));
    }
    
    template<typename T>
    void safeDeallocate(void* ptr, const std::string& context = "") {
        if (ptr) {
            operator delete(ptr);
        }
    }
    
    // Input validation wrapper
    template<typename T>
    T validateInput(const T& value, const std::string& paramName,
                   const std::string& minMsg = "", const std::string& maxMsg = "") {
        // Basic validation with helpful error messages
        if (std::is_integral<T>::value) {
            if (value < 0) {
                throw InvalidArgumentException(
                    "Parameter '" + paramName + "': value cannot be negative: " +
                    std::to_string(value),
                    paramName
                ).withSuggestion(minMsg);
            }
        } else {
            if (value < 0.0) {
                throw InvalidArgumentException(
                    "Parameter '" + paramName + "': value cannot be negative: " +
                    std::to_string(value),
                    paramName
                ).withSuggestion(minMsg);
            }
        }
        return value;
    }
    
    // Configuration validation helper
    void validateConfigFile(const std::string& filepath) {
        NLM_VALIDATE_FILE_ACCESS(filepath, "reading configuration", 
                               "Check if file exists and is readable");
        
        // Additional file format validation can be added here
    }
    
    // System initialization validation
    template<typename T>
    bool validateSystemReady(const T& system, const std::string& operation) {
        if (!system) {
            throw SystemStateException(
                "System not properly initialized for operation: " + operation,
                operation
            ).withSuggestion("Ensure system was successfully initialized");
        }
        return true;
    }
    
    // Safe vector access
    template<typename T>
    const T& safeVectorAccess(const std::vector<T>& vec, size_t index,
                             const std::string& context = "") {
        NLM_VALIDATE(index < vec.size(), "Vector index out of bounds",
                     "Index: " + std::to_string(index) + ", Size: " + std::to_string(vec.size()),
                     "Check if index is within vector bounds");
        return vec[index];
    }
    
    // Safe map access
    template<typename K, typename V>
    const V& safeMapAccess(const std::map<K, V>& map, const K& key,
                          const std::string& context = "") {
        auto it = map.find(key);
        NLM_VALIDATE(it != map.end(), "Map key not found: " + std::to_string(key),
                     "Key: " + std::to_string(key) + ". " + context,
                     "Check if key exists in map or add it");
        return it->second;
    }
    
    // Generic validation with custom error handling
    template<typename Func, typename... Args>
    auto safeCall(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Validation error in safeCall: " + std::string(e.what()));
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Standard exception in safeCall: " + std::string(e.what()));
            throw MemoryManagementException("Unexpected error in safe operation: " + 
                                           std::string(e.what()));
        }
    }
    
    // Type conversion validation
    template<typename From, typename To>
    To safeConvert(const From& from, const std::string& fromType,
                  const std::string& toType,
                  const std::string& context = "") {
        try {
            return static_cast<To>(from);
        } catch (...) {
            throw PythonBindingException(
                "Failed to convert from " + fromType + " to " + toType + " in " + context,
                fromType
            ).withSuggestion("Check if values are compatible for conversion");
        }
    }
}

#endif // VALIDATION_INCLUDES_HPP
