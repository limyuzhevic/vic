// Implementation of NLM Error Handling System

#include "ErrorHandling.hpp"

namespace nlm {

// ErrorContext static member definitions
std::vector<std::string> ErrorContext::contextStack_;
std::mutex ErrorContext::contextMutex_;

// Additional error utility functions

/**
 * Check if error code is of specific category
 * 
 * @param error Error to check
 * @param category Error category to check against
 * @return true If error is in the specified category
 */
bool isErrorCategory(const NLMError& error, ErrorCode category) {
    return error.getCode() == category;
}

/**
 * Get severity level as string for logging
 * 
 * @param severity Error severity
 * @return std::string String representation
 */
std::string severityToString(ErrorSeverity severity) {
    return ErrorReporter::severityToString(severity);
}

/**
 * Format error information for display
 * 
 * @param error Error to format
 * @return std::string Formatted error information
 */
std::string formatErrorInfo(const NLMError& error) {
    std::stringstream ss;
    ss << "[" << severityToString(error.getSeverity()) << "] "
       << "Code: " << static_cast<int>(error.getCode()) << " "
       << error.getMessage();
    if (!error.getContext().empty()) {
        ss << " (Context: " << error.getContext() << ")";
    }
    return ss.str();
}

/**
 * Handle error with appropriate recovery
 * 
 * @param error Error to handle
 * @param customRecovery Optional custom recovery action
 * @return true If error was handled and recovered
 */
bool handleError(const NLMError& error, const std::function<bool()>& customRecovery = nullptr) {
    return ErrorReporter::logError(error, customRecovery);
}

/**
 * Create error from exception
 * 
 * @param e Exception to convert
 * @return NLMError Converted error
 */
NLMError createErrorFromException(const std::exception& e) {
    return NLMError(ErrorCode::INTERNAL_ERROR, e.what(), ErrorSeverity::ERROR, "Exception converted to NLM error");
}

/**
 * Error code checker utility
 */
class ErrorCodeChecker {
public:
    /**
     * Check if error code is memory-related
     * 
     * @param code Error code to check
     * @return true If error is memory-related
     */
    static bool isMemoryError(ErrorCode code) {
        return code >= ErrorCode::MEMORY_ALLOCATION_FAILED && 
               code <= ErrorCode::MEMORY_CORRUPTION_DETECTED;
    }
    
    /**
     * Check if error code is configuration-related
     * 
     * @param code Error code to check
     * @return true If error is configuration-related
     */
    static bool isConfigurationError(ErrorCode code) {
        return code >= ErrorCode::CONFIG_FILE_NOT_FOUND && 
               code <= ErrorCode::CONFIG_VALUE_INVALID;
    }
    
    /**
     * Check if error code is validation-related
     * 
     * @param code Error code to check
     * @return true If error is validation-related
     */
    static bool isValidationError(ErrorCode code) {
        return code >= ErrorCode::VALIDATION_FAILED && 
               code <= ErrorCode::VALIDATION_CONSTRAINT_VIOLATION;
    }
    
    /**
     * Check if error code is performance-related
     * 
     * @param code Error code to check
     * @return true If error is performance-related
     */
    static bool isPerformanceError(ErrorCode code) {
        return code >= ErrorCode::PERFORMANCE_BOTTLE_SNECOUNTED && 
               code <= ErrorCode::PERFORMANCE_SIMD_NOT_AVAILABLE;
    }
};

/**
 * Error statistics collector
 */
class ErrorStatistics {
public:
    /**
     * Record an error
     * 
     * @param error Error to record
     */
    void recordError(const NLMError& error) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        errorCount_++;
        errorCounts_[error.getCode()]++;
        severityCounts_[error.getSeverity()]++;
        
        // Track most recent errors
        recentErrors_.push_back(error);
        if (recentErrors_.size() > maxRecentErrors_) {
            recentErrors_.pop_front();
        }
        
        // Update statistics
        updateStatistics();
    }
    
    /**
     * Get total error count
     * 
     * @return size_t Total error count
     */
    size_t getTotalErrorCount() const { return errorCount_; }
    
    /**
     * Get error count for specific code
     * 
     * @param code Error code
     * @return size_t Error count for code
     */
    size_t getErrorCount(ErrorCode code) const {
        auto it = errorCounts_.find(code);
        return it != errorCounts_.end() ? it->second : 0;
    }
    
    /**
     * Get recent errors
     * 
     * @param count Number of recent errors to return
     * @return std::deque<NLMError> Recent errors
     */
    std::deque<NLMError> getRecentErrors(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        std::deque<NLMError> result(recentErrors_.begin(), 
                                   std::min(recentErrors_.end(), 
                                           recentErrors_.begin() + count));
        return result;
    }
    
    /**
     * Get error statistics summary
     * 
     * @return std::string Statistics summary
     */
    std::string getStatisticsSummary() const {
        std::stringstream ss;
        ss << "Error Statistics:\n"
           << "  Total errors: " << errorCount_ << "\n"
           << "  Error codes:\n";
        
        for (const auto& pair : errorCounts_) {
            ss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
        
        ss << "  Severity levels:\n";
        for (const auto& pair : severityCounts_) {
            ss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * Reset statistics
     */
    void reset() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        errorCount_ = 0;
        errorCounts_.clear();
        severityCounts_.clear();
        recentErrors_.clear();
        updateStatistics();
    }

private:
    void updateStatistics() {
        // Placeholder for statistics update logic
    }
    
    std::mutex statsMutex_;
    size_t errorCount_ = 0;
    std::map<ErrorCode, size_t> errorCounts_;
    std::map<ErrorSeverity, size_t> severityCounts_;
    std::deque<NLMError> recentErrors_;
    static const size_t maxRecentErrors_ = 100;
};

// Global error statistics instance
ErrorStatistics globalErrorStats;

/**
 * Get global error statistics
 * 
 * @return ErrorStatistics& Reference to global statistics
 */
ErrorStatistics& getErrorStatistics() {
    return globalErrorStats;
}

/**
 * Error logging callback function type
 */
using ErrorLoggingCallback = std::function<void(const NLMError&)>;

/**
 * Register error logging callback
 * 
 * @param callback Callback function to register
 */
void registerErrorLoggingCallback(const ErrorLoggingCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    errorLoggingCallbacks_.push_back(callback);
}

/**
 * Log error using registered callbacks
 * 
 * @param error Error to log
 */
void logErrorWithCallbacks(const NLMError& error) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    for (const auto& callback : errorLoggingCallbacks_) {
        try {
            callback(error);
        } catch (const std::exception& e) {
            std::cerr << "Error logging callback failed: " << e.what() << std::endl;
        }
    }
}

// Global error logging callbacks
std::vector<ErrorLoggingCallback> errorLoggingCallbacks_;
std::mutex callbackMutex_;

} // namespace nlm