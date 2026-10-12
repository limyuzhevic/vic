#include "ErrorHandling.hpp"
#include "Logger.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

// Global error handling instance
class ErrorHandlingSystem {
private:
    std::unique_ptr<ErrorRecoveryManager> recoveryManager_;
    std::unique_ptr<Logger> errorLogger_;
    std::mutex globalErrorMutex_;
    std::atomic<uint64_t> totalErrors_;
    std::atomic<uint64_t> totalRecoveries_;
    std::chrono::steady_clock::time_point lastCleanup_;

    // Configuration
    ErrorSeverity minHandledSeverity_;
    uint32_t errorRetentionHours_;
    bool enableStructuredReporting_;
    bool enableErrorCorrelation_;

public:
    ErrorHandlingSystem() : totalErrors_(0), totalRecoveries_(0), 
                           lastCleanup_(std::chrono::steady_clock::now()) {
        recoveryManager_ = std::make_unique<ErrorRecoveryManager>();
        errorLogger_ = std::make_unique<Logger>();
        configureDefaultSettings();
    }

    void initialize() {
        // Initialize error handling system
        errorLogger_->setLevel(LogLevel::Warning);
        errorLogger_->debug("Error handling system initialized");
    }

    // Error handling interface
    void handleError(std::unique_ptr<NLMError> error) {
        if (!error) return;

        std::lock_guard<std::mutex> lock(globalErrorMutex_);
        totalErrors_++;

        errorLogger_->debug("Handling error: " + error->getMessage());

        // Record context and diagnostics
        recoveryManager_->recordErrorContext(error.get(), "timestamp", error->getFormattedTimestamp());
        recoveryManager_->recordErrorContext(error.get(), "thread_id", std::to_string(std::hash<std::thread::id>{}(error->getThreadId())));
        recoveryManager_->recordErrorContext(error.get(), "process_id", std::to_string(error->getProcessId()));

        // Perform recovery
        std::unique_ptr<NLMError> recoveredError = recoveryManager_->attemptRecovery(std::move(error));
        
        if (recoveredError) {
            // Log the error
            logError(*recoveredError);

            // Report error if recovery failed
            if (!recoveredError->isRecoverySuccessful()) {
                reportError(*recoveredError);
            }

            // Alert for critical errors
            if (recoveredError->getSeverity() >= ErrorSeverity::Critical) {
                alertError(*recoveredError);
            }
        }

        // Cleanup old errors
        cleanupOldErrors();
    }

    std::vector<std::unique_ptr<NLMError>> handleErrorSync(std::unique_ptr<NLMError> error) {
        if (!error) return {};

        std::lock_guard<std::mutex> lock(globalErrorMutex_);
        totalErrors_++;

        // Record context and diagnostics
        recoveryManager_->recordErrorContext(error.get(), "timestamp", error->getFormattedTimestamp());
        recoveryManager_->recordErrorContext(error.get(), "thread_id", std::to_string(std::hash<std::thread::id>{}(error->getThreadId())));
        recoveryManager_->recordErrorContext(error.get(), "process_id", std::to_string(error->getProcessId()));

        // Perform synchronous recovery
        std::unique_ptr<NLMError> recoveredError = recoveryManager_->attemptRecovery(std::move(error));
        
        std::vector<std::unique_ptr<NLMError>> result;
        if (recoveredError) {
            // Log the error
            logError(*recoveredError);

            // Report error if recovery failed
            if (!recoveredError->isRecoverySuccessful()) {
                reportError(*recoveredError);
            }

            // Alert for critical errors
            if (recoveredError->getSeverity() >= ErrorSeverity::Critical) {
                alertError(*recoveredError);
            }

            result.push_back(std::move(recoveredError));
        }

        // Cleanup old errors
        cleanupOldErrors();

        return result;
    }

    // Error reporting and analysis
    void logError(const NLMError& error) {
        std::stringstream ss;
        ss << "Error: " << error.getMessage();
        if (!error.getSourceLocation().empty()) {
            ss << " at " << error.getSourceLocation();
        }
        ss << " (Severity: " << static_cast<int>(error.getSeverity()) << ")";
        ss << " (Category: " << static_cast<int>(error.getCategory()) << ")";

        errorLogger_->error(ss.str());

        // Structured logging
        if (enableStructuredReporting_) {
            errorLogger_->info("Error Details: " + error.toJSON());
        }
    }

