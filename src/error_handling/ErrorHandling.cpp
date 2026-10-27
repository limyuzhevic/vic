// Error Handling and User Feedback System Implementation
// Implementation of comprehensive error handling and user feedback for NLM

#include "error_handling/ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace nlm;

// Global error handling instances
UserFeedbackSystem gUserFeedbackSystem;
ErrorRecoverySystem gErrorRecoverySystem;
ErrorReportingSystem gErrorReportingSystem;

// UserFeedbackSystem implementation
struct UserFeedbackSystem::Impl {
    FeedbackLevel feedbackLevel;
    std::vector<std::shared_ptr<IErrorHandler>> errorHandlers;
    std::vector<UserFeedback> feedbackHistory;
    std::vector<NLMError> errorHistory;
    
    bool consoleOutputEnabled;
    std::string logFilename;
    size_t maxLogFileSizeMB;
    size_t maxLogFiles;
    
    // Statistics
    size_t totalErrors;
    size_t errorsBySeverity[6]; // DEBUG, INFO, WARNING, ERROR, CRITICAL, FATAL
    size_t errorsByCategory[20]; // All ErrorCategory values
    
    // Performance
    std::chrono::high_resolution_clock::time_point startTime;
};

UserFeedbackSystem::UserFeedbackSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->feedbackLevel = FeedbackLevel::INFO;
    pImpl->consoleOutputEnabled = true;
    pImpl->maxLogFileSizeMB = 10;
    pImpl->maxLogFiles = 5;
    pImpl->totalErrors = 0;
    std::fill(std::begin(pImpl->errorsBySeverity), std::end(pImpl->errorsBySeverity), 0);
    std::fill(std::begin(pImpl->errorsByCategory), std::end(pImpl->errorsByCategory), 0);
    
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

UserFeedbackSystem::~UserFeedbackSystem() = default;

void UserFeedbackSystem::setFeedbackLevel(FeedbackLevel level) {
    pImpl->feedbackLevel = level;
}

FeedbackLevel UserFeedbackSystem::getFeedbackLevel() const {
    return pImpl->feedbackLevel;
}

void UserFeedbackSystem::addErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    pImpl->errorHandlers.push_back(handler);
}

UserAction UserFeedbackSystem::handleError(const NLMError& error) {
    // Report error to all handlers
    for (const auto& handler : pImpl->errorHandlers) {
        handler->logError(error);
        if (pImpl->feedbackLevel >= FeedbackLevel::ERROR_ONLY) {
            handler->displayError(error);
        }
    }
    
    // Determine user action
    UserAction userAction = UserAction::CONTINUE;
    for (const auto& handler : pImpl->errorHandlers) {
        UserAction action = handler->getUserAction(error);
        if (action != UserAction::CONTINUE) {
            userAction = action;
            break;
        }
    }
    
    // Create user feedback
    UserFeedback feedback;
    feedback.message = error.getFormattedMessage();
    feedback.severity = error.getSeverity();
    feedback.level = pImpl->feedbackLevel;
    feedback.action = userAction;
    feedback.timestamp = error.getTimestamp();
    feedback.requiresUserInput = (userAction != UserAction::CONTINUE);
    
    // Process feedback
    processFeedback(feedback);
    
    // Update error history
    pImpl->errorHistory.push_back(error);
    pImpl->feedbackHistory.push_back(feedback);
    pImpl->totalErrors++;
    
    // Update statistics
    size_t severityIndex = static_cast<size_t>(error.getSeverity());
    if (severityIndex < 6) {
        pImpl->errorsBySeverity[severityIndex]++;
    }
    
    size_t categoryIndex = static_cast<size_t>(error.getType().category);
    if (categoryIndex < 20) {
        pImpl->errorsByCategory[categoryIndex]++;
    }
    
    return userAction;
}

