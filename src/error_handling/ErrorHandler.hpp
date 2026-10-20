#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>
#include <filesystem>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

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

// Error context information
struct ErrorContext {
    std::string operation;
    std::string component;
    std::string testName;
    std::string file;
    int line;
    std::string function;
    std::chrono::steady_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ErrorContext(const std::string& op, const std::string& comp, const std::string& test,
                 const std::string& f = "", int l = 0, const std::string& func = "")
        : operation(op), component(comp), testName(test), file(f), line(l), function(func) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// Error information
struct ErrorInfo {
    ErrorSeverity severity;
    std::string message;
    std::string code;
    std::string category;
    std::vector<std::string> suggestions;
    std::vector<std::string> recoverySteps;
    bool recoverable;
    std::chrono::milliseconds estimatedRecoveryTime;
    
    ErrorInfo(ErrorSeverity sev, const std::string& msg, const std::string& code = "")
        : severity(sev), message(msg), code(code), recoverable(false), 
          estimatedRecoveryTime(0) {}
};

// Recovery action
struct RecoveryAction {
    std::string actionId;
    std::string description;
    std::function<bool()> executeFunction;
    std::chrono::milliseconds estimatedTime;
    bool requiresUserInput;
    std::string riskLevel; // low, medium, high
    
    RecoveryAction(const std::string& id, const std::string& desc, 
                   std::function<bool()> func, std::chrono::milliseconds time)
        : actionId(id), description(desc), executeFunction(func), 
          estimatedTime(time), requiresUserInput(false), riskLevel("medium") {}
};

// Input validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        if (!valid) {
            ss << "INVALID: " << errors.size() << " error(s), " << warnings.size() << " warning(s)";
        } else if (!warnings.empty()) {
            ss << "VALID with " << warnings.size() << " warning(s)";
        } else {
            ss << "VALID";
        }
        return ss.str();
    }
};

// Exception hierarchy for NLM
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& code = "")
        : std::runtime_error(message), severity(severity), code(code) {}
    
    ErrorSeverity getSeverity() const { return severity; }
    const std::string& getCode() const { return code; }
    
protected:
    ErrorSeverity severity;
    std::string code;
};

class NLMValidationException : public NLMException {
public:
    NLMValidationException(const std::string& message, const std::vector<std::string>& errors)
        : NLMException(message, ErrorSeverity::Error, "VALIDATION_ERROR"), errors(errors) {}
    
    const std::vector<std::string>& getErrors() const { return errors; }
    
private:
    std::vector<std::string> errors;
};

class NLMConfigException : public NLMException {
public:
    NLMConfigException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, severity, "CONFIG_ERROR") {}
};

class NLMSimulationException : public NLMException {
public:
    NLMSimulationException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical)
        : NLMException(message, severity, "SIMULATION_ERROR") {}
};

// Comprehensive error handler
class ErrorHandler {
public:
    ErrorHandler(std::shared_ptr<Logger> logger);
    ~ErrorHandler();
    
    // Error reporting
    void reportError(const ErrorInfo& error, const ErrorContext& context = ErrorContext("", "", ""));
    void reportError(ErrorSeverity severity, const std::string& message,
                    const std::string& code = "", const ErrorContext& context = ErrorContext("", "", ""));
    void reportError(const NLMException& exception, const ErrorContext& context = ErrorContext("", "", ""));
    
    // Exception handling
    bool handleException(const std::exception& e, const ErrorContext& context = ErrorContext("", "", ""));
    bool handleException(const NLMException& e, const ErrorContext& context = ErrorContext("", "", ""));
    
    // Error recovery
    bool attemptRecovery(const std::string& errorId, const std::string& recoveryActionId);
    std::vector<RecoveryAction> getAvailableRecoveryActions(const std::string& errorId) const;
    
    // Input validation
    ValidationResult validateInput(const std::string& key, const std::string& value,
                                   const std::string& testName = "") const;
    ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) const;
    bool addValidationRule(const std::string& ruleName, 
                          std::function<ValidationResult(const std::string&, const std::string&)> validator);
    
    // File operation error handling
    bool safeFileRead(const std::string& filepath, std::string& content,
                     const ErrorContext& context = ErrorContext("", "", ""));
    bool safeFileWrite(const std::string& filepath, const std::string& content,
                      const ErrorContext& context = ErrorContext("", "", ""));
    bool safeDirectoryCreate(const std::string& dirpath,
                            const ErrorContext& context = ErrorContext("", "", ""));
    
    // Error analysis and reporting
    std::vector<ErrorInfo> getRecentErrors(size_t limit = 10) const;
    std::vector<ErrorInfo> getErrorsBySeverity(ErrorSeverity severity) const;
    std::vector<ErrorInfo> getErrorsByComponent(const std::string& component) const;
    std::string generateErrorReport(const std::vector<ErrorInfo>& errors) const;
    
    // Error statistics
    size_t getTotalErrorCount() const;
    size_t getErrorCount(ErrorSeverity severity) const;
    double getRecoverySuccessRate() const;
    
    // Configuration
    void setMaxErrors(size_t maxErrors);
    void setAutoRecovery(bool enable);
    void setRecoveryTimeout(std::chrono::seconds timeout);
    void setErrorReportPath(const std::string& path);
    
    // Error history and persistence
    bool saveErrorHistory(const std::string& filepath) const;
    bool loadErrorHistory(const std::string& filepath);
    void clearErrorHistory();
    
    // Global error handling
    static ErrorHandler& getGlobalErrorHandler();
    static void setGlobalErrorHandler(std::shared_ptr<ErrorHandler> handler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void logError(const ErrorInfo& error, const ErrorContext& context);
    bool attemptRecoveryAction(const RecoveryAction& action);
    ValidationResult validateAgainstRules(const std::string& key, const std::string& value) const;
    std::string getErrorId(const ErrorInfo& error, const ErrorContext& context) const;
    std::chrono::steady_clock::time_point getSessionStartTime() const;
    bool isSessionTimeExpired(const ErrorContext& context) const;
    
    // Error recovery
    void registerDefaultRecoveryActions();
    std::vector<RecoveryAction> getRecoveryActionsForError(const std::string& errorId) const;
    
    // Validation rule management
    void registerDefaultValidationRules();
    ValidationResult validateNeuralParameters(const std::map<std::string, std::string>& params) const;
    ValidationResult validatePlasticityParameters(const std::map<std::string, std::string>& params) const;
    ValidationResult validateMemoryParameters(const std::map<std::string, std::string>& params) const;
    ValidationResult validatePerformanceParameters(const std::map<std::string, std::string>& params) const;
};

// Input parameter validator
class InputValidator {
public:
    InputValidator(std::shared_ptr<ErrorHandler> errorHandler);
    ~InputValidator();
    
    // Parameter validation
    bool validateParameter(const std::string& key, const std::string& value,
                          const std::string& testName = "") const;
    ValidationResult validateParameters(const std::map<std::string, std::string>& params,
                                       const std::string& testName = "") const;
    
    // Range validation
    ValidationResult validateIntRange(const std::string& key, const std::string& value,
                                     int min, int max, const std::string& testName = "") const;
    ValidationResult validateDoubleRange(const std::string& key, const std::string& value,
                                         double min, double max, const std::string& testName = "") const;
    ValidationResult validateStringLength(const std::string& key, const std::string& value,
                                          size_t minLength, size_t maxLength,
                                          const std::string& testName = "") const;
    
    // Format validation
    ValidationResult validateFilePath(const std::string& key, const std::string& value,
                                     const std::string& testName = "") const;
    ValidationResult validateURL(const std::string& key, const std::string& value,
                                const std::string& testName = "") const;
    ValidationResult validateTestName(const std::string& key, const std::string& value,
                                     const std::string& testName = "") const;
    
    // Special validation
    ValidationResult validateTestParameters(const std::string& testName,
                                            const std::map<std::string, std::string>& params) const;
    ValidationResult validateExecutionMode(const std::string& mode) const;
    
    // Dependency validation
    ValidationResult validateDependencies(const std::map<std::string, std::string>& params) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool isValidParameterType(const std::string& key, const std::string& value) const;
    std::string getParameterType(const std::string& key) const;
    ValidationResult validateAgainstSchema(const std::string& key, const std::string& value,
                                           const std::string& testName = "") const;
    
    // Schema validation
    ValidationResult validateAgainstNeuralSchema(const std::map<std::string, std::string>& params) const;
    ValidationResult validateAgainstPlasticitySchema(const std::map<std::string, std::string>& params) const;
    ValidationResult validateAgainstMemorySchema(const std::map<std::string, std::string>& params) const;
    ValidationResult validateAgainstPerformanceSchema(const std::map<std::string, std::string>& params) const;
};

// Graceful failure handler
class GracefulFailureHandler {
public:
    GracefulFailureHandler(std::shared_ptr<Logger> logger, std::shared_ptr<ErrorHandler> errorHandler);
    ~GracefulFailureHandler();
    
    // Test failure handling
    bool handleTestFailure(const std::string& testName, const std::string& error,
                          const ErrorContext& context = ErrorContext("", "", ""));
    bool handleTestFailure(const NLMException& exception, const ErrorContext& context = ErrorContext("", "", ""));
    
    // Simulation failure handling
    bool handleSimulationFailure(const std::string& error, const ErrorContext& context = ErrorContext("", "", ""));
    bool continueWithDegradedMode();
    bool rollbackToPreviousState();
    
    // Recovery mechanisms
    bool attemptConfigurationRecovery();
    bool attemptDataRecovery();
    bool attemptResourceRecovery();
    
    // State management
    bool saveFailureState();
    bool restoreFailureState();
    void clearFailureState();
    
    // Statistics
    size_t getTotalFailures() const;
    size_t getFailuresByTest(const std::string& testName) const;
    double getRecoverySuccessRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool shouldAttemptRecovery(const ErrorInfo& error) const;
    RecoveryAction getBestRecoveryAction(const std::string& errorId) const;
    bool executeRecoveryAction(const RecoveryAction& action);
    bool isInRecoveryMode() const;
};

} // namespace nlm
