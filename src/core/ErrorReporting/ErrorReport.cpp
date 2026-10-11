#include "ErrorReport.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <ctime>

namespace nlm {

// Default error reporter implementation
class DefaultErrorReporter : public IErrorReporter {
private:
    std::vector<DetailedError> errorHistory_;
    std::vector<DetailedError> warningHistory_;
    mutable std::mutex mutex_;
    std::map<ErrorCategory, uint64_t> errorCounts_;
    std::map<ErrorCategory, uint64_t> warningCounts_;
    std::map<ErrorCategory, std::chrono::system_clock::time_point> lastErrorTime_;
    std::map<ErrorCategory, std::chrono::system_clock::time_point> lastWarningTime_;

public:
    void reportError(const DetailedError& error) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Update counts
        errorCounts_[error.category]++;
        lastErrorTime_[error.category] = std::chrono::system_clock::now();
        
        // Add to history
        errorHistory_.push_back(error);
        
        // Log to logger system
        logDetailedError(error);
        
        // Check if error should be filtered
        if (error.severity == ErrorSeverity::Critical || error.severity == ErrorSeverity::Fatal) {
            // Critical errors go to the main error log
            logCriticalError(error);
        }
    }

    void reportWarning(const DetailedError& warning) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Update counts
        warningCounts_[warning.category]++;
        lastWarningTime_[warning.category] = std::chrono::system_clock::now();
        
        // Add to history
        warningHistory_.push_back(warning);
        
        // Log to logger system
        logDetailedWarning(warning);
    }

    uint64_t getErrorCount(ErrorCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (category == ErrorCategory::None) {
            uint64_t total = 0;
            for (const auto& pair : errorCounts_) {
                total += pair.second;
            }
            return total;
        }
        
        auto it = errorCounts_.find(category);
        return (it != errorCounts_.end()) ? it->second : 0;
    }

    uint64_t getWarningCount(ErrorCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (category == ErrorCategory::None) {
            uint64_t total = 0;
            for (const auto& pair : warningCounts_) {
                total += pair.second;
            }
            return total;
        }
        
        auto it = warningCounts_.find(category);
        return (it != warningCounts_.end()) ? it->second : 0;
    }

    std::chrono::system_clock::time_point getLastErrorTime(ErrorCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (category == ErrorCategory::None) {
            std::chrono::system_clock::time_point latest;
            for (const auto& pair : lastErrorTime_) {
                if (pair.second > latest) {
                    latest = pair.second;
                }
            }
            return latest;
        }
        
        auto it = lastErrorTime_.find(category);
        return (it != lastErrorTime_.end()) ? it->second : std::chrono::system_clock::time_point();
    }

    std::vector<DetailedError> getRecentErrors(size_t maxCount) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (errorHistory_.size() <= maxCount) {
            return errorHistory_;
        }
        
        size_t startIndex = errorHistory_.size() - maxCount;
        return std::vector<DetailedError>(errorHistory_.begin() + startIndex, errorHistory_.end());
    }

    std::vector<DetailedError> getRecentWarnings(size_t maxCount) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (warningHistory_.size() <= maxCount) {
            return warningHistory_;
        }
        
        size_t startIndex = warningHistory_.size() - maxCount;
        return std::vector<DetailedError>(warningHistory_.begin() + startIndex, warningHistory_.end());
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        errorHistory_.clear();
        warningHistory_.clear();
        errorCounts_.clear();
        warningCounts_.clear();
        lastErrorTime_.clear();
        lastWarningTime_.clear();
    }

    ErrorReport generateReport() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        ErrorReport report;
        report.errors = errorHistory_;
        report.warnings = warningHistory_;
        report.errorCounts = errorCounts_;
        report.totalErrors = errorHistory_.size();
        report.totalWarnings = warningHistory_.size();
        report.summary = generateSummary();
        
        return report;
    }

    void configure(const std::map<std::string, std::string>& config) override {
        // Configuration for error reporter
        // This can be used to configure filtering, logging levels, etc.
    }

