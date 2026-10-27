// Enhanced error handling system for NLM Brain
// Comprehensive error management with recovery and diagnostics

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <atomic>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error category for classification
enum class ErrorCategory {
    Configuration,
    MemoryManagement,
    Plasticity,
    NeuralDynamics,
    MemorySystem,
    Neuromodulation,
    Development,
    InputOutput,
    Performance,
    Checkpoint,
    Network,
    Unknown
};

// Detailed error information
struct ErrorInfo {
    std::string message;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string file;
    std::string function;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> stackTrace;
    std::unordered_map<std::string, std::string> context;
    
    ErrorInfo(const std::string& msg, ErrorSeverity sev = ErrorSeverity::Error,
              ErrorCategory cat = ErrorCategory::Unknown,
              const std::string& fileName = "",
              const std::string& funcName = "",
              int lineNum = 0)
        : message(msg), severity(sev), category(cat),
          file(fileName), function(funcName), line(lineNum),
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity) << "] " << message;
        if (!file.empty()) {
            oss << " (" << file << ":" << line << ")";
        }
        if (!stackTrace.empty()) {
            oss << "\nStack trace:";
            for (const auto& trace : stackTrace) {
                oss << "\n  " << trace;
            }
        }
        return oss.str();
    }
    
    std::string getContextString() const {
        std::ostringstream oss;
        if (!context.empty()) {
            oss << "Context: ";
            bool first = true;
            for (const auto& pair : context) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        return oss.str();
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual bool shouldHandle(const ErrorInfo& error) const = 0;
    virtual std::string getName() const = 0;
};

// Exception wrapper for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                  ErrorCategory category = ErrorCategory::Unknown,
                  const std::string& file = "",
                  const std::string& function = "",
                  int line = 0)
        : std::runtime_error(message), severity(severity), category(category),
          file(file), function(function), line(line), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getFile() const { return file; }
    const std::string& getFunction() const { return function; }
    int getLine() const { return line; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    
    std::string getFullDescription() const {
        std::ostringstream oss;
        oss << what();
        oss << " [Severity: " << static_cast<int>(severity) << "]";
        oss << " [Category: " << static_cast<int>(category) << "]";
        if (!file.empty()) {
            oss << " [" << file << ":" << line << "]";
        }
        oss << " [" << std::chrono::system_clock::to_time_t(timestamp) << "]";
        return oss.str();
    }
    
private:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string file;
    std::string function;
    int line;
    std::chrono::system_clock::time_point timestamp;
};

// Error recovery strategies
enum class RecoveryStrategy {
    None,
    Retry,
    Fallback,
    Reset,
    Shutdown,
    AutoRecovery,
    ManualIntervention
};

// Error recovery action
struct RecoveryAction {
    RecoveryStrategy strategy;
    std::string description;
    std::function<bool()> action;
    int maxRetries;
    std::chrono::milliseconds retryDelay;
    
    RecoveryAction(RecoveryStrategy strat, const std::string& desc,
                   const std::function<bool()>& act,
                   int retries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100))
        : strategy(strat), description(desc), action(act),
          maxRetries(retries), retryDelay(delay) {}
    
    bool execute() {
        if (maxRetries <= 0) {
            return action();
        }
        
        for (int attempt = 0; attempt < maxRetries; ++attempt) {
            try {
                if (action()) {
                    return true;
                }
            } catch (...) {
                // Ignore exceptions during recovery attempt
            }
            
            if (attempt < maxRetries - 1) {
                std::this_thread::sleep_for(retryDelay * (attempt + 1));
            }
        }
        
        return false;
    }
};

// Comprehensive error manager
class ErrorManager {
public:
    ErrorManager();
    ~ErrorManager();
    
    // Disable copying, enable moving
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    ErrorManager(ErrorManager&&) noexcept;
    ErrorManager& operator=(ErrorManager&&) noexcept;
    