    void reportError(const NLMError& error) {
        // Report to error reporting system
        std::stringstream ss;
        ss << "Error Report: " << error.getMessage();
        ss << "\nSource: " << error.getSourceLocation();
        ss << "\nTime: " << error.getFormattedTimestamp();
        ss << "\nThread: " << std::hex << error.getThreadId();
        ss << "\nProcess: " << error.getProcessId();
        ss << "\nSeverity: " << static_cast<int>(error.getSeverity());
        ss << "\nCategory: " << static_cast<int>(error.getCategory());
        ss << "\nStack Trace:\n" << error.getFormattedStackTrace();

        std::cout << ss.str() << std::endl;
    }

    void alertError(const NLMError& error) {
        // Send urgent notification
        std::stringstream ss;
        ss << "*** CRITICAL ERROR ***\n";
        ss << "Message: " << error.getMessage() << "\n";
        ss << "Source: " << error.getSourceLocation() << "\n";
        ss << "Time: " << error.getFormattedTimestamp() << "\n";
        ss << "Impact: This error may cause system instability\n";

        std::cerr << ss.str() << std::endl;
    }

    // Error aggregation and correlation
    void addErrorToGroup(const std::string& groupId, std::unique_ptr<NLMError> error) {
        if (enableErrorCorrelation_) {
            recoveryManager_->addErrorToGroup(groupId, std::move(error));
        }
    }

    std::unique_ptr<ErrorGroup> getErrorGroup(const std::string& groupId) const {
        return recoveryManager_->getErrorGroup(groupId);
    }

    std::vector<std::unique_ptr<ErrorGroup>> getAllErrorGroups() const {
        return recoveryManager_->getAllErrorGroups();
    }

    // Error statistics
    uint64_t getTotalErrors() const { return totalErrors_.load(); }
    uint64_t getTotalRecoveries() const { return totalRecoveries_.load(); }
    ErrorHandlingSystem::SystemStats getSystemStats() const {
        SystemStats stats;
        stats.totalErrors = totalErrors_.load();
        stats.totalRecoveries = totalRecoveries_.load();
        
        ErrorRecoveryManager::RecoveryStats recoveryStats = recoveryManager_->getRecoveryStats();
        stats.recoveryStats = recoveryStats;
        
        return stats;
    }

    // Error recovery manager access
    ErrorRecoveryManager* getRecoveryManager() { return recoveryManager_.get(); }
    const ErrorRecoveryManager* getRecoveryManager() const { return recoveryManager_.get(); }

    // Configuration
    void configure(const std::unordered_map<std::string, std::string>& config) {
        if (config.find("min_handled_severity") != config.end()) {
            minHandledSeverity_ = stringToSeverity(config.at("min_handled_severity"));
        }
        if (config.find("error_retention_hours") != config.end()) {
            errorRetentionHours_ = std::stoul(config.at("error_retention_hours"));
        }
        if (config.find("enable_structured_reporting") != config.end()) {
            enableStructuredReporting_ = (config.at("enable_structured_reporting") == "true");
        }
        if (config.find("enable_error_correlation") != config.end()) {
            enableErrorCorrelation_ = (config.at("enable_error_correlation") == "true");
        }

        recoveryManager_->configure(config);
    }

    std::string getConfiguration() const {
        std::stringstream ss;
        ss << "{\"min_handled_severity\": \"" << severityToString(minHandledSeverity_) << "\",\"\n"
           << " \"error_retention_hours\": " << errorRetentionHours_ << ",\"\n"
           << " \"enable_structured_reporting\": " << (enableStructuredReporting_ ? "true" : "false") << ",\"\n"
           << " \"enable_error_correlation\": " << (enableErrorCorrelation_ ? "true" : "false") << "\"\n}";
        return ss.str();
    }

    // Configuration helpers
    void configureDefaultSettings() {
        minHandledSeverity_ = ErrorSeverity::Warning;
        errorRetentionHours_ = 24;
        enableStructuredReporting_ = true;
        enableErrorCorrelation_ = true;
    }

    // Error handling macros
    #define NLM_THROW_ERROR(errorType, message) \
        do { \
            auto error = errorType::createError(message); \
            handleError(std::move(error)); \
            throw *error; \
        } while(0)

    #define NLM_THROW_ERROR_WITH_CONTEXT(errorType, message, context) \
        do { \
            auto error = errorType::createError(message); \
            for (const auto& pair : context) { \
                error->addContext(pair.first, pair.second); \
            } \
            handleError(std::move(error)); \
            throw *error; \
        } while(0)