private:
    void logDetailedError(const DetailedError& error) {
        // Convert DetailedError to LogEntry and log it
        LogEntry entry;
        entry.level = (error.severity >= ErrorSeverity::Error) ? 
                      (error.severity >= ErrorSeverity::Critical ? LogLevel::Critical : LogLevel::Error) :
                      (error.severity >= ErrorSeverity::Warning ? LogLevel::Warning : 
                       (error.severity >= ErrorSeverity::Info ? LogLevel::Info : LogLevel::Debug));
        
        std::ostringstream oss;
        oss << "[" << getCategoryString(error.category) << "] " 
            << "[" << getSeverityString(error.severity) << "] " 
            << error.code << ": " << error.message;
        
        if (!error.context.empty()) {
            oss << " (Context: " << error.context << ")";
        }
        
        if (!error.file.empty()) {
            oss << " at " << error.file;
            if (error.line > 0) oss << ":" << error.line;
        }
        
        if (error.stackTrace.empty()) {
            oss << " [" << getFormattedTimestamp(error.timestamp) << "]";
        }
        
        entry.message = oss.str();
        entry.file = error.file;
        entry.line = error.line;
        entry.function = error.function;
        entry.timestamp = error.timestamp;
        
        // Use global logger to log the error
        Logger::getGlobal().log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }

    void logDetailedWarning(const DetailedError& warning) {
        // Similar to logDetailedError but for warnings
        LogEntry entry;
        entry.level = LogLevel::Warning;
        
        std::ostringstream oss;
        oss << "[" << getCategoryString(warning.category) << "] " 
            << "[" << getSeverityString(warning.severity) << "] " 
            << warning.code << ": " << warning.message;
        
        if (!warning.context.empty()) {
            oss << " (Context: " << warning.context << ")";
        }
        
        entry.message = oss.str();
        entry.file = warning.file;
        entry.line = warning.line;
        entry.function = warning.function;
        entry.timestamp = warning.timestamp;
        
        Logger::getGlobal().log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }

    void logCriticalError(const DetailedError& error) {
        // Critical errors go to a separate critical log
        LogEntry entry;
        entry.level = LogLevel::Critical;
        
        std::ostringstream oss;
        oss << "*** CRITICAL ERROR *** " 
            << "[" << getCategoryString(error.category) << "] " 
            << error.code << ": " << error.message;
        
        if (!error.context.empty()) {
            oss << " (Context: " << error.context << ")";
        }
        
        if (!error.file.empty()) {
            oss << " at " << error.file;
            if (error.line > 0) oss << ":" << error.line;
        }
        
        oss << " - IMMINENT SYSTEM FAILURE";
        
        entry.message = oss.str();
        entry.file = error.file;
        entry.line = error.line;
        entry.function = error.function;
        entry.timestamp = error.timestamp;
        
        Logger::getGlobal().log(entry.level, entry.message, entry.file, entry.line, entry.function);
    }

    std::string getCategoryString(ErrorCategory category) const {
        static const std::unordered_map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "NONE"},
            {ErrorCategory::Configuration, "CONFIG"},
            {ErrorCategory::Memory, "MEMORY"},
            {ErrorCategory::Brain, "BRAIN"},
            {ErrorCategory::Logger, "LOGGER"},
            {ErrorCategory::World, "WORLD"},
            {ErrorCategory::Validation, "VALIDATION"},
            {ErrorCategory::System, "SYSTEM"},
            {ErrorCategory::Network, "NETWORK"},
            {ErrorCategory::IO, "IO"},
            {ErrorCategory::Plasticity, "PLASTICITY"},
            {ErrorCategory::Neuromodulation, "NEUROMOD"},
            {ErrorCategory::Cognitive, "COGNITIVE"},
            {ErrorCategory::Development, "DEVELOPMENT"},
            {ErrorCategory::Prediction, "PREDICTION"},
            {ErrorCategory::MemorySystem, "MEMSYS"},
            {ErrorCategory::Checkpoint, "CHECKPOINT"}
        };
        
        auto it = categoryStrings.find(category);
        return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
    }

    std::string getSeverityString(ErrorSeverity severity) const {
        static const std::unordered_map<ErrorSeverity, std::string> severityStrings = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARN"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRIT"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        
        auto it = severityStrings.find(severity);
        return (it != severityStrings.end()) ? it->second : "UNKNOWN";
    }

    std::string getFormattedTimestamp(std::chrono::system_clock::time_point timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return buf;
    }

    std::string generateSummary() const {
        std::ostringstream oss;
        oss << "Error Report Summary:\n";
        oss << "Total Errors: " << errorHistory_.size() << "\n";
        oss << "Total Warnings: " << warningHistory_.size() << "\n";
        
        if (!errorCounts_.empty()) {
            oss << "\nErrors by Category:\n";
            for (const auto& pair : errorCounts_) {
                oss << "  " << getCategoryString(pair.first) << ": " << pair.second << "\n";
            }
        }
        
        if (!warningCounts_.empty()) {
            oss << "\nWarnings by Category:\n";
            for (const auto& pair : warningCounts_) {
                oss << "  " << getCategoryString(pair.first) << ": " << pair.second << "\n";
            }
        }
        
        return oss.str();
    }
};