    // Error management
    void logError(const ErrorInfo& error);
    void logError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                  ErrorCategory category = ErrorCategory::Unknown,
                  const std::string& file = "",
                  const std::string& function = "",
                  int line = 0);
    void logNLMException(const NLMException& exception);
    
    // Error retrieval
    std::vector<ErrorInfo> getErrors() const;
    std::vector<ErrorInfo> getErrors(ErrorSeverity minSeverity) const;
    std::vector<ErrorInfo> getErrors(ErrorCategory category) const;
    std::vector<ErrorInfo> getRecentErrors(size_t count) const;
    
    // Error analysis
    std::string analyzeErrors() const;
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    double getErrorRate() const;
    
    // Error recovery
    bool attemptRecovery(const ErrorInfo& error);
    bool attemptRecovery(ErrorCategory category);
    void applyRecoveryStrategy(const ErrorInfo& error, RecoveryStrategy strategy);
    
    // Error handler management
    void registerErrorHandler(std::unique_ptr<ErrorHandler> handler);
    void unregisterErrorHandler(const std::string& name);
    void clearErrorHandlers();
    
    // Configuration
    void setMaxErrors(size_t maxErrors) { maxErrorsStored = maxErrors; }
    void setErrorLogFile(const std::string& filename) { errorLogFile = filename; }
    void setAutoRecovery(bool enabled) { autoRecoveryEnabled = enabled; }
    void setRecoveryDelay(std::chrono::milliseconds delay) { recoveryDelay = delay; }
    
    // Error statistics
    struct ErrorStats {
        size_t totalErrors;
        size_t errorsBySeverity[6]; // ErrorSeverity enum
        size_t errorsByCategory[12]; // ErrorCategory enum
        double errorRate;
        std::chrono::system_clock::time_point lastErrorTime;
        size_t recoveryAttempts;
        size_t successfulRecoveries;
        
        ErrorStats() : totalErrors(0), errorRate(0.0), 
                      recoveryAttempts(0), successfulRecoveries(0) {
            std::fill(std::begin(errorsBySeverity), std::end(errorsBySeverity), 0);
            std::fill(std::begin(errorsByCategory), std::end(errorsByCategory), 0);
        }
    };
    
    ErrorStats getStats() const;
    
    // Error monitoring
    bool isHealthy() const;
    std::string getHealthStatus() const;
    std::vector<std::string> getIssues() const;
    
    // Advanced error handling
    template<typename Func>
    void executeWithErrorHandling(Func&& func, ErrorCategory category = ErrorCategory::Unknown);
    
    void pushRecoveryAction(ErrorCategory category, RecoveryAction action);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void writeToLogFile(const ErrorInfo& error);
    bool shouldHandleError(const ErrorInfo& error) const;
    RecoveryStrategy getRecoveryStrategy(const ErrorInfo& error) const;
    std::vector<RecoveryAction> getRecoveryActions(ErrorCategory category) const;
    void cleanupOldErrors();
};

// Specialized error handlers

// File I/O error handler
class FileErrorHandler : public ErrorHandler {
public:
    FileErrorHandler(const std::string& filePath);
    
    void handleError(const ErrorInfo& error) override;
    bool shouldHandle(const ErrorInfo& error) const override;
    std::string getName() const override { return "FileErrorHandler"; }
    
    void setAutoRecovery(bool enabled) { autoRecoveryEnabled = enabled; }
    
private:
    std::string filePath;
    bool autoRecoveryEnabled;
    std::mutex mutex;
};

// Memory error handler
class MemoryErrorHandler : public ErrorHandler {
public:
    MemoryErrorHandler();
    
    void handleError(const ErrorInfo& error) override;
    bool shouldHandle(const ErrorInfo& error) const override;
    std::string getName() const override { return "MemoryErrorHandler"; }
    
    void performMemoryAnalysis();
    void suggestMemoryOptimization();
    
private:
    std::mutex mutex;
    size_t consecutiveMemoryErrors;
};

// Plasticity error handler
class PlasticityErrorHandler : public ErrorHandler {
public:
    PlasticityErrorHandler(std::shared_ptr<Brain> brain);
    
    void handleError(const ErrorInfo& error) override;
    bool shouldHandle(const ErrorInfo& error) const override;
    std::string getName() const override { return "PlasticityErrorHandler"; }
    
