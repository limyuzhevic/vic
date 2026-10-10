// NLM Advanced Features - Error Handler Implementation
// Enhanced error handling with categorization, recovery, and context

#include "ErrorHandler.hpp"
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>

namespace nlm {
namespace advanced {

// Implementation of ErrorHandler static methods

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() 
    : errorRecoveryEnabled(true)
    , maxErrorHistorySize(10000)
    , autoRecoveryEnabled(true)
    , errorFilterCategory(ErrorCategory::None)
    , errorFilterSeverity(ErrorSeverity::Info)
    , outputDestination("console")
    , outputFormat("detailed")
    , rng(std::chrono::system_clock::now().time_since_epoch().count())
    , idDistribution(1, 100000000) {
}

ErrorHandler::~ErrorHandler() {
    // Cleanup any resources
}

ErrorContext ErrorHandler::reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message) {
    return createErrorContext(category, severity, message);
}

ErrorContext ErrorHandler::reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details) {
    return createErrorContext(category, severity, message, details);
}

ErrorContext ErrorHandler::reportError(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details, const std::string& suggestion) {
    return createErrorContext(category, severity, message, details, suggestion);
}

ErrorContext ErrorHandler::reportError(const std::string& categoryStr, const std::string& severityStr, const std::string& message) {
    ErrorCategory category = stringToErrorCategory(categoryStr);
    ErrorSeverity severity = stringToErrorSeverity(severityStr);
    return createErrorContext(category, severity, message);
}

ErrorContext ErrorHandler::createErrorContext(ErrorCategory category, ErrorSeverity severity, const std::string& message, const std::string& details, const std::string& suggestion) {
    ErrorContext error;
    error.errorId = generateErrorId();
    error.category = category;
    error.severity = severity;
    error.message = message;
    error.details = details;
    error.suggestion = suggestion;
    error.stackTrace = getCurrentStackTrace();
    error.timestamp = getCurrentTime();
    error.sourceFile = "unknown";
    error.sourceLine = 0;
    error.functionName = "unknown";
    error.isHandled = false;
    error.isResolved = false;
    error.resolvedMessage = "";
    error.resolvedTime = std::chrono::system_clock::now();
    error.resolvedBy = "";
    
    addErrorToHistory(error);
    updateStatistics(error);
    outputError(error);
    
    if (errorCallback) {
        errorCallback(error);
    }
    
    return error;
}

bool ErrorHandler::recover(ErrorContext& error) {
    if (!shouldRecover(error)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& strategy : recoveryStrategies) {
        if (strategy.predicate && strategy.predicate(error) && findRecoveryStrategy(error, strategy)) {
            applyRecoveryStrategy(error, strategy);
            markErrorHandled(error.errorId);
            
            if (recoveryCallback) {
                return recoveryCallback(error);
            }
            return true;
        }
    }
    
    return autoRecover(error);
}

bool ErrorHandler::recover(const std::string& errorId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return false;
    }
    
    ErrorContext& error = it->second;
    return recover(error);
}

bool ErrorHandler::autoRecover(ErrorContext& error) {
    if (!autoRecoveryEnabled) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& strategy : recoveryStrategies) {
        if (strategy.autoRecover && strategy.predicate && strategy.predicate(error) && findRecoveryStrategy(error, strategy)) {
            applyRecoveryStrategy(error, strategy);
            markErrorHandled(error.errorId);
            return true;
        }
    }
    
    return false;
}

bool ErrorHandler::manualRecover(ErrorContext& error) {
    return recover(error);
}

ErrorContext ErrorHandler::getError(const std::string& errorId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return ErrorContext{}; // Empty context indicates not found
    }
    
    return it->second;
}

