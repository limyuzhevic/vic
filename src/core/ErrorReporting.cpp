// Implementation of ErrorReporting for NLM

#include "ErrorReporting.hpp"
#include <thread>
#include <algorithm>
#include <ctime>
#include <random>

namespace nlm {

struct ErrorReporting::Impl {
    bool logErrors;
    bool includeStackTrace;
    bool includeContext;
    size_t maxStackTraceDepth;
    bool useColors;
    bool showRecoverySuggestions;
    bool enableAutoReporting;
    
    std::function<void(const ErrorReport&)> errorReportCallback;
    
    std::map<std::string, std::shared_ptr<IErrorHandler>> errorHandlers;
    
    std::unordered_map<std::string, ErrorReport> errorRegistry;
    
    std::map<std::string, std::vector<RecoverySuggestion>> errorRecoverySuggestions;
    
    std::chrono::system_clock::time_point lastErrorTime;
    
    std::string lastOperation;
    std::string lastFile;
    int lastLine;
    std::string lastFunction;
    std::string lastComponent;
};

ErrorReporting::ErrorReporting() : pImpl(std::make_unique<Impl>()) {
    pImpl->logErrors = true;
    pImpl->includeStackTrace = true;
    pImpl->includeContext = true;
    pImpl->maxStackTraceDepth = 32;
    pImpl->useColors = true;
    pImpl->showRecoverySuggestions = true;
    pImpl->enableAutoReporting = true;
}

ErrorReporting::~ErrorReporting() {
}

void ErrorReporting::setLogErrors(bool logErrors) {
    pImpl->logErrors = logErrors;
}

void ErrorReporting::setIncludeStackTrace(bool includeStackTrace) {
    pImpl->includeStackTrace = includeStackTrace;
}

void ErrorReporting::setIncludeContext(bool includeContext) {
    pImpl->includeContext = includeContext;
}

void ErrorReporting::setMaxStackTraceDepth(size_t depth) {
    pImpl->maxStackTraceDepth = depth;
}

void ErrorReporting::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

void ErrorReporting::setShowRecoverySuggestions(bool show) {
    pImpl->showRecoverySuggestions = show;
}

void ErrorReporting::setEnableAutoReporting(bool enabled) {
    pImpl->enableAutoReporting = enabled;
}

void ErrorReporting::setErrorReportCallback(const std::function<void(const ErrorReport&)>& callback) {
    pImpl->errorReportCallback = callback;
}

std::string ErrorReporting::reportError(const std::string& message, ErrorSeverity severity,
                                        ErrorCategory category, const std::string& code,
                                        const ErrorContext& context) {
    std::vector<StackTraceEntry> stackTrace;
    if (pImpl->includeStackTrace) {
        stackTrace = captureStackTrace(pImpl->maxStackTraceDepth);
    }
    
    std::string errorId;
    createErrorReport(message, severity, category, code, context, stackTrace, errorId);
    
    if (pImpl->logErrors) {
        logError(errorId);
    }
    
    if (pImpl->enableAutoReporting && pImpl->errorReportCallback) {
        pImpl->errorReportCallback(pImpl->errorRegistry[errorId]);
    }
    
    // Check for auto-handlers
    for (const auto& pair : pImpl->errorHandlers) {
        if (pair.second->canHandleError(pImpl->errorRegistry[errorId])) {
            pair.second->handleError(pImpl->errorRegistry[errorId]);
        }
    }
    
    return errorId;
}

std::string ErrorReporting::reportError(const std::exception& e, ErrorSeverity severity,
                                        ErrorCategory category, const std::string& code,
                                        const ErrorContext& context) {
    std::string message = extractErrorMessage(e);
    std::string extractedCode = code.empty() ? extractErrorCode(message) : code;
    ErrorCategory determinedCategory = category;
    
    if (category == ErrorCategory::Unknown) {
        determinedCategory = categorizeError(message, context);
    }
    
    if (severity == ErrorSeverity::Error) {
        severity = determineErrorSeverity(message, determinedCategory);
    }
    
    return reportError(message, severity, determinedCategory, extractedCode, context);
}

void ErrorReporting::handleError(const std::string& errorId, bool resolve,
                                const std::string& resolutionNotes) {
    auto it = pImpl->errorRegistry.find(errorId);
    if (it == pImpl->errorRegistry.end()) {
        return;
    }
    
    ErrorReport& error = it->second;
    
    if (resolve) {
        error.isResolved = true;
        error.resolvedTime = std::chrono::system_clock::now();
        error.resolutionNotes = resolutionNotes;
    }
    
    // Notify handlers
    for (const auto& pair : pImpl->errorHandlers) {
        if (pair.second->canHandleError(error)) {
            pair.second->handleError(error);
        }
    }
}

void ErrorReporting::addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    // Generate a simple ID for the handler (in real implementation, use UUID)
    static int handlerId = 0;
    std::string id = "handler_" + std::to_string(++handlerId);
    
