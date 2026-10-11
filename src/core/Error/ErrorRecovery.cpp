#include "ErrorRecovery.hpp"
#include <sstream>
#include <iostream>

namespace nlm {

namespace ErrorRecovery {
    
    // Config recovery callback
define_error_recovery_callback configRecoveryCallback = [](const NLMException& error) -> bool {
        // Attempt to reset config to last known good state
        NLM_REPORT_ERROR("Attempting config recovery for: " + error.what(),
                        ErrorCategory::Configuration, ErrorSeverity::Warning);
        
        // Implement config recovery logic here
        return true;
    };

    // Logger recovery callback
define_error_recovery_callback loggerRecoveryCallback = [](const NLMException& error) -> bool {
        // Attempt to recover from logging errors
        NLM_REPORT_ERROR("Attempting logger recovery for: " + error.what(),
                        ErrorCategory::Logger, ErrorSeverity::Warning);
        
        // Implement logger recovery logic here
        return true;
    };

    // Neuron recovery callback
define_error_recovery_callback neuronRecoveryCallback = [](const NLMException& error) -> bool {
        // Attempt to recover neuron state
        NLM_REPORT_ERROR("Attempting neuron recovery for: " + error.what(),
                        ErrorCategory::Simulation, ErrorSeverity::Warning);
        
        // Implement neuron recovery logic here
        return true;
    };

    // Synapse recovery callback
define_error_recovery_callback synapseRecoveryCallback = [](const NLMException& error) -> bool {
        // Attempt to recover synapse state
        NLM_REPORT_ERROR("Attempting synapse recovery for: " + error.what(),
                        ErrorCategory::Simulation, ErrorSeverity::Warning);
        
        // Implement synapse recovery logic here
        return true;
    };

    // Health checking utilities
    namespace HealthCheck {
        
        // Check if all components are healthy
        bool isSystemHealthy() {
            // Check if global error reporter is available
            auto reporter = ErrorHandling::getGlobalErrorReporter();
            if (!reporter) {
                NLM_REPORT_ERROR("Global error reporter not available",
                                ErrorCategory::System, ErrorSeverity::Error);
                return false;
            }
            
            // Check if logger is healthy
            auto& logger = nlm::Logger::getGlobal();
            if (!logger.isHealthy()) {
                auto issues = logger.getHealthIssues();
                for (const auto& issue : issues) {
                    NLM_REPORT_ERROR("Logger health issue: " + issue,
                                    ErrorCategory::Logger, ErrorSeverity::Warning);
                }
                return false;
            }
            
            // Additional health checks can be added here
            
            return true;
        }
        
        // Generate health report
        std::string generateHealthReport() {
            std::stringstream report;
            report << "NLM System Health Report\n";
            report << "========================\n";
            report << "Timestamp: " << ErrorHandling::getCurrentTimestampString() << "\n";
            report << "\n";
            
            // Check individual components
            auto& logger = nlm::Logger::getGlobal();
            report << "Logger Status: " << (logger.isHealthy() ? "HEALTHY" : "UNHEALTHY") << "\n";
            
            if (!logger.isHealthy()) {
                auto issues = logger.getHealthIssues();
                report << "Logger Issues:\n";
                for (const auto& issue : issues) {
                    report << "  - " << issue << "\n";
                }
            }
            
            // Config health check (if available)
            // report << "Config Status: HEALTHY\n";
            
            // System overall status
            bool overallHealthy = isSystemHealthy();
            report << "\nOverall System Status: " << (overallHealthy ? "HEALTHY" : "UNHEALTHY") << "\n";
            
            return report.str();
        }
        
    } // namespace HealthCheck

    // Error monitoring utilities
    namespace ErrorMonitor {
        
        struct ErrorStats {
            uint64_t totalErrors = 0;
            uint64_t errorsByCategory[(int)ErrorCategory::System + 1] = {0};
            uint64_t errorsBySeverity[(int)ErrorSeverity::Critical + 1] = {0};
            std::chrono::system_clock::time_point firstError;
            std::chrono::system_clock::time_point lastError;
            std::string mostRecentError;
        };
        
        static ErrorStats errorStats;
        static std::mutex statsMutex;
        
