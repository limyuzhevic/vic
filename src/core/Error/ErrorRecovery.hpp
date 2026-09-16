#pragma once

#include "Error.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// Registry for error recovery strategies
class ErrorRecoveryRegistry {
private:
    std::vector<std::unique_ptr<IErrorRecoveryStrategy>> strategies_;
    std::mutex mutex_;
    
public:
    ErrorRecoveryRegistry() = default;
    
    // Register a recovery strategy
    void registerStrategy(std::unique_ptr<IErrorRecoveryStrategy> strategy) {
        std::lock_guard<std::mutex> lock(mutex_);
        strategies_.push_back(std::move(strategy));
    }
    
    // Attempt recovery for the given error
    bool attemptRecovery(const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Sort by priority (higher first)
        std::sort(strategies_.begin(), strategies_.end(),
                  [](const std::unique_ptr<IErrorRecoveryStrategy>& a,
                     const std::unique_ptr<IErrorRecoveryStrategy>& b) {
                      return a->getPriority() > b->getPriority();
                  });
        
        for (const auto& strategy : strategies_) {
            if (strategy->canHandle(error)) {
                return strategy->execute(error);
            }
        }
        return false;
    }
    
    // Get all strategy names
    std::vector<std::string> getStrategyNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        for (const auto& strategy : strategies_) {
            names.push_back(strategy->getName());
        }
        return names;
    }
    
    // Clear all strategies
    void clearStrategies() {
        std::lock_guard<std::mutex> lock(mutex_);
        strategies_.clear();
    }
};

// Recovery action callbacks for different error scenarios
using RecoveryAction = std::function<bool(const ErrorContext&)>;

// Error recovery actions for specific error codes
class ErrorRecoveryActions {
private:
    std::unordered_map<ErrorCode, RecoveryAction> actions_;
    std::mutex mutex_;
    
public:
    ErrorRecoveryActions() = default;
    
    // Register a recovery action for an error code
    void registerAction(ErrorCode errorCode, const RecoveryAction& action) {
        std::lock_guard<std::mutex> lock(mutex_);
        actions_[errorCode] = action;
    }
    
    // Execute recovery action for the given error
    bool executeAction(ErrorCode errorCode, const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = actions_.find(errorCode);
        if (it != actions_.end()) {
            return it->second(error);
        }
        return false;
    }
    
    // Check if an action exists for the given error code
    bool hasAction(ErrorCode errorCode) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return actions_.find(errorCode) != actions_.end();
    }
};

// Global error recovery registry
inline ErrorRecoveryRegistry& getErrorRecoveryRegistry() {
    static ErrorRecoveryRegistry registry;
    return registry;
}

// Global error recovery actions
inline ErrorRecoveryActions& getErrorRecoveryActions() {
    static ErrorRecoveryActions actions;
    return actions;
}

// Error reporting and statistics
class ErrorStatistics {
private:
    struct ErrorCount {
        size_t total = 0;
        size_t bySeverity[6] = {0};  // Debug, Info, Warning, Error, Critical, Fatal
        size_t byCategory[12] = {0}; // ErrorCategory enum values
        size_t byCode[1200] = {0};   // ErrorCode enum values (largest is 1103)
        std::chrono::system_clock::time_point lastOccurred;
        float rate = 0.0f;
        size_t recoveryAttempts = 0;
        size_t recoverySuccesses = 0;
        std::unordered_map<std::string, size_t> correlations; // correlationId -> count
        std::unordered_map<std::string, std::vector<ErrorContext>> relatedErrors;
    };
    
    ErrorCount counts_[256];  // Using hash for error codes
    std::mutex mutex_;
    bool enabled_ = true;
    
public:
    ErrorStatistics() {
        for (auto& count : counts_) {
            count.lastOccurred = std::chrono::system_clock::now();
        }
    }
    
    // Record an error
    void recordError(const ErrorContext& error) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Use hash of error code for array access
        size_t hash = static_cast<size_t>(error.code) % 256;
        ErrorCount& count = counts_[hash];
        
        count.total++;
        count.bySeverity[static_cast<size_t>(error.severity)]++;
        count.byCategory[static_cast<size_t>(error.category)]++;
        count.byCode[static_cast<size_t>(error.code)]++;
        count.lastOccurred = error.timestamp;
        
