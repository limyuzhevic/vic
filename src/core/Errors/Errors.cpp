#include "Errors.hpp"
#include <fstream>
#include <sys/stat.h>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {

// ============================================================================
// VALIDATION UTILS IMPLEMENTATION
// ============================================================================

void ValidationUtils::validateConfig(const std::string& key, const std::string& value) {
    // Basic validation for configuration values
    if (key.empty()) {
        throw NLMValueError("Configuration key cannot be empty", __FILE__, __LINE__, __func__);
    }
    
    // Add more specific validation based on key
    if (key == "random_seed") {
        try {
            uint64_t seed = std::stoull(value);
            if (seed == 0) {
                throw NLMValueError("Random seed should be non-zero for deterministic results", __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            throw NLMValueError("Invalid random_seed format: " + value, __FILE__, __LINE__, __func__);
        }
    }
    
    if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0) {
                throw NLMValueError("Simulation timestep must be positive", __FILE__, __LINE__, __func__);
            }
        } catch (const std::exception& e) {
            throw NLMValueError("Invalid simulation_timestep format: " + value, __FILE__, __LINE__, __func__);
        }
    }
}

bool ValidationUtils::isValidState(const std::string& component) {
    // Check if a component is in a valid state
    // This is a placeholder implementation
    return true;
}

bool ValidationUtils::isMemoryAvailable(size_t bytesNeeded) {
    // Check if memory is available (placeholder implementation)
    return true;
}

void ValidationUtils::safeResourceCleanup(std::function<void()> cleanupFunc) {
    if (cleanupFunc) {
        try {
            cleanupFunc();
        } catch (const std::exception& e) {
            Logger::getGlobal().error(std::string("Resource cleanup failed: ") + e.what(), __FILE__, __LINE__, __func__);
        }
    }
}