// Default error handler implementation
class DefaultErrorHandler : public IErrorHandler {
public:
    void reportError(const DetailedError& error) override {
        // Simple error reporting for backward compatibility
        // In a real implementation, this would use the ErrorManager
        std::cout << "Error: " << error.message << std::endl;
        if (!error.context.empty()) {
            std::cout << "Context: " << error.context << std::endl;
        }
    }

    void reportWarning(const DetailedError& warning) override {
        // Simple warning reporting for backward compatibility
        std::cout << "Warning: " << warning.message << std::endl;
        if (!warning.context.empty()) {
            std::cout << "Context: " << warning.context << std::endl;
        }
    }

    bool handleError(const DetailedError& error) override {
        // Default error handling - just log it
        reportError(error);
        return false; // Not handled - let other handlers try
    }

    bool handleWarning(const DetailedError& warning) override {
        // Default warning handling - just log it
        reportWarning(warning);
        return false; // Not handled - let other handlers try
    }

    bool canHandle(ErrorCategory category, ErrorSeverity severity) const override {
        // Default handler can handle everything
        return true;
    }

    int getPriority() const override {
        // Low priority - other handlers should get first chance
        return 0;
    }

    std::string getName() const override {
        return "DefaultErrorHandler";
    }
};

// Default error monitor implementation
class DefaultErrorMonitor : public IErrorMonitor {
private:
    bool monitoring_ = false;
    std::map<ErrorCategory, uint64_t> errorThresholds_;
    std::map<ErrorCategory, uint64_t> currentErrorCounts_;
    mutable std::mutex mutex_;

public:
    void startMonitoring() override {
        std::lock_guard<std::mutex> lock(mutex_);
        monitoring_ = true;
        // Reset error counts
        currentErrorCounts_.clear();
    }

    void stopMonitoring() override {
        std::lock_guard<std::mutex> lock(mutex_);
        monitoring_ = false;
    }

    bool isHealthy() const override {
        if (!monitoring_) {
            return true; // Not monitoring, assume healthy
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : errorThresholds_) {
            auto it = currentErrorCounts_.find(pair.first);
            if (it != currentErrorCounts_.end() && it->second >= pair.second) {
                return false; // Threshold exceeded
            }
        }
        return true;
    }

    std::string getHealthReport() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ostringstream oss;
        oss << "System Health Report:\n";
        oss << "Monitoring: " << (monitoring_ ? "Enabled" : "Disabled") << "\n";
        
        if (isHealthy()) {
            oss << "Status: HEALTHY\n";
        } else {
            oss << "Status: UNHEALTHY - Error thresholds exceeded\n";
            oss << "Thresholds:\n";
            for (const auto& pair : errorThresholds_) {
                auto it = currentErrorCounts_.find(pair.first);
                uint64_t current = (it != currentErrorCounts_.end()) ? it->second : 0;
                oss << "  " << getCategoryString(pair.first) << ": " << current << "/" << pair.second << "\n";
            }
        }
        
