#pragma once

#include <pybind11/pybind11.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <cmath>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;

namespace nlm {

// Enhanced Python-specific exceptions
class PythonNLMError : public std::runtime_error {
public:
    explicit PythonNLMError(const std::string& message) : std::runtime_error(message) {}
};

class TypeError : public PythonNLMError {
public:
    explicit TypeError(const std::string& message) : PythonNLMError("Type Error: " + message) {}
};

class ValueError : public PythonNLMError {
public:
    explicit ValueError(const std::string& message) : PythonNLMError("Value Error: " + message) {}
};

class RuntimeError : public PythonNLMError {
public:
    explicit RuntimeError(const std::string& message) : PythonNLMError("Runtime Error: " + message) {}
};

class InvalidOperationError : public PythonNLMError {
public:
    explicit InvalidOperationError(const std::string& message) : PythonNLMError("Invalid Operation: " + message) {}
};

class ValidationError : public PythonNLMError {
public:
    explicit ValidationError(const std::string& message) : PythonNLMError("Validation Error: " + message) {}
};

// Safe execution wrapper for method calls
template<typename Func, typename... Args>
auto safeExecute(Func&& func, Args&&... args) {
    try {
        return func(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
        throw PythonNLMError(std::string("Safe execution failed: ") + e.what());
    }
}

// Range validation helper
class RangeValidator {
public:
    template<typename T>
    static void validate(const T& value, const T& min, const T& max, const std::string& paramName) {
        if (value < min || value > max) {
            throw ValueError("Parameter '" + paramName + "' must be in range [" + 
                           std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
        }
    }
    
    template<typename T>
    static void validateNonNegative(const T& value, const std::string& paramName) {
        if (value < 0) {
            throw ValueError("Parameter '" + paramName + "' must be non-negative, got " + std::to_string(value));
        }
    }
    
    template<typename T>
    static void validatePositive(const T& value, const std::string& paramName) {
        if (value <= 0) {
            throw ValueError("Parameter '" + paramName + "' must be positive, got " + std::to_string(value));
        }
    }
    
    template<typename T>
    static void validateFinite(const T& value, const std::string& paramName) {
        if (!std::isfinite(value)) {
            throw ValueError("Parameter '" + paramName + "' must be finite, got " + std::to_string(value));
        }
    }
};

// Serialization/deserialization helpers
class Serializer {
public:
    static std::string toJson(const Config& config) {
        // Implementation would serialize config to JSON string
        return "{};
    }
    
    static bool fromJson(Config& config, const std::string& json) {
        // Implementation would deserialize config from JSON string
        return true;
    }
    
    static std::string toJson(const Brain& brain) {
        // Implementation would serialize brain to JSON string
        return "{};
    }
    
    static bool fromJson(Brain& brain, const std::string& json) {
        // Implementation would deserialize brain from JSON string
        return true;
    }
};

// Clone helper
class Cloner {
public:
    template<typename T>
    static std::shared_ptr<T> clone(const std::shared_ptr<T>& obj) {
        if (!obj) {
            throw PythonNLMError("Cannot clone null object");
        }
        // For simple types with copy constructor or clone method
        if constexpr (std::is_copy_constructible_v<T>) {
            return std::make_shared<T>(*obj);
        }
        else if constexpr (requires(T t) { t.clone(); }) {
            return std::make_shared<T>(obj->clone());
        }
        else {
            throw PythonNLMError("Object type does not support cloning");
        }
    }
};