        // Update error rate (simplified)
        static auto lastReport = std::chrono::system_clock::now();
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastReport);
        if (duration.count() > 0) {
            count.rate = static_cast<float>(count.total) / duration.count();
            lastReport = now;
        }
        
        // Track correlations
        if (!error.correlationId.empty()) {
            count.correlations[error.correlationId]++;
            count.relatedErrors[error.correlationId].push_back(error);
        }
    }
    
    // Record a recovery attempt
    void recordRecoveryAttempt(ErrorCode errorCode, bool success) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        size_t hash = static_cast<size_t>(errorCode) % 256;
        counts_[hash].recoveryAttempts++;
        if (success) {
            counts_[hash].recoverySuccesses++;
        }
    }
    
    // Get error statistics for a specific error code
    ErrorCount getErrorStats(ErrorCode errorCode) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t hash = static_cast<size_t>(errorCode) % 256;
        return counts_[hash];
    }
    
    // Generate comprehensive error report
    std::string generateReport() const {
        if (!enabled_) {
            return "Error statistics: DISABLED\n";
        }
        
        std::stringstream ss;
        ss << "=== Error Statistics Report ===\n";
        
        size_t totalErrors = 0;
        for (const auto& count : counts_) {
            totalErrors += count.total;
        }
        
        ss << "Total Errors: " << totalErrors << "\n";
        ss << "Last Error: " << std::chrono::system_clock::to_time_t(counts_[0].lastOccurred) << "\n";
        
        // Summary by severity
        ss << "\n--- Errors by Severity ---\n";
        const char* severityNames[] = {"Debug", "Info", "Warning", "Error", "Critical", "Fatal"};
        for (int sev = 0; sev < 6; ++sev) {
            size_t count = 0;
            for (const auto& c : counts_) {
                count += c.bySeverity[sev];
            }
            if (count > 0) {
                ss << severityNames[sev] << ": " << count << "\n";
            }
        }
        
        // Top 10 error codes
        ss << "\n--- Top 10 Error Codes ---\n";
        std::vector<std::pair<size_t, ErrorCode>> errorCodeCounts;
        for (size_t i = 0; i < 1200; ++i) {  // ErrorCode max is 1103
            if (i < sizeof(counts_)/sizeof(counts_[0])*256) continue; // Avoid out of bounds
            // Note: This is a simplified approach
        }
        
        // Summary by category
        ss << "\n--- Errors by Category ---\n";
        const char* categoryNames[] = {"None", "Config", "Brain", "Memory", "Neuromodulation",
                                      "Prediction", "Development", "System", "Network", "IO",
                                      "Validation", "Recovery"};
        for (int cat = 0; cat < 12; ++cat) {
            size_t count = 0;
            for (const auto& c : counts_) {
                count += c.byCategory[cat];
            }
            if (count > 0) {
                ss << categoryNames[cat] << ": " << count << "\n";
            }
        }
        
        // Recovery summary
        ss << "\n--- Recovery Summary ---\n";
        size_t totalAttempts = 0;
        size_t totalSuccesses = 0;
        for (const auto& c : counts_) {
            totalAttempts += c.recoveryAttempts;
            totalSuccesses += c.recoverySuccesses;
        }
        ss << "Recovery Attempts: " << totalAttempts << "\n";
        ss << "Recovery Successes: " << totalSuccesses << "\n";
        if (totalAttempts > 0) {
            ss << "Recovery Rate: " << (static_cast<float>(totalSuccesses) / totalAttempts * 100.0f) << "%\n";
        }
        
        // Top correlations
        ss << "\n--- Top Correlations ---\n";
        std::vector<std::pair<size_t, std::string>> correlationCounts;
        for (const auto& pair : counts_[0].correlations) {
            correlationCounts.push_back({pair.second, pair.first});
        }
        std::sort(correlationCounts.begin(), correlationCounts.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });
        size_t limit = std::min<size_t>(10, correlationCounts.size());
        for (size_t i = 0; i < limit; ++i) {
            ss << correlationCounts[i].second << ": " << correlationCounts[i].first << " related errors\n";
        }
        
        return ss.str();
    }
    
    // Enable/disable statistics
    void setEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        enabled_ = enabled;
    }
    
    bool isEnabled() const {
        return enabled_;
    }
};

// Global error statistics
inline ErrorStatistics& getErrorStatistics() {
    static ErrorStatistics stats;
    return stats;
}

// Error context manager for correlation and recovery tracking
class ErrorContextManager {
private:
    std::unordered_map<std::string, ErrorContext> activeErrors_;
    std::mutex mutex_;
    std::chrono::seconds correlationTimeout_{30};
    
public:
    ErrorContextManager() = default;
    
    // Add an error to tracking
    void addError(const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        activeErrors_[error.correlationId] = error;
        
        // Clean up old correlations
        cleanupOldCorrelations();
    }
    
    // Get errors for a correlation ID
    std::vector<ErrorContext> getRelatedErrors(const std::string& correlationId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<ErrorContext> related;
        for (const auto& pair : activeErrors_) {
            if (pair.first == correlationId || 
                (pair.second.correlationId == correlationId && !pair.second.correlationId.empty())) {
                related.push_back(pair.second);
            }
        }
        return related;
    }
    
