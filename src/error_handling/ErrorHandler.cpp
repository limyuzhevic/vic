#include "ErrorHandler.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <thread>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ErrorHandler::Impl {
    std::shared_ptr<Logger> logger;
    
    // Error tracking
    std::vector<ErrorInfo> recentErrors;
    std::map<std::string, std::vector<RecoveryAction>> recoveryActions;
    mutable std::mutex errorMutex;
    
    // Statistics
    size_t totalErrors;
    size_t errorsBySeverity[5]; // Debug, Info, Warning, Error, Critical, Fatal
    size_t totalRecoveries;
    size_t successfulRecoveries;
    
    // Configuration
    size_t maxErrors;
    bool autoRecovery;
    std::chrono::seconds recoveryTimeout;
    std::string errorReportPath;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Recovery actions registry
    std::map<std::string, RecoveryAction> globalRecoveryActions;
    
    Impl(std::shared_ptr<Logger> logg) : logger(logg), totalErrors(0), 
                                         totalRecoveries(0), successfulRecoveries(0),
                                         maxErrors(100), autoRecovery(true),
                                         recoveryTimeout(std::chrono::seconds(30)) {
        sessionStartTime = std::chrono::steady_clock::now();
        std::fill(std::begin(errorsBySeverity), std::end(errorsBySeverity), 0);
        
        registerDefaultRecoveryActions();
        registerDefaultValidationRules();
    }
    
    void incrementErrorCount(ErrorSeverity severity) {
        totalErrors++;
        int index = static_cast<int>(severity);
        if (index >= 0 && index < 5) {
            errorsBySeverity[index]++;
        }
    }
    
    void incrementRecoveryCount(bool success) {
        totalRecoveries++;
        if (success) {
            successfulRecoveries++;
        }
    }
};

ErrorHandler::ErrorHandler(std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>(logger)) {
}

ErrorHandler::~ErrorHandler() = default;

void ErrorHandler::reportError(const ErrorInfo& error, const ErrorContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    // Validate error
    if (error.severity == ErrorSeverity::Fatal) {
        // Fatal errors cannot be recovered from
        pImpl->incrementErrorCount(error.severity);
        logError(error, context);
        
        // Throw exception for fatal errors
        throw NLMException(error.message, error.severity, error.code);
    }
    
    // Record error
    pImpl->incrementErrorCount(error.severity);
    pImpl->recentErrors.push_back(error);
    
    // Log error
    logError(error, context);
    
    // Attempt auto-recovery if enabled
    if (pImpl->autoRecovery && error.recoverable) {
        std::string errorId = getErrorId(error, context);
        auto actions = getRecoveryActionsForError(errorId);
        if (!actions.empty()) {
            attemptRecoveryAction(actions[0]);
        }
    }
}

void ErrorHandler::reportError(ErrorSeverity severity, const std::string& message,
                              const std::string& code, const ErrorContext& context) {
    ErrorInfo error(severity, message, code);
    reportError(error, context);
}

void ErrorHandler::reportError(const NLMException& exception, const ErrorContext& context) {
    ErrorInfo error(exception.getSeverity(), exception.what(), exception.getCode());
    reportError(error, context);
}

bool ErrorHandler::handleException(const std::exception& e, const ErrorContext& context) {
    try {
        throw e;
    } catch (const NLMException& nlmEx) {
        return handleException(nlmEx, context);
    } catch (const std::exception& stdEx) {
        ErrorInfo error(ErrorSeverity::Error, stdEx.what(), "STD_EXCEPTION");
        reportError(error, context);
        return true;
    }
    return false;
}

bool ErrorHandler::handleException(const NLMException& e, const ErrorContext& context) {
    ErrorInfo error(e.getSeverity(), e.what(), e.getCode());
    reportError(error, context);
    return true;
}

bool ErrorHandler::attemptRecovery(const std::string& errorId, const std::string& recoveryActionId) {
    auto it = pImpl->recoveryActions.find(errorId);
    if (it == pImpl->recoveryActions.end()) {
        return false;
    }
    
    for (const auto& action : it->second) {
        if (action.actionId == recoveryActionId) {
            return attemptRecoveryAction(action);
        }
    }
    
    return false;
}

std::vector<RecoveryAction> ErrorHandler::getAvailableRecoveryActions(const std::string& errorId) const {
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    auto it = pImpl->recoveryActions.find(errorId);
    if (it != pImpl->recoveryActions.end()) {
        return it->second;
    }
    
    return std::vector<RecoveryAction>();
}