    #define NLM_THROW_ERROR_SYNC(errorType, message) \
        do { \
            auto errors = handleErrorSync(errorType::createError(message)); \
            if (!errors.empty()) { \
                throw *errors[0]; \
            } \
        } while(0)

protected:
    // Error cleanup
    void cleanupOldErrors() {
        auto now = std::chrono::steady_clock::now();
        auto hoursSinceCleanup = std::chrono::duration_cast<std::chrono::hours>(now - lastCleanup_).count();
        
        if (hoursSinceCleanup >= 1) { // Cleanup every hour
            lastCleanup_ = now;
            
            // Cleanup old error groups
            auto groups = recoveryManager_->getAllErrorGroups();
            for (const auto& group : groups) {
                auto groupTime = std::chrono::system_clock::time_point(); // TODO: Get from group
                auto hoursSinceGroup = std::chrono::duration_cast<std::chrono::hours>(now - groupTime).count();
                if (hoursSinceGroup > errorRetentionHours_) {
                    // Remove old group (implementation depends on ErrorGroup management)
                }
            }
        }
    }

    // Conversion utilities
    ErrorSeverity stringToSeverity(const std::string& severityStr) const {
        std::string upper = severityStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        if (upper == "DEBUG") return ErrorSeverity::Debug;
        if (upper == "INFO") return ErrorSeverity::Info;
        if (upper == "WARNING") return ErrorSeverity::Warning;
        if (upper == "ERROR") return ErrorSeverity::Error;
        if (upper == "CRITICAL") return ErrorSeverity::Critical;
        if (upper == "FATAL") return ErrorSeverity::Fatal;
        if (upper == "ALERT") return ErrorSeverity::Alert;
        return ErrorSeverity::Warning;
    }

    std::string severityToString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            case ErrorSeverity::Alert: return "ALERT";
            default: return "ERROR";
        }
    }

private:
    struct SystemStats {
        uint64_t totalErrors;
        uint64_t totalRecoveries;
        ErrorRecoveryManager::RecoveryStats recoveryStats;
    };
};

// Global error handling instance
ErrorHandlingSystem& getErrorHandlingSystem() {
    static ErrorHandlingSystem instance;
    return instance;
}

// Convenience functions
void initializeErrorHandling(const std::unordered_map<std::string, std::string>& config) {
    getErrorHandlingSystem().initialize();
    getErrorHandlingSystem().configure(config);
}

void configureErrorHandling(const std::unordered_map<std::string, std::string>& config) {
    getErrorHandlingSystem().configure(config);
}

void handleError(std::unique_ptr<NLMError> error) {
    getErrorHandlingSystem().handleError(std::move(error));
}

std::vector<std::unique_ptr<NLMError>> handleErrorSync(std::unique_ptr<NLMError> error) {
    return getErrorHandlingSystem().handleErrorSync(std::move(error));
}

void addErrorToGroup(const std::string& groupId, std::unique_ptr<NLMError> error) {
    getErrorHandlingSystem().addErrorToGroup(groupId, std::move(error));
}

std::unique_ptr<ErrorGroup> getErrorGroup(const std::string& groupId) {
    return getErrorHandlingSystem().getErrorGroup(groupId);
}

ErrorHandlingSystem::SystemStats getErrorHandlingStats() {
    return getErrorHandlingSystem().getSystemStats();
}

// Macro for throwing errors with source location
#define NLM_THROW_ERROR_AT(errorType, message) \\
    do { \\
        auto error = errorType::createError(message); \\
        error->setSourceLocation(__FILE__, __LINE__, __func__); \\
        handleError(std::move(error)); \\
        throw *error; \\
    } while(0)

#define NLM_THROW_ERROR_AT_SYNC(errorType, message) \\
    do { \\
        auto errors = handleErrorSync(errorType::createError(message)); \\
        if (!errors.empty()) { \\
            errors[0]->setSourceLocation(__FILE__, __LINE__, __func__); \\
            throw *errors[0]; \\
        } \\
    } while(0)

#define NLM_ERROR_HANDLED_FUNCTION_BEGIN \\
    try {

#define NLM_ERROR_HANDLED_FUNCTION_END(continueOnError) \\
    } catch (const NLMError& e) { \\
        if (continueOnError) { \\
            std::cout << "Error handled: " << e.what() << std::endl; \\
        } else { \\
            throw; \\
        } \\
    }

// Exception translation
void translateLegacyException(const std::exception& e) {
    // Translate legacy std::exception to NLMError
    auto error = std::make_unique<NLMError>(
        std::string("Legacy Exception: ") + e.what(),
        ErrorSeverity::Error,
        ErrorCategory::Unknown
    );
    error->addDiagnostic("legacy_exception", "true");
    handleError(std::move(error));
}

} // namespace nlm