    void analyzePlasticityFailure(const ErrorInfo& error);
    void attemptPlasticityRecovery();
    
private:
    std::shared_ptr<Brain> brain;
    std::mutex mutex;
    size_t consecutivePlasticityErrors;
};

// Network error handler
class NetworkErrorHandler : public ErrorHandler {
public:
    NetworkErrorHandler();
    
    void handleError(const ErrorInfo& error) override;
    bool shouldHandle(const ErrorInfo& error) const override;
    std::string getName() const override { return "NetworkErrorHandler"; }
    
    void attemptNetworkRecovery();
    void diagnoseNetworkIssue();
    
private:
    std::mutex mutex;
    size_t consecutiveNetworkErrors;
    std::chrono::steady_clock::time_point lastNetworkErrorTime;
};

// Checkpoint error handler
class CheckpointErrorHandler : public ErrorHandler {
public:
    CheckpointErrorHandler();
    
    void handleError(const ErrorInfo& error) override;
    bool shouldHandle(const ErrorInfo& error) const override;
    std::string getName() const override { return "CheckpointErrorHandler"; }
    
    void attemptCheckpointRecovery();
    void suggestCheckpointBackup();
    
private:
    std::mutex mutex;
    std::string lastCheckpointFile;
};

// Enhanced error macros

#define NLM_LOG_ERROR(msg) \
    nlm::ErrorManager::getInstance().logError(msg, nlm::ErrorSeverity::Error, \
                                             nlm::ErrorCategory::Unknown, __FILE__, __func__, __LINE__)

#define NLM_LOG_WARNING(msg) \
    nlm::ErrorManager::getInstance().logError(msg, nlm::ErrorSeverity::Warning, \
                                             nlm::ErrorCategory::Unknown, __FILE__, __func__, __LINE__)

#define NLM_LOG_INFO(msg) \
    nlm::ErrorManager::getInstance().logError(msg, nlm::ErrorSeverity::Info, \
                                             nlm::ErrorCategory::Unknown, __FILE__, __func__, __LINE__)

#define NLM_THROW_ERROR(msg, severity) \
    throw nlm::NLMException(msg, severity, nlm::ErrorCategory::Unknown, __FILE__, __func__, __LINE__)

#define NLM_THROW_ERROR_WITH_CATEGORY(msg, category) \
    throw nlm::NLMException(msg, nlm::ErrorSeverity::Error, category, __FILE__, __func__, __LINE__)

#define NLM_TRY_ERROR_CATEGORY(category, code) \
    try { code; } catch (const nlm::NLMException& e) { \
        nlm::ErrorManager::getInstance().logNLMException(e); \
        if (e.getCategory() == category) { throw; } \
    } catch (...) { \
        nlm::ErrorManager::getInstance().logError("Unknown exception", nlm::ErrorSeverity::Error, category); \
        throw; \
    }

#define NLM_EXECUTE_WITH_RECOVERY(category, code) \
    nlm::ErrorManager::getInstance().executeWithErrorHandling([&]() { code; }, category)

// Global error manager instance
class GlobalErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    static void initialize() {
        getInstance();
    }
    
    static void shutdown() {
        getInstance();
    }
    
    static bool isInitialized() {
        return getInstance().isHealthy();
    }
};

// Convenience functions for error handling in Brain methods
namespace ErrorHandling {
    
    // Safe brain operations with error handling
    template<typename T>
    std::optional<T> safeBrainOperation(std::function<T()> operation,
                                        ErrorCategory category = ErrorCategory::Unknown,
                                        T defaultValue = T()) {
        try {
            return operation();
        } catch (const NLMException& e) {
            ErrorManager::getInstance().logNLMException(e);
            if (e.getCategory() == category) {
                throw;
            }
        } catch (...) {
            ErrorManager::getInstance().logError("Unknown exception in brain operation",
                                                ErrorSeverity::Error, category);
        }
        
        return std::nullopt;
    }
    
