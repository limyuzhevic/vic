#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <mutex>
#include <format>

namespace nlm {

// Common error codes
enum class ErrorCode {
    Success = 0,
    NullPointer = 1,
    InvalidParameter = 2,
    OutOfBounds = 3,
    ConfigurationError = 4,
    NotInitialized = 5,
    MemoryError = 6,
    ConcurrencyError = 7,
    FileError = 8
};

// Common error information structure
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
};

// Validation utilities namespace
namespace Validation {
    
    // Helper to get current file and line
    const char* GetFile() { return "__FILE__"; }
    int GetLine() { return __LINE__; }
    const char* GetFunction() { return "__func__"; }
    
    // Check if a pointer is null
    template<typename T>
    bool IsNull(T* ptr) {
        return ptr == nullptr;
    }
    
    // Check if a vector is empty
    template<typename T>
    bool IsEmpty(const std::vector<T>& vec) {
        return vec.empty();
    }
    
    // Check bounds for floating point values
    bool IsFinite(float value) {
        return std::isfinite(value);
    }
    
    // Check if value is within range
    template<typename T>
    bool InRange(T value, T min, T max) {
        return value >= min && value <= max;
    }
    
    // Check if value is positive
    template<typename T>
    bool IsPositive(T value) {
        return value > 0;
    }
    
    // Check if value is negative
    template<typename T>
    bool IsNegative(T value) {
        return value < 0;
    }
    
    // Clamp value to range
    template<typename T>
    T Clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    // Create error message
    std::string CreateErrorMessage(ErrorCode code, const std::string& message, 
                                   const std::string& context = "") {
        std::ostringstream oss;
        oss << "[ErrorCode " << static_cast<int>(code) << "] " << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }
    
    // Thread-safe logging for errors
    class ErrorLogger {
    private:
        std::mutex mutex_;
        std::vector<ErrorInfo> errorHistory_;
        static ErrorLogger& GetInstance() {
            static ErrorLogger instance;
            return instance;
        }
    
    public:
        void LogError(ErrorCode code, const std::string& message, 
                     const std::string& file = "", int line = 0, 
                     const std::string& function = "", 
                     const std::string& context = "") {
            std::lock_guard<std::mutex> lock(mutex_);
            
            ErrorInfo error;
            error.code = code;
            error.message = CreateErrorMessage(code, message, context);
            error.file = file.empty() ? GetFile() : file;
            error.line = line == 0 ? GetLine() : line;
            error.function = function.empty() ? GetFunction() : function;
            error.context = context;
            error.timestamp = std::chrono::system_clock::now();
            
            errorHistory_.push_back(error);
            
            // Output to stderr
            std::cerr << "ERROR: " << error.message 
                      << " (File: " << error.file 
                      << ", Line: " << error.line 
                      << ", Function: " << error.function << ")" 
                      << std::endl;
        }
        
        void ClearHistory() {
            std::lock_guard<std::mutex> lock(mutex_);
            errorHistory_.clear();
        }
        
        std::vector<ErrorInfo> GetErrorHistory() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return errorHistory_;
        }
        