ValidationResult ErrorHandler::validateInput(const std::string& key, const std::string& value,
                                           const std::string& testName) const {
    ValidationResult result;
    
    // Basic type validation
    if (!isValidParameterType(key, value)) {
        result.addError("Invalid type for parameter " + key + ": " + value);
    }
    
    // Value validation
    result = validateAgainstRules(key, value);
    
    return result;
}

ValidationResult ErrorHandler::validateConfiguration(const std::shared_ptr<Config>& config) const {
    ValidationResult result;
    
    if (!config) {
        result.addError("Configuration is null");
        return result;
    }
    
    // Validate all configuration parameters
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        auto val = config->get<std::string>(key);
        if (val.has_value()) {
            ValidationResult paramResult = validateInput(key, val.value());
            if (!paramResult.valid) {
                for (const auto& error : paramResult.errors) {
                    result.addError("Parameter " + key + ": " + error);
                }
                for (const auto& warning : paramResult.warnings) {
                    result.addWarning("Parameter " + key + ": " + warning);
                }
            }
        }
    }
    
    return result;
}

bool ErrorHandler::addValidationRule(const std::string& ruleName,
                                    std::function<ValidationResult(const std::string&, const std::string&)> validator) {
    // In real implementation, would store validation rules
    return true;
}

bool ErrorHandler::safeFileRead(const std::string& filepath, std::string& content,
                               const ErrorContext& context) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ErrorInfo error(ErrorSeverity::Error, "Failed to read file: " + filepath, "FILE_READ_ERROR");
        reportError(error, context);
        return false;
    }
    
    try {
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    } catch (const std::exception& e) {
        ErrorInfo error(ErrorSeverity::Error, "Exception reading file: " + std::string(e.what()), "FILE_READ_EXCEPTION");
        reportError(error, context);
        return false;
    }
    
    return true;
}

bool ErrorHandler::safeFileWrite(const std::string& filepath, const std::string& content,
                                const ErrorContext& context) {
    // Ensure directory exists
    std::filesystem::path filePath(filepath);
    std::filesystem::path dirPath = filePath.parent_path();
    
    if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
        if (!safeDirectoryCreate(dirPath.string(), context)) {
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        ErrorInfo error(ErrorSeverity::Error, "Failed to write file: " + filepath, "FILE_WRITE_ERROR");
        reportError(error, context);
        return false;
    }
    
    try {
        file << content;
    } catch (const std::exception& e) {
        ErrorInfo error(ErrorSeverity::Error, "Exception writing file: " + std::string(e.what()), "FILE_WRITE_EXCEPTION");
        reportError(error, context);
        return false;
    }
    
    return true;
}

bool ErrorHandler::safeDirectoryCreate(const std::string& dirpath,
                                      const ErrorContext& context) {
    try {
        if (!std::filesystem::create_directories(dirpath)) {
            ErrorInfo error(ErrorSeverity::Error, "Failed to create directory: " + dirpath, "DIRECTORY_CREATE_ERROR");
            reportError(error, context);
            return false;
        }
    } catch (const std::exception& e) {
        ErrorInfo error(ErrorSeverity::Error, "Exception creating directory: " + std::string(e.what()), "DIRECTORY_CREATE_EXCEPTION");
        reportError(error, context);
        return false;
    }
    
    return true;
}

std::vector<ErrorInfo> ErrorHandler::getRecentErrors(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    std::vector<ErrorInfo> errors(pImpl->recentErrors);
    if (limit > 0 && limit < errors.size()) {
        errors.resize(limit);
    }
    
    // Sort by timestamp (newest first)
    std::sort(errors.begin(), errors.end(),
        [](const ErrorInfo& a, const ErrorInfo& b) {
            return a.timestamp > b.timestamp;
        });
    
    return errors;
}