        return oss.str();
    }

    void setErrorThreshold(ErrorSeverity severity, uint64_t maxErrors) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Convert severity to category for thresholds
        ErrorCategory category;
        if (severity >= ErrorSeverity::Critical) {
            category = ErrorCategory::System;
        } else if (severity >= ErrorSeverity::Error) {
            category = ErrorCategory::Brain;
        } else if (severity >= ErrorSeverity::Warning) {
            category = ErrorCategory::Logger;
        } else {
            category = ErrorCategory::None;
        }
        
        errorThresholds_[category] = maxErrors;
    }

    bool isErrorThresholdExceeded(ErrorCategory category) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = errorThresholds_.find(category);
        if (it == errorThresholds_.end()) {
            return false; // No threshold set
        }
        
        auto countIt = currentErrorCounts_.find(category);
        if (countIt == currentErrorCounts_.end()) {
            return false; // No errors
        }
        
        return countIt->second >= it->second;
    }

    void incrementErrorCount(ErrorCategory category) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentErrorCounts_[category]++;
    }

    void resetErrorCount(ErrorCategory category) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentErrorCounts_[category] = 0;
    }

private:
    std::string getCategoryString(ErrorCategory category) const {
        static const std::unordered_map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::None, "NONE"},
            {ErrorCategory::Configuration, "CONFIG"},
            {ErrorCategory::Memory, "MEMORY"},
            {ErrorCategory::Brain, "BRAIN"},
            {ErrorCategory::Logger, "LOGGER"},
            {ErrorCategory::World, "WORLD"},
            {ErrorCategory::Validation, "VALIDATION"},
            {ErrorCategory::System, "SYSTEM"},
            {ErrorCategory::Network, "NETWORK"},
            {ErrorCategory::IO, "IO"},
            {ErrorCategory::Plasticity, "PLASTICITY"},
            {ErrorCategory::Neuromodulation, "NEUROMOD"},
            {ErrorCategory::Cognitive, "COGNITIVE"},
            {ErrorCategory::Development, "DEVELOPMENT"},
            {ErrorCategory::Prediction, "PREDICTION"},
            {ErrorCategory::MemorySystem, "MEMSYS"},
            {ErrorCategory::Checkpoint, "CHECKPOINT"}
        };
        
        auto it = categoryStrings.find(category);
        return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
    }
};

// ErrorManager implementation
void ErrorManager::addErrorReporter(std::unique_ptr<IErrorReporter> reporter) {
    if (reporter) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorReporters_.push_back(std::move(reporter));
    }
}

void ErrorManager::removeErrorReporter(IErrorReporter* reporter) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorReporters_.erase(
        std::remove_if(errorReporters_.begin(), errorReporters_.end(),
                      [reporter](const std::unique_ptr<IErrorReporter>& r) { return r.get() == reporter; }),
        errorReporters_.end()
    );
}

void ErrorManager::addErrorHandler(std::unique_ptr<IErrorHandler> handler) {
    if (handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorHandlers_.push_back(std::move(handler));
    }
}

void ErrorManager::removeErrorHandler(IErrorHandler* handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorHandlers_.erase(
        std::remove_if(errorHandlers_.begin(), errorHandlers_.end(),
                      [handler](const std::unique_ptr<IErrorHandler>& h) { return h.get() == handler; }),
        errorHandlers_.end()
    );
}

void ErrorManager::addErrorMonitor(std::unique_ptr<IErrorMonitor> monitor) {
    if (monitor) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorMonitors_.push_back(std::move(monitor));
    }
}

void ErrorManager::removeErrorMonitor(IErrorMonitor* monitor) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorMonitors_.erase(
        std::remove_if(errorMonitors_.begin(), errorMonitors_.end(),
                      [monitor](const std::unique_ptr<IErrorMonitor>& m) { return m.get() == monitor; }),
        errorMonitors_.end()
    );
}

