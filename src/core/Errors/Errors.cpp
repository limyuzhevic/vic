#include "Errors.hpp"
#include <mutex>

namespace nlm {

// Error reporting function to integrate with Logger
void logError(const NLMError& error) {
    // Get global logger
    auto& logger = Logger::getGlobal();
    
    // Convert error category to log level
    LogLevel level;
    switch (error.getCategory()) {
        case ErrorCategory::System:
        case ErrorCategory::Memory:
            level = LogLevel::Critical;
            break;
        case ErrorCategory::Configuration:
        case ErrorCategory::Validation:
            level = LogLevel::Error;
            break;
        case ErrorCategory::File:
        case ErrorCategory::Network:
            level = LogLevel::Error;
            break;
        case ErrorCategory::Plasticity:
            level = LogLevel::Warning;
            break;
        case ErrorCategory::Initialization:
        case ErrorCategory::Simulation:
            level = LogLevel::Error;
            break;
        case ErrorCategory::Python:
            level = LogLevel::Warning;
            break;
        default:
            level = LogLevel::Error;
            break;
    }
    
    // Log the error with full context
    logger.error(error.what(), error.getFilePath(), 0, "");
}

// Convenience function to log errors with type information
void logErrorWithType(const NLMError& error, const std::string& context) {
    auto& logger = Logger::getGlobal();
    logger.error("Error Type: " + error.getErrorType() + " - " + context + "\n" + error.what(),
                 error.getFilePath(), 0, "");
}

// Error handling macro implementations
#define NLM_THROW_ERROR(type, message, ...) throw type(message, ##__VA_ARGS__)

#define NLM_REPORT_ERROR(logger, error) \
    if (&(error) != nullptr) { \
        logger->error(error.what()); \
    }

// Error recovery utilities
namespace ErrorRecovery {
    
    template<typename Func>
    bool handleWithRecovery(Func func, const std::string& errorContext = "") {
        try {
            func();
            return true;
        } catch (const NLMError& e) {
            logError(e);
            if (!errorContext.empty()) {
                logErrorWithType(e, "Context: " + errorContext);
            }
        } catch (const std::exception& e) {
            SystemError systemError(std::string("Unexpected exception: ") + e.what());
            logError(systemError);
        } catch (...) {
            SystemError systemError("Unknown error occurred");
            logError(systemError);
        }
        return false;
    }
    
    template<typename Func>
    bool handleWithRetry(Func func, int maxRetries = 3, int retryDelayMs = 100) {
        int attempts = 0;
        while (attempts < maxRetries) {
            try {
                func();
                return true;
            } catch (const NLMError& e) {
                logErrorWithType(e, "Attempt " + std::to_string(++attempts) + "/" + std::to_string(maxRetries));
                if (attempts >= maxRetries) throw;
                
                // Simple delay (in real implementation, use proper sleep)
                // std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
            }
        }
        return false;
    }
}

// Error category utility functions
std::string getCategoryName(ErrorCategory category) {
    return NLMError::categoryToString(category);
}

// Error chain management
class ErrorChain {
private:
    std::vector<std::shared_ptr<NLMError>> errors_;
    std::mutex mutex_;
    
public:
    void addError(std::shared_ptr<NLMError> error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(std::move(error));
    }
    
    bool hasErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !errors_.empty();
    }
    
    std::vector<std::shared_ptr<NLMError>> getErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return errors_;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
    }
    
    void logAllErrors(const Logger& logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& error : errors_) {
            logger.error(error->what());
        }
    }
};

// Global error chain
inline ErrorChain& getGlobalErrorChain() {
    static ErrorChain chain;
    return chain;
}

// Enhanced logging for error reporting
void logErrorWithContext(const NLMError& error, const Logger& logger, const std::string& additionalContext = "") {
    std::ostringstream oss;
    oss << "=== " << error.getErrorType() << " ===\n";
    oss << "Category: " << error.getCategoryString() << "\n";
    oss << "Message: " << error.what() << "\n";
    
    if (!additionalContext.empty()) {
        oss << "Additional Context: " << additionalContext << "\n";
    }
    
    oss << "============================\n";
    
    logger.error(oss.str());
}

// Error suppression utilities
class ErrorSuppression {
private:
    std::unordered_map<ErrorCategory, int> suppressionCount_;
    std::mutex mutex_;
    int globalSuppressionCount_ = 0;
    
public:
    bool shouldSuppress(ErrorCategory category) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check global suppression
        if (globalSuppressionCount_ > 0) return true;
        
        // Check category-specific suppression
        if (suppressionCount_.find(category) != suppressionCount_.end()) {
            return suppressionCount_[category] > 0;
        }
        
        return false;
    }
    
    void suppress(ErrorCategory category, int count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        suppressionCount_[category] += count;
    }
    
    void suppressAll(int count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        globalSuppressionCount_ += count;
    }
    
    void resume(ErrorCategory category, int count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (suppressionCount_.find(category) != suppressionCount_.end()) {
            suppressionCount_[category] = std::max(0, suppressionCount_[category] - count);
            if (suppressionCount_[category] == 0) {
                suppressionCount_.erase(category);
            }
        }
    }
    
    void resumeAll(int count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        globalSuppressionCount_ = std::max(0, globalSuppressionCount_ - count);
    }
};

inline ErrorSuppression& getGlobalErrorSuppression() {
    static ErrorSuppression suppression;
    return suppression;
}

} // namespace nlm