void UserFeedbackSystem::reportError(const NLMError& error) {
    if (pImpl->feedbackLevel < FeedbackLevel::ERROR_ONLY) {
        return;
    }
    
    UserAction action = handleError(error);
    
    if (action == UserAction::STOP || action == UserAction::CANCEL) {
        std::cerr << "Error requires stopping: " << error.what() << std::endl;
        std::exit(1);
    }
}

std::vector<UserFeedback> UserFeedbackSystem::getFeedbackHistory() const {
    return pImpl->feedbackHistory;
}

std::vector<NLMError> UserFeedbackSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void UserFeedbackSystem::clearHistory() {
    pImpl->feedbackHistory.clear();
    pImpl->errorHistory.clear();
    pImpl->totalErrors = 0;
    std::fill(std::begin(pImpl->errorsBySeverity), std::end(pImpl->errorsBySeverity), 0);
    std::fill(std::begin(pImpl->errorsByCategory), std::end(pImpl->errorsByCategory), 0);
}

void UserFeedbackSystem::enableConsoleOutput(bool enable) {
    pImpl->consoleOutputEnabled = enable;
}

void UserFeedbackSystem::enableFileOutput(const std::string& filename) {
    pImpl->logFilename = filename;
}

void UserFeedbackSystem::enableLogRotation(size_t maxSizeMB, size_t maxFiles) {
    pImpl->maxLogFileSizeMB = maxSizeMB;
    pImpl->maxLogFiles = maxFiles;
}

size_t UserFeedbackSystem::getTotalErrors() const {
    return pImpl->totalErrors;
}

size_t UserFeedbackSystem::getErrorsBySeverity(ErrorSeverity severity) const {
    size_t index = static_cast<size_t>(severity);
    if (index < 6) {
        return pImpl->errorsBySeverity[index];
    }
    return 0;
}

double UserFeedbackSystem::getErrorRate() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->startTime).count();
    
    if (duration <= 0) return 0.0;
    
    return static_cast<double>(pImpl->totalErrors) / duration;
}

void UserFeedbackSystem::processFeedback(const UserFeedback& feedback) {
    if (pImpl->feedbackLevel >= feedback.level) {
        if (pImpl->consoleOutputEnabled) {
            displayError(NLMError(feedback.message));
        }
        
        if (!pImpl->logFilename.empty()) {
            writeToFile(feedback);
        }
    }
}

void UserFeedbackSystem::writeToFile(const UserFeedback& feedback) {
    std::ofstream file(pImpl->logFilename, std::ios::app);
    if (!file.is_open()) {
        return;
    }
    
    file << formatFeedback(feedback) << "\n";
    file.close();
    
    // Check for log rotation
    if (std::filesystem::exists(pImpl->logFilename) && 
        std::filesystem::file_size(pImpl->logFilename) > pImpl->maxLogFileSizeMB * 1024 * 1024) {
        // Rotate logs (simplified implementation)
    }
}

std::string UserFeedbackSystem::formatFeedback(const UserFeedback& feedback) const {
    std::stringstream ss;
    ss << "[" << feedback.timestamp << "] " << getFeedbackLevelString(feedback.level) << ": ";
    ss << feedback.message;
    if (feedback.requiresUserInput) {
        ss << " [Requires user action]";
    }
    return ss.str();
}

void UserFeedbackSystem::displayError(const NLMError& error) {
    std::cerr << error.what() << std::endl;
}

std::string UserFeedbackSystem::getFeedbackLevelString(FeedbackLevel level) {
    switch (level) {
        case FeedbackLevel::SILENT: return "SILENT";
        case FeedbackLevel::ERROR_ONLY: return "ERROR";
        case FeedbackLevel::INFO: return "INFO";
        case FeedbackLevel::PROGRESS: return "PROGRESS";
        case FeedbackLevel::DETAILED: return "DETAILED";
        default: return "UNKNOWN";
    }
}

// ErrorRecoverySystem implementation
struct ErrorRecoverySystem::Impl {
    std::map<ErrorCategory, std::function<UserAction(const NLMError&)>> categoryStrategies;
    std::map<ErrorType, std::function<UserAction(const NLMError&)>> typeStrategies;
    