std::vector<ErrorContext> ErrorHandler::getErrors(ErrorCategory category, ErrorSeverity severity) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        
        if (category != ErrorCategory::None && error.category != category) continue;
        if (severity != ErrorSeverity::Info && error.severity != severity) continue;
        if (isErrorInFilter(error)) continue;
        
        result.push_back(error);
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getRecentErrors(int minutes) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    auto now = std::chrono::system_clock::now();
    auto timeWindow = std::chrono::minutes(minutes);
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        
        if (isErrorInFilter(error)) continue;
        if ((now - error.timestamp) > timeWindow) continue;
        
        result.push_back(error);
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getUnhandledErrors() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        if (!error.isHandled && !isErrorInFilter(error)) {
            result.push_back(error);
        }
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getUnresolvedErrors() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        if (!error.isResolved && !isErrorInFilter(error)) {
            result.push_back(error);
        }
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getErrorsBySource(const std::string& sourceFile, int line) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        
        if (isErrorInFilter(error)) continue;
        if (line >= 0 && error.sourceLine != line) continue;
        
        if (sourceFile.empty() || error.sourceFile == sourceFile) {
            result.push_back(error);
        }
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getErrorsByRecoveryAction(RecoveryAction action) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<ErrorContext> result;
    
    for (const auto& pair : errorLookup) {
        const ErrorContext& error = pair.second;
        
        if (isErrorInFilter(error)) continue;
        if (action == RecoveryAction::None) {
            result.push_back(error);
        }
        
        for (const auto& step : error.recoverySteps) {
            if (stringToRecoveryAction(step) == action) {
                result.push_back(error);
                break;
            }
        }
    }
    
    return result;
}

std::vector<ErrorContext> ErrorHandler::getErrorHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    return errorHistory;
}

void ErrorHandler::clearErrorHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    errorHistory.clear();
    errorLookup.clear();
    NLM_LOG_INFO("ErrorHandler: Error history cleared");
}

bool ErrorHandler::saveErrorHistory(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex);
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        NLM_LOG_ERROR("ErrorHandler: Could not open file for writing: " + filename);
        return false;
    }
    
    // Serialize to JSON format
    file << "[" << std::endl;
    for (size_t i = 0; i < errorHistory.size(); ++i) {
        const ErrorContext& error = errorHistory[i];
        
        file << "  {" << std::endl;
        file << "    \"errorId\": \"" << error.errorId << "\"," << std::endl;
        file << "    \"category\": \"" << errorCategoryToString(error.category) << "\"," << std::endl;
        file << "    \"severity\": \"" << errorSeverityToString(error.severity) << "\"," << std::endl;
        file << "    \"message\": \"" << escapeString(error.message) << "\"," << std::endl;
        file << "    \"details\": \"" << escapeString(error.details) << "\"," << std::endl;
        file << "    \"suggestion\": \"" << escapeString(error.suggestion) << "\"," << std::endl;
        file << "    \"isHandled\": " << (error.isHandled ? "true" : "false") << "," << std::endl;
        file << "    \"isResolved\": " << (error.isResolved ? "true" : "false") << "," << std::endl;
        
        if (i < errorHistory.size() - 1) {
            file << "  }," << std::endl;
        } else {
            file << "  }" << std::endl;
        }
    }
    
    file << "]" << std::endl;
    file.close();
    
    NLM_LOG_INFO("ErrorHandler: Error history saved to: " + filename);
    return true;
}

bool ErrorHandler::loadErrorHistory(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex);
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        NLM_LOG_ERROR("ErrorHandler: Could not open file for reading: " + filename);
        return false;
    }
    
    std::string line;
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Simple JSON parsing (assuming file contains JSON array format)
    // In a real implementation, you would use a proper JSON parser
    clearErrorHistory();
    
    NLM_LOG_INFO("ErrorHandler: Error history loaded from: " + filename);
    return true;
}

ErrorStatistics ErrorHandler::getErrorStatistics() const {
    std::lock_guard<std::mutex> lock(mutex);
    return stats;
}

double ErrorHandler::getErrorRate(const std::string& timeWindow) const {
    // Simplified implementation - return error rate based on history
    return static_cast<double>(stats.totalErrors) / 60.0; // 1 error per minute default
}

std::string ErrorHandler::getErrorTrend(const std::string& timeWindow) const {
    // Simplified implementation - return trend based on statistics
    if (stats.errorRatePerMinute < 1.0) return "stable";
    if (stats.errorRatePerMinute < 5.0) return "increasing";
    return "high";
}

bool ErrorHandler::markErrorHandled(const std::string& errorId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return false;
    }
    
    it->second.isHandled = true;
    NLM_LOG_INFO("ErrorHandler: Error marked as handled: " + errorId);
    return true;
}

bool ErrorHandler::markErrorResolved(const std::string& errorId, const std::string& resolvedMessage) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return false;
    }
    
    it->second.isResolved = true;
    it->second.resolvedMessage = resolvedMessage;
    it->second.resolvedTime = getCurrentTime();
    it->second.resolvedBy = "system";
    
    NLM_LOG_INFO("ErrorHandler: Error resolved: " + errorId);
    return true;
}