    // Safe neural region operations
    template<typename T>
    std::optional<T> safeNeuralOperation(std::function<T()> operation,
                                         ErrorCategory category = ErrorCategory::Unknown,
                                         T defaultValue = T()) {
        try {
            return operation();
        } catch (const NLMException& e) {
            ErrorManager::getInstance().logNLMException(e);
            if (e.getCategory() == category) {
                throw;
            }
        } catch (...) {
            ErrorManager::getInstance().logError("Unknown exception in neural operation",
                                                ErrorSeverity::Error, category);
        }
        
        return std::nullopt;
    }
    
    // Safe memory operations
    template<typename T>
    std::optional<T> safeMemoryOperation(std::function<T()> operation,
                                         ErrorCategory category = ErrorCategory::MemorySystem,
                                         T defaultValue = T()) {
        try {
            return operation();
        } catch (const NLMException& e) {
            ErrorManager::getInstance().logNLMException(e);
            if (e.getCategory() == category) {
                throw;
            }
        } catch (...) {
            ErrorManager::getInstance().logError("Unknown exception in memory operation",
                                                ErrorSeverity::Error, category);
        }
        
        return std::nullopt;
    }
    
    // Safe configuration operations
    template<typename T>
    std::optional<T> safeConfigOperation(std::function<T()> operation,
                                         ErrorCategory category = ErrorCategory::Configuration,
                                         T defaultValue = T()) {
        try {
            return operation();
        } catch (const NLMException& e) {
            ErrorManager::getInstance().logNLMException(e);
            if (e.getCategory() == category) {
                throw;
            }
        } catch (...) {
            ErrorManager::getInstance().logError("Unknown exception in configuration operation",
                                                ErrorSeverity::Error, category);
        }
        
        return std::nullopt;
    }
    
    // Memory error checking
    bool checkMemoryUsage(size_t thresholdBytes = 100 * 1024 * 1024); // 100 MB
    
    // Performance error checking
    bool checkPerformance(size_t maxStepTime = 1000); // 1 second max per step
    
    // Network error checking
    bool checkConnectivity();
    
    // Plasticity health checking
    bool checkPlasticityHealth();
};

} // namespace nlm

// ErrorManager implementation
inline ErrorManager::ErrorManager() : pImpl(std::make_unique<Impl>()) {}

inline ErrorManager::~ErrorManager() = default;

