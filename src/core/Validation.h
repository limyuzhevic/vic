// Validation utilities for NLM
#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Validation exceptions
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class RangeError : public ValidationError {
public:
    RangeError(const std::string& param, double value, double min, double max)
        : ValidationError("Parameter '" + param + "' out of range: " + 
                         std::to_string(value) + " not in [" + 
                         std::to_string(min) + ", " + std::to_string(max) + "]") {}
};

class ValueError : public ValidationError {
public:
    ValueError(const std::string& message) : ValidationError("Invalid value: " + message) {}
};

class ConfigurationError : public ValidationError {
public:
    ConfigurationError(const std::string& message) : ValidationError("Configuration error: " + message) {}
};

class NeuralNetworkError : public ValidationError {
public:
    NeuralNetworkError(const std::string& message) : ValidationError("Neural network error: " + message) {}
};

// Validation helper functions
class Validator {
public:
    // Range validation
    template<typename T>
    static void validateRange(const std::string& param, T value, T min, T max) {
        if (value < min || value > max) {
            throw RangeError(param, static_cast<double>(value), static_cast<double>(min), static_cast<double>(max));
        }
    }
    
    // Non-negative validation
    template<typename T>
    static void validateNonNegative(const std::string& param, T value) {
        if (value < T(0)) {
            throw RangeError(param, static_cast<double>(value), 0.0, std::numeric_limits<double>::max());
        }
    }
    
    // Positive validation
    template<typename T>
    static void validatePositive(const std::string& param, T value) {
        if (value <= T(0)) {
            throw RangeError(param, static_cast<double>(value), 0.0, std::numeric_limits<double>::max());
        }
    }
    
    // Valid configuration key
    static void validateConfigKey(const std::string& key) {
        if (key.empty()) {
            throw ValueError("Configuration key cannot be empty");
        }
        if (key.size() > 100) {
            throw ValueError("Configuration key too long (max 100 characters)");
        }
    }
    
    // Valid neuron count
    static void validateNeuronCount(size_t count) {
        validateRange("neuron_count", static_cast<double>(count), 1.0, 1000000.0);  // Up to 1M neurons
    }
    
    // Valid connection probability
    static void validateConnectionProbability(float prob) {
        validateRange("connection_probability", prob, 0.0f, 1.0f);
    }
    
    // Valid timestep
    static void validateTimestep(double timestep) {
        validateRange("simulation_timestep", timestep, 1e-6, 0.1);  // 1µs to 100ms
    }
    
    // Valid current amount
    static void validateCurrent(float current) {
        validateRange("current_injection", current, -1000.0f, 1000.0f);  // +/- 1000 nA
    }
    
    // Valid refractory period
    static void validateRefractoryPeriod(float period) {
        validateRange("refractory_period", period, 1.0f, 10000.0f);  // 1 step to 10000 steps
    }
    
    // Valid spike rate
    static void validateSpikeRate(float rate) {
        validateRange("spike_rate", rate, 0.0f, 100000.0f);  // Up to 100 kHz
    }
    
    // String validation
    static void validateString(const std::string& str, const std::string& param, size_t maxLength = 1000) {
        if (str.empty()) {
            throw ValueError(param + " cannot be empty");
        }
        if (str.size() > maxLength) {
            throw ValueError(param + " too long (max " + std::to_string(maxLength) + " characters)");
        }
    }
    
