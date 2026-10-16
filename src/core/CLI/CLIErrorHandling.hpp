// CLI error handling integration for NLM
#pragma once

#include "core/Errors/ErrorSystem.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Recovery/ConfigRecovery.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <csignal>
#include <exception>

namespace nlm {

// CLI error handling modes
enum class CLIErrorHandlingMode {
    Strict,           // Fail on any error
    Lenient,          // Continue on non-critical errors
    Graceful,         // Attempt recovery and continue
    Silent           // Suppress error output
};

// CLI error handler configuration
struct CLIErrorHandlingConfig {
    CLIErrorHandlingMode errorHandlingMode;
    ErrorSeverity errorThreshold;
    bool enableAutoRecovery;
    bool enableGracefulDegradation;
    bool enableCheckpointRecovery;
    bool enableMemoryRecovery;
    bool enableResourceRecovery;
    std::string errorOutputFile;
    std::string checkpointPath;
    std::vector<std::string> suppressErrorCodes;
    std::vector<std::string> escalateErrorCodes;
    
    CLIErrorHandlingConfig()
        : errorHandlingMode(CLIErrorHandlingMode::Graceful),
          errorThreshold(ErrorSeverity::Warning),
          enableAutoRecovery(true),
          enableGracefulDegradation(true),
          enableCheckpointRecovery(true),
          enableMemoryRecovery(true),
          enableResourceRecovery(true),
          errorOutputFile(""),
          checkpointPath("./checkpoints") {}
};

// CLI error handler for application-level error handling
class CLIErrorHandler {
private:
    CLIErrorHandlingConfig config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<SystemRecoveryManager> recoveryManager_;
    std::map<std::string, std::function<void(const std::exception&)>> errorHandlers_;
    std::map<std::string, std::function<void(const std::exception&, Config&)>> recoveryHandlers_;
    bool isInitialized_;
    
public:
    CLIErrorHandler()
        : isInitialized_(false) {
        logger_ = &Logger::getGlobal();
        recoveryManager_ = &getGlobalRecoveryManager();
        isInitialized_ = true;
    }
    
    // Initialize CLI error handler with configuration
    bool initialize(const CLIErrorHandlingConfig& config) {
        if (!isInitialized_) {
            return false;
        }
        
        config_ = config;
        
        // Configure error reporting
        logger_->setLevel(convertToLogLevel(config.errorThreshold));
        
        // Register signal handlers
        registerSignalHandlers();
        
        // Initialize recovery manager
        recoveryManager_->setAutomaticRecoveryEnabled(config.enableAutoRecovery);
        recoveryManager_->setGracefulDegradationEnabled(config.enableGracefulDegradation);
        
        return true;
    }
    
    // Handle exception with CLI-specific error handling
    std::exception handle(const std::exception& error, const std::string& context = "") {
        if (!isInitialized_) {
            return error;
        }
        
        // Convert to NLMException if not already
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        ErrorSeverity severity = nlmEx ? nlmEx->getSeverity() : ErrorSeverity::Error;
        
        // Check if error should be suppressed
        if (shouldSuppressError(error)) {
            return error;
        }
        
        // Check if error should be escalated
        if (shouldEscalateError(error)) {
            severity = ErrorSeverity::Critical;
        }
        
        // Log error based on mode
        logError(error, context, severity);
        
        // Check if error should cause application exit
        if (shouldExitApplication(error, severity)) {
            logApplicationExitInfo(error);
        }
        
        // Attempt recovery based on mode
        if (config_.enableAutoRecovery && shouldAttemptRecovery(error, severity)) {
            return attemptRecovery(error);
        }
        
        return error;
    }
    
    // Handle configuration error
    bool handleConfigError(const Config& config, const std::string& context = "") {
        if (!isInitialized_) {
            return false;
        }
        
        // Get validation errors
        auto errors = recoveryManager_->getConfigValidator()->getErrors(config);
        
        if (errors.empty()) {
            return true;
        }
        
        // Log configuration errors
        for (const auto& error : errors) {
            logConfigError(error, context);
        }
        
        // Attempt to recover configuration
        bool recoverySuccess = recoveryManager_->getConfigRecovery()->validateAndRecover(const_cast<Config&>(config));
        
        if (recoverySuccess) {
            logRecoverySuccess("Configuration", context);
        } else {
            logRecoveryFailure("Configuration", context);
        }
        
        return recoverySuccess;
    }
    
    // Handle system error
    bool handleSystemError(const std::exception& error, Config& config, const std::string& context = "") {
        if (!isInitialized_) {
            return false;
        }
        
        // Use recovery manager to handle system error
        return recoveryManager_->recoverFromError(error, config);
    }
    