inline ErrorManager::ErrorManager(ErrorManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

inline ErrorManager& ErrorManager::operator=(ErrorManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

inline void ErrorManager::logError(const ErrorInfo& error) {
    if (!pImpl) return;
    
    // Check if error should be handled
    if (!shouldHandleError(error)) {
        return;
    }
    
    // Log to error handlers
    for (const auto& handler : pImpl->errorHandlers) {
        if (handler->shouldHandle(error)) {
            try {
                handler->handleError(error);
            } catch (...) {
                // Ignore handler exceptions
            }
        }
    }
    
    // Store error
    pImpl->errors.push_back(error);
    
    // Update statistics
    pImpl->stats.totalErrors++;
    pImpl->stats.errorsBySeverity[static_cast<size_t>(error.severity)]++;
    pImpl->stats.errorsByCategory[static_cast<size_t>(error.category)]++;
    pImpl->stats.lastErrorTime = error.timestamp;
    
    // Write to log file if configured
    if (!pImpl->errorLogFile.empty()) {
        writeToLogFile(error);
    }
    
    // Attempt recovery if enabled
    if (autoRecoveryEnabled) {
        applyRecoveryStrategy(error, getRecoveryStrategy(error));
    }
}

inline void ErrorManager::logError(const std::string& message, ErrorSeverity severity,
                                   ErrorCategory category, const std::string& file,
                                   const std::string& function, int line) {
    ErrorInfo error(message, severity, category, file, function, line);
    logError(error);
}

inline void ErrorManager::logNLMException(const NLMException& exception) {
    ErrorInfo error(exception.what(), exception.getSeverity(), exception.getCategory(),
                    exception.getFile(), exception.getFunction(), exception.getLine());
    logError(error);
}

inline std::vector<ErrorInfo> ErrorManager::getErrors() const {
    return pImpl ? pImpl->errors : std::vector<ErrorInfo>{};
}

inline std::vector<ErrorInfo> ErrorManager::getErrors(ErrorSeverity minSeverity) const {
    std::vector<ErrorInfo> result;
    if (!pImpl) return result;
    
    for (const auto& error : pImpl->errors) {
        if (error.severity >= minSeverity) {
            result.push_back(error);
        }
    }
    
    return result;
}

inline std::vector<ErrorInfo> ErrorManager::getErrors(ErrorCategory category) const {
    std::vector<ErrorInfo> result;
    if (!pImpl) return result;
    
    for (const auto& error : pImpl->errors) {
        if (error.category == category) {
            result.push_back(error);
        }
    }
    
    return result;
}

inline std::vector<ErrorInfo> ErrorManager::getRecentErrors(size_t count) const {
    if (!pImpl) return std::vector<ErrorInfo>{};
    
    size_t startIndex = std::max<size_t>(0, pImpl->errors.size() - count);
    return std::vector<ErrorInfo>(pImpl->errors.begin() + startIndex, pImpl->errors.end());
}

inline std::string ErrorManager::analyzeErrors() const {
    if (!pImpl || pImpl->errors.empty()) {
        return "No errors to analyze.\n";
    }
    
    std::ostringstream oss;
    oss << "=== Error Analysis ===\n";
    oss << "Total errors: " << pImpl->stats.totalErrors << "\n";
    
    // Analyze by severity
    oss << "\nBy severity:\n";
    const char* severityNames[] = {"Debug", "Info", "Warning", "Error", "Critical", "Fatal"};
    for (int i = static_cast<int>(ErrorSeverity::Error); i < static_cast<int>(ErrorSeverity::Fatal); ++i) {
        size_t count = pImpl->stats.errorsBySeverity[i];
        if (count > 0) {
            oss << "  " << severityNames[i] << ": " << count << "\n";
        }
    }
    
    // Analyze by category
    oss << "\nBy category:\n";
    const char* categoryNames[] = {"Configuration", "MemoryManagement", "Plasticity", 
                                   "NeuralDynamics", "MemorySystem", "Neuromodulation",
                                   "Development", "InputOutput", "Performance", 
                                   "Checkpoint", "Network", "Unknown"};
    for (int i = 0; i < static_cast<int>(ErrorCategory::Unknown); ++i) {
        size_t count = pImpl->stats.errorsByCategory[i];
        if (count > 0) {
            oss << "  " << categoryNames[i] << ": " << count << "\n";
        }
    }
    
    // Error rate
    oss << "\nError rate: " << pImpl->stats.errorRate * 100.0 << "%\n";
    oss << "Last error time: " << std::chrono::system_clock::to_time_t(pImpl->stats.lastErrorTime) << "\n";
    
    // Recovery statistics
    oss << "\nRecovery statistics:\n";
    oss << "  Recovery attempts: " << pImpl->stats.recoveryAttempts << "\n";
    oss << "  Successful recoveries: " << pImpl->stats.successfulRecoveries << "\n";
    if (pImpl->stats.recoveryAttempts > 0) {
        oss << "  Success rate: " << (static_cast<double>(pImpl->stats.successfulRecoveries) / pImpl->stats.recoveryAttempts * 100.0) << "%\n";
    }
    
    return oss.str();
}

inline size_t ErrorManager::getErrorCount(ErrorSeverity severity) const {
    if (!pImpl) return 0;
    
    size_t count = 0;
    for (const auto& error : pImpl->errors) {
        if (error.severity >= severity) {
            count++;
        }
    }
    
    return count;
}

inline double ErrorManager::getErrorRate() const {
    if (!pImpl) return 0.0;
    
    // Calculate error rate per second
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->stats.lastErrorTime);
    
    if (duration.count() <= 0) return 0.0;
    
    return static_cast<double>(pImpl->stats.totalErrors) / duration.count();
}

inline bool ErrorManager::attemptRecovery(const ErrorInfo& error) {
    RecoveryStrategy strategy = getRecoveryStrategy(error);
    return applyRecoveryStrategy(error, strategy);
}

inline bool ErrorManager::attemptRecovery(ErrorCategory category) {
    auto actions = getRecoveryActions(category);
    
    bool success = false;
    for (const auto& action : actions) {
        if (action.execute()) {
            success = true;
            break;
        }
    }
    
    pImpl->stats.recoveryAttempts++;
    if (success) {
        pImpl->stats.successfulRecoveries++;
    }
    
    return success;
}

inline void ErrorManager::applyRecoveryStrategy(const ErrorInfo& error, RecoveryStrategy strategy) {
    auto actions = getRecoveryActions(error.category);
    
    for (const auto& action : actions) {
        if (action.strategy == strategy) {
            bool success = action.execute();
            pImpl->stats.recoveryAttempts++;
            if (success) {
                pImpl->stats.successfulRecoveries++;
            }
            return;
        }
    }
    
    // No specific action found, try any auto-recoveries
    for (const auto& action : actions) {
        if (action.strategy == RecoveryStrategy::AutoRecovery) {
            bool success = action.execute();
            pImpl->stats.recoveryAttempts++;
            if (success) {
                pImpl->stats.successfulRecoveries++;
            }
            return;
        }
    }
}

inline void ErrorManager::registerErrorHandler(std::unique_ptr<ErrorHandler> handler) {
    if (!pImpl || !handler) return;
    
    pImpl->errorHandlers.push_back(std::move(handler));
}

inline void ErrorManager::unregisterErrorHandler(const std::string& name) {
    if (!pImpl) return;
    
    auto it = std::remove_if(pImpl->errorHandlers.begin(), pImpl->errorHandlers.end(),
                            [&name](const std::unique_ptr<ErrorHandler>& handler) {
                                return handler->getName() == name;
                            });
    
    pImpl->errorHandlers.erase(it, pImpl->errorHandlers.end());
}

inline void ErrorManager::clearErrorHandlers() {
    if (pImpl) {
        pImpl->errorHandlers.clear();
    }
}

inline ErrorManager::ErrorStats ErrorManager::getStats() const {
    return pImpl ? pImpl->stats : ErrorStats{};
}

inline bool ErrorManager::isHealthy() const {
    if (!pImpl) return false;
    
    // Check if there are critical errors
    size_t criticalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Critical)];
    size_t fatalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Fatal)];
    
    return criticalErrors == 0 && fatalErrors == 0;
}

