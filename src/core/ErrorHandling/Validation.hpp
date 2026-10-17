#pragma once

#include "Error.hpp"
#include <string>
#include <vector>

namespace nlm {

// Simple validation utilities
class ValidationUtils {
public:
    // Parameter validation
    template<typename T>
    static bool isValid(const T& value, const std::string& paramName) {
        if constexpr (std::is_integral_v<T>) {
            if (std::numeric_limits<T>::is_signed && value < std::numeric_limits<T>::min()) {
                return false;
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if (std::isnan(value) || std::isinf(value)) {
                return false;
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (value.empty()) {
                return false;
            }
        }
        return true;
    }
    
    static bool isInRange(double value, double min, double max) {
        return value >= min && value <= max;
    }
    
    // String validation
    static bool matchesPattern(const std::string& value, const std::string& pattern) {
        if (pattern == "alphanumeric") {
            return std::all_of(value.begin(), value.end(), 
                             [](char c) { return std::isalnum(c); });
        }
        return true;
    }
    
    static bool isNotEmpty(const std::string& value) {
        return !value.empty();
    }
    
    static bool isNotEmpty(const std::vector<std::string>& value) {
        return !value.empty();
    }
};

// State validation utilities
class StateValidator {
public:
    template<typename State>
    static bool isValidState(const State& state) {
        if constexpr (requires { state.isValid(); }) {
            return state.isValid();
        }
        return true;
    }
    
    template<typename State, typename Predicate>
    static bool checkCondition(const State& state, const std::string& conditionName,
                              Predicate predicate, const std::string& errorMsg = "") {
        if (!predicate(state)) {
            throw ValidationException(errorMsg.empty() ? "State validation failed: " + conditionName : errorMsg);
        }
        return true;
    }
};

} // namespace nlm
