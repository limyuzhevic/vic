#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <unordered_map>
#include <optional>
#include <atomic>

namespace nlm {

// Enhanced error categories for NLM
enum class ErrorCategory {
    Configuration,
    Simulation,
    Memory,
    Plasticity,
    Neuromodulation,
    Development,
    IO,
    Network,
    Unknown,
    User
};

// Enhanced exception class with error context
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message,
                 ErrorCategory category = ErrorCategory::Unknown,
                 const std::string& context = "")
        : std::runtime_error(message), category_(category), context_(context) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    NLMException(const std::string& message,
                 const std::exception& cause,
                 ErrorCategory category = ErrorCategory::Unknown,
                 const std::string& context = "")
        : std::runtime_error(message), category_(category), context_(context), 
          cause_(&cause) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    const std::exception* getCause() const { return cause_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(category_) << "] ";
        oss << timestampToString(timestamp_) << " ";
        if (!context_.empty()) {
            oss << "Context: " << context_ << ". ";
        }
        oss << what();
        if (cause_) {
            oss << " (caused by: " << cause_->what() << ")";
        }
        return oss.str();
    }
    
    static std::string timestampToString(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
    
private:
    ErrorCategory category_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
    const std::exception* cause_ = nullptr;
};

// Enhanced logging with error tracking
class ErrorLogger {
public:
    ErrorLogger();
    ~ErrorLogger();
    
    // Log an exception
    void logException(const NLMException& ex, bool shouldThrow = false);
    
    // Log an error with context
    void logError(const std::string& message,
                  ErrorCategory category = ErrorCategory::Unknown,
                  const std::string& context = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& function = "");
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        size_t errorsByCategory[static_cast<size_t>(ErrorCategory::Unknown) + 1] = {0};
        std::chrono::system_clock::time_point firstError;
        std::chrono::system_clock::time_point lastError;
        
        std::string toString() const {
            std::ostringstream oss;
            oss << "Error Statistics:" << std::endl;
            oss << "  Total errors: " << totalErrors << std::endl;
            oss << "  Errors by category:" << std::endl;
            oss << "    Configuration: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Configuration)] << std::endl;
            oss << "    Simulation: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Simulation)] << std::endl;
            oss << "    Memory: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Memory)] << std::endl;
            oss << "    Plasticity: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Plasticity)] << std::endl;
            oss << "    Neuromodulation: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Neuromodulation)] << std::endl;
            oss << "    Development: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Development)] << std::endl;
            oss << "    IO: " << errorsByCategory[static_cast<size_t>(ErrorCategory::IO)] << std::endl;
            oss << "    Network: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Network)] << std::endl;
            oss << "    Unknown: " << errorsByCategory[static_cast<size_t>(ErrorCategory::Unknown)] << std::endl;
            oss << "    User: " << errorsByCategory[static_cast<size_t>(ErrorCategory::User)] << std::endl;
            return oss.str();
        }
    };
    
    ErrorStats getStats() const;
    
    // Clear statistics
    void clearStats();
    
    // Get recent errors
    std::vector<NLMException> getRecentErrors(size_t limit = 10) const;
    
    // Check if errors occurred recently
    bool hasRecentErrors(std::chrono::seconds threshold = std::chrono::seconds(60)) const;
    
    // Set error callback
    using ErrorCallback = std::function<void(const NLMException&)>
    void setErrorCallback(ErrorCallback callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global error logger instance
ErrorLogger& getGlobalErrorLogger();

// Enhanced exception types for specific error scenarios
class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message, const std::string& context = "")
        : NLMException(message, ErrorCategory::Configuration, context) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message, const std::string& context = "")
        : NLMException(message, ErrorCategory::Simulation, context) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message, const std::string& context = "")
        : NLMException(message, ErrorCategory::Memory, context) {}
};

class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message, const std::string& context = "")
        : NLMException(message, ErrorCategory::Plasticity, context) {}
};

// Enhanced macros for error handling
#define NLM_THROW_ERROR(category, message, context) \
    do { \
        std::string full_msg = "Error in " + std::string(context) + ": " + std::string(message); \
        nlm::getGlobalErrorLogger().logError(full_msg, category, context, __FILE__, __LINE__, __func__); \
        throw nlm::NLMException(full_msg, category, context); \
    } while(0)

#define NLM_THROW_CONFIG_ERROR(message, context) \
    NLM_THROW_ERROR(nlm::ErrorCategory::Configuration, message, context)

#define NLM_THROW_SIMULATION_ERROR(message, context) \
    NLM_THROW_ERROR(nlm::ErrorCategory::Simulation, message, context)

#define NLM_THROW_MEMORY_ERROR(message, context) \
    NLM_THROW_ERROR(nlm::ErrorCategory::Memory, message, context)

#define NLM_THROW_PLASTICITY_ERROR(message, context) \
    NLM_THROW_ERROR(nlm::ErrorCategory::Plasticity, message, context)

// Safe execution macros with error recovery
define NLM_SAFE_EXEC(statement, onError) {
    try {
        statement;
    } catch (const std::exception& e) {
        if (onError) {
            onError(e);
        } else {
            throw;
        }
    }
}

} // namespace nlm

// Implementation details
#include "ErrorLogger.tpp"