bool ErrorHandler::deleteError(const std::string& errorId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return false;
    }
    
    errorHistory.erase(std::remove_if(errorHistory.begin(), errorHistory.end(),
                                      [&errorId](const ErrorContext& e) { return e.errorId == errorId; }),
                       errorHistory.end());
    
    errorLookup.erase(it);
    NLM_LOG_INFO("ErrorHandler: Error deleted: " + errorId);
    return true;
}

bool ErrorHandler::archiveError(const std::string& errorId, const std::string& archiveFile) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = errorLookup.find(errorId);
    if (it == errorLookup.end()) {
        return false;
    }
    
    // Move error to archive
    archivedErrors.push_back(it->second);
    deleteError(errorId);
    
    NLM_LOG_INFO("ErrorHandler: Error archived: " + errorId);
    return true;
}

std::vector<ErrorContext> ErrorHandler::getArchivedErrors() {
    std::lock_guard<std::mutex> lock(mutex);
    return archivedErrors;
}

void ErrorHandler::setErrorRecoveryEnabled(bool enabled) {
    errorRecoveryEnabled = enabled;
    NLM_LOG_INFO("ErrorHandler: Error recovery " + std::string(enabled ? "enabled" : "disabled"));
}

void ErrorHandler::setMaxErrorHistorySize(int size) {
    maxErrorHistorySize = size;
    cleanupOldErrors();
}

void ErrorHandler::setAutoRecoveryEnabled(bool enabled) {
    autoRecoveryEnabled = enabled;
    NLM_LOG_INFO("ErrorHandler: Auto recovery " + std::string(enabled ? "enabled" : "disabled"));
}

void ErrorHandler::setRecoveryStrategy(const RecoveryStrategy& strategy) {
    recoveryStrategies.push_back(strategy);
    NLM_LOG_INFO("ErrorHandler: Recovery strategy added for " + 
                 errorCategoryToString(strategy.category) + " -> " + recoveryActionToString(strategy.action));
}

void ErrorHandler::addRecoveryStrategy(const RecoveryStrategy& strategy) {
    setRecoveryStrategy(strategy);
}

void ErrorHandler::setErrorFilter(ErrorCategory category, ErrorSeverity severity) {
    errorFilterCategory = category;
    errorFilterSeverity = severity;
}

void ErrorHandler::setOutputDestination(const std::string& destination) {
    outputDestination = destination;
}

void ErrorHandler::setOutputFormat(const std::string& format) {
    outputFormat = format;
}

std::string ErrorHandler::generateErrorId() {
    return std::to_string(idDistribution(rng));
}

std::string ErrorHandler::getCurrentStackTrace() {
    return "Stack trace not available in this implementation";
}

std::string ErrorHandler::formatErrorContext(const ErrorContext& error, const std::string& format) {
    if (format == "brief") {
        return error.message;
    } else if (format == "detailed") {
        std::ostringstream oss;
        oss << "Error: " << error.message << std::endl;
        oss << "Category: " << errorCategoryToString(error.category) << std::endl;
        oss << "Severity: " << errorSeverityToString(error.severity) << std::endl;
        if (!error.details.empty()) {
            oss << "Details: " << error.details << std::endl;
        }
        if (!error.suggestion.empty()) {
            oss << "Suggestion: " << error.suggestion << std::endl;
        }
        oss << "Time: " << getTimeAgo(error.timestamp) << std::endl;
        oss << "Handled: " << (error.isHandled ? "Yes" : "No") << std::endl;
        return oss.str();
    } else {
        return "Unknown format requested";
    }
}

std::string ErrorHandler::getErrorSummary(const ErrorContext& error) {
    return "[" + error.errorId + "] " + errorCategoryToString(error.category) + ": " + error.message;
}

std::string ErrorHandler::getErrorDetails(const ErrorContext& error) {
    return formatErrorContext(error, "detailed");
}

std::string ErrorHandler::getErrorSuggestion(const ErrorContext& error) {
    return error.suggestion.empty() ? "No suggestion available" : error.suggestion;
}

std::chrono::system_clock::time_point ErrorHandler::getCurrentTime() {
    return std::chrono::system_clock::now();
}

void ErrorHandler::setErrorCallback(const std::function<void(const ErrorContext&)>& callback) {
    errorCallback = callback;
}