    size_t maxRetries;
    double recoveryTimeout;
    bool autoRecoveryEnabled;
    
    // Statistics
    size_t recoveryAttempts;
    size_t recoveryFailures;
};

ErrorRecoverySystem::ErrorRecoverySystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxRetries = 3;
    pImpl->recoveryTimeout = 30.0;
    pImpl->autoRecoveryEnabled = true;
    pImpl->recoveryAttempts = 0;
    pImpl->recoveryFailures = 0;
}

ErrorRecoverySystem::~ErrorRecoverySystem() = default;

void ErrorRecoverySystem::registerRecoveryStrategy(ErrorCategory category,
                                                    std::function<UserAction(const NLMError&)> strategy) {
    pImpl->categoryStrategies[category] = strategy;
}

void ErrorRecoverySystem::registerRecoveryStrategy(ErrorType type,
                                                    std::function<UserAction(const NLMError&)> strategy) {
    pImpl->typeStrategies[type] = strategy;
}

UserAction ErrorRecoverySystem::handleWithRecovery(const NLMError& error) {
    if (!isRecoverable(error)) {
        return UserAction::CONTINUE;
    }
    
    return attemptRecovery(error);
}

bool ErrorRecoverySystem::isRecoverable(const NLMError& error) const {
    // Check if error is in recovery strategies
    auto categoryIt = pImpl->categoryStrategies.find(error.getType().category);
    if (categoryIt != pImpl->categoryStrategies.end()) {
        return true;
    }
    
    auto typeIt = pImpl->typeStrategies.find(error.getType());
    if (typeIt != pImpl->typeStrategies.end()) {
        return true;
    }
    
    // Check if error severity is recoverable
    if (error.getSeverity() >= ErrorSeverity::WARNING && 
        error.getSeverity() <= ErrorSeverity::ERROR) {
        return true;
    }
    
    return false;
}

void ErrorRecoverySystem::setMaxRetries(size_t maxRetries) {
    pImpl->maxRetries = maxRetries;
}

void ErrorRecoverySystem::setRecoveryTimeout(double timeout) {
    pImpl->recoveryTimeout = timeout;
}

void ErrorRecoverySystem::enableAutomaticRecovery(bool enable) {
    pImpl->autoRecoveryEnabled = enable;
}

UserAction ErrorRecoverySystem::attemptRecovery(const NLMError& error) {
    if (!isRecoverable(error)) {
        return UserAction::CONTINUE;
    }
    
    pImpl->recoveryAttempts++;
    
    // Find appropriate recovery strategy
    std::function<UserAction(const NLMError&)> strategy;
    
    auto typeIt = pImpl->typeStrategies.find(error.getType());
    if (typeIt != pImpl->typeStrategies.end()) {
        strategy = typeIt->second;
    } else {
        auto categoryIt = pImpl->categoryStrategies.find(error.getType().category);
        if (categoryIt != pImpl->categoryStrategies.end()) {
            strategy = categoryIt->second;
        }
    }
    
    if (strategy) {
        try {
            UserAction action = strategy(error);
            if (action == UserAction::CONTINUE) {
                return UserAction::CONTINUE;
            } else {
                pImpl->recoveryFailures++;
                return action;
            }
        } catch (...) {
            pImpl->recoveryFailures++;
            return UserAction::CONTINUE;
        }
    }
    
    pImpl->recoveryFailures++;
    return UserAction::CONTINUE;
}

size_t ErrorRecoverySystem::getRecoveryAttempts() const {
    return pImpl->recoveryAttempts;
}

size_t ErrorRecoverySystem::getRecoveryFailures() const {
    return pImpl->recoveryFailures;
}

double ErrorRecoverySystem::getRecoverySuccessRate() const {
    size_t total = pImpl->recoveryAttempts + pImpl->recoveryFailures;
    if (total == 0) return 0.0;
    return static_cast<double>(pImpl->recoveryAttempts) / total;
}

