// Enhanced error handling implementation for NLM Brain
// Comprehensive error management with recovery and diagnostics

#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <algorithm>
#include <thread>

namespace nlm {

// ErrorManager implementation
inline ErrorManager::ErrorManager() : pImpl(std::make_unique<Impl>()) {
    // Initialize error log file
    errorLogFile = "./nlm_errors.log";
    maxErrorsStored = 10000;
    autoRecoveryEnabled = true;
    recoveryDelay = std::chrono::milliseconds(100);
}

inline ErrorManager::~ErrorManager() {
    // Cleanup error log file
    if (!errorLogFile.empty()) {
        std::remove(errorLogFile.c_str());
    }
}

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
    
    // Cleanup old errors periodically
    if (pImpl->errors.size() > maxErrorsStored * 0.8) {
        cleanupOldErrors();
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

} // namespace nlm

// Global error manager instance (defined elsewhere)
namespace {
    ErrorManager& getGlobalErrorManager() {
        static ErrorManager instance;
        return instance;
    }
}

// Convenience functions
inline void initializeErrorSystem() {
    GlobalErrorManager::getInstance();
}

inline void shutdownErrorSystem() {
    // Cleanup
}

inline bool isErrorSystemHealthy() {
    return GlobalErrorManager::getInstance().isHealthy();
}

inline void logGlobalError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                          ErrorCategory category = ErrorCategory::Unknown) {
    GlobalErrorManager::getInstance().logError(message, severity, category);
}

} // namespace nlm