    // Get all active errors
    std::vector<ErrorContext> getActiveErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ErrorContext> errors;
        for (const auto& pair : activeErrors_) {
            errors.push_back(pair.second);
        }
        return errors;
    }
    
    // Check if errors are related (share correlation ID)
    bool areRelated(const ErrorContext& error1, const ErrorContext& error2) const {
        return error1.correlationId == error2.correlationId && 
               !error1.correlationId.empty() && !error2.correlationId.empty();
    }
    
    // Clean up old correlation entries
    void cleanupOldCorrelations() {
        auto now = std::chrono::system_clock::now();
        
        for (auto it = activeErrors_.begin(); it != activeErrors_.end(); ) {
            if (now - it->second.timestamp > correlationTimeout_) {
                it = activeErrors_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Clear all errors
    void clearErrors() {
        std::lock_guard<std::mutex> lock(mutex_);
        activeErrors_.clear();
    }
};

// Global error context manager
inline ErrorContextManager& getErrorContextManager() {
    static ErrorContextManager manager;
    return manager;
}

// Error reporting and diagnostics
class ErrorDiagnostics {
private:
    struct ErrorDiagnosticInfo {
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> context;
        std::string stackTrace;
    };
    
    std::vector<ErrorDiagnosticInfo> diagnostics_;
    std::mutex mutex_;
    size_t maxDiagnostics_ = 1000;
    
public:
    ErrorDiagnostics() = default;
    
    // Record diagnostic information
    void recordDiagnostic(const std::string& file, int line, 
                         const std::string& function, const std::string& message,
                         const std::unordered_map<std::string, std::string>& context = {}) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        ErrorDiagnosticInfo info;
        info.file = file;
        info.line = line;
        info.function = function;
        info.timestamp = std::chrono::system_clock::now();
        info.context = context;
        
        // Generate stack trace (simplified)
        std::stringstream ss;
        ss << "File: " << file << "\n";
        ss << "Line: " << line << "\n";
        ss << "Function: " << function << "\n";
        ss << "Message: " << message << "\n";
        ss << "Context: ";
        bool first = true;
        for (const auto& pair : context) {
            if (!first) ss << ", ";
            ss << pair.first << "=" << pair.second;
            first = false;
        }
        info.stackTrace = ss.str();
        
        diagnostics_.push_back(info);
        
        // Limit diagnostics size
        if (diagnostics_.size() > maxDiagnostics_) {
            diagnostics_.erase(diagnostics_.begin());
        }
    }
    
    // Get diagnostics for a specific file
    std::vector<ErrorDiagnosticInfo> getDiagnosticsForFile(const std::string& file) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ErrorDiagnosticInfo> result;
        for (const auto& info : diagnostics_) {
            if (info.file.find(file) != std::string::npos) {
                result.push_back(info);
            }
        }
        return result;
    }
    
    // Get recent diagnostics
    std::vector<ErrorDiagnosticInfo> getRecentDiagnostics(size_t count = 10) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t start = std::max<size_t>(0, diagnostics_.size() - count);
        std::vector<ErrorDiagnosticInfo> result;
        for (size_t i = start; i < diagnostics_.size(); ++i) {
            result.push_back(diagnostics_[i]);
        }
        return result;
    }
    
    // Generate diagnostic report
    std::string generateDiagnosticReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        ss << "=== Error Diagnostics Report ===\n";
        ss << "Total Diagnostics: " << diagnostics_.size() << "\n";
        ss << "Max Diagnostics: " << maxDiagnostics_ << "\n";
        
        // Group by file
        std::unordered_map<std::string, size_t> fileCount;
        for (const auto& info : diagnostics_) {
            fileCount[info.file]++;
        }
        
        ss << "\n--- Diagnostics by File ---\n";
        for (const auto& pair : fileCount) {
            ss << pair.first << ": " << pair.second << " occurrences\n";
        }
        
        // Most recent diagnostics
        ss << "\n--- Most Recent Diagnostics ---\n";
        size_t limit = std::min<size_t>(5, diagnostics_.size());
        for (size_t i = 0; i < limit; ++i) {
            size_t idx = diagnostics_.size() - 1 - i;
            const auto& info = diagnostics_[idx];
            ss << "[" << info.timestamp << "] " << info.function << " (" << info.file << ":" << info.line << ")\n";
            ss << "  " << info.stackTrace << "\n\n";
        }
        
        return ss.str();
    }
    
    // Clear diagnostics
    void clearDiagnostics() {
        std::lock_guard<std::mutex> lock(mutex_);
        diagnostics_.clear();
    }
};

// Global error diagnostics
inline ErrorDiagnostics& getErrorDiagnostics() {
    static ErrorDiagnostics diagnostics;
    return diagnostics;
}

// Utility function to record error and update diagnostics
inline void reportError(const ErrorContext& error) {
    // Report to error reporter
    getErrorReporter().report(error);
    
    // Add to error statistics
    getErrorStatistics().recordError(error);
    
    // Add to error context manager
    getErrorContextManager().addError(error);
    
    // Record diagnostic information
    getErrorDiagnostics().recordDiagnostic(error.file, error.line, error.function, error.message, error.context);
}

} // namespace nlm