        size_t GetErrorCount(ErrorCode code) const {
            std::lock_guard<std::mutex> lock(mutex_);
            size_t count = 0;
            for (const auto& error : errorHistory_) {
                if (error.code == code) {
                    count++;
                }
            }
            return count;
        }
    };
    
    // Convenience macros for error handling
    #define VALIDATE_NULL(ptr, msg) \
        if (::nlm::Validation::IsNull(ptr)) { \
            ::nlm::Validation::ErrorLogger::GetInstance().LogError( \
                ::nlm::ErrorCode::NullPointer, msg, __FILE__, __LINE__, __func__); \
            return; \
        }
    
    #define VALIDATE_NULL_RETURN(ptr, msg, ret) \
        if (::nlm::Validation::IsNull(ptr)) { \
            ::nlm::Validation::ErrorLogger::GetInstance().LogError( \
                ::nlm::ErrorCode::NullPointer, msg, __FILE__, __LINE__, __func__); \
            return ret; \
        }
    
    #define VALIDATE_PARAMETER(cond, msg, code) \
        if (!(cond)) { \
            ::nlm::Validation::ErrorLogger::GetInstance().LogError( \
                code, msg, __FILE__, __LINE__, __func__); \
            return; \
        }
    
    #define VALIDATE_PARAMETER_RETURN(cond, msg, code, ret) \
        if (!(cond)) { \
            ::nlm::Validation::ErrorLogger::GetInstance().LogError( \
                code, msg, __FILE__, __LINE__, __func__); \
            return ret; \
        }
    
    #define CHECK_BOUNDS(value, min, max, msg) \
        if (!(::nlm::Validation::InRange(value, min, max))) { \
            ::nlm::Validation::ErrorLogger::GetInstance().LogError( \
                ::nlm::ErrorCode::OutOfBounds, msg, __FILE__, __LINE__, __func__); \
            return; \
        }
    
    #define LOG_WARNING(msg) \
        ::nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    
    #define LOG_ERROR(msg) \
        ::nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    
    #define LOG_INFO(msg) \
        ::nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    
    // Validate synaptic weight bounds
    bool IsValidWeight(float weight) {
        return Validation::IsFinite(weight) && 
               Validation::InRange(weight, -1.0f, 1.0f);
    }
    
    // Validate time constant bounds
    bool IsValidTimeConstant(float tau) {
        return Validation::IsFinite(tau) && 
               Validation::InRange(tau, 1.0f, 100.0f);
    }
    
    // Validate learning rate bounds
    bool IsValidLearningRate(float rate) {
        return Validation::IsFinite(rate) && 
               Validation::InRange(rate, 0.0f, 1.0f);
    }
    
    // Validate timestamp
    bool IsValidTimestamp(Timestamp timestamp) {
        return timestamp >= 0;
    }
    
    // Validate timestep
    bool IsValidTimestep(float dt) {
        return Validation::IsFinite(dt) && dt > 0.0f;
    }
} // namespace Validation

// Convenience functions for common validation patterns
inline bool ValidateSynapse(const Synapse* synapse, const std::string& context = "") {
    if (!synapse) {
        Validation::ErrorLogger::GetInstance().LogError(
            ErrorCode::NullPointer, 
            "Synapse pointer is null", 
            "", 0, "", context);
        return false;
    }
    return true;
}

inline bool ValidateSpikeHistory(const std::vector<Timestamp>& spikes, 
                                const std::string& context = "") {
    if (Validation::IsEmpty(spikes)) {
        Validation::ErrorLogger::GetInstance().LogError(
            ErrorCode::InvalidParameter,
            "Spike history is empty",
            "", 0, "", context);
        return false;
    }
    return true;
}

inline bool ValidateWeightChange(float delta, float minWeight, float maxWeight) {
    if (!Validation::IsFinite(delta)) {
        Validation::ErrorLogger::GetInstance().LogError(
            ErrorCode::InvalidParameter,
            "Weight change is not finite",
            "", 0, "", "Delta: " + std::to_string(delta));
        return false;
    }
    
    if (std::abs(delta) > 1e-6f) {
        float newWeight = minWeight + delta;  // Assuming current weight is minWeight
        if (!Validation::InRange(newWeight, minWeight, maxWeight)) {
            Validation::ErrorLogger::GetInstance().LogError(
                ErrorCode::OutOfBounds,
                "Weight change would exceed bounds",
                "", 0, "", "New weight: " + std::to_string(newWeight) + ", Bounds: [" + 
                std::to_string(minWeight) + ", " + std::to_string(maxWeight) + "]");
            return false;
        }
    }
    return true;
}

} // namespace nlm

// Include the logging system
#include "Logger.hpp"