    // Vector validation
    template<typename T>
    static void validateVector(const std::string& param, const std::vector<T>& vec, 
                              size_t maxSize = 1000000, T minValue = std::numeric_limits<T>::lowest(),
                              T maxValue = std::numeric_limits<T>::max()) {
        if (vec.size() > maxSize) {
            throw ValueError(param + " vector too large (max " + std::to_string(maxSize) + " elements)");
        }
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] < minValue || vec[i] > maxValue) {
                throw ValueError(param + " vector element " + std::to_string(i) + 
                               " out of range: " + std::to_string(vec[i]));
            }
        }
    }
    
    // Configuration validation
    static void validateConfig(const Config& config) {
        try {
            // Validate neuron count
            if (config.has("neuron_count")) {
                auto countOpt = config.get<int64_t>("neuron_count");
                if (countOpt) {
                    validateNeuronCount(static_cast<size_t>(*countOpt));
                }
            }
            
            // Validate connection probability
            if (config.has("connection_probability")) {
                auto probOpt = config.get<float>("connection_probability");
                if (probOpt) {
                    validateConnectionProbability(*probOpt);
                }
            }
            
            // Validate simulation timestep
            if (config.has("simulation_timestep")) {
                auto timestepOpt = config.get<double>("simulation_timestep");
                if (timestepOpt) {
                    validateTimestep(*timestepOpt);
                }
            }
            
            // Validate random seed
            if (config.has("random_seed")) {
                auto seedOpt = config.get<int64_t>("random_seed");
                if (seedOpt) {
                    validateRange("random_seed", static_cast<double>(*seedOpt), 0.0, 1e9);
                }
            }
            
            // Validate STDP parameters
            if (config.has("stdp_ltp_weight")) {
                auto ltpOpt = config.get<float>("stdp_ltp_weight");
                if (ltpOpt) {
                    validateRange("stdp_ltp_weight", *ltpOpt, 0.0f, 1.0f);
                }
            }
            
            if (config.has("stdp_ltd_weight")) {
                auto ltdOpt = config.get<float>("stdp_ltd_weight");
                if (ltdOpt) {
                    validateRange("stdp_ltd_weight", *ltdOpt, 0.0f, 1.0f);
                }
            }
            
            if (config.has("stdp_tau")) {
                auto tauOpt = config.get<float>("stdp_tau");
                if (tauOpt) {
                    validateRange("stdp_tau", *tauOpt, 0.0f, 1000.0f);
                }
            }
            
        } catch (const ValidationError& e) {
            NLM_LOG_ERROR(std::string("Configuration validation failed: ") + e.what());
            throw;
        }
    }
    
    // Neural network validation
    static void validateBrainInitialization(const Config& config) {
        try {
            // Ensure basic configuration is present
            if (!config.has("neuron_count")) {
                throw ConfigurationError("neuron_count configuration required");
            }
            
            if (!config.has("region_count")) {
                throw ConfigurationError("region_count configuration required");
            }
            
            if (!config.has("connection_probability")) {
                throw ConfigurationError("connection_probability configuration required");
            }
            
            if (!config.has("random_seed")) {
                throw ConfigurationError("random_seed configuration required");
            }
            
            // Validate neuron count
            auto countOpt = config.get<int64_t>("neuron_count");
            if (countOpt && *countOpt <= 0) {
                throw ConfigurationError("neuron_count must be positive");
            }
            
            // Validate region count
            auto regionCountOpt = config.get<int64_t>("region_count");
            if (regionCountOpt && *regionCountOpt <= 0) {
                throw ConfigurationError("region_count must be positive");
            }
            
            // Validate connection probability
            auto probOpt = config.get<float>("connection_probability");
            if (probOpt && (*probOpt < 0.0f || *probOpt > 1.0f)) {
                throw ConfigurationError("connection_probability must be between 0.0 and 1.0");
            }
            
        } catch (const ValidationError& e) {
            NLM_LOG_ERROR(std::string("Brain initialization validation failed: ") + e.what());
            throw;
        }
    }
    
    // Error handling wrapper
    template<typename Func>
    static auto handleValidationError(const std::string& operation, Func&& func) {
        try {
            return func();
        } catch (const ValidationError& e) {
            NLM_LOG_ERROR(std::string("Validation error during ") + operation + ": " + e.what());
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Unexpected error during ") + operation + ": " + e.what());
            throw NeuralNetworkError(std::string("Operation ") + operation + " failed: " + e.what());
        }
    }
};

// Validation macros for easy use
#define VALIDATE_RANGE(param, value, min, max) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            throw RangeError(param, static_cast<double>(value), static_cast<double>(min), static_cast<double>(max)); \
        } \
    } while(0)

#define VALIDATE_NON_NEGATIVE(param, value) \
    do { \
        if ((value) < 0) { \
            throw RangeError(param, static_cast<double>(value), 0.0, std::numeric_limits<double>::max()); \
        } \
    } while(0)

#define VALIDATE_POSITIVE(param, value) \
    do { \
        if ((value) <= 0) { \
            throw RangeError(param, static_cast<double>(value), 0.0, std::numeric_limits<double>::max()); \
        } \
    } while(0)

#define VALIDATE_THROW(msg) \
    do { \
        throw ValidationError(msg); \
    } while(0)

#define VALIDATE_LOG_THROW(level, msg) \
    do { \
        NLM_LOG_##level(msg); \
        throw ValidationError(msg); \
    } while(0)

// Validation template for containers
namespace ValidationUtils {
    template<typename T>
    static bool isFinite(T value) {
        return std::isfinite(static_cast<double>(value));
    }
    
    template<typename T>
    static bool isPositive(T value) {
        return value > T(0);
    }
    
    template<typename T>
    static bool isNonNegative(T value) {
        return value >= T(0);
    }
    
    template<typename Container>
    static bool isValidContainer(const Container& container, size_t maxSize = 1000000) {
        return container.size() <= maxSize;
    }
}

} // namespace nlm
