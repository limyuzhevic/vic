#pragma once

#include "Error.hpp"
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <queue>
#include <thread>
#include <atomic>

namespace nlm {

// Error statistics
struct ErrorStats {
    uint64_t totalErrors{0};
    uint64_t errorsByCategory[255]{0}; // Simplified category array
    uint64_t concurrentErrors{0};
    std::chrono::system_clock::time_point lastErrorTime;
    std::chrono::system_clock::time_point firstErrorTime;
    
    ErrorStats() {
        firstErrorTime = std::chrono::system_clock::now();
    }
    
    void recordError(const ErrorContext& context) {
        totalErrors++;
        errorsByCategory[static_cast<size_t>(context.category)]++;
        concurrentErrors++;
    }
    
    void errorCompleted() {
        concurrentErrors--;
    }
    
    std::string getStatsSummary() const {
        std::ostringstream oss;
        oss << "Total Errors: " << totalErrors << "\n";
        oss << "Concurrent Errors: " << concurrentErrors << "\n";
        return oss.str();
    }
};

// Main Error Reporting Infrastructure
class ErrorReportingInfrastructure {
public:
    static ErrorReportingInfrastructure& getInstance();
    
    void initialize(const std::string& configFile = "");
    
    void shutdown() noexcept;
    
    // Add error handlers
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler);
    void removeErrorHandler(const IErrorHandler* handler);
    
    // Set primary error handler
    void setPrimaryErrorHandler(std::shared_ptr<IErrorHandler> handler);
    
    // Get error handlers
    std::vector<std::shared_ptr<IErrorHandler>> getErrorHandlers() const;
    
    // Global error handling functions
    void reportError(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                    ErrorCategory category = ErrorCategory::None);
    
    void reportError(const ErrorContext& context);
    
    void reportException(const std::exception& e);
    
    void setGlobalErrorLevel(LogLevel level);
    LogLevel getGlobalErrorLevel() const;
    
    // Metrics and recovery
    void enableMetrics(bool enable);
    bool isMetricsEnabled() const;
    
    ErrorStats getErrorStats() const;
    
    void enableGracefulDegradation(bool enable);
    bool isGracefulDegradationEnabled() const;
    
    // Error recovery
    bool attemptRecovery(const ErrorContext& context);
    
    // Configuration
    void configureFromFile(const std::string& filepath);
    void configure(const std::string& key, const std::string& value);
    
    // Memory management optimizations
    static constexpr bool isSmallObject() noexcept { return true; }
    
private:
    ErrorReportingInfrastructure();
    ~ErrorReportingInfrastructure() noexcept;
    
    ErrorReportingInfrastructure(const ErrorReportingInfrastructure&) = delete;
    ErrorReportingInfrastructure& operator=(const ErrorReportingInfrastructure&) = delete;
    
    void initializeDefaultHandlers();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static ErrorReportingInfrastructure* instance_;
};

// Convenience functions for error reporting
inline void reportError(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                       ErrorCategory category = ErrorCategory::None) {
    ErrorReportingInfrastructure::getInstance().reportError(message, code, category);
}

inline void reportError(const ErrorContext& context) {
    ErrorReportingInfrastructure::getInstance().reportError(context);
}

inline void reportException(const std::exception& e) {
    ErrorReportingInfrastructure::getInstance().reportException(e);
}

inline void setGlobalErrorLevel(LogLevel level) {
    ErrorReportingInfrastructure::getInstance().setGlobalErrorLevel(level);
}

inline ErrorStats getErrorStats() {
    return ErrorReportingInfrastructure::getInstance().getErrorStats();
}

// Simple execution wrapper
size_t safeExecute(const std::function<void()>& task,
                  const std::string& taskName = "",
                  size_t maxRetries = 3,
                  double retryDelay = 0.1);

// Context-aware execution wrapper
template<typename Func>
auto safeExecuteWithContext(const std::string& contextName,
                           Func&& func,
                           const std::string& errorContext = "",
                           ErrorCode errorCode = ErrorCode::Unknown) {
    try {
        return func();
    } catch (const NLMException& e) {
        if (errorContext.empty()) {
            ErrorReportingInfrastructure::getInstance().reportError(
                contextName + ": " + e.what(), e.getCode(), e.getCategory());
        } else {
            ErrorReportingInfrastructure::getInstance().reportError(errorContext, errorCode);
        }
        throw;
    } catch (const std::exception& e) {
        ErrorReportingInfrastructure::getInstance().reportError(
            contextName + ": " + e.what());
        throw;
    }
}

} // namespace nlm

