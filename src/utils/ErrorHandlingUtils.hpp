// ErrorHandlingUtils.hpp - Comprehensive error handling utilities for NLM
#pragma once

#include "src/core/Logger/Logger.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <cassert>

namespace nlm {
namespace utils {

// Error categories for NLM
enum class ErrorCategory {
    Configuration,
    Memory,
    Plasticity,
    Simulation,
    IO,
    Initialization,
    Validation,
    System
};

// Detailed error information
struct ErrorInfo {
    ErrorCategory category;
    std::string message;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> contextData;
    std::shared_ptr<std::exception> underlyingException;
    
    ErrorInfo(ErrorCategory cat, const std::string& msg, 
              const std::string& file = __FILE__, int line = __LINE__)
        : category(cat), message(msg), file(file), line(line), 
          timestamp(std::chrono::system_clock::now()), 
          underlyingException(nullptr) {}
};

// Custom exception class with context
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCategory category, const std::string& message,
                 const std::string& file = __FILE__, int line = __LINE__)
        : std::runtime_error(formatMessage(category, message, file, line)),
          category_(category), file_(file), line_(line) {
        // Add to error log
        logError(category, message, file, line);
    }
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    
    // Create a copy with additional context
    NLMException withContext(const std::string& key, const std::string& value) const {
        NLMException newEx = *this;
        newEx.context_[key] = value;
        return newEx;
    }
    
    const std::map<std::string, std::string>& getContext() const { return context_; }
    
private:
    std::string formatMessage(ErrorCategory category, const std::string& message,
                              const std::string& file, int line) {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(category) << "] " << message;
        oss << " (" << file << ":" << line << ")";
        return oss.str();
    }
    
    void logError(ErrorCategory category, const std::string& message,
                  const std::string& file, int line) {
        std::string categoryStr;
        switch (category) {
            case ErrorCategory::Configuration: categoryStr = "CONFIG"; break;
            case ErrorCategory::Memory: categoryStr = "MEMORY"; break;
            case ErrorCategory::Plasticity: categoryStr = "PLASTICITY"; break;
            case ErrorCategory::Simulation: categoryStr = "SIMULATION"; break;
            case ErrorCategory::IO: categoryStr = "IO"; break;
            case ErrorCategory::Initialization: categoryStr = "INITIALIZATION"; break;
            case ErrorCategory::Validation: categoryStr = "VALIDATION"; break;
            case ErrorCategory::System: categoryStr = "SYSTEM"; break;
            default: categoryStr = "UNKNOWN";
        }
        
        NLM_LOG_ERROR(std::string("[") + categoryStr + "] " + message + 
                     " at " + file + ":" + std::to_string(line));
    }
    
    ErrorCategory category_;
    std::string file_;
    int line_;
    mutable std::map<std::string, std::string> context_;
};

// Error handling macros
#define NLM_THROW(category, message) throw nlm::utils::NLMException(category, message)

#define NLM_THROW_WITH_CONTEXT(category, message, key, value) \
    throw nlm::utils::NLMException(category, message).withContext(key, value)

#define NLM_CHECK(condition, category, message) \
    if (!(condition)) { NLM_THROW(category, message); }

#define NLM_CHECK_PTR(ptr, category, message) \
    if (!(ptr)) { NLM_THROW(category, message); }

// Error recovery utilities
class ErrorRecovery {
public:
    // Recovery action type
    enum class RecoveryAction {
        Retry,
        Fallback,
        Skip,
        Halt,
        PartialRecovery
    };
    
    // Recovery strategy
    struct RecoveryStrategy {
        ErrorCategory category;
        RecoveryAction action;
        int maxRetries;
        float retryDelay;
        std::function<bool(const ErrorInfo&)> shouldRetry;
        std::function<void(const ErrorInfo&)> onRecovery;
    };\n    
    // Configuration
    struct Configuration {
        std::vector<RecoveryStrategy> strategies;
        bool enableAutoRecovery = true;
        int maxConsecutiveErrors = 5;
        std::chrono::seconds errorCooldown = std::chrono::seconds(10);
    };
    
    // Error statistics
    struct Statistics {
        int totalErrors = 0;
        int recoveredErrors = 0;
        std::map<ErrorCategory, int> errorsByCategory;
        std::chrono::system_clock::time_point lastErrorTime;
        bool isInCooldown = false;
    };
    
    ErrorRecovery(const Configuration& config = Configuration()) 
        : config_(config), stats_() {}
    