std::vector<ErrorInfo> ErrorHandler::getErrorsBySeverity(ErrorSeverity severity) const {
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    std::vector<ErrorInfo> errors;
    for (const auto& error : pImpl->recentErrors) {
        if (error.severity == severity) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::vector<ErrorInfo> ErrorHandler::getErrorsByComponent(const std::string& component) const {
    std::lock_guard<std::mutex> lock(pImpl->errorMutex);
    
    std::vector<ErrorInfo> errors;
    for (const auto& error : pImpl->recentErrors) {
        if (error.category == component) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string ErrorHandler::generateErrorReport(const std::vector<ErrorInfo>& errors) const {
    json report;
    
    report["generated_at"] = std::chrono::system_clock::now();
    report["error_count"] = errors.size();
    
    json severityCounts;
    for (int i = 0; i < 5; ++i) {
        ErrorSeverity sev = static_cast<ErrorSeverity>(i);
        int count = std::count_if(errors.begin(), errors.end(),
                                 [sev](const ErrorInfo& e) { return e.severity == sev; });
        severityCounts[std::to_string(i)] = count;
    }
    
    report["severity_counts"] = severityCounts;
    
    json errorList = json::array();
    for (const auto& error : errors) {
        json errorJson;
        errorJson["severity"] = static_cast<int>(error.severity);
        errorJson["message"] = error.message;
        errorJson["code"] = error.code;
        errorJson["category"] = error.category;
        errorJson["recoverable"] = error.recoverable;
        errorJson["suggestions"] = error.suggestions;
        errorJson["recovery_steps"] = error.recoverySteps;
        
        errorList.push_back(errorJson);
    }
    
    report["errors"] = errorList;
    
    return report.dump(2);
}

size_t ErrorHandler::getTotalErrorCount() const {
    return pImpl->totalErrors;
}

size_t ErrorHandler::getErrorCount(ErrorSeverity severity) const {
    int index = static_cast<int>(severity);
    if (index >= 0 && index < 5) {
        return pImpl->errorsBySeverity[index];
    }
    return 0;
}

double ErrorHandler::getRecoverySuccessRate() const {
    if (pImpl->totalRecoveries == 0) return 0.0;
    return static_cast<double>(pImpl->successfulRecoveries) / pImpl->totalRecoveries;
}

void ErrorHandler::setMaxErrors(size_t maxErrors) {
    pImpl->maxErrors = maxErrors;
}

void ErrorHandler::setAutoRecovery(bool enable) {
    pImpl->autoRecovery = enable;
}

void ErrorHandler::setRecoveryTimeout(std::chrono::seconds timeout) {
    pImpl->recoveryTimeout = timeout;
}

void ErrorHandler::setErrorReportPath(const std::string& path) {
    pImpl->errorReportPath = path;
}

std::chrono::steady_clock::time_point ErrorHandler::getSessionStartTime() const {
    return pImpl->sessionStartTime;
}

bool ErrorHandler::isSessionTimeExpired(const ErrorContext& context) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->sessionStartTime);
    return elapsed > std::chrono::seconds(3600); // 1 hour session limit
}

void ErrorHandler::logError(const ErrorInfo& error, const ErrorContext& context) {
    std::string logMessage = error.message;
    if (!context.testName.empty()) {
        logMessage = "[" + context.testName + "] " + logMessage;
    }
    if (!context.component.empty()) {
        logMessage = "[" + context.component + "] " + logMessage;
    }
    
    if (pImpl->logger) {
        switch (error.severity) {
            case ErrorSeverity::Debug:
                pImpl->logger->debug(logMessage);
                break;
            case ErrorSeverity::Info:
                pImpl->logger->info(logMessage);
                break;
            case ErrorSeverity::Warning:
                pImpl->logger->warning(logMessage);
                break;
            case ErrorSeverity::Error:
                pImpl->logger->error(logMessage);
                break;
            case ErrorSeverity::Critical:
            case ErrorSeverity::Fatal:
                pImpl->logger->critical(logMessage);
                break;
        }
    }
}

bool ErrorHandler::attemptRecoveryAction(const RecoveryAction& action) {
    try {
        bool success = action.executeFunction();
        pImpl->incrementRecoveryCount(success);
        return success;
    } catch (const std::exception& e) {
        return false;
    }
}

ValidationResult ErrorHandler::validateAgainstRules(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    // Basic validation for known parameters
    if (key == "neuron_count") {
        try {
            int count = std::stoi(value);
            if (count <= 0) {
                result.addError("neuron_count must be positive");
            }
            if (count > 100000) {
                result.addWarning("neuron_count is very large, may impact performance");
            }
        } catch (...) {
            result.addError("neuron_count must be an integer");
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0.0) {
                result.addError("simulation_timestep must be positive");
            }
            if (timestep > 1.0) {
                result.addWarning("simulation_timestep is large, may impact accuracy");
            }
        } catch (...) {
            result.addError("simulation_timestep must be a number");
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            if (prob < 0.0) {
                result.addError("connection_probability must be non-negative");
            }
            if (prob > 1.0) {
                result.addError("connection_probability must not exceed 1.0");
            }
        } catch (...) {
            result.addError("connection_probability must be a number");
        }
    }
    
    return result;
}

std::string ErrorHandler::getErrorId(const ErrorInfo& error, const ErrorContext& context) const {
    std::stringstream ss;
    ss << static_cast<int>(error.severity) << "_" << error.code << "_" << context.component;
    return ss.str();
}

void ErrorHandler::registerDefaultRecoveryActions() {
    // Configuration recovery
    RecoveryAction configRecovery("CONFIG_RESTORE", "Restore from backup", 
        []() {
            // In real implementation, would restore configuration from backup
            return true;
        },
        std::chrono::seconds(5));
    
    pImpl->globalRecoveryActions["CONFIG_RESTORE"] = configRecovery;
    
    // Data recovery
    RecoveryAction dataRecovery("DATA_RESTORE", "Restore from backup",
        []() {
            // In real implementation, would restore data from backup
            return true;
        },
        std::chrono::seconds(10));
    
    pImpl->globalRecoveryActions["DATA_RESTORE"] = dataRecovery;
    
    // Resource recovery
    RecoveryAction resourceRecovery("RESOURCE_CLEANUP", "Clean up resources and retry",
        []() {
            // In real implementation, would clean up resources
            return true;
        },
        std::chrono::seconds(2));
    
    pImpl->globalRecoveryActions["RESOURCE_CLEANUP"] = resourceRecovery;
}

std::vector<RecoveryAction> ErrorHandler::getRecoveryActionsForError(const std::string& errorId) const {
    std::vector<RecoveryAction> actions;
    
    // Get recovery actions from registry
    auto it = pImpl->globalRecoveryActions.find(errorId);
    if (it != pImpl->globalRecoveryActions.end()) {
        actions.push_back(it->second);
    }
    
    // Get recovery actions for this error ID from error-specific registry
    auto errorIt = pImpl->recoveryActions.find(errorId);
    if (errorIt != pImpl->recoveryActions.end()) {
        actions.insert(actions.end(), errorIt->second.begin(), errorIt->second.end());
    }
    
    return actions;
}

void ErrorHandler::registerDefaultValidationRules() {
    // Register default validation rules
    // In real implementation, would set up comprehensive validation rules
}

ErrorHandler& ErrorHandler::getGlobalErrorHandler() {
    static std::shared_ptr<ErrorHandler> globalHandler;
    if (!globalHandler) {
        globalHandler = std::make_shared<ErrorHandler>(Logger::getGlobal());
    }
    return *globalHandler;
}

void ErrorHandler::setGlobalErrorHandler(std::shared_ptr<ErrorHandler> handler) {
    // In real implementation, would set the global error handler
}

InputValidator::InputValidator(std::shared_ptr<ErrorHandler> errorHandler) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->errorHandler = errorHandler;
}