// ErrorReportingSystem implementation
struct ErrorReportingSystem::Impl {
    std::string endpoint;
    std::string apiKey;
    bool emailReports;
    std::string fromEmail;
    std::vector<std::string> toEmails;
    bool systemLogs;
    
    bool includeSystemInfo;
    bool includeStackTrace;
    bool includePerformanceData;
    
    bool compressionEnabled;
    bool encryptionEnabled;
    
    std::function<bool(const NLMError&)> reportFilter;
    std::vector<ErrorCategory> includeCategories;
    std::vector<ErrorCategory> excludeCategories;
    
    // Statistics
    size_t reportsSent;
    size_t reportsFailed;
};

ErrorReportingSystem::ErrorReportingSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->emailReports = false;
    pImpl->systemLogs = true;
    pImpl->includeSystemInfo = true;
    pImpl->includeStackTrace = false;
    pImpl->includePerformanceData = true;
    pImpl->compressionEnabled = true;
    pImpl->encryptionEnabled = false;
    pImpl->reportsSent = 0;
    pImpl->reportsFailed = 0;
}

ErrorReportingSystem::~ErrorReportingSystem() = default;

void ErrorReportingSystem::sendErrorReport(const NLMError& error) {
    if (reportFilter && !reportFilter(error)) {
        return;
    }
    
    if (!pImpl->includeCategories.empty()) {
        bool include = false;
        ErrorCategory category = error.getType().category;
        for (ErrorCategory incCat : pImpl->includeCategories) {
            if (incCat == category) {
                include = true;
                break;
            }
        }
        if (!include) {
            return;
        }
    }
    
    if (!pImpl->excludeCategories.empty()) {
        ErrorCategory category = error.getType().category;
        for (ErrorCategory excCat : pImpl->excludeCategories) {
            if (excCat == category) {
                return;
            }
        }
    }
    
    sendReport(error);
}

void ErrorReportingSystem::sendBatchErrorReports(const std::vector<NLMError>& errors) {
    for (const auto& error : errors) {
        sendErrorReport(error);
    }
}

void ErrorReportingSystem::setReportEndpoint(const std::string& endpoint) {
    pImpl->endpoint = endpoint;
}

void ErrorReportingSystem::setApiKey(const std::string& apiKey) {
    pImpl->apiKey = apiKey;
}

void ErrorReportingSystem::enableEmailReports(bool enable, const std::string& from,
                                               const std::vector<std::string>& to) {
    pImpl->emailReports = enable;
    pImpl->fromEmail = from;
    pImpl->toEmails = to;
}

void ErrorReportingSystem::enableSystemLogs(bool enable) {
    pImpl->systemLogs = enable;
}

void ErrorReportingSystem::setIncludeSystemInfo(bool include) {
    pImpl->includeSystemInfo = include;
}

void ErrorReportingSystem::setIncludeStackTrace(bool include) {
    pImpl->includeStackTrace = include;
}

void ErrorReportingSystem::setIncludePerformanceData(bool include) {
    pImpl->includePerformanceData = include;
}

void ErrorReportingSystem::setCompressionEnabled(bool enable) {
    pImpl->compressionEnabled = enable;
}

void ErrorReportingSystem::setEncryptionEnabled(bool enable) {
    pImpl->encryptionEnabled = enable;
}

void ErrorReportingSystem::setReportFilter(std::function<bool(const NLMError&)> filter) {
    pImpl->reportFilter = filter;
}

void ErrorReportingSystem::addReportIncludeCategory(ErrorCategory category) {
    pImpl->includeCategories.push_back(category);
}

void ErrorReportingSystem::addReportExcludeCategory(ErrorCategory category) {
    pImpl->excludeCategories.push_back(category);
}

size_t ErrorReportingSystem::getReportsSent() const {
    return pImpl->reportsSent;
}

size_t ErrorReportingSystem::getReportsFailed() const {
    return pImpl->reportsFailed;
}