bool ValidationUtils::attemptRecovery(std::function<bool()> operation, int maxRetries) {
    if (!operation) {
        return false;
    }
    
    int attempts = 0;
    while (attempts < maxRetries) {
        try {
            if (operation()) {
                return true;
            }
        } catch (const std::exception& e) {
            Logger::getGlobal().warning(std::string("Recovery attempt ") + std::to_string(attempts + 1) + ": " + e.what(), __FILE__, __LINE__, __func__);
        }
        
        attempts++;
        if (attempts < maxRetries) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    return false;
}

// ============================================================================
// ERROR REPORTING UTILS IMPLEMENTATION
// ============================================================================

static std::vector<ErrorReportingUtils::ErrorReport> recentErrors;
static std::function<void(const ErrorReportingUtils::ErrorReport&)> errorHandler;

void ErrorReportingUtils::reportError(const std::string& type, const std::string& message,
                                     const std::string& file, int line,
                                     const std::string& function) {
    ErrorReport report;
    report.errorType = type;
    report.message = message;
    report.file = file;
    report.line = line;
    report.function = function;
    report.timestamp = std::chrono::system_clock::now();
    
    recentErrors.push_back(report);
    
    // Trim error history if too large
    if (recentErrors.size() > 1000) {
        recentErrors.erase(recentErrors.begin(), recentErrors.begin() + recentErrors.size() - 1000);
    }
    
    // Call error handler if set
    if (errorHandler) {
        errorHandler(report);
    }
}

void ErrorReportingUtils::reportWarning(const std::string& type, const std::string& message,
                                        const std::string& file, int line,
                                        const std::string& function) {
    reportError(type + ": WARNING", message, file, line, function);
}

std::vector<ErrorReportingUtils::ErrorReport> ErrorReportingUtils::getRecentErrors(size_t limit) {
    if (limit > recentErrors.size()) {
        limit = recentErrors.size();
    }
    
    std::vector<ErrorReport> result(recentErrors.end() - limit, recentErrors.end());
    return result;
}

void ErrorReportingUtils::clearErrorHistory() {
    recentErrors.clear();
}

std::string ErrorReportingUtils::generateErrorSummary() {
    std::stringstream ss;
    ss << "=== ERROR SUMMARY ===\n";
    ss << "Total errors recorded: " << recentErrors.size() << "\n";
    
    if (!recentErrors.empty()) {
        ss << "\nRecent errors:\n";
        size_t count = std::min<size_t>(10, recentErrors.size());
        for (size_t i = recentErrors.size() - count; i < recentErrors.size(); ++i) {
            const auto& error = recentErrors[i];
            ss << "  " << error.errorType << ": " << error.message;
            if (!error.file.empty() && error.line > 0) {
                ss << " (at " << error.file << ":" << error.line << ")";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

void ErrorReportingUtils::setErrorHandler(std::function<void(const ErrorReport&)> handler) {
    errorHandler = handler;
}

// ============================================================================
// ERROR RECOVERY UTILS IMPLEMENTATION
// ============================================================================

static std::unordered_map<std::string, std::function<void(const NLMException&)>> errorHandlers;

bool ErrorRecoveryUtils::executeWithRecovery(
    std::function<bool()> operation,
    const RecoveryContext& context,
    std::function<void(const std::string&)> errorCallback) {
    
    if (!operation) {
        if (errorCallback) {
            errorCallback("Invalid operation function");
        }
        return false;
    }
    
    int attempts = 0;
    while (attempts < context.maxRetries) {
        try {
            // Validate state if validation check is provided
            if (context.validationCheck && !context.validationCheck()) {
                if (errorCallback) {
                    errorCallback("Validation check failed");
                }
                if (context.cleanupAction) {
                    context.cleanupAction();
                }
                continue;
            }
            
            // Execute operation
            if (operation()) {
                return true;
            }
            
        } catch (const NLMException& e) {
            if (errorCallback) {
                errorCallback(e.what());
            }
            
            // Try to register error handler for this component
            auto it = errorHandlers.find(context.operation);
            if (it != errorHandlers.end()) {
                it->second(e);
            }
            
            // Cleanup if needed
            if (context.cleanupAction && (context.strategy == RecoveryStrategy::CleanupAndRetry || 
                                           context.strategy == RecoveryStrategy::GracefulDegradation)) {
                context.cleanupAction();
            }
            
            // For other strategies, we continue to next attempt
            if (context.strategy != RecoveryStrategy::GracefulDegradation && 
                context.strategy != RecoveryStrategy::Fallback) {
                continue;
            } else {
                // For fallback or graceful degradation, break
                break;
            }
        }
        
        attempts++;
        if (attempts < context.maxRetries) {
            std::this_thread::sleep_for(context.retryDelay);
        }
    }
    
    return false;
}

void ErrorRecoveryUtils::registerErrorHandler(const std::string& component,
                                               std::function<void(const NLMException&)> handler) {
    errorHandlers[component] = handler;
}

void ErrorRecoveryUtils::unregisterErrorHandler(const std::string& component) {
    errorHandlers.erase(component);
}

void ErrorRecoveryUtils::attemptGracefulShutdown(const std::string& reason) {
    Logger::getGlobal().critical(std::string("Attempting graceful shutdown: ") + reason, __FILE__, __LINE__, __func__);
    
    // Notify all registered error handlers
    for (auto& pair : errorHandlers) {
        try {
            // Create a generic NLMStateException
            NLMStateException ex(reason, __FILE__, __LINE__, "attemptGracefulShutdown");
            pair.second(ex);
        } catch (const std::exception& e) {
            // Ignore exceptions during shutdown
        }
    }
    
    // Log final status
    Logger::getGlobal().critical(std::string("Graceful shutdown complete: ") + reason, __FILE__, __LINE__, __func__);
}

// ============================================================================
// SAFE GUARD UTILS IMPLEMENTATION
// ============================================================================

static std::vector<std::unique_ptr<SafeGuardUtils::Guard>> guards;

bool SafeGuardUtils::MemoryGuard::check() const {
    // Check if required memory is available
    // This is a placeholder - real implementation would use system calls
    struct stat memInfo;
    if (stat("/proc/meminfo", &memInfo) != 0) {
        return true; // Can't check, assume OK
    }
    
    // Parse /proc/meminfo to get available memory
    std::ifstream memInfoFile("/proc/meminfo");
    std::string line;
    uint64_t availableMem = 0;
    
    while (std::getline(memInfoFile, line)) {
        if (line.find("MemAvailable:") == 0) {
            std::istringstream iss(line);
            std::string key;
            uint64_t memKB;
            iss >> key >> memKB;
            availableMem = memKB * 1024; // Convert KB to bytes
            break;
        }
    }
    
    return availableMem >= requiredMemory_;
}

bool SafeGuardUtils::StateGuard::check() const {
    return validationFunc_();
}

bool SafeGuardUtils::ResourceGuard::check() const {
    return checkFunc_(resourceName_);
}

bool SafeGuardUtils::runAllGuards() {
    bool allPassed = true;
    for (const auto& guard : guards) {
        if (!guard->check()) {
            Logger::getGlobal().error(std::string("Guard failed: ") + guard->getDescription(), __FILE__, __LINE__, __func__);
            allPassed = false;
        }
    }
    return allPassed;
}

void SafeGuardUtils::addGuard(std::unique_ptr<Guard> guard) {
    if (guard) {
        guards.push_back(std::move(guard));
    }
}

void SafeGuardUtils::removeGuard(const std::string& description) {
    guards.erase(std::remove_if(guards.begin(), guards.end(),
                                 [&description](const std::unique_ptr<Guard>& g) {
                                     return g->getDescription() == description;
                                 }),
                 guards.end());
}

void SafeGuardUtils::clearAllGuards() {
    guards.clear();
}

// ============================================================================
// STRUCTURED ERROR LOGGER IMPLEMENTATION
// ============================================================================

static std::vector<StructuredErrorLogger::StructuredError> structuredErrors;
static std::function<void(const StructuredErrorLogger::StructuredError&)> structuredErrorCallback;

void StructuredErrorLogger::logStructuredError(const StructuredError& error) {
    structuredErrors.push_back(error);
    
    if (structuredErrorCallback) {
        structuredErrorCallback(error);
    }
}

void StructuredErrorLogger::logStructuredWarning(const StructuredError& warning) {
    // Convert warning to error with higher severity threshold
    StructuredError error = warning;
    error.severity = "WARNING";
    logStructuredError(error);
}

std::vector<StructuredErrorLogger::StructuredError> StructuredErrorLogger::getRecentStructuredErrors(size_t limit) {
    if (limit > structuredErrors.size()) {
        limit = structuredErrors.size();
    }
    
    std::vector<StructuredError> result(structuredErrors.end() - limit, structuredErrors.end());
    return result;
}

void StructuredErrorLogger::clearStructuredErrorHistory() {
    structuredErrors.clear();
}

void StructuredErrorLogger::setStructuredErrorCallback(std::function<void(const StructuredError&)> callback) {
    structuredErrorCallback = callback;
}

void StructuredErrorLogger::logMemoryError(const std::string& message, const std::string& component) {
    StructuredError error;
    error.severity = "ERROR";
    error.category = "MEMORY";
    error.message = message;
    error.component = component;
    error.file = __FILE__;
    error.line = __LINE__;
    error.function = __func__;
    error.timestamp = std::chrono::system_clock::now();
    logStructuredError(error);
}

void StructuredErrorLogger::logValidationError(const std::string& message, const std::string& component) {
    StructuredError error;
    error.severity = "ERROR";
    error.category = "VALIDATION";
    error.message = message;
    error.component = component;
    error.file = __FILE__;
    error.line = __LINE__;
    error.function = __func__;
    error.timestamp = std::chrono::system_clock::now();
    logStructuredError(error);
}

void StructuredErrorLogger::logStateError(const std::string& message, const std::string& component) {
    StructuredError error;
    error.severity = "ERROR";
    error.category = "STATE";
    error.message = message;
    error.component = component;
    error.file = __FILE__;
    error.line = __LINE__;
    error.function = __func__;
    error.timestamp = std::chrono::system_clock::now();
    logStructuredError(error);
}

void StructuredErrorLogger::logResourceError(const std::string& message, const std::string& component) {
    StructuredError error;
    error.severity = "ERROR";
    error.category = "RESOURCE";
    error.message = message;
    error.component = component;
    error.file = __FILE__;
    error.line = __LINE__;
    error.function = __func__;
    error.timestamp = std::chrono::system_clock::now();
    logStructuredError(error);
}

void StructuredErrorLogger::logConfigurationError(const std::string& message, const std::string& component) {
    StructuredError error;
    error.severity = "ERROR";
    error.category = "CONFIGURATION";
    error.message = message;
    error.component = component;
    error.file = __FILE__;
    error.line = __LINE__;
    error.function = __func__;
    error.timestamp = std::chrono::system_clock::now();
    logStructuredError(error);
}

// ============================================================================
// GLOBAL ERROR HANDLER IMPLEMENTATION
// ============================================================================

namespace {
    bool structuredLoggingEnabled = false;
    bool errorRecoveryEnabled = false;
    bool guardCheckingEnabled = false;
}

void GlobalErrorHandler::initialize() {
    structuredLoggingEnabled = true;
    errorRecoveryEnabled = true;
    guardCheckingEnabled = true;
    
    Logger::getGlobal().info("Error handling system initialized", __FILE__, __LINE__, __func__);
}

void GlobalErrorHandler::shutdown() {
    Logger::getGlobal().info("Error handling system shutdown", __FILE__, __LINE__, __func__);
    
    errorHandlers.clear();
    recentErrors.clear();
    structuredErrors.clear();
}

void GlobalErrorHandler::enableStructuredLogging(bool enable) {
    structuredLoggingEnabled = enable;
}

void GlobalErrorHandler::enableErrorRecovery(bool enable) {
    errorRecoveryEnabled = enable;
}

void GlobalErrorHandler::enableGuardChecking(bool enable) {
    guardCheckingEnabled = enable;
}

bool GlobalErrorHandler::isStructuredLoggingEnabled() {
    return structuredLoggingEnabled;
}

bool GlobalErrorHandler::isErrorRecoveryEnabled() {
    return errorRecoveryEnabled;
}

bool GlobalErrorHandler::isGuardCheckingEnabled() {
    return guardCheckingEnabled;
}

std::string GlobalErrorHandler::getErrorSummary() {
    std::stringstream ss;
    ss << "=== GLOBAL ERROR SUMMARY ===\n";
    ss << "Structured logging enabled: " << (structuredLoggingEnabled ? "yes" : "no") << "\n";
    ss << "Error recovery enabled: " << (errorRecoveryEnabled ? "yes" : "no") << "\n";
    ss << "Guard checking enabled: " << (guardCheckingEnabled ? "yes" : "no") << "\n";
    ss << "Total errors: " << recentErrors.size() << "\n";
    ss << "Total structured errors: " << structuredErrors.size() << "\n";
    ss << "Registered error handlers: " << errorHandlers.size() << "\n";
    ss << "Active guards: " << guards.size() << "\n";
    
    return ss.str();
}

void GlobalErrorHandler::logSystemStatus() {
    Logger::getGlobal().info(std::string("=== SYSTEM STATUS ===\n") + getErrorSummary(), __FILE__, __LINE__, __func__);
}

} // namespace nlm
