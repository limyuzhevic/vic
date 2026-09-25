#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// Validation utilities for different parameter types
class Validation {
public:
    // Range validation for numeric types
    template<typename T>
    static bool isInRange(T value, T min, T max) {
        static_assert(std::is_arithmetic_v<T>, "Type must be numeric");
        return value >= min && value <= max;
    }
    
    // Range validation with tolerance
    template<typename T>
    static bool isInRangeWithTolerance(T value, T min, T max, T tolerance) {
        static_assert(std::is_arithmetic_v<T>, "Type must be numeric");
        return value >= (min - tolerance) && value <= (max + tolerance);
    }
    
    // String validation
    static bool isValidString(const std::string& value, size_t minLength = 0, 
                            size_t maxLength = std::string::max_size, 
                            const std::string& pattern = "") {
        if (value.length() < minLength || value.length() > maxLength) {
            return false;
        }
        // TODO: Implement pattern matching if needed
        return true;
    }
    
    // Vector range validation (all elements must be in range)
    template<typename T>
    static bool allInRange(const std::vector<T>& values, T min, T max) {
        for (const auto& v : values) {
            if (v < min || v > max) {
                return false;
            }
        }
        return true;
    }
    
    // Vector bounds checking
    template<typename T>
    static bool vectorWithinBounds(const std::vector<T>& values, size_t maxSize) {
        return values.size() <= maxSize;
    }
    
    // Positive value check
    template<typename T>
    static bool isPositive(T value) {
        static_assert(std::is_arithmetic_v<T>, "Type must be numeric");
        return value > 0;
    }
    
    // Non-negative check
    template<typename T>
    static bool isNonNegative(T value) {
        static_assert(std::is_arithmetic_v<T>, "Type must be numeric");
        return value >= 0;
    }
    
    // Check if value is a power of 2
    static bool isPowerOfTwo(size_t value) {
        return value > 0 && (value & (value - 1)) == 0;
    }
    
    // Check if vector elements are strictly increasing
    template<typename T>
    static bool isStrictlyIncreasing(const std::vector<T>& values) {
        for (size_t i = 1; i < values.size(); ++i) {
            if (values[i] <= values[i-1]) {
                return false;
            }
        }
        return true;
    }
    
    // Check if vector elements are non-decreasing
    template<typename T>
    static bool isNonDecreasing(const std::vector<T>& values) {
        for (size_t i = 1; i < values.size(); ++i) {
            if (values[i] < values[i-1]) {
                return false;
            }
        }
        return true;
    }
    
    // String equality check (case insensitive)
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
        if (str1.length() != str2.length()) {
            return false;
        }
        return std::equal(str1.begin(), str1.end(), str2.begin(),
                         [](char c1, char c2) {
                             return std::tolower(c1) == std::tolower(c2);
                         });
    }
    
    // Check if string contains only alphanumeric characters
    static bool isAlphanumeric(const std::string& str) {
        return std::all_of(str.begin(), str.end(),
                         [](char c) { return std::isalnum(static_cast<unsigned char>(c)); });
    }
    
    // Check for ID validity (positive integer)
    static bool isValidId(uint64_t id) {
        return id > 0 && id != UINT64_MAX;
    }
    
    // Check memory allocation safety (check if size would exceed limits)
    static bool isMemorySafe(size_t count, size_t elementSize, 
                           size_t maxTotalBytes = 1024 * 1024 * 1024) {
        // Prevent overflow in multiplication
        if (elementSize > maxTotalBytes || count > maxTotalBytes / elementSize) {
            return false;
        }
        return count * elementSize <= maxTotalBytes;
    }
    
    // Check if value is finite (for floating point types)
    template<typename T>
    static bool isFinite(T value) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        return std::isfinite(value);
    }
    
    // Check if value is normalized (between -1 and 1 for floats)
    template<typename T>
    static bool isNormalized(T value) {
        static_assert(std::is_floating_point_v<T>, "Type must be floating point");
        return std::abs(value) <= 1.0f;
    }
    
    // Validate configuration key naming convention
    static bool isValidConfigKey(const std::string& key) {
        if (key.empty() || key.length() > 100) {
            return false;
        }
        // Should contain only alphanumeric, underscore, and hyphen
        return std::all_of(key.begin(), key.end(),
                          [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
    }
    
    // Comprehensive configuration validation that checks multiple constraints
    static std::vector<std::string> validateConfigConstraints(
        const std::string& key, const std::string& value,
        const std::string& type = "string") {
        std::vector<std::string> errors;
        
        if (!isValidConfigKey(key)) {
            errors.push_back("Invalid configuration key: " + key);
        }
        
        if (type == "string") {
            if (!isValidString(value, 1, 1000)) {
                errors.push_back("Invalid string value for key " + key);
            }
        } else if (type == "number") {
            try {
                double num = std::stod(value);
                if (!isFinite(num)) {
                    errors.push_back("Invalid number (not finite) for key " + key);
                }
            } catch (...) {
                errors.push_back("Invalid numeric value for key " + key);
            }
        }
        
        return errors;
    }
    
    // Check dependency between configuration keys
    static std::vector<std::string> validateConfigDependencies(
        const std::map<std::string, std::string>& config) {
        std::vector<std::string> errors;
        
        // Check neuron count >= region count
        auto neuronCountIt = config.find("neuron_count");
        auto regionCountIt = config.find("region_count");
        
        if (neuronCountIt != config.end() && regionCountIt != config.end()) {
            try {
                size_t neurons = std::stoul(neuronCountIt->second);
                size_t regions = std::stoul(regionCountIt->second);
                
                if (neurons < regions) {
                    errors.push_back("neuron_count (" + neuronCountIt->second + ") "
                                  "must be >= region_count (" + regionCountIt->second + ")");
                }
            } catch (...) {
                errors.push_back("Invalid numeric values in neuron_count or region_count");
            }
        }
        
        // Check that connection probability is between 0 and 1
        auto connProbIt = config.find("connection_probability");
        if (connProbIt != config.end()) {
            try {
                float prob = std::stof(connProbIt->second);
                if (prob < 0.0f || prob > 1.0f) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0");
                }
            } catch (...) {
                errors.push_back("Invalid connection_probability value");
            }
        }
        
        return errors;
    }
};

} // namespace nlm
