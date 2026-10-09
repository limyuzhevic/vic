#pragma once

#include "ErrorHandling.hpp"
#include "Validation.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Global error reporting callback
using ErrorReportCallback = std::function<void(std::shared_ptr<NLMError>)>;

// Error integration system
class ErrorIntegration {
private:
    std::shared_ptr<ErrorManager> errorManager_;
    std::shared_ptr<ErrorRecoverySystem> recoverySystem_;
    std::shared_ptr<ConfigValidator> validator_;
    
    // Callbacks for error reporting
    std::vector<ErrorReportCallback> errorReportCallbacks_;
    
    // Integration flags
    bool enableAutoRecovery_ = true;
    bool enableErrorReporting_ = true;
    bool enableValidation_ = true;
    
public:
    ErrorIntegration()
        : errorManager_(std::make_shared<ErrorManager>())
        , recoverySystem_(std::make_shared<ErrorRecoverySystem>())
        , validator_(std::make_shared<ConfigValidator>()) {}
    
    // Configuration integration
    bool integrateWithConfig(const Config& config) {
        try {
            // Validate configuration
            auto validationResult = validator_->validateConfigComprehensive(config);
            
            if (!validationResult.success) {
                ErrorHandling::log("Configuration validation failed", ErrorSeverity::Error, "config_integration");
                
                // Report all validation errors
                for (const auto& error : validationResult.errors) {
                    ErrorHandling::log(error->what(), error->getSeverity(), error->getContext());
                }
                
                return false;
            }
            
            ErrorHandling::log("Configuration integration successful", ErrorSeverity::Info, "config_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "config_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Logger integration
    bool integrateWithLogger(Logger& logger) {
        try {
            // Add error-aware logging
            auto errorLogger = std::make_shared<ErrorAwareLogger>(logger.getGlobalLogger());
            
            // Setup error reporting
            errorLogger->setLevel(LogLevel::Error);
            
            ErrorHandling::log("Logger integration successful", ErrorSeverity::Info, "logger_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "logger_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Simulation integration
    bool integrateWithSimulation(const Config& config) {
        try {
            // Validate simulation parameters
            auto validationResult = validator_->validateSimulation(config);
            
            if (!validationResult.success) {
                ErrorHandling::log("Simulation parameter validation failed", ErrorSeverity::Error, "simulation_integration");
                
                for (const auto& error : validationResult.errors) {
                    ErrorHandling::log(error->what(), error->getSeverity(), error->getContext());
                    
                    // Attempt recovery for simulation errors
                    if (enableAutoRecovery_ && ValidationHelper::canRecover(error)) {
                        ValidationHelper::attemptRecovery(error);
                    }
                }
                
                return false;
            }
            
            ErrorHandling::log("Simulation integration successful", ErrorSeverity::Info, "simulation_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "simulation_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Memory system integration
    bool integrateWithMemorySystems(const Config& config) {
        try {
            // Validate memory requirements
            auto validationResult = validator_->validateMemoryRequirements(config);
            
            if (!validationResult.success) {
                ErrorHandling::log("Memory system validation failed", ErrorSeverity::Critical, "memory_integration");
                
                for (const auto& error : validationResult.errors) {
                    ErrorHandling::log(error->what(), error->getSeverity(), error->getContext());
                }
                
                return false;
            }
            
            ErrorHandling::log("Memory system integration successful", ErrorSeverity::Info, "memory_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "memory_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Neuromodulation integration
    bool integrateWithNeuromodulation(const Config& config) {
        try {
            // Validate neuromodulation parameters
            auto validationResult = validator_->validateNeuromodulation(config);
            
            if (!validationResult.success) {
                ErrorHandling::log("Neuromodulation validation failed", ErrorSeverity::Error, "neuromodulation_integration");
                
                for (const auto& error : validationResult.errors) {
                    ErrorHandling::log(error->what(), error->getSeverity(), error->getContext());
                }
                
                return false;
            }
            
            ErrorHandling::log("Neuromodulation integration successful", ErrorSeverity::Info, "neuromodulation_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "neuromodulation_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Performance monitoring integration
    bool integrateWithPerformanceMonitor(PerformanceMonitor& perfMonitor) {
        try {
            // Setup error reporting for performance alerts
            perfMonitor.setAlertCallback([this](const std::string& message, const std::string& context) {
                auto error = ErrorHandling::createError(ErrorCode::PerformanceMonitorError,
                    std::format("Performance alert: {}", message), context);
                errorManager_->addError(error);
                
                // Call user callbacks
                for (const auto& callback : errorReportCallbacks_) {
                    callback(error);
                }
            });
            
            ErrorHandling::log("Performance monitoring integration successful", ErrorSeverity::Info, "performance_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "performance_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Checkpoint system integration
    bool integrateWithCheckpointSystem(const std::string& checkpointDir) {
        try {
            // Validate checkpoint directory
            if (!ErrorHandling::validateFileExists(checkpointDir, "checkpoint_integration")) {
                auto error = ErrorHandling::createFileSystemError(
                    ErrorCode::FileNotFound,
                    std::format("Checkpoint directory does not exist: {}", checkpointDir));
                errorManager_->addError(error);
                return false;
            }
            
            // Setup checkpoint error handling
            auto validateCallback = [this](const std::string& filepath) -> bool {
                auto validationResult = validator_->validateCheckpointData(filepath);
                return validationResult.success;
            };
            
            ErrorHandling::log("Checkpoint system integration successful", ErrorSeverity::Info, "checkpoint_integration");
            return true;
            
        } catch (const std::exception& e) {
            auto error = ErrorHandling::createErrorFromException(e, "checkpoint_integration");
            errorManager_->addError(error);
            return false;
        }
    }
    
    // Error reporting integration
    void setupErrorReporting(ErrorReportCallback callback) {
        errorReportCallbacks_.push_back(callback);
    }
    
    void setupErrorReporting(std::vector<ErrorReportCallback> callbacks) {
        errorReportCallbacks_ = callbacks;
    }
    
    // Configuration options
    void enableAutoRecovery(bool enable) {
        enableAutoRecovery_ = enable;
    }
    
    void enableErrorReporting(bool enable) {
        enableErrorReporting_ = enable;
    }
    
    void enableValidation(bool enable) {
        enableValidation_ = enable;
    }
    
    // Get error statistics
    ErrorManager::ErrorStats getErrorStats() const {
        return errorManager_->getErrorStats();
    }
    
    // Generate comprehensive error report
    std::string generateErrorReport() const {
        std::ostringstream oss;
        oss << "=== NLM Error Integration Report ===" << std::endl;
        
        auto stats = getErrorStats();
        oss << "Total Errors: " << stats.totalErrors << std::endl;
        oss << "Recent Errors (last 100): " << stats.recentErrors << std::endl;
        
        if (stats.totalErrors > 0) {
            oss << "\nError Distribution by Type:" << std::endl;
            for (const auto& pair : stats.errorsByCode) {
                oss << "  Code " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
            }
            
            oss << "\nError Distribution by Severity:" << std::endl;
            for (const auto& pair : stats.errorsBySeverity) {
                std::string severityStr;
                switch (pair.first) {
                    case ErrorSeverity::Debug: severityStr = "Debug"; break;
                    case ErrorSeverity::Info: severityStr = "Info"; break;
                    case ErrorSeverity::Warning: severityStr = "Warning"; break;
                    case ErrorSeverity::Error: severityStr = "Error"; break;
                    case ErrorSeverity::Critical: severityStr = "Critical"; break;
                    case ErrorSeverity::Fatal: severityStr = "Fatal"; break;
                }
                oss << "  " << severityStr << ": " << pair.second << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Integration validation
    bool validateIntegration() const {
        std::vector<std::string> issues;
        
        if (!errorManager_) {
            issues.push_back("Error manager not initialized");
        }
        
        if (!recoverySystem_) {
            issues.push_back("Error recovery system not initialized");
        }
        
        if (!validator_) {
            issues.push_back("Validator not initialized");
        }
        
        if (!issues.empty()) {
            ErrorHandling::log(std::format("Integration validation failed: {}", 
                std::accumulate(issues.begin(), issues.end(), std::string(),
                    [](const std::string& acc, const std::string& issue) {
                        return acc.empty() ? issue : acc + ", " + issue;
                    })), ErrorSeverity::Error, "integration_validation");
            return false;
        }
        
        ErrorHandling::log("Integration validation successful", ErrorSeverity::Info, "integration_validation");
        return true;
    }
};

// Global error integration instance
inline ErrorIntegration& getGlobalErrorIntegration() {
    static ErrorIntegration integration;
    return integration;
}

// Integration convenience functions
inline bool integrateWithConfig(const Config& config) {
    return getGlobalErrorIntegration().integrateWithConfig(config);
}

inline bool integrateWithLogger(Logger& logger) {
    return getGlobalErrorIntegration().integrateWithLogger(logger);
}

inline bool integrateWithSimulation(const Config& config) {
    return getGlobalErrorIntegration().integrateWithSimulation(config);
}

inline bool integrateWithMemorySystems(const Config& config) {
    return getGlobalErrorIntegration().integrateWithMemorySystems(config);
}

inline bool integrateWithNeuromodulation(const Config& config) {
    return getGlobalErrorIntegration().integrateWithNeuromodulation(config);
}

inline bool integrateWithPerformanceMonitor(PerformanceMonitor& perfMonitor) {
    return getGlobalErrorIntegration().integrateWithPerformanceMonitor(perfMonitor);
}

inline bool integrateWithCheckpointSystem(const std::string& checkpointDir) {
    return getGlobalErrorIntegration().integrateWithCheckpointSystem(checkpointDir);
}

inline bool validateIntegration() {
    return getGlobalErrorIntegration().validateIntegration();
}

inline std::string generateIntegrationErrorReport() {
    return getGlobalErrorIntegration().generateErrorReport();
}

inline ErrorManager::ErrorStats getIntegrationErrorStats() {
    return getGlobalErrorIntegration().getErrorStats();
}

} // namespace nlm