// Error handling utilities for NLM codebase

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Safe pointer access helper function
// Logs error if pointer is null and returns default value
template<typename T>
T* safe_ptr_access(T* ptr, const std::string& context) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pointer access in " + context);
    }
    return ptr;
}

template<typename T>
const T* safe_ptr_access(const T* ptr, const std::string& context) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pointer access in " + context);
    }
    return ptr;
}

// Safe shared_ptr access helper function
template<typename T>
T* safe_ptr_access(const std::shared_ptr<T>& ptr, const std::string& context) {
    if (!ptr) {
        NLM_LOG_ERROR("Null shared_ptr access in " + context);
    }
    return ptr.get();
}

// Range validation helper function
bool validate_range(size_t index, size_t size, const std::string& context);

// Parameter validation helper functions
bool validate_positive(float value, const std::string& paramName, const std::string& context);
bool validate_non_negative(float value, const std::string& paramName, const std::string& context);
bool validate_array_not_empty(const std::vector<float>& arr, const std::string& paramName, const std::string& context);
bool validate_size_within_limits(size_t size, size_t maxSize, const std::string& paramName, const std::string& context);

// Error handling macros
#define NLM_SAFE_ACCESS(ptr, context) safe_ptr_access(ptr, context)
#define NLM_VALIDATE_RANGE(index, size, context) validate_range(index, size, context)
#define NLM_VALIDATE_POS(value, param, context) validate_positive(value, param, context)

} // namespace nlm