void ErrorManager::reportError(const DetailedError& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Report error to all reporters
    for (const auto& reporter : errorReporters_) {
        reporter->reportError(error);
    }
    
    // Handle error with all handlers
    for (const auto& handler : errorHandlers_) {
        if (handler->canHandle(error.category, error.severity)) {
            if (handler->handleError(error)) {
                return; // Error was handled
            }
        }
    }
    
    // Report to monitors
    for (const auto& monitor : errorMonitors_) {
        // Update error counts for thresholds
        static_cast<DefaultErrorMonitor*>(monitor.get())->incrementErrorCount(error.category);
    }
}

void ErrorManager::reportWarning(const DetailedError& warning) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Report warning to all reporters
    for (const auto& reporter : errorReporters_) {
        reporter->reportWarning(warning);
    }
    
    // Handle warning with all handlers
    for (const auto& handler : errorHandlers_) {
        if (handler->canHandle(warning.category, warning.severity)) {
            if (handler->handleWarning(warning)) {
                return; // Warning was handled
            }
        }
    }
}

void ErrorManager::reportError(ErrorCategory category, const std::string& code,
                              const std::string& message, const std::string& context,
                              ErrorSeverity severity, const std::string& file,
                              int line, const std::string& function) {
    DetailedError error(category, code, message, context, severity);
    error.file = file;
    error.line = line;
    error.function = function;
    reportError(error);
}

void ErrorManager::reportWarning(ErrorCategory category, const std::string& code,
                                const std::string& message, const std::string& context,
                                const std::string& file, int line, const std::string& function) {
    DetailedError warning(category, code, message, context, ErrorSeverity::Warning);
    warning.file = file;
    warning.line = line;
    warning.function = function;
    reportWarning(warning);
}

ErrorReport ErrorManager::getErrorReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ErrorReport report;
    
    // Generate report from first reporter if available
    if (!errorReporters_.empty()) {
        report = errorReporters_[0]->generateReport();
    }
    
    return report;
}

void ErrorManager::clearErrors() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& reporter : errorReporters_) {
        reporter->clear();
    }
}

void ErrorManager::configure(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& reporter : errorReporters_) {
        reporter->configure(config);
    }
}

uint64_t ErrorManager::getTotalErrorCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint64_t total = 0;
    for (const auto& reporter : errorReporters_) {
        total += reporter->getErrorCount(ErrorCategory::None);
    }
    return total;
}

uint64_t ErrorManager::getTotalWarningCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    uint64_t total = 0;
    for (const auto& reporter : errorReporters_) {
        total += reporter->getWarningCount(ErrorCategory::None);
    }
    return total;
}

uint64_t ErrorManager::getErrorCount(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (errorReporters_.empty()) {
        return 0;
    }
    
    return errorReporters_[0]->getErrorCount(category);
}

uint64_t ErrorManager::getWarningCount(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (errorReporters_.empty()) {
        return 0;
    }
    
    return errorReporters_[0]->getWarningCount(category);
}

bool ErrorManager::isSystemHealthy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& monitor : errorMonitors_) {
        if (!monitor->isHealthy()) {
            return false;
        }
    }
    return true;
}

std::string ErrorManager::getHealthReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "ErrorManager Health Report:\n";
    oss << "Total Errors: " << getTotalErrorCount() << "\n";
    oss << "Total Warnings: " << getTotalWarningCount() << "\n";
    oss << "System Healthy: " << (isSystemHealthy() ? "Yes" : "No") << "\n\n";
    
    for (const auto& monitor : errorMonitors_) {
        oss << monitor->getHealthReport() << "\n";
    }
    
    return oss.str();
}

void ErrorManager::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& monitor : errorMonitors_) {
        monitor->startMonitoring();
    }
}

void ErrorManager::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& monitor : errorMonitors_) {
        monitor->stopMonitoring();
    }
}