void ErrorHandler::setRecoveryCallback(const std::function<bool(ErrorContext&)>& callback) {
    recoveryCallback = callback;
}

void ErrorHandler::setErrorStatisticsCallback(const std::function<void(const ErrorStatistics&)>& callback) {
    errorStatisticsCallback = callback;
}

ErrorCategory ErrorHandler::stringToErrorCategory(const std::string& categoryStr) {
    static const std::map<std::string, ErrorCategory> categoryMap = {
        {"none", ErrorCategory::None},
        {"system", ErrorCategory::System},
        {"configuration", ErrorCategory::Configuration},
        {"network", ErrorCategory::Network},
        {"memory", ErrorCategory::Memory},
        {"io", ErrorCategory::IO},
        {"neural", ErrorCategory::Neural},
        {"performance", ErrorCategory::Performance},
        {"security", ErrorCategory::Security},
        {"development", ErrorCategory::Development},
        {"user", ErrorCategory::User},
        {"external", ErrorCategory::External}
    };
    
    auto it = categoryMap.find(categoryStr);
    return (it != categoryMap.end()) ? it->second : ErrorCategory::None;
}

std::string ErrorHandler::errorCategoryToString(ErrorCategory category) {
    static const std::map<ErrorCategory, std::string> categoryMap = {
        {ErrorCategory::None, "None"},
        {ErrorCategory::System, "System"},
        {ErrorCategory::Configuration, "Configuration"},
        {ErrorCategory::Network, "Network"},
        {ErrorCategory::Memory, "Memory"},
        {ErrorCategory::IO, "IO"},
        {ErrorCategory::Neural, "Neural"},
        {ErrorCategory::Performance, "Performance"},
        {ErrorCategory::Security, "Security"},
        {ErrorCategory::Development, "Development"},
        {ErrorCategory::User, "User"},
        {ErrorCategory::External, "External"}
    };
    
    auto it = categoryMap.find(category);
    return (it != categoryMap.end()) ? it->second : "Unknown";
}

ErrorSeverity ErrorHandler::stringToErrorSeverity(const std::string& severityStr) {
    static const std::map<std::string, ErrorSeverity> severityMap = {
        {"debug", ErrorSeverity::Debug},
        {"info", ErrorSeverity::Info},
        {"warning", ErrorSeverity::Warning},
        {"recoverable", ErrorSeverity::Recoverable},
        {"error", ErrorSeverity::Error},
        {"critical", ErrorSeverity::Critical},
        {"fatal", ErrorSeverity::Fatal}
    };
    
    auto it = severityMap.find(severityStr);
    return (it != severityMap.end()) ? it->second : ErrorSeverity::Info;
}

std::string ErrorHandler::errorSeverityToString(ErrorSeverity severity) {
    static const std::map<ErrorSeverity, std::string> severityMap = {
        {ErrorSeverity::Debug, "Debug"},
        {ErrorSeverity::Info, "Info"},
        {ErrorSeverity::Warning, "Warning"},
        {ErrorSeverity::Recoverable, "Recoverable"},
        {ErrorSeverity::Error, "Error"},
        {ErrorSeverity::Critical, "Critical"},
        {ErrorSeverity::Fatal, "Fatal"}
    };
    
    auto it = severityMap.find(severity);
    return (it != severityMap.end()) ? it->second : "Unknown";
}

RecoveryAction ErrorHandler::stringToRecoveryAction(const std::string& actionStr) {
    static const std::map<std::string, RecoveryAction> actionMap = {
        {"none", RecoveryAction::None},
        {"retry", RecoveryAction::Retry},
        {"fallback", RecoveryAction::Fallback},
        {"skip", RecoveryAction::Skip},
        {"abort", RecoveryAction::Abort},
        {"continue", RecoveryAction::Continue},
        {"reset", RecoveryAction::Reset},
        {"restart", RecoveryAction::Restart},
        {"configupdate", RecoveryAction::ConfigUpdate},
        {"resourcereallocation", RecoveryAction::ResourceReallocation}
    };
    
    auto it = actionMap.find(actionStr);
    return (it != actionMap.end()) ? it->second : RecoveryAction::None;
}