    // Register error handler for specific error code
    void registerErrorHandler(const std::string& errorCode,
                             std::function<void(const std::exception&)> handler) {
        errorHandlers_[errorCode] = handler;
    }
    
    // Register recovery handler for specific error code
    void registerRecoveryHandler(const std::string& errorCode,
                                 std::function<bool(const std::exception&, Config&)> handler) {
        recoveryHandlers_[errorCode] = handler;
    }
    
    // Get error handler configuration
    const CLIErrorHandlingConfig& getConfig() const {
        return config_;
    }
    
    // Set error handler configuration
    void setConfig(const CLIErrorHandlingConfig& config) {
        config_ = config;
        if (isInitialized_) {
            initialize(config);
        }
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t totalHandled_ = 0;
        size_t autoRecovered_ = 0;
        size_t recovered_ = 0;
        size_t suppressed_ = 0;
        size_t escalated_ = 0;
        std::map<ErrorSeverity, size_t> severityCounts;
        std::map<std::string, size_t> errorCodeCounts;
    };
    
    ErrorStats getErrorStats() const {
        ErrorStats stats;
        // Implementation would collect statistics from logger and recovery manager
        return stats;
    }
    
    // Generate error report
    std::string generateErrorReport() const {
        std::ostringstream oss;
        oss << "=== CLI Error Report ===" << std::endl;
        oss << "Error handling mode: " << static_cast<int>(config_.errorHandlingMode) << std::endl;
        oss << "Error threshold: " << static_cast<int>(config_.errorThreshold) << std::endl;
        oss << "Auto-recovery: " << (config_.enableAutoRecovery ? "ENABLED" : "DISABLED") << std::endl;
        oss << "Graceful degradation: " << (config_.enableGracefulDegradation ? "ENABLED" : "DISABLED") << std::endl;
        oss << "Checkpoint recovery: " << (config_.enableCheckpointRecovery ? "ENABLED" : "DISABLED") << std::endl;
        oss << "Memory recovery: " << (config_.enableMemoryRecovery ? "ENABLED" : "DISABLED") << std::endl;
        oss << "Resource recovery: " << (config_.enableResourceRecovery ? "ENABLED" : "DISABLED") << std::endl;
        if (!config_.errorOutputFile.empty()) {
            oss << "Error output file: " << config_.errorOutputFile << std::endl;
        }
        if (!config_.checkpointPath.empty()) {
            oss << "Checkpoint path: " << config_.checkpointPath << std::endl;
        }
        return oss.str();
    }
    
private:
    // Convert error severity to log level
    LogLevel convertToLogLevel(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return LogLevel::Debug;
            case ErrorSeverity::Info: return LogLevel::Info;
            case ErrorSeverity::Warning: return LogLevel::Warning;
            case ErrorSeverity::Error: return LogLevel::Error;
            case ErrorSeverity::Critical: return LogLevel::Critical;
            default: return LogLevel::Error;
        }
    }
    
    // Check if error should be suppressed
    bool shouldSuppressError(const std::exception& error) const {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (!nlmEx) return false;
        
        for (const auto& code : config_.suppressErrorCodes) {
            if (nlmEx->getErrorCode().find(code) != std::string::npos) {
                return true;
            }
        }
        
        return config_.errorHandlingMode == CLIErrorHandlingMode::Silent;
    }
    