double ErrorReportingSystem::getReportSuccessRate() const {
    size_t total = pImpl->reportsSent + pImpl->reportsFailed;
    if (total == 0) return 0.0;
    return static_cast<double>(pImpl->reportsSent) / total;
}

void ErrorReportingSystem::sendReport(const NLMError& error) {
    try {
        std::string payload = createReportPayload(error);
        bool success = sendReportToEndpoint(payload);
        
        if (success) {
            pImpl->reportsSent++;
        } else {
            pImpl->reportsFailed++;
        }
        
        // Send email report if enabled
        if (pImpl->emailReports) {
            // Email sending implementation would go here
        }
        
        // Log to system if enabled
        if (pImpl->systemLogs) {
            // System logging implementation would go here
        }
        
    } catch (...) {
        pImpl->reportsFailed++;
    }
}

std::string ErrorReportingSystem::createReportPayload(const NLMError& error) const {
    // Create JSON or other structured report
    std::stringstream payload;
    payload << "{\n";
    payload << "  \"timestamp\": \"" << error.getTimestamp() << "\",\n";
    payload << "  \"severity\": \"" << getSeverityString(error.getSeverity()) << "\",\n";
    payload << "  \"category\": \"" << getCategoryString(error.getType().category) << "\",\n";
    payload << "  \"code\": \"" << error.getType().code << "\",\n";
    payload << "  \"description\": \"" << error.getType().description << "\",\n";
    payload << "  \"details\": \"" << error.getDetails() << "\",\n";
    
    if (pImpl->includeSystemInfo) {
        // Add system information
    }
    
    if (pImpl->includeStackTrace) {
        // Add stack trace
    }
    
    if (pImpl->includePerformanceData) {
        // Add performance data
    }
    
    payload << "}\n";
    return payload.str();
}

bool ErrorReportingSystem::sendReportToEndpoint(const std::string& payload) {
    // HTTP POST request implementation would go here
    // For now, just return true (simulated success)
    return true;
}

std::string ErrorReportingSystem::getSeverityString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::DEBUG: return "DEBUG";
        case ErrorSeverity::INFO: return "INFO";
        case ErrorSeverity::WARNING: return "WARNING";
        case ErrorSeverity::ERROR: return "ERROR";
        case ErrorSeverity::CRITICAL: return "CRITICAL";
        case ErrorSeverity::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string ErrorReportingSystem::getCategoryString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::SYSTEM_INITIALIZATION: return "SYSTEM_INITIALIZATION";
        case ErrorCategory::CONFIGURATION: return "CONFIGURATION";
        case ErrorCategory::NEURAL_COMPUTATION: return "NEURAL_COMPUTATION";
        case ErrorCategory::MEMORY_ALLOCATION: return "MEMORY_ALLOCATION";
        case ErrorCategory::FILE_IO: return "FILE_IO";
        case ErrorCategory::NETWORK: return "NETWORK";
        case ErrorCategory::PLUGIN_LOADING: return "PLUGIN_LOADING";
        case ErrorCategory::SIMULATION_STEP: return "SIMULATION_STEP";
        case ErrorCategory::MOTOR_OUTPUT: return "MOTOR_OUTPUT";
        case ErrorCategory::SENSORY_INPUT: return "SENSORY_INPUT";
        case ErrorCategory::DEVELOPMENT: return "DEVELOPMENT";
        case ErrorCategory::NEUROMODULATION: return "NEUROMODULATION";
        case ErrorCategory::MEMORY_SYSTEM: return "MEMORY_SYSTEM";
        case ErrorCategory::PLANNING: return "PLANNING";
        case ErrorCategory::ATTENTION: return "ATTENTION";
        case ErrorCategory::CONCEPT_FORMATION: return "CONCEPT_FORMATION";
        case ErrorCategory::PREDICTION: return "PREDICTION";
        case ErrorCategory::USER_INPUT: return "USER_INPUT";
        case ErrorCategory::PERFORMANCE: return "PERFORMANCE";
        case ErrorCategory::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

