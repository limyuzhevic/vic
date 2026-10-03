#pragma once

#include "NLMErrors.hpp"
#include <numeric_limits>
#include <mutex>

namespace nlm {

// High-level error handling macros and utilities for better integration

// Safe file operations
inline std::ifstream openInputFileSafe(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_THROW_IO_ERROR("Failed to open input file: " + filepath);
    }
    return file;
}

inline std::ofstream openOutputFileSafe(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_THROW_IO_ERROR("Failed to open output file: " + filepath);
    }
    return file;
}

// Safe range checking
inline bool safeRangeCheck(double value, double min, double max, bool inclusiveMin = false, bool inclusiveMax = false) {
    bool result = (inclusiveMin ? value >= min : value > min) && 
                  (inclusiveMax ? value <= max : value < max);
    if (!result) {
        NLM_THROW_VALIDATION_ERROR("Value " + std::to_string(value) + " out of range [" + 
                                  (inclusiveMin ? "[" : "(") + std::to_string(min) + ", " +
                                  std::to_string(max) + (inclusiveMax ? "]" : ")") + "]");
    }
    return result;
}

// Safe numeric conversion
inline int safeToInt(double value, double min = std::numeric_limits<int>::lowest(), 
                   double max = std::numeric_limits<int>::max()) {
    if (value < min || value > max) {
        NLM_THROW_VALIDATION_ERROR("Value " + std::to_string(value) + " cannot be safely converted to int");
    }
    return static_cast<int>(value);
}

inline double safeToDouble(int value) {
    return static_cast<double>(value);
}

// Resource validation
inline bool validateResourceAvailable() {
    // Basic memory check - in real implementation could check memory pools, etc.
    if (std::numeric_limits<size_t>::max() / 1024 < 1024) {
        NLM_THROW_RESOURCE_ERROR("Insufficient memory available for operations");
        return false;
    }
    return true;
}

// Thread-safe validation wrapper
class ThreadSafeValidator {
private:
    std::mutex mutex_;
    
public:
    template<typename F, typename... Args>
    auto validate(F&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        std::lock_guard<std::mutex> lock(mutex_);
        return func(std::forward<Args>(args)...);
    }
};

// Error recovery utilities
inline void recoverFromError(std::shared_ptr<NLMError> error) {
    if (error->errorCode() == 2001) { // ResourceError
        // Attempt resource cleanup and retry
        // Implementation depends on specific resource management
    }
}

inline bool canRecoverFromError(std::shared_ptr<NLMError> error) {
    return error->errorCode() == 2001 ||  // ResourceError
            error->errorCode() == 4001;    // RuntimeError
}

} // namespace nlm