    // Check if error should be escalated
    bool shouldEscalateError(const std::exception& error) const {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (!nlmEx) return false;
        
        for (const auto& code : config_.escalateErrorCodes) {
            if (nlmEx->getErrorCode().find(code) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    // Check if application should exit
    bool shouldExitApplication(const std::exception& error, ErrorSeverity severity) const {
        switch (config_.errorHandlingMode) {
            case CLIErrorHandlingMode::Strict:
                return severity >= ErrorSeverity::Error;
            case CLIErrorHandlingMode::Lenient:
                return severity >= ErrorSeverity::Critical;
            case CLIErrorHandlingMode::Graceful:
                return severity >= ErrorSeverity::Critical;
            case CLIErrorHandlingMode::Silent:
                return false;
            default:
                return severity >= ErrorSeverity::Critical;
        }
    }
    
    // Check if recovery should be attempted
    bool shouldAttemptRecovery(const std::exception& error, ErrorSeverity severity) const {
        return config_.enableAutoRecovery && severity < ErrorSeverity::Critical;
    }
    
    // Log error
    void logError(const std::exception& error, const std::string& context, ErrorSeverity severity) {
        std::ostringstream oss;
        oss << "Error";
        if (!context.empty()) {
            oss << " in " << context;
        }
        oss << ": " << error.what();
        
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (nlmEx) {
            oss << " (Error Code: " << nlmEx->getErrorCode() << ")";
        }
        
        switch (severity) {
            case ErrorSeverity::Debug: logger_->debug(oss.str()); break;
            case ErrorSeverity::Info: logger_->info(oss.str()); break;
            case ErrorSeverity::Warning: logger_->warning(oss.str()); break;
            case ErrorSeverity::Error: logger_->error(oss.str()); break;
            case ErrorSeverity::Critical: logger_->critical(oss.str()); break;
        }
        
        // Output to file if configured
        if (!config_.errorOutputFile.empty()) {
            std::ofstream file(config_.errorOutputFile, std::ios::app);
            if (file.is_open()) {
                file << oss.str() << std::endl;
                file.close();
            }
        }
    }
    
    // Log configuration error
    void logConfigError(const ValidationError& error, const std::string& context) {
        std::ostringstream oss;
        oss << "Config Error";
        if (!context.empty()) {
            oss << " in " << context;
        }
        oss << " (" << error.field << "): " << error.message;
        
        if (!error.errorCode.empty()) {
            oss << " [" << error.errorCode << "]";
        }
        
        oss << " - Suggested action: " << error.suggestedAction;
        
        switch (error.severity) {
            case ValidationSeverity::Warning: logger_->warning(oss.str()); break;
            case ValidationSeverity::Error: logger_->error(oss.str()); break;
            case ValidationSeverity::Critical: logger_->critical(oss.str()); break;
        }
    }
    
    // Log recovery success
    void logRecoverySuccess(const std::string& component, const std::string& context) {
        std::ostringstream oss;
        oss << "Recovery";
        if (!context.empty()) {
            oss << " for " << context;
        }
        oss << " in " << component << ": SUCCESS";
        logger_->info(oss.str());
    }
    
    // Log recovery failure
    void logRecoveryFailure(const std::string& component, const std::string& context) {
        std::ostringstream oss;
        oss << "Recovery";
        if (!context.empty()) {
            oss << " for " << context;
        }
        oss << " in " << component << ": FAILED";
        logger_->error(oss.str());
    }
    
    // Log application exit info
    void logApplicationExitInfo(const std::exception& error) {
        std::ostringstream oss;
        oss << "Application will exit due to critical error: " << error.what();
        logger_->critical(oss.str());
    }
    
    // Attempt recovery
    std::exception attemptRecovery(const std::exception& error) {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (!nlmEx) return error;
        
        // Try registered recovery handler
        auto it = recoveryHandlers_.find(nlmEx->getErrorCode());
        if (it != recoveryHandlers_.end()) {
            Config dummyConfig;
            if (it->second(error, dummyConfig)) {
                logRecoverySuccess(nlmEx->getErrorCode(), "");
                return error;
            }
        }
        
        // Try global recovery manager
        Config dummyConfig;
        if (recoveryManager_->recoverFromError(error, dummyConfig)) {
            logRecoverySuccess("Global", "");
            return error;
        }
        
        logRecoveryFailure("Global", "");
        return error;
    }
    
    // Register signal handlers
    void registerSignalHandlers() {
        // Set up signal handlers for graceful shutdown
        signal(SIGINT, [](int signum) {
            NLM_LOG_WARNING("Received interrupt signal");
            std::exit(1);
        });
        
        signal(SIGTERM, [](int signum) {
            NLM_LOG_WARNING("Received termination signal");
            std::exit(1);
        });
    }
};

// Global CLI error handler instance
inline CLIErrorHandler& getGlobalCLIErrorHandler() {
    static CLIErrorHandler handler;
    return handler;
}

// Convenience functions for CLI error handling
inline void initializeCLIErrorHandler(const CLIErrorHandlingConfig& config) {
    getGlobalCLIErrorHandler().initialize(config);
}

inline std::exception handleCLIError(const std::exception& error, const std::string& context = "") {
    return getGlobalCLIErrorHandler().handle(error, context);
}

inline bool handleCLIConfigError(const Config& config, const std::string& context = "") {
    return getGlobalCLIErrorHandler().handleConfigError(config, context);
}

inline bool handleCLISystemError(const std::exception& error, Config& config, const std::string& context = "") {
    return getGlobalCLIErrorHandler().handleSystemError(error, config, context);
}

inline void registerCLIErrorHandler(const std::string& errorCode,
                                   std::function<void(const std::exception&)> handler) {
    getGlobalCLIErrorHandler().registerErrorHandler(errorCode, handler);
}

inline void registerCLIRecoveryHandler(const std::string& errorCode,
                                       std::function<bool(const std::exception&, Config&)> handler) {
    getGlobalCLIErrorHandler().registerRecoveryHandler(errorCode, handler);
}

inline std::string generateCLIErrorReport() {
    return getGlobalCLIErrorHandler().generateErrorReport();
}

} // namespace nlm