// ErrorHandling convenience functions
void ErrorHandling::report(const NLMError& error) {
    gUserFeedbackSystem.reportError(error);
}

void ErrorHandling::report(ErrorCategory category, const std::string& code, const std::string& details) {
    ErrorType type;
    type.category = category;
    type.code = code;
    type.description = "Error in " + getCategoryString(category);
    type.suggestedAction = "Check system logs for details";
    
    ErrorContext context;
    context.operation = "unknown";
    
    NLMError error(type, context, details);
    gUserFeedbackSystem.reportError(error);
}

void ErrorHandling::report(ErrorCategory category, const std::string& message) {
    report(category, "" , message);
}

bool ErrorHandling::shouldContinue(const NLMError& error) {
    return gUserFeedbackSystem.handleError(error) == UserAction::CONTINUE;
}

ErrorStatistics ErrorHandling::getStatistics() {
    ErrorStatistics stats;
    
    stats.totalErrors = gUserFeedbackSystem.getTotalErrors();
    
    for (int i = 0; i < 6; ++i) {
        stats.errorsBySeverity[i] = gUserFeedbackSystem.getErrorsBySeverity(static_cast<ErrorSeverity>(i));
    }
    
    // Get category counts (simplified)
    for (int i = 0; i < 20; ++i) {
        stats.errorsByCategory[i] = 0; // Would need to implement category counting
    }
    
    stats.errorRate = gUserFeedbackSystem.getErrorRate();
    stats.recoverySuccessRate = gErrorRecoverySystem.getRecoverySuccessRate();
    stats.reportSuccessRate = gErrorReportingSystem.getReportSuccessRate();
    
    return stats;
}

void ErrorHandling::configure(FeedbackLevel level) {
    gUserFeedbackSystem.setFeedbackLevel(level);
}

void ErrorHandling::setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    gUserFeedbackSystem.addErrorHandler(handler);
}

void ErrorHandling::enableAutoRecovery(bool enable) {
    gErrorRecoverySystem.enableAutomaticRecovery(enable);
}

UserAction ErrorHandling::attemptRecovery(const NLMError& error) {
    return gErrorRecoverySystem.handleWithRecovery(error);
}

void ErrorHandling::showProgress(const std::string& message) {
    std::cout << "[PROGRESS] " << message << std::endl;
}

void ErrorHandling::showInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void ErrorHandling::showWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

void ErrorHandling::showError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void ErrorHandling::showFatal(const std::string& message) {
    std::cerr << "[FATAL] " << message << std::endl;
}

void ErrorHandling::setErrorContext(const std::string& operation, const std::string& location) {
    // Set global error context
}

void ErrorHandling::addErrorContext(const std::string& key, const std::string& value) {
    // Add to global error context
}

void ErrorHandling::ignoreErrorCategory(ErrorCategory category) {
    // Implementation would ignore specific error category
}

void ErrorHandling::ignoreErrorSeverity(ErrorSeverity severity) {
    // Implementation would ignore specific error severity
}

void ErrorHandling::enableLogging(const std::string& filename) {
    gUserFeedbackSystem.enableFileOutput(filename);
}

void ErrorHandling::disableLogging() {
    gUserFeedbackSystem.enableFileOutput("");
}

bool ErrorHandling::isLoggingEnabled() {
    // Check if logging is enabled
    return true; // Simplified
}

// Convenience NLMError constructor
NLMError::NLMError(const std::string& message) {
    ErrorType type;
    type.category = ErrorCategory::UNKNOWN;
    type.code = "GENERIC";
    type.description = message;
    type.suggestedAction = "Check system logs for details";
    
    ErrorContext context;
    context.operation = "unknown";
    
    details_ = message;
    timestamp_ = getCurrentTimestamp();
    severity_ = ErrorSeverity::INFO;
    
    generateFormattedMessage();
}