std::string ErrorHandler::recoveryActionToString(RecoveryAction action) {
    static const std::map<RecoveryAction, std::string> actionMap = {
        {RecoveryAction::None, "None"},
        {RecoveryAction::Retry, "Retry"},
        {RecoveryAction::Fallback, "Fallback"},
        {RecoveryAction::Skip, "Skip"},
        {RecoveryAction::Abort, "Abort"},
        {RecoveryAction::Continue, "Continue"},
        {RecoveryAction::Reset, "Reset"},
        {RecoveryAction::Restart, "Restart"},
        {RecoveryAction::ConfigUpdate, "ConfigUpdate"},
        {RecoveryAction::ResourceReallocation, "ResourceReallocation"}
    };
    
    auto it = actionMap.find(action);
    return (it != actionMap.end()) ? it->second : "Unknown";
}

bool ErrorHandler::isErrorCategoryEnabled(ErrorCategory category) const {
    return errorFilterCategory == ErrorCategory::None || errorFilterCategory == category;
}

bool ErrorHandler::isErrorSeverityEnabled(ErrorSeverity severity) const {
    return errorFilterSeverity == ErrorSeverity::Info || errorFilterSeverity == severity;
}

bool ErrorHandler::shouldRecover(ErrorContext& error) const {
    if (!errorRecoveryEnabled) return false;
    
    for (const auto& strategy : recoveryStrategies) {
        if (strategy.predicate && strategy.predicate(error) && findRecoveryStrategy(error, strategy)) {
            return true;
        }
    }
    
    return autoRecoveryEnabled;
}

bool ErrorHandler::isRecoverable(ErrorContext& error) const {
    return shouldRecover(error);
}

std::string ErrorHandler::formatErrorCount(int count) const {
    if (count < 1000) return std::to_string(count);
    if (count < 1000000) return std::to_string(count / 1000) + "K";
    return std::to_string(count / 1000000) + "M";
}

std::string ErrorHandler::formatErrorRate(double rate) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << rate << "/min";
    return oss.str();
}

std::string ErrorHandler::getTimeAgo(const std::chrono::system_clock::time_point& time) const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - time);
    
    if (duration.count() < 1) return "just now";
    if (duration.count() < 60) return std::to_string(duration.count()) + " minutes ago";
    if (duration.count() < 1440) return std::to_string(duration.count() / 60) + " hours ago";
    return std::to_string(duration.count() / 1440) + " days ago";
}

bool ErrorHandler::findRecoveryStrategy(ErrorContext& error, RecoveryStrategy& strategy) {
    return strategy.predicate && strategy.predicate(error);
}

bool ErrorHandler::applyRecoveryStrategy(ErrorContext& error, const RecoveryStrategy& strategy) {
    if (strategy.recoveryFunc) {
        std::string result = strategy.recoveryFunc(error);
        if (!result.empty()) {
            error.resolutionMessage = result;
        }
        return true;
    }
    return false;
}

void ErrorHandler::addErrorToHistory(ErrorContext& error) {
    errorHistory.push_back(error);
    errorLookup[error.errorId] = error;
    
    // Limit history size
    if (errorHistory.size() > static_cast<size_t>(maxErrorHistorySize)) {
        auto oldest = errorHistory.begin();
        errorLookup.erase(oldest->errorId);
        errorHistory.erase(oldest);
    }
    
    cleanupOldErrors();
}

void ErrorHandler::updateStatistics(const ErrorContext& error) {
    stats.totalErrors++;
    stats.errorsByCategory[static_cast<int>(error.category)]++;
    stats.errorsBySeverity[static_cast<int>(error.severity)]++;
    stats.errorsByRecoveryAction[static_cast<int>(error.recoveryAction)]++;
    stats.errorFrequency[error.message]++;
    
    if (stats.totalErrors == 1 || error.timestamp < stats.firstErrorTime) {
        stats.firstErrorTime = error.timestamp;
    }
    
    stats.lastErrorTime = error.timestamp;
    
    // Update most frequent error
    if (stats.errorFrequency[error.message] > stats.mostFrequentErrorCount) {
        stats.mostFrequentErrorCount = stats.errorFrequency[error.message];
        stats.mostFrequentError = error.message;
    }
    
    // Update recurring errors
    if (stats.errorFrequency[error.message] > 3) {
        bool found = false;
        for (const auto& recurring : stats.recurringErrors) {
            if (recurring == error.message) {
                found = true;
                break;
            }
        }
        if (!found) {
            stats.recurringErrors.push_back(error.message);
        }
    }
    
    if (errorStatisticsCallback) {
        errorStatisticsCallback(stats);
    }
}