inline std::string ErrorManager::getHealthStatus() const {
    if (isHealthy()) {
        return "Healthy";
    }
    
    std::ostringstream oss;
    oss << "Issues detected:\n";
    
    size_t criticalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Critical)];
    size_t fatalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Fatal)];
    
    if (criticalErrors > 0) {
        oss << "- " << criticalErrors << " critical errors\n";
    }
    if (fatalErrors > 0) {
        oss << "- " << fatalErrors << " fatal errors\n";
    }
    
    return oss.str();
}

inline std::vector<std::string> ErrorManager::getIssues() const {
    std::vector<std::string> issues;
    
    size_t criticalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Critical)];
    size_t fatalErrors = pImpl->stats.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Fatal)];
    
    if (criticalErrors > 0) {
        issues.push_back("System in critical state - requires immediate attention");
    }
    if (fatalErrors > 0) {
        issues.push_back("System in fatal state - requires shutdown");
    }
    
    return issues;
}

inline template<typename Func>
void ErrorManager::executeWithErrorHandling(Func&& func, ErrorCategory category) {
    try {
        func();
    } catch (const NLMException& e) {
        logNLMException(e);
        if (e.getCategory() == category) {
            throw;
        }
    } catch (...) {
        logError("Unknown exception in error-handled operation", ErrorSeverity::Error, category);
        throw;
    }
}

inline void ErrorManager::pushRecoveryAction(ErrorCategory category, RecoveryAction action) {
    if (!pImpl) return;
    
    pImpl->recoveryActions[category].push_back(action);
}

inline void ErrorManager::writeToLogFile(const ErrorInfo& error) {
    if (pImpl->errorLogFile.empty()) return;
    
    try {
        std::ofstream file(pImpl->errorLogFile, std::ios::app);
        if (file.is_open()) {
            file << error.getFormattedMessage() << "\n";
            file << error.getContextString() << "\n";
            file << "---\n";
        }
    } catch (...) {
        // Ignore log file write errors
    }
}