    // Handle error with recovery
    bool handleError(const ErrorInfo& error) {
        stats_.totalErrors++;
        stats_.errorsByCategory[error.category]++;
        stats_.lastErrorTime = error.timestamp;
        
        if (isInCooldown()) {
            NLM_LOG_WARNING("Error handled during cooldown: " + error.message);
            return false;
        }
        
        // Find matching recovery strategy
        for (const auto& strategy : config_.strategies) {
            if (strategy.category == error.category && 
                strategy.shouldRetry(error)) {
                
                if (strategy.action == RecoveryAction::Halt) {
                    NLM_LOG_ERROR("Error recovery strategy halted execution: " + error.message);
                    return false;
                } else if (strategy.action == RecoveryAction::Skip) {
                    NLM_LOG_WARNING("Skipping operation due to error: " + error.message);
                    return true;
                } else if (strategy.action == RecoveryAction::Retry) {
                    // Implement retry logic (simplified)
                    NLM_LOG_INFO("Retrying operation after error: " + error.message);
                    stats_.recoveredErrors++;
                    strategy.onRecovery(error);
                    return true;
                }
            }
        }
        
        // No specific strategy found, use default
        NLM_LOG_ERROR("No recovery strategy found for error: " + error.message);
        return false;
    }
    
    // Check if in cooldown period
    bool isInCooldown() const {
        if (stats_.totalErrors >= config_.maxConsecutiveErrors) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats_.lastErrorTime);
            if (duration < config_.errorCooldown) {
                stats_.isInCooldown = true;
                return true;
            }
            stats_.isInCooldown = false;
        }
        return stats_.isInCooldown;
    }
    
    // Get error statistics
    const Statistics& getStatistics() const { return stats_; }
    
    // Reset statistics
    void resetStatistics() {
        stats_ = Statistics();
    }
    
private:
    Configuration config_;
    Statistics stats_;
};

// Resource management utilities
class ResourceManager {
public:
    template<typename T>
    class ResourceHandle {
    public:
        ResourceHandle(std::shared_ptr<T> resource, ResourceManager* manager = nullptr)
            : resource_(resource), manager_(manager), valid_(true) {}
        
        ~ResourceHandle() {
            release();
        }
        
        ResourceHandle(const ResourceHandle&) = delete;
        ResourceHandle& operator=(const ResourceHandle&) = delete;
        
        ResourceHandle(ResourceHandle&& other) noexcept
            : resource_(std::move(other.resource_)), manager_(other.manager_), valid_(other.valid_) {
            other.valid_ = false;
        }
        
        ResourceHandle& operator=(ResourceHandle&& other) noexcept {
            if (this != &other) {
                release();
                resource_ = std::move(other.resource_);
                manager_ = other.manager_;
                valid_ = other.valid_;
                other.valid_ = false;
            }
            return *this;
        }
        
        // Dereference operators
        T* operator->() { return resource_.get(); }
        const T* operator->() const { return resource_.get(); }
        T& operator*() { return *resource_; }
        const T& operator*() const { return *resource_; }
        
        // Conversion to bool
        explicit operator bool() const { return valid_ && resource_ != nullptr; }
        bool operator!() const { return !valid_ || resource_ == nullptr; }
        
        // Release the resource
        void release() {
            if (valid_ && manager_) {
                manager_->releaseResource(this);
                valid_ = false;
            }
        }
        
        // Get resource
        T* get() { return resource_.get(); }
        const T* get() const { return resource_.get(); }
        
        // Check validity
        bool isValid() const { return valid_ && resource_ != nullptr; }
        
    private:
        std::shared_ptr<T> resource_;
        ResourceManager* manager_;
        bool valid_;
        
        friend class ResourceManager;
    };
    
    ResourceManager() = default;
    ~ResourceManager() {
        // Release all resources on destruction
        for (auto* handle : activeHandles_) {
            delete handle;
        }
        activeHandles_.clear();
    }
    
    // Allocate a resource
    template<typename T, typename... Args>
    ResourceHandle<T> allocate(Args&&... args) {
        auto resource = std::make_shared<T>(std::forward<Args>(args)...);
        auto* handle = new ResourceHandle<T>(resource, this);
        activeHandles_.insert(handle);
        return *handle;
    }
    
    // Deallocate a resource
    template<typename T>
    void deallocate(ResourceHandle<T>& handle) {
        if (handle.isValid()) {
            handle.release();
        }
    }
    
    // Cleanup all resources
    void cleanup() {
        for (auto* handle : activeHandles_) {
            delete handle;
        }
        activeHandles_.clear();
    }
    
private:
    template<typename T>
    void releaseResource(ResourceHandle<T>* handle) {
        auto it = activeHandles_.find(handle);
        if (it != activeHandles_.end()) {
            activeHandles_.erase(it);
        }
    }
    
    std::unordered_set<void*> activeHandles_;
};