std::vector<IErrorMonitor*> ErrorManager::getActiveMonitors() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<IErrorMonitor*> monitors;
    for (const auto& monitor : errorMonitors_) {
        monitors.push_back(monitor.get());
    }
    return monitors;
}

bool ErrorManager::shouldReport(const DetailedError& error) const {
    // Default implementation - report all errors
    return true;
}

bool ErrorManager::shouldHandle(const DetailedError& error) const {
    // Default implementation - try all handlers
    return true;
}

// Global ErrorManager instance
static ErrorManager& getGlobalErrorManager() {
    static ErrorManager manager;
    return manager;
}

// ErrorReporting namespace functions
void ErrorReporting::initialize() {
    getGlobalErrorManager().startMonitoring();
}

void ErrorReporting::shutdown() {
    getGlobalErrorManager().stopMonitoring();
}

void ErrorReporting::report(ErrorCategory category, const std::string& code,
                           const std::string& message, const std::string& context,
                           ErrorSeverity severity, const std::string& file,
                           int line, const std::string& function) {
    DetailedError error(category, code, message, context, severity);
    error.file = file;
    error.line = line;
    error.function = function;
    
    getGlobalErrorManager().reportError(error);
}

void ErrorReporting::reportWarning(ErrorCategory category, const std::string& code,
                                  const std::string& message, const std::string& context,
                                  const std::string& file, int line, const std::string& function) {
    DetailedError warning(category, code, message, context, ErrorSeverity::Warning);
    warning.file = file;
    warning.line = line;
    warning.function = function;
    
    getGlobalErrorManager().reportWarning(warning);
}

uint64_t ErrorReporting::getErrorCount(ErrorCategory category) {
    return getGlobalErrorManager().getErrorCount(category);
}

uint64_t ErrorReporting::getWarningCount(ErrorCategory category) {
    return getGlobalErrorManager().getWarningCount(category);
}

ErrorReport ErrorReporting::getErrorReport() {
    return getGlobalErrorManager().getErrorReport();
}

bool ErrorReporting::isSystemHealthy() {
    return getGlobalErrorManager().isSystemHealthy();
}

std::string ErrorReporting::getHealthReport() {
    return getGlobalErrorManager().getHealthReport();
}

void ErrorReporting::configure(const std::map<std::string, std::string>& config) {
    getGlobalErrorManager().configure(config);
}

void ErrorReporting::clearErrors() {
    getGlobalErrorManager().clearErrors();
}

} // namespace nlm

// Global error reporting setup
namespace {
    bool initialized = false;
    std::mutex setupMutex;
}

// Global function to initialize error reporting
void initializeErrorReporting() {
    std::lock_guard<std::mutex> lock(nlm::ErrorReporting::setupMutex);
    
    if (!nlm::ErrorReporting::initialized) {
        ErrorManager& manager = getGlobalErrorManager();
        
        // Add default error reporter
        manager.addErrorReporter(std::make_unique<DefaultErrorReporter>());
        
        // Add default error handler
        manager.addErrorHandler(std::make_unique<DefaultErrorHandler>());
        
        // Add default error monitor
        manager.addErrorMonitor(std::make_unique<DefaultErrorMonitor>());
        
        // Configure error manager
        std::map<std::string, std::string> config;
        config["report_level"] = "ALL";
        config["filter_critical"] = "true";
        config["output_file"] = "nlm_errors.log";
        
        manager.configure(config);
        
        // Start monitoring
        manager.startMonitoring();
        
        nlm::ErrorReporting::initialized = true;
    }
}

// Global function to shutdown error reporting
void shutdownErrorReporting() {
    std::lock_guard<std::mutex> lock(nlm::ErrorReporting::setupMutex);
    
    if (nlm::ErrorReporting::initialized) {
        getGlobalErrorManager().stopMonitoring();
        nlm::ErrorReporting::initialized = false;
    }
}

// Error reporting system initialization function
void setupErrorReporting() {
    initializeErrorReporting();
}

// Error reporting system cleanup function
void cleanupErrorReporting() {
    shutdownErrorReporting();
}