    pImpl->errorHandlers[id] = handler;
}

void ErrorReporting::removeErrorHandler(const std::string& handlerId) {
    pImpl->errorHandlers.erase(handlerId);
}

void ErrorReporting::clearErrorHandlers() {
    pImpl->errorHandlers.clear();
}

ErrorCategory ErrorReporting::categorizeError(const std::string& message, const ErrorContext& context) const {
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    if (lowerMessage.find("memory") != std::string::npos || 
        lowerMessage.find("heap") != std::string::npos ||
        lowerMessage.find("allocate") != std::string::npos) {
        return ErrorCategory::Memory;
    }
    
    if (lowerMessage.find("cpu") != std::string::npos ||
        lowerMessage.find("performance") != std::string::npos ||
        lowerMessage.find("slow") != std::string::npos) {
        return ErrorCategory::Performance;
    }
    
    if (lowerMessage.find("config") != std::string::npos ||
        lowerMessage.find("parameter") != std::string::npos ||
        lowerMessage.find("setting") != std::string::npos) {
        return ErrorCategory::Configuration;
    }
    
    if (lowerMessage.find("brain") != std::string::npos ||
        lowerMessage.find("neuron") != std::string::npos ||
        lowerMessage.find("synapse") != std::string::npos) {
        return ErrorCategory::Brain;
    }
    
    if (lowerMessage.find("file") != std::string::npos ||
        lowerMessage.find("io") != std::string::npos ||
        lowerMessage.find("read") != std::string::npos ||
        lowerMessage.find("write") != std::string::npos) {
        return ErrorCategory::InputOutput;
    }
    
    if (lowerMessage.find("network") != std::string::npos ||
        lowerMessage.find("connection") != std::string::npos) {
        return ErrorCategory::Network;
    }
    
    if (context.component == "simulation" || 
        lowerMessage.find("simulation") != std::string::npos) {
        return ErrorCategory::Simulation;
    }
    
    if (context.component == "experiment" ||
        lowerMessage.find("experiment") != std::string::npos) {
        return ErrorCategory::Experiment;
    }
    
    if (context.component == "development" ||
        lowerMessage.find("development") != std::string::npos) {
        return ErrorCategory::Development;
    }
    
    return ErrorCategory::Unknown;
}

std::string ErrorReporting::errorCategoryToString(ErrorCategory category) const {
    static const std::unordered_map<ErrorCategory, std::string> categoryStrings = {
        {ErrorCategory::System, "System"},
        {ErrorCategory::Brain, "Brain"},
        {ErrorCategory::Memory, "Memory"},
        {ErrorCategory::Performance, "Performance"},
        {ErrorCategory::Configuration, "Configuration"},
        {ErrorCategory::Network, "Network"},
        {ErrorCategory::InputOutput, "Input/Output"},
        {ErrorCategory::Simulation, "Simulation"},
        {ErrorCategory::Experiment, "Experiment"},
        {ErrorCategory::Development, "Development"},
        {ErrorCategory::UserInput, "User Input"},
        {ErrorCategory::Resource, "Resource"},
        {ErrorCategory::Security, "Security"},
        {ErrorCategory::Plugin, "Plugin"},
        {ErrorCategory::Unknown, "Unknown"}
    };
    
    auto it = categoryStrings.find(category);
    return (it != categoryStrings.end()) ? it->second : "Unknown";
}

