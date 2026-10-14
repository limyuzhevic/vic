#pragma once

#include "core/Exceptions/Exceptions.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Exception handling manager
class ExceptionHandler {
public:
    static ExceptionHandler& getInstance() {
        static ExceptionHandler instance;
        return instance;
    }
    
    // Handle exception
    void handleException(const std::shared_ptr<NLMException>& exception) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Log exception
        logException(exception);
        
        // Store for analysis
        exceptions_history_.push_back(exception);
        
        // Apply handler based on severity
        switch (exception->getSeverity()) {
            case ErrorSeverity::Debug:
                break; // Only log, no action needed
            case ErrorSeverity::Info:
                break; // Only log, no action needed
            case ErrorSeverity::Warning:
                break; // Only log, no action needed
            case ErrorSeverity::Error:
                break; // Log and continue
            case ErrorSeverity::Critical:
                // Try recovery operations
                tryRecoveryOperations(exception);
                break;
            case ErrorSeverity::Fatal:
                // Log and exit
                logFatalException(exception);
                break;
        }
    }
    
    // Set exception callback
    void setExceptionCallback(std::function<void(const std::shared_ptr<NLMException>&)> callback) {
        exception_callback_ = callback;
    }
    
    // Get exception statistics
    std::unordered_map<std::string, int> getExceptionStatistics() const {
        std::unordered_map<std::string, int> stats;
        for (const auto& exception : exceptions_history_) {
            std::string category_str = getCategoryString(exception->getCategory());
            stats[category_str]++;
        }
        return stats;
    }
    
    // Get recent exceptions
    std::vector<std::shared_ptr<NLMException>> getRecentExceptions(size_t count) const {
        if (exceptions_history_.size() <= count) {
            return exceptions_history_;
        }
        return std::vector<std::shared_ptr<NLMException>>(
            exceptions_history_.end() - count, exceptions_history_.end()
        );
    }
    
    // Clear exception history
    void clearExceptionHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        exceptions_history_.clear();
    }
    
    // Enable/disable exception handling
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
private:
    ExceptionHandler() : enabled_(true) {}
    
    // Log exception
    void logException(const std::shared_ptr<NLMException>& exception) {
        std::ostringstream oss;
        oss << "NLM Exception: " << exception->toString() << std::endl;
        oss << exception->formatError();
        
        // Use logger if available
        if (Logger::getGlobalLogger()) {
            Logger::getGlobalLogger()->log(oss.str(), LogLevel::Error);
        } else {
            std::cerr << oss.str();
        }
        
        // Call user callback if set
        if (exception_callback_) {
            exception_callback_(exception);
        }
    }
    
    // Log fatal exception
    void logFatalException(const std::shared_ptr<NLMException>& exception) {
        std::ostringstream oss;
        oss << "FATAL NLM Exception: " << exception->toString() << std::endl;
        oss << exception->formatError();
        oss << std::endl;
        oss << "Application will terminate due to fatal error." << std::endl;
        
        if (Logger::getGlobalLogger()) {
            Logger::getGlobalLogger()->log(oss.str(), LogLevel::Critical);
        } else {
            std::cerr << oss.str();
        }
        
        // Call user callback if set
        if (exception_callback_) {
            exception_callback_(exception);
        }
        
        // Exit the application
        std::exit(1);
    }
    
    // Try recovery operations
    void tryRecoveryOperations(const std::shared_ptr<NLMException>& exception) {
        // Implement recovery strategies based on exception type
        switch (exception->getCategory()) {
            case ErrorCategory::IOError:
                // Log file error and attempt to continue
                NLM_LOG_WARNING("IO error occurred. Continuing with available data.");
                break;
            case ErrorCategory::Memory:
                // Memory error - try to free resources
                NLM_LOG_WARNING("Memory error occurred. Attempting recovery...");
                break;
            case ErrorCategory::Validation:
                // Validation error - try to use defaults
                NLM_LOG_WARNING("Validation error occurred. Using default values.");
                break;
            case ErrorCategory::Configuration:
                // Configuration error - try to reload configuration
                NLM_LOG_WARNING("Configuration error occurred. Attempting to reload configuration.");
                break;
            default:
                NLM_LOG_WARNING("Unexpected error occurred. Continuing with caution.");
                break;
        }
    }
    
    // Get category string
    std::string getCategoryString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Validation: return "Validation";
            case ErrorCategory::Runtime: return "Runtime";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::IOError: return "IOError";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::System: return "System";
            case ErrorCategory::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    std::mutex mutex_;
    std::vector<std::shared_ptr<NLMException>> exceptions_history_;
    std::function<void(const std::shared_ptr<NLMException>&)> exception_callback_;
    bool enabled_;
};

// Convenience functions for exception handling
namespace ExceptionUtils {
    // Handle exception with default handler
    inline void handle(const std::shared_ptr<NLMException>& exception) {
        ExceptionHandler::getInstance().handleException(exception);
    }
    
    // Handle exception with custom callback
    inline void handle(
        const std::shared_ptr<NLMException>& exception,
        std::function<void(const std::shared_ptr<NLMException>&)> callback
    ) {
        ExceptionHandler::getInstance().setExceptionCallback(callback);
        ExceptionHandler::getInstance().handleException(exception);
    }
    
    // Create and handle exception
    template<typename ExceptionType, typename... Args>
    inline void throwAndHandle(const Args&... args) {
        auto exception = std::make_shared<ExceptionType>(args...);
        ExceptionHandler::getInstance().handleException(exception);
    }
    
    // Safe operation with exception handling
    template<typename Func, typename... Args>
    inline bool safeOperation(Func func, const Args&... args) {
        try {
            func(args...);
            return true;
        } catch (const NLMException& e) {
            ExceptionHandler::getInstance().handleException(std::make_shared<NLMException>(e));
            return false;
        } catch (const std::exception& e) {
            auto generic_exception = ExceptionFactory::createGenericException(
                std::string("Standard exception: ") + e.what(),
                ErrorCategory::System
            );
            ExceptionHandler::getInstance().handleException(generic_exception);
            return false;
        } catch (...) {
            auto generic_exception = ExceptionFactory::createGenericException(
                "Unknown exception occurred",
                ErrorCategory::Internal
            );
            ExceptionHandler::getInstance().handleException(generic_exception);
            return false;
        }
    }
}

} // namespace nlm