inline bool ErrorManager::shouldHandleError(const ErrorInfo& error) const {
    if (!pImpl) return false;
    
    // Check if error has exceeded maximum allowed
    if (pImpl->maxErrorsStored > 0 && pImpl->errors.size() >= pImpl->maxErrorsStored) {
        return false;
    }
    
    // Check error severity
    if (error.severity == ErrorSeverity::Debug) {
        return false; // Don't log debug errors by default
    }
    
    return true;
}

inline RecoveryStrategy ErrorManager::getRecoveryStrategy(const ErrorInfo& error) const {
    if (!pImpl) return RecoveryStrategy::None;
    
    // Default recovery strategies by error category
    static const std::unordered_map<ErrorCategory, RecoveryStrategy> defaultStrategies = {
        {ErrorCategory::Configuration, RecoveryStrategy::Fallback},
        {ErrorCategory::MemoryManagement, RecoveryStrategy::Reset},
        {ErrorCategory::Plasticity, RecoveryStrategy::AutoRecovery},
        {ErrorCategory::NeuralDynamics, RecoveryStrategy::Retry},
        {ErrorCategory::MemorySystem, RecoveryStrategy::Fallback},
        {ErrorCategory::Neuromodulation, RecoveryStrategy::AutoRecovery},
        {ErrorCategory::Development, RecoveryStrategy::AutoRecovery},
        {ErrorCategory::InputOutput, RecoveryStrategy::Retry},
        {ErrorCategory::Performance, RecoveryStrategy::None},
        {ErrorCategory::Checkpoint, RecoveryStrategy::Fallback},
        {ErrorCategory::Network, RecoveryStrategy::AutoRecovery},
        {ErrorCategory::Unknown, RecoveryStrategy::None}
    };
    
    auto it = defaultStrategies.find(error.category);
    if (it != defaultStrategies.end()) {
        return it->second;
    }
    
    return RecoveryStrategy::None;
}

inline std::vector<RecoveryAction> ErrorManager::getRecoveryActions(ErrorCategory category) const {
    if (!pImpl) return std::vector<RecoveryAction>{};
    
    return pImpl->recoveryActions[category];
}

inline void ErrorManager::cleanupOldErrors() {
    if (!pImpl) return;
    
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24); // Remove errors older than 24 hours
    
    auto it = std::remove_if(pImpl->errors.begin(), pImpl->errors.end(),
                            [cutoff](const ErrorInfo& error) {
                                return error.timestamp < cutoff;
                            });
    
    pImpl->errors.erase(it, pImpl->errors.end());
    
    // Update statistics
    pImpl->stats.totalErrors = pImpl->errors.size();
}

// Error handler implementations
inline FileErrorHandler::FileErrorHandler(const std::string& filePath) 
    : filePath(filePath), autoRecoveryEnabled(false) {}

inline void FileErrorHandler::handleError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (shouldHandle(error)) {
        // Log to file
        std::ofstream file(filePath, std::ios::app);
        if (file.is_open()) {
            file << error.getFormattedMessage() << "\n";
            file << error.getContextString() << "\n";
            file << "---\n";
        }
        
        // Auto-recovery if enabled
        if (autoRecoveryEnabled) {
            // Attempt recovery
            // Implementation would depend on the specific error
        }
    }
}

inline bool FileErrorHandler::shouldHandle(const ErrorInfo& error) const {
    return error.category == ErrorCategory::FileError ||
           error.category == ErrorCategory::Configuration ||
           error.category == ErrorCategory::Checkpoint;
}

inline MemoryErrorHandler::MemoryErrorHandler() : consecutiveMemoryErrors(0) {}

inline void MemoryErrorHandler::handleError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (shouldHandle(error)) {
        consecutiveMemoryErrors++;
        
        // Log the error
        NLM_LOG_ERROR(error.getFormattedMessage());
        
        // Perform memory analysis if many consecutive errors
        if (consecutiveMemoryErrors >= 3) {
            performMemoryAnalysis();
            suggestMemoryOptimization();
            consecutiveMemoryErrors = 0;
        }
    }
}

inline bool MemoryErrorHandler::shouldHandle(const ErrorInfo& error) const {
    return error.category == ErrorCategory::MemoryManagement;
}