InputValidator::~InputValidator() = default;

bool InputValidator::validateParameter(const std::string& key, const std::string& value,
                                      const std::string& testName) const {
    ValidationResult result = pImpl->errorHandler->validateInput(key, value, testName);
    return result.valid;
}

ValidationResult InputValidator::validateParameters(const std::map<std::string, std::string>& params,
                                                   const std::string& testName) const {
    ValidationResult result;
    
    for (const auto& pair : params) {
        ValidationResult paramResult = pImpl->errorHandler->validateInput(pair.first, pair.second, testName);
        if (!paramResult.valid) {
            for (const auto& error : paramResult.errors) {
                result.addError("Parameter " + pair.first + ": " + error);
            }
            for (const auto& warning : paramResult.warnings) {
                result.addWarning("Parameter " + pair.first + ": " + warning);
            }
        }
    }
    
    return result;
}

// ErrorHandler::Impl nested class implementation
struct ErrorHandler::Impl {
    std::shared_ptr<Logger> logger;
    std::vector<ErrorInfo> recentErrors;
    std::map<std::string, std::vector<RecoveryAction>> recoveryActions;
    mutable std::mutex errorMutex;
    size_t totalErrors;
    size_t errorsBySeverity[5];
    size_t totalRecoveries;
    size_t successfulRecoveries;
    size_t maxErrors;
    bool autoRecovery;
    std::chrono::seconds recoveryTimeout;
    std::string errorReportPath;
    std::chrono::steady_clock::time_point sessionStartTime;
    std::map<std::string, RecoveryAction> globalRecoveryActions;
    
    Impl(std::shared_ptr<Logger> logg) : logger(logg), totalErrors(0), 
                                         totalRecoveries(0), successfulRecoveries(0),
                                         maxErrors(100), autoRecovery(true),
                                         recoveryTimeout(std::chrono::seconds(30)) {
        sessionStartTime = std::chrono::steady_clock::now();
        std::fill(std::begin(errorsBySeverity), std::end(errorsBySeverity), 0);
    }
    
    void incrementErrorCount(ErrorSeverity severity) {
        totalErrors++;
        int index = static_cast<int>(severity);
        if (index >= 0 && index < 5) {
            errorsBySeverity[index]++;
        }
    }
    
    void incrementRecoveryCount(bool success) {
        totalRecoveries++;
        if (success) {
            successfulRecoveries++;
        }
    }
};

} // namespace nlm
