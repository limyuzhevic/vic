// Error handling utilities implementation

#include "ErrorHandling.hpp"
#include <cmath>

namespace nlm {

bool validate_range(size_t index, size_t size, const std::string& context) {
    if (index >= size) {
        NLM_LOG_ERROR("Range violation in " + context + ": index " + std::to_string(index) + 
                     ", size " + std::to_string(size));
        return false;
    }
    return true;
}

bool validate_positive(float value, const std::string& paramName, const std::string& context) {
    if (value <= 0.0f) {
        NLM_LOG_ERROR("Invalid parameter in " + context + ": " + paramName + " must be positive, got " + std::to_string(value));
        return false;
    }
    return true;
}

bool validate_non_negative(float value, const std::string& paramName, const std::string& context) {
    if (value < 0.0f) {
        NLM_LOG_ERROR("Invalid parameter in " + context + ": " + paramName + " must be non-negative, got " + std::to_string(value));
        return false;
    }
    return true;
}

bool validate_array_not_empty(const std::vector<float>& arr, const std::string& paramName, const std::string& context) {
    if (arr.empty()) {
        NLM_LOG_ERROR("Invalid parameter in " + context + ": " + paramName + " cannot be empty");
        return false;
    }
    return true;
}

bool validate_size_within_limits(size_t size, size_t maxSize, const std::string& paramName, const std::string& context) {
    if (size > maxSize) {
        NLM_LOG_ERROR("Invalid parameter in " + context + ": " + paramName + " exceeds maximum size " + std::to_string(maxSize) + ", got " + std::to_string(size));
        return false;
    }
    return true;
}

} // namespace nlm