inline void MemoryErrorHandler::performMemoryAnalysis() {
    // Implementation would perform memory analysis
    NLM_LOG_INFO("Performing memory analysis due to memory errors...");
}

inline void MemoryErrorHandler::suggestMemoryOptimization() {
    // Implementation would suggest memory optimizations
    NLM_LOG_INFO("Suggesting memory optimizations...");
}

inline PlasticityErrorHandler::PlasticityErrorHandler(std::shared_ptr<Brain> brain) 
    : brain(brain), consecutivePlasticityErrors(0) {}

inline void PlasticityErrorHandler::handleError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (shouldHandle(error)) {
        consecutivePlasticityErrors++;
        
        // Log the error
        NLM_LOG_ERROR(error.getFormattedMessage());
        
        // Analyze plasticity failure if many consecutive errors
        if (consecutivePlasticityErrors >= 2) {
            analyzePlasticityFailure(error);
            attemptPlasticityRecovery();
            consecutivePlasticityErrors = 0;
        }
    }
}

inline bool PlasticityErrorHandler::shouldHandle(const ErrorInfo& error) const {
    return error.category == ErrorCategory::Plasticity;
}

inline void PlasticityErrorHandler::analyzePlasticityFailure(const ErrorInfo& error) {
    NLM_LOG_INFO("Analyzing plasticity failure due to error: " + error.message);
    // Implementation would analyze plasticity failure
}

inline void PlasticityErrorHandler::attemptPlasticityRecovery() {
    NLM_LOG_INFO("Attempting plasticity recovery...");
    // Implementation would attempt plasticity recovery
}

inline NetworkErrorHandler::NetworkErrorHandler() : consecutiveNetworkErrors(0) {}

inline void NetworkErrorHandler::handleError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (shouldHandle(error)) {
        consecutiveNetworkErrors++;
        lastNetworkErrorTime = std::chrono::steady_clock::now();
        
        // Log the error
        NLM_LOG_ERROR(error.getFormattedMessage());
        
        // Attempt network recovery
        if (consecutiveNetworkErrors >= 1) {
            attemptNetworkRecovery();
        }
    }
}

inline bool NetworkErrorHandler::shouldHandle(const ErrorInfo& error) const {
    return error.category == ErrorCategory::Network;
}

inline void NetworkErrorHandler::attemptNetworkRecovery() {
    NLM_LOG_INFO("Attempting network recovery...");
    // Implementation would attempt network recovery
}

inline void NetworkErrorHandler::diagnoseNetworkIssue() {
    NLM_LOG_INFO("Diagnosing network issue...");
    // Implementation would diagnose network issues
}

inline CheckpointErrorHandler::CheckpointErrorHandler() : lastCheckpointFile("") {}

inline void CheckpointErrorHandler::handleError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (shouldHandle(error)) {
        // Log the error
        NLM_LOG_ERROR(error.getFormattedMessage());
        
        // Attempt checkpoint recovery
        attemptCheckpointRecovery();
        suggestCheckpointBackup();
    }
}

inline bool CheckpointErrorHandler::shouldHandle(const ErrorInfo& error) const {
    return error.category == ErrorCategory::Checkpoint;
}

inline void CheckpointErrorHandler::attemptCheckpointRecovery() {
    NLM_LOG_INFO("Attempting checkpoint recovery...");
    // Implementation would attempt checkpoint recovery
}

inline void CheckpointErrorHandler::suggestCheckpointBackup() {
    NLM_LOG_INFO("Suggesting checkpoint backup...");
    // Implementation would suggest checkpoint backup
}

// Error handling convenience functions
inline bool ErrorHandling::checkMemoryUsage(size_t thresholdBytes) {
    // Implementation would check memory usage
    return false;
}

inline bool ErrorHandling::checkPerformance(size_t maxStepTime) {
    // Implementation would check performance
    return false;
}

inline bool ErrorHandling::checkConnectivity() {
    // Implementation would check connectivity
    return false;
}

inline bool ErrorHandling::checkPlasticityHealth() {
    // Implementation would check plasticity health
    return false;
}

} // namespace nlm