// Validation utilities
class ValidationUtils {
public:
    // Validate configuration values
    static bool validateConfigValue(const std::string& key, const ConfigValue& value,
                                   ErrorCategory category = ErrorCategory::Validation) {
        // Add validation logic for different key-value types
        if (key == "neuron_count" && std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        } else if (key == "connection_probability" && std::holds_alternative<float>(value)) {
            float prob = std::get<float>(value);
            return prob >= 0.0f && prob <= 1.0f;
        } else if (key == "simulation_timestep" && std::holds_alternative<double>(value)) {
            double dt = std::get<double>(value);
            return dt > 0.0 && dt < 1.0; // Reasonable timestep range
        }
        
        // Default validation passes for unknown keys
        return true;
    }
    
    // Validate brain state
    static bool validateBrainState(const class Brain& brain, 
                                  ErrorCategory category = ErrorCategory::Validation) {
        // Check basic brain invariants
        if (brain.getTotalNeuronCount() == 0) {
            NLM_THROW(category, "Brain initialized with 0 neurons.");
            return false;
        }
        
        if (brain.getTotalSynapseCount() < 0) {
            NLM_THROW(category, "Invalid synapse count in brain.");
            return false;
        }
        
        return true;
    }
    
    // Validate memory allocation
    static bool validateMemoryAllocation(size_t size, size_t maxSize,
                                        ErrorCategory category = ErrorCategory::Memory) {
        if (size > maxSize) {
            NLM_THROW(category, "Memory allocation exceeds maximum allowed size.");
            return false;
        }
        return true;
    }
    
    // Validate range
    template<typename T>
    static bool validateRange(const T& value, const T& min, const T& max,
                             ErrorCategory category = ErrorCategory::Validation,
                             const std::string& description = "value") {
        if (value < min || value > max) {
            NLM_THROW(category, "Invalid " + description + ": " + std::to_string(value) +
                         " (valid range: " + std::to_string(min) + " to " + std::to_string(max) + ")");
            return false;
        }
        return true;
    }
};\n
// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct TimingData {
        std::string operation;
        std::chrono::duration<double> duration;
        int count;
        double totalDuration() const { return duration.count() * count; }
        double averageDuration() const { return count > 0 ? totalDuration() / count : 0.0; }
    };
    
    PerformanceMonitor() = default;
    
    // Start timing an operation
    void startTiming(const std::string& operation) {
        activeTimers_[operation] = std::chrono::high_resolution_clock::now();
    }
    
    // Stop timing an operation
    void stopTiming(const std::string& operation) {
        auto it = activeTimers_.find(operation);
        if (it != activeTimers_.end()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - it->second);
            
            // Update timing data
            auto& timingData = timingData_[operation];
            timingData.operation = operation;
            timingData.duration += duration;
            timingData.count++;
            
            activeTimers_.erase(it);
        }
    }
    
    // Get timing data for an operation
    TimingData getTimingData(const std::string& operation) const {
        auto it = timingData_.find(operation);
        return (it != timingData_.end()) ? it->second : TimingData{operation, std::chrono::duration<double>(0.0), 0};
    }
    
    // Get all timing data
    std::map<std::string, TimingData> getAllTimingData() const { return timingData_; }
    
    // Log performance summary
    void logPerformanceSummary() const {
        NLM_LOG_INFO("=== Performance Summary ===");
        for (const auto& [operation, data] : timingData_) {
            NLM_LOG_INFO(std::string("Operation: ") + operation +
                        " | Calls: " + std::to_string(data.count) +
                        " | Total time: " + std::to_string(data.totalDuration()) + "s" +
                        " | Average: " + std::to_string(data.averageDuration()) + "s");
        }
    }
    
    // Clear all timing data
    void clear() {
        timingData_.clear();
        activeTimers_.clear();
    }
    
private:
    std::map<std::string, std::chrono::high_resolution_clock::time_point> activeTimers_;
    std::map<std::string, TimingData> timingData_;
};

} // namespace utils

// Convenience functions
inline utils::ErrorRecovery& getErrorRecovery() {
    static utils::ErrorRecovery recovery;
    return recovery;
}

inline utils::ResourceManager& getResourceManager() {
    static utils::ResourceManager manager;
    return manager;
}

inline utils::PerformanceMonitor& getPerformanceMonitor() {
    static utils::PerformanceMonitor monitor;
    return monitor;
}

// Error handling macros for common operations
#define NLM_SAFE_OPERATION(operation, errorCategory, errorMessage) \
    try { operation; } catch (const nlm::utils::NLMException& e) { \
        NLM_LOG_ERROR(errorMessage); \
        return false; \
    } catch (const std::exception& e) { \
        NLM_LOG_ERROR(std::string("Unexpected error: ") + e.what()); \
        return false; \
    }

#define NLM_THROW_IF(condition, category, message) \
    if (condition) { NLM_THROW(category, message); }

} // namespace nlm

#endif // ERROR_HANDLING_UTILS_HPP