#include "ErrorReporting.hpp"
#include <fstream>
#include <iostream>

namespace nlm {

namespace ErrorReporting {
    
    // Initialize error reporting system
    class ErrorReportingInitializer {
    public:
        ErrorReportingInitializer() {
            // Set up global error reporter
            auto consoleReporter = std::make_shared<ConsoleErrorReporter>();
            consoleReporter->setSeverityThreshold(ErrorSeverity::Warning);
            ErrorHandling::setGlobalErrorReporter(consoleReporter);
            
            // Register default error recovery callbacks
            ErrorRecovery::registerDefaultErrorRecoveryCallbacks();
            
            // Initialize error monitoring
            ErrorMonitor::getErrorStats(); // Initialize static variables
            
            // Log system initialization
            auto& logger = Logger::getGlobal();
            logger.debug("Error reporting system initialized");
        }
    };
    
    // Global initializer instance
    static ErrorReportingInitializer reportingInitializer;
    
    // Generate comprehensive error report
    std::string generateComprehensiveErrorReport() {
        std::stringstream report;
        report << "=== NLM COMPREHENSIVE ERROR REPORT ===\n";
        report << "Generated: " << ErrorHandling::getCurrentTimestampString() << "\n\n";
        
        // System health check
        report << "--- SYSTEM HEALTH ---\n";
        bool systemHealthy = HealthCheck::isSystemHealthy();
        report << "System Health: " << (systemHealthy ? "HEALTHY" : "UNHEALTHY") << "\n";
        report << HealthCheck::generateHealthReport() << "\n";
        
        // Error statistics
        report << "--- ERROR STATISTICS ---\n";
        report << ErrorMonitor::generateErrorReport() << "\n";
        
        // Recent errors
        report << "--- RECENT ERRORS ---\n";
        auto stats = ErrorMonitor::getErrorStats();
        if (stats.totalErrors == 0) {
            report << "No errors recorded.\n";
        } else {
            report << "Total Errors: " << stats.totalErrors << "\n";
            report << "Last Error: " << stats.mostRecentError << "\n";
            report << "First Error: " << ErrorHandling::getTimestampString(stats.firstError) << "\n";
            
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                stats.lastError - stats.firstError);
            report << "Error Period: " << duration.count() << " seconds\n\n";
        }
        
        // Component status
        report << "--- COMPONENT STATUS ---\n";
        auto& logger = Logger::getGlobal();
        report << "Logger: " << (logger.isHealthy() ? "HEALTHY" : "UNHEALTHY") << "\n";
        
        if (!logger.isHealthy()) {
            auto issues = logger.getHealthIssues();
            report << "Logger Issues:";
            for (const auto& issue : issues) {
                report << "\n  - " << issue;
            }
            report << "\n";
        }
        
        // Global error reporter status
        auto reporter = ErrorHandling::getGlobalErrorReporter();
        if (reporter) {
            report << "Error Reporter: AVAILABLE\n";
            report << "Reporter Severity Threshold: " << 
                static_cast<int>(reporter->getSeverityThreshold()) << "\n";
        } else {
            report << "Error Reporter: NOT AVAILABLE\n";
        }
        
        report << "\n=== END OF REPORT ===\n";
        
        return report.str();
    }
    
    // Save error report to file
    bool saveErrorReport(const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_THROW_FILE_ERROR("Failed to open error report file: " + filepath);
            }
            
            std::string report = generateComprehensiveErrorReport();
            file << report;
            file.flush();
            
            if (!file.good()) {
                NLM_THROW_FILE_ERROR("Error writing to error report file: " + filepath);
            }
            
            return true;
        } catch (const NLMException& e) {
            NLM_REPORT_ERROR("Failed to save error report: " + std::string(e.what()),
                            ErrorCategory::FileIO, ErrorSeverity::Error);
            return false;
        }
    }
    
    // Clear error statistics
    void clearErrorStats() {
        std::lock_guard<std::mutex> lock(ErrorMonitor::statsMutex);
        ErrorMonitor::errorStats = ErrorMonitor::ErrorStats();
    }
    
    // Get error report as string
    std::string getErrorReportString() {
        return generateComprehensiveErrorReport();
    }
    
} // namespace ErrorReporting

// Convenience functions for error reporting
inline void reportErrorWithContext(const std::string& message,
                                 const std::string& component = "",
                                 ErrorCategory category = ErrorCategory::System,
                                 ErrorSeverity severity = ErrorSeverity::Error) {
    auto context = ErrorHandling::createContext(component);
    ErrorHandling::reportError(message, context, category, severity);
}

inline void logError(const std::string& message,
                    const std::string& component = "",
                    ErrorCategory category = ErrorCategory::System,
                    ErrorSeverity severity = ErrorSeverity::Error) {
    auto context = ErrorHandling::createContext(component);
    ErrorHandling::reportError(message, context, category, severity);
    
    // Also log to system logger if available
    auto& logger = Logger::getGlobal();
    logger.error(message, "", 0, component);
}

inline bool attemptSystemRecovery(const NLMException& error) {
    return globalErrorRecoveryManager.attemptRecovery(error);
}

inline std::string getSystemErrorReport() {
    return ErrorReporting::getErrorReportString();
}

inline bool saveSystemErrorReport(const std::string& filepath) {
    return ErrorReporting::saveErrorReport(filepath);
}

inline void clearSystemErrorStats() {
    ErrorReporting::clearErrorStats();
}

} // namespace nlm