        // Record an error
        void recordError(const NLMException& error) {
            std::lock_guard<std::mutex> lock(statsMutex);
            
            errorStats.totalErrors++;
            errorStats.errorsByCategory[(int)error.getCategory()]++;
            errorStats.errorsBySeverity[(int)error.getSeverity()]++;
            errorStats.lastError = std::chrono::system_clock::now();
            if (errorStats.totalErrors == 1) {
                errorStats.firstError = errorStats.lastError;
            }
            errorStats.mostRecentError = error.what();
        }
        
        // Get error statistics
        ErrorStats getErrorStats() {
            std::lock_guard<std::mutex> lock(statsMutex);
            return errorStats;
        }
        
        // Generate error report
        std::string generateErrorReport() {
            std::stringstream report;
            report << "NLM Error Statistics Report\n";
            report << "============================\n";
            report << "Timestamp: " << ErrorHandling::getCurrentTimestampString() << "\n";
            report << "Total Errors: " << errorStats.totalErrors << "\n";
            report << "\n";
            
            report << "Errors by Category:\n";
            for (int i = 0; i <= (int)ErrorCategory::System; ++i) {
                if (errorStats.errorsByCategory[i] > 0) {
                    report << "  " << errorCategoryToString(static_cast<ErrorCategory>(i)) << 
                           ": " << errorStats.errorsByCategory[i] << "\n";
                }
            }
            
            report << "\nErrors by Severity:\n";
            for (int i = 0; i <= (int)ErrorSeverity::Critical; ++i) {
                if (errorStats.errorsBySeverity[i] > 0) {
                    report << "  " << errorSeverityToString(static_cast<ErrorSeverity>(i)) << 
                           ": " << errorStats.errorsBySeverity[i] << "\n";
                }
            }
            
            if (errorStats.totalErrors > 0) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    errorStats.lastError - errorStats.firstError);
                report << "\nError Summary:\n";
                report << "  First Error: " << ErrorHandling::getTimestampString(errorStats.firstError) << "\n";
                report << "  Last Error: " << ErrorHandling::getTimestampString(errorStats.lastError) << "\n";
                report << "  Duration: " << duration.count() << " seconds\n";
                report << "  Most Recent: " << errorStats.mostRecentError << "\n";
            }
            
            return report.str();
        }
        
    private:
        std::string errorCategoryToString(ErrorCategory category) {
            switch (category) {
                case ErrorCategory::Configuration: return "Configuration";
                case ErrorCategory::FileIO: return "File I/O";
                case ErrorCategory::Validation: return "Validation";
                case ErrorCategory::Network: return "Network";
                case ErrorCategory::Memory: return "Memory";
                case ErrorCategory::Simulation: return "Simulation";
                case ErrorCategory::Logger: return "Logger";
                case ErrorCategory::Random: return "Random";
                case ErrorCategory::System: return "System";
                default: return "Unknown";
            }
        }
        
        std::string errorSeverityToString(ErrorSeverity severity) {
            switch (severity) {
                case ErrorSeverity::Debug: return "Debug";
                case ErrorSeverity::Info: return "Info";
                case ErrorSeverity::Warning: return "Warning";
                case ErrorSeverity::Error: return "Error";
                case ErrorSeverity::Critical: return "Critical";
                default: return "Unknown";
            }
        }
        
    } // namespace ErrorMonitor
    
} // namespace ErrorRecovery

// Global error recovery registration
inline void registerDefaultErrorRecoveryCallbacks() {
    globalErrorRecoveryManager.registerRecoveryCallback(
        ErrorCategory::Configuration, 
        ErrorRecovery::configRecoveryCallback
    );
    
    globalErrorRecoveryManager.registerRecoveryCallback(
        ErrorCategory::Logger,
        ErrorRecovery::loggerRecoveryCallback
    );
    
    globalErrorRecoveryManager.registerRecoveryCallback(
        ErrorCategory::Simulation,
        ErrorRecovery::neuronRecoveryCallback
    );
    
    globalErrorRecoveryManager.registerRecoveryCallback(
        ErrorCategory::Simulation,
        ErrorRecovery::synapseRecoveryCallback
    );
}

// Initialize default error recovery callbacks
static bool errorRecoveryInitialized = []() {
    registerDefaultErrorRecoveryCallbacks();
    return true;
}();

} // namespace nlm