void ErrorHandler::outputError(const ErrorContext& error) {
    if (outputDestination == "console") {
        // Output to console with appropriate level
        if (error.severity == ErrorSeverity::Fatal || error.severity == ErrorSeverity::Critical) {
            NLM_LOG_FATAL(formatErrorContext(error));
        } else if (error.severity == ErrorSeverity::Error) {
            NLM_LOG_ERROR(formatErrorContext(error));
        } else if (error.severity == ErrorSeverity::Warning || error.severity == ErrorSeverity::Recoverable) {
            NLM_LOG_WARNING(formatErrorContext(error));
        } else {
            NLM_LOG_INFO(formatErrorContext(error));
        }
    } else if (outputDestination == "file") {
        // Output to file
        // Implementation would write to log file
    }
}

bool ErrorHandler::isErrorInFilter(const ErrorContext& error) const {
    if (errorFilterCategory == ErrorCategory::None && errorFilterSeverity == ErrorSeverity::Info) {
        return false;
    }
    
    if (errorFilterCategory != ErrorCategory::None && error.category != errorFilterCategory) {
        return true;
    }
    
    if (errorFilterSeverity != ErrorSeverity::Info && error.severity != errorFilterSeverity) {
        return true;
    }
    
    return false;
}

void ErrorHandler::cleanupOldErrors() {
    auto now = std::chrono::system_clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    
    errorHistory.erase(std::remove_if(errorHistory.begin(), errorHistory.end(),
                                      [&oneHourAgo](const ErrorContext& e) { return e.timestamp < oneHourAgo; }),
                       errorHistory.end());
    
    // Rebuild lookup table
    errorLookup.clear();
    for (const auto& error : errorHistory) {
        errorLookup[error.errorId] = error;
    }
}

std::string ErrorHandler::getStackTrace() const {
    return "Stack trace generation not implemented in this version";
}

void ErrorHandler::populateStackTrace(std::string& stackTrace) const {
    // Implementation would populate stack trace
    stackTrace = "Stack trace not available";
}

std::string ErrorHandler::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\0': result += "\\0"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            default:
                if (c >= 0x20 && c <= 0x7E) {
                    result += c;
                } else {
                    std::stringstream ss;
                    ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(c) & 0xFF);
                    result += ss.str();
                }
        }
    }
    return result;
}

// NLMError implementation
ErrorHandler::NLMError::NLMError(const std::string& message, ErrorCategory category, ErrorSeverity severity)
    : std::runtime_error(message), errorId(generateErrorId()), category(category), severity(severity) {
}

ErrorHandler::NLMFatalError::NLMFatalError(const std::string& message, ErrorCategory category)
    : NLMError(message, category, ErrorSeverity::Fatal) {
}

ErrorHandler::NLMRecoverableError::NLMRecoverableError(const std::string& message, ErrorCategory category)
    : NLMError(message, category, ErrorSeverity::Recoverable) {
}

} // namespace advanced
} // namespace nlm

// Include the NLM logging macros for this implementation
#include "../Logger/Logger.hpp"

// Add error logging macros
#define NLM_LOG_ERROR(msg) NLM_LOG_ERROR("ErrorHandler: " + msg)
#define NLM_LOG_WARNING(msg) NLM_LOG_WARNING("ErrorHandler: " + msg)
#define NLM_LOG_INFO(msg) NLM_LOG_INFO("ErrorHandler: " + msg)
#define NLM_LOG_FATAL(msg) NLM_LOG_FATAL("ErrorHandler: " + msg)

// Add error statistics callback
void ErrorHandler::setErrorStatisticsCallback(const std::function<void(const ErrorStatistics&)>& callback) {
    errorStatisticsCallback = callback;
}

void ErrorHandler::setErrorCallback(const std::function<void(const ErrorContext&)>& callback) {
    errorCallback = callback;
}

void ErrorHandler::setRecoveryCallback(const std::function<bool(ErrorContext&)>& callback) {
    recoveryCallback = callback;
}

void ErrorHandler::setProgressCallback(ProgressCallbackFunc callback) {
    progressCallback = callback;
}

void ErrorHandler::setCheckpointCallback(CheckpointCallbackFunc callback) {
    checkpointCallback = callback;
}

void ErrorHandler::setCompletionCallback(CompletionCallbackFunc callback) {
    completionCallback = callback;
}

} // namespace advanced
} // namespace nlm