ErrorSeverity ErrorReporting::determineErrorSeverity(const std::string& message, ErrorCategory category) const {
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    if (lowerMessage.find("crash") != std::string::npos ||
        lowerMessage.find("fatal") != std::string::npos ||
        lowerMessage.find("segfault") != std::string::npos ||
        lowerMessage.find("access violation") != std::string::npos) {
        return ErrorSeverity::Fatal;
    }
    
    if (lowerMessage.find("error") != std::string::npos ||
        lowerMessage.find("failed") != std::string::npos ||
        category == ErrorCategory::Memory ||
        category == ErrorCategory::Configuration) {
        return ErrorSeverity::Error;
    }
    
    if (lowerMessage.find("warning") != std::string::npos ||
        lowerMessage.find("deprecated") != std::string::npos ||
        category == ErrorCategory::Performance ||
        category == ErrorCategory::Development) {
        return ErrorSeverity::Warning;
    }
    
    if (lowerMessage.find("debug") != std::string::npos ||
        category == ErrorCategory::System) {
        return ErrorSeverity::Debug;
    }
    
    return ErrorSeverity::Info;
}

std::vector<RecoverySuggestion> ErrorReporting::generateRecoverySuggestions(const ErrorReport& error) const {
    std::vector<RecoverySuggestion> suggestions;
    
    switch (error.category) {
        case ErrorCategory::Memory:
            suggestions.push_back({"Restart Application", 
                                  "Close unused applications and restart NLM to free up memory"});
            suggestions.push_back({"Check Memory Usage", 
                                  "Monitor memory usage to identify memory leaks"});
            suggestions.push_back({"Increase Memory", 
                                  "Ensure sufficient system memory is available"});
            break;
        
        case ErrorCategory::Performance:
            suggestions.push_back({"Close Unnecessary Applications", 
                                  "Reduce the number of running applications to free CPU resources"});
            suggestions.push_back({"Check Resource Usage", 
                                  "Monitor CPU and resource usage to identify bottlenecks"});
            break;
        
        case ErrorCategory::Configuration:
            suggestions.push_back({"Check Configuration", 
                                  "Review configuration files for errors or missing values"});
            suggestions.push_back({"Reset to Defaults", 
                                  "Reset configuration to default values and modify as needed"});
            break;
        
        case ErrorCategory::Brain:
            suggestions.push_back({"Check Brain Parameters", 
                                  "Review brain configuration parameters"});
            suggestions.push_back({"Restart Simulation", 
                                  "Try restarting the simulation to reset brain state"});
            break;
        
        case ErrorCategory::InputOutput:
            suggestions.push_back({"Check File Permissions", 
                                  "Ensure read/write permissions for required files"});
            suggestions.push_back({"Verify File Paths", 
                                  "Check that all file paths are correct and accessible"});
            break;
        
        case ErrorCategory::Network:
            suggestions.push_back({"Check Network Connection", 
                                  "Verify network connectivity and router settings"});
            suggestions.push_back({"Restart Network Interface", 
                                  "Restart network interfaces if necessary"});
            break;
        
        case ErrorCategory::Simulation:
            suggestions.push_back({"Check Simulation Parameters", 
                                  "Review simulation parameters and settings"});
            suggestions.push_back({"Reduce Complexity", 
                                  "Simplify simulation parameters if possible"});
            break;
        
        default:
            suggestions.push_back({"Consult Documentation", 
                                  "Review NLM documentation for error resolution steps"});
            suggestions.push_back({"Contact Support", 
                                  "Report this error to NLM technical support"});
            break;
    }
    
    // Add general suggestions
    suggestions.push_back({"Review Error Details", 
                          "Review error details and context for specific information"});
    suggestions.push_back({"Restart Application", 
                          "Try restarting the application to resolve the error"});
    
    return suggestions;
}

void ErrorReporting::addRecoverySuggestion(const std::string& errorId, const RecoverySuggestion& suggestion) {
    pImpl->errorRecoverySuggestions[errorId].push_back(suggestion);
}

void ErrorReporting::removeRecoverySuggestion(const std::string& errorId, const std::string& action) {
    auto it = pImpl->errorRecoverySuggestions.find(errorId);
    if (it != pImpl->errorRecoverySuggestions.end()) {
        auto& suggestions = it->second;
        suggestions.erase(std::remove_if(suggestions.begin(), suggestions.end(),
                                        [&action](const RecoverySuggestion& s) { return s.action == action; }),
                         suggestions.end());
    }
}

void ErrorReporting::clearResolvedErrors() {
    for (auto it = pImpl->errorRegistry.begin(); it != pImpl->errorRegistry.end();) {
        if (it->second.isResolved) {
            it = pImpl->errorRegistry.erase(it);
        } else {
            ++it;
        }
    }
}

void ErrorReporting::clearAllErrors() {
    pImpl->errorRegistry.clear();
    pImpl->errorRecoverySuggestions.clear();
}

bool ErrorReporting::isErrorResolved(const std::string& errorId) const {
    auto it = pImpl->errorRegistry.find(errorId);
    return (it != pImpl->errorRegistry.end()) ? it->second.isResolved : false;
}

std::vector<ErrorReport> ErrorReporting::getErrors(ErrorSeverity severity, ErrorCategory category,
                                                   bool includeResolved) const {
    std::vector<ErrorReport> result;
    
    for (const auto& pair : pImpl->errorRegistry) {
        const auto& error = pair.second;
        
        if (!includeResolved && error.isResolved) continue;
        if (severity != ErrorSeverity::Error && error.severity != severity) continue;
        if (category != ErrorCategory::Unknown && error.category != category) continue;
        
        result.push_back(error);
    }
    
    return result;
}

const ErrorReport* ErrorReporting::getError(const std::string& errorId) const {
    auto it = pImpl->errorRegistry.find(errorId);
    return (it != pImpl->errorRegistry.end()) ? &it->second : nullptr;
}

std::string ErrorReporting::generateErrorReport(const std::string& errorId, bool includeStackTrace) const {
    std::stringstream ss;
    
    if (!errorId.empty()) {
        auto it = pImpl->errorRegistry.find(errorId);
        if (it == pImpl->errorRegistry.end()) {
            return "Error not found: " + errorId;
        }
        const ErrorReport& error = it->second;
        formatErrorReport(ss, error, includeStackTrace);
    } else {
        // Generate report for all errors
        ss << "=== NLM Error Report ===" << std::endl;
        ss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
        ss << std::endl;
        
        auto allErrors = getErrors();
        if (allErrors.empty()) {
            ss << "No errors recorded." << std::endl;
            return ss.str();
        }
        
        for (const auto& error : allErrors) {
            formatErrorReport(ss, error, includeStackTrace);
            ss << std::endl;
        }
    }
    
    return ss.str();
}

std::string ErrorReporting::generateErrorAnalysisReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Error Analysis Report ===" << std::endl;
    ss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    auto allErrors = getErrors();
    
    // Statistics by severity
    std::map<ErrorSeverity, size_t> severityCounts;
    for (const auto& error : allErrors) {
        severityCounts[error.severity]++;
    }
    
    ss << "Errors by Severity:" << std::endl;
    for (const auto& pair : severityCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Statistics by category
    std::map<ErrorCategory, size_t> categoryCounts;
    for (const auto& error : allErrors) {
        categoryCounts[error.category]++;
    }
    
    ss << "Errors by Category:" << std::endl;
    for (const auto& pair : categoryCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Most common error
    if (!allErrors.empty()) {
        std::map<std::string, size_t> errorMessageCounts;
        for (const auto& error : allErrors) {
            errorMessageCounts[error.message]++;
        }
        
        std::string mostCommonMessage;
        size_t maxCount = 0;
        for (const auto& pair : errorMessageCounts) {
            if (pair.second > maxCount) {
                mostCommonMessage = pair.first;
                maxCount = pair.second;
            }
        }
        
        ss << "Most Common Error:" << std::endl;
        ss << "  Message: " << mostCommonMessage << std::endl;
        ss << "  Count: " << maxCount << std::endl;
        ss << "  Severity: " << static_cast<int>(mostCommonMessage.empty() ? ErrorSeverity::Error : ErrorSeverity::Error) << std::endl;
    }
    
    return ss.str();
}

std::string ErrorReporting::generateErrorSummaryReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Error Summary ===" << std::endl;
    ss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    auto allErrors = getErrors();
    
    ss << "Total Errors: " << allErrors.size() << std::endl;
    ss << "Resolved Errors: " << std::count_if(allErrors.begin(), allErrors.end(),
                                              [](const ErrorReport& e) { return e.isResolved; }) << std::endl;
    ss << "Unresolved Errors: " << std::count_if(allErrors.begin(), allErrors.end(),
                                                [](const ErrorReport& e) { return !e.isResolved; }) << std::endl;
    
    if (!allErrors.empty()) {
        ss << "Last Error: " << std::chrono::system_clock::to_time_t(allErrors.back().timestamp) << std::endl;
        ss << "Last Error ID: " << allErrors.back().id << std::endl;
    }
    
    return ss.str();
}

std::vector<StackTraceEntry> ErrorReporting::captureStackTrace(size_t maxDepth) const {
    std::vector<StackTraceEntry> stackTrace;
    
    // Simplified stack trace capture - in real implementation, would use std::stacktrace or platform-specific APIs
    stackTrace.emplace_back("Unknown", "unknown", 0);
    
    return stackTrace;
}

std::string ErrorReporting::formatStackTrace(const std::vector<StackTraceEntry>& stackTrace) const {
    std::stringstream ss;
    
    for (const auto& entry : stackTrace) {
        ss << "  " << entry.file << ":" << entry.line << " " << entry.function << std::endl;
    }
    
    return ss.str();
}

ErrorContext ErrorReporting::extractContext() const {
    return ErrorContext(pImpl->lastOperation, pImpl->lastFile, 0, pImpl->lastFunction, pImpl->lastComponent);
}

std::string ErrorReporting::formatErrorContext(const ErrorContext& context) const {
    std::stringstream ss;
    
    ss << "Context:" << std::endl;
    ss << "  Operation: " << context.operation << std::endl;
    ss << "  File: " << context.file << std::endl;
    ss << "  Line: " << context.line << std::endl;
    ss << "  Function: " << context.function << std::endl;
    ss << "  Component: " << context.component << std::endl;
    
    return ss.str();
}

bool ErrorReporting::exportErrorReport(const std::string& filepath, const std::string& format,
                                       const std::vector<std::string>& errorIds) const {
    std::vector<ErrorReport> errors;
    
    if (errorIds.empty()) {
        errors = getErrors();
    } else {
        for (const auto& id : errorIds) {
            const ErrorReport* error = getError(id);
            if (error) {
                errors.push_back(*error);
            }
        }
    }
    
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (format == "json") {
        // JSON export (simplified)
        file << "[{" << std::endl;
        for (size_t i = 0; i < errors.size(); ++i) {
            file << "  {\"id\": \"" << errors[i].id << "\"," << std::endl;
            file << "   \"message\": \"" << errors[i].message << "\"," << std::endl;
            file << "   \"severity\": " << static_cast<int>(errors[i].severity) << std::endl;
            if (i < errors.size() - 1) {
                file << "  },";
            } else {
                file << "  }";
            }
            file << std::endl;
        }
        file << "}]" << std::endl;
    } else {
        // Text format
        file << generateErrorReport("", false);
    }
    
    return true;
}

void ErrorReporting::logError(const ErrorReport& error) const {
    // Console output with color coding
    if (pImpl->useColors) {
        switch (error.severity) {
            case ErrorSeverity::Debug:
                std::cout << "\033[36m"; // Cyan
                break;
            case ErrorSeverity::Info:
                std::cout << "\033[32m"; // Green
                break;
            case ErrorSeverity::Warning:
                std::cout << "\033[33m"; // Yellow
                break;
            case ErrorSeverity::Error:
                std::cout << "\033[31m"; // Red
                break;
            case ErrorSeverity::Critical:
                std::cout << "\033[91m"; // Bright Red
                break;
            case ErrorSeverity::Fatal:
                std::cout << "\033[95m"; // Magenta
                break;
        }
    }
    
    std::cout << "[" << std::chrono::system_clock::to_time_t(error.timestamp) << "] "
              << errorCategoryToString(error.category) << ": " << error.message << std::endl;
    
    if (pImpl->useColors) {
        std::cout << "\033[0m"; // Reset
    }
}

void ErrorReporting::logError(const std::string& errorId) const {
    const ErrorReport* error = getError(errorId);
    if (error) {
        logError(*error);
    }
}

void ErrorReporting::createErrorReport(const std::string& message, ErrorSeverity severity,
                                       ErrorCategory category, const std::string& code,
                                       const ErrorContext& context, const std::vector<StackTraceEntry>& stackTrace,
                                       std::string& errorId) {
    errorId = generateErrorId();
    
    ErrorReport error;
    error.id = errorId;
    error.severity = severity;
    error.category = category;
    error.message = message;
    error.description = "Error: " + message;
    error.code = code;
    error.timestamp = std::chrono::system_clock::now();
    error.context = context;
    error.stackTrace = stackTrace;
    error.isResolved = false;
    
    // Add recovery suggestions
    std::vector<RecoverySuggestion> suggestions = generateRecoverySuggestions(error);
    for (const auto& suggestion : suggestions) {
        error.recoverySuggestions.push_back(suggestion);
    }
    
    pImpl->errorRegistry[errorId] = error;
    pImpl->lastErrorTime = error.timestamp;
}

std::string ErrorReporting::generateErrorId() const {
    // Generate a unique error ID
    static std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::uniform_int_distribution<int> dist(100000, 999999);
    return "ERR_" + std::to_string(dist(rng));
}

std::string ErrorReporting::extractErrorCode(const std::string& message) const {
    // Extract error code from message (simplified)
    size_t pos = message.find("ERROR_");
    if (pos != std::string::npos) {
        size_t end = message.find(" ", pos);
        return message.substr(pos, end - pos);
    }
    return "";
}

std::string ErrorReporting::extractErrorMessage(const std::exception& e) const {
    return e.what();
}

size_t ErrorReporting::getErrorCount(ErrorSeverity severity) const {
    size_t count = 0;
    for (const auto& pair : pImpl->errorRegistry) {
        if (pair.second.severity == severity || severity == ErrorSeverity::Error) {
            count++;
        }
    }
    return count;
}

double ErrorReporting::getErrorRate(ErrorSeverity severity) const {
    // Simplified error rate calculation
    return static_cast<double>(getErrorCount(severity)) / 100.0; // Placeholder
}

std::string ErrorReporting::getMostCommonErrorCategory() const {
    std::map<ErrorCategory, size_t> categoryCounts;
    for (const auto& pair : pImpl->errorRegistry) {
        categoryCounts[pair.second.category]++;
    }
    
    if (categoryCounts.empty()) {
        return "Unknown";
    }
    
    ErrorCategory mostCommon = ErrorCategory::Unknown;
    size_t maxCount = 0;
    for (const auto& pair : categoryCounts) {
        if (pair.second > maxCount) {
            mostCommon = pair.first;
            maxCount = pair.second;
        }
    }
    
    return errorCategoryToString(mostCommon);
}

std::chrono::system_clock::time_point ErrorReporting::getLastErrorTime() const {
    return pImpl->lastErrorTime;
}

void ErrorReporting::formatErrorReport(std::stringstream& ss, const ErrorReport& error, bool includeStackTrace) const {
    ss << "=== Error Report ===" << std::endl;
    ss << "Error ID: " << error.id << std::endl;
    ss << "Timestamp: " << std::chrono::system_clock::to_time_t(error.timestamp) << std::endl;
    ss << "Severity: " << static_cast<int>(error.severity) << std::endl;
    ss << "Category: " << errorCategoryToString(error.category) << std::endl;
    ss << "Message: " << error.message << std::endl;
    ss << "Description: " << error.description << std::endl;
    if (!error.code.empty()) {
        ss << "Code: " << error.code << std::endl;
    }
    
    if (pImpl->includeContext && !error.context.operation.empty()) {
        ss << std::endl << "Context:" << std::endl;
        ss << "  Operation: " << error.context.operation << std::endl;
        ss << "  File: " << error.context.file << std::endl;
        ss << "  Line: " << error.context.line << std::endl;
        ss << "  Function: " << error.context.function << std::endl;
        ss << "  Component: " << error.context.component << std::endl;
    }
    
    if (includeStackTrace && !error.stackTrace.empty()) {
        ss << std::endl << "Stack Trace:" << std::endl;
        ss << formatStackTrace(error.stackTrace);
    }
    
    if (pImpl->showRecoverySuggestions && !error.recoverySuggestions.empty()) {
        ss << std::endl << "Recovery Suggestions:" << std::endl;
        for (const auto& suggestion : error.recoverySuggestions) {
            ss << "  " << suggestion.action << ": " << suggestion.description << std::endl;
        }
    }
    
    ss << std::endl << "Status: " << (error.isResolved ? "Resolved" : "Unresolved") << std::endl;
}

} // namespace nlm
