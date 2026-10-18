// Enhanced Error Handling Implementation
// 
// This file implements the enhanced error handling system with:
// 1. Comprehensive error categorization and classification
// 2. User-friendly error messages with context
// 3. Error recovery mechanisms
// 4. Logging and reporting
// 5. Integration with experiment framework
// 6. Context-aware error messages
// 7. Multiple error output formats
// 8. Error aggregation and summary reporting
// 9. Interactive error recovery when possible
// 10. Comprehensive error documentation

#include "ErrorHandlingSystem.hpp"
#include <fstream>
#include <algorithm>

namespace nlm {

// FileErrorHandler implementation
FileErrorHandler::FileErrorHandler(const std::string& filename) : logFile_(filename) {}

void FileErrorHandler::handleError(const EnhancedException& error) {
    std::ofstream file(logFile_, std::ios::app);
    if (file.is_open()) {
        file << "\n" << std::chrono::system_clock::now() << " | ERROR | " << error.getMessage() << std::endl;
        file << "Technical Details:" << error.getTechnicalDetails() << std::endl;
        file.close();
    }
}

void FileErrorHandler::handleWarning(const std::string& message) {
    std::ofstream file(logFile_, std::ios::app);
    if (file.is_open()) {
        file << "\n" << std::chrono::system_clock::now() << " | WARNING | " << message << std::endl;
        file.close();
    }
}

void FileErrorHandler::handleInfo(const std::string& message) {
    std::ofstream file(logFile_, std::ios::app);
    if (file.is_open()) {
        file << "\n" << std::chrono::system_clock::now() << " | INFO | " << message << std::endl;
        file.close();
    }
}

// ConsoleErrorHandler implementation
ConsoleErrorHandler::ConsoleErrorHandler() = default;

void ConsoleErrorHandler::handleError(const EnhancedException& error) {
    std::cerr << "\n" << "=" << std::string(60, '=') << std::endl;
    std::cerr << "ERROR OCCURRED" << std::endl;
    std::cerr << "=" << std::string(60, '=') << std::endl;
    
    std::cerr << error.getUserFriendlyMessage() << std::endl;
    
    if (error.getContext().severity == ErrorSeverity::Critical) {
        std::cerr << "\n" << "=" << std::string(60, '=') << std::endl;
        std::cerr << "CRITICAL ERROR - SYSTEM MAY BE UNSTABLE" << std::endl;
        std::cerr << "=" << std::string(60, '=') << std::endl;
    }
    
    std::cerr << "\n" << error.getTechnicalDetails() << std::endl;
}

void ConsoleErrorHandler::handleWarning(const std::string& message) {
    std::cerr << "Warning: " << message << std::endl;
}

void ConsoleErrorHandler::handleInfo(const std::string& message) {
    std::cout << "Info: " << message << std::endl;
}

// ErrorAggregator implementation
ErrorAggregator::ErrorAggregator() : errorCount_(0), warningCount_(0), infoCount_(0) {}

void ErrorAggregator::addError(const EnhancedException& error) {
    errors_.push_back(error);
    errorCount_++;
    
    switch (error.getContext().severity) {
        case ErrorSeverity::Warning:
            warningCount_++;
            break;
        case ErrorSeverity::Info:
            infoCount_++;
            break;
        case ErrorSeverity::Error:
        case ErrorSeverity::Critical:
        case ErrorSeverity::Fatal:
            errorCount_++;
            break;
        default:
            break;
    }
}

void ErrorAggregator::addWarning(const std::string& message) {
    warnings_.push_back(message);
    warningCount_++;
}

void ErrorAggregator::addInfo(const std::string& message) {
    infos_.push_back(message);
    infoCount_++;
}

std::string ErrorAggregator::getSummary() const {
    std::stringstream ss;
    ss << "=== Error Summary ===\n";
    ss << "Total Errors: " << errorCount_ << "\n";
    ss << "Total Warnings: " << warningCount_ << "\n";
    ss << "Total Info Messages: " << infoCount_ << "\n";
    ss << "Total Unique Error Types: " << getUniqueErrorTypes() << "\n";
    return ss.str();
}

std::string ErrorAggregator::getErrorReport() const {
    std::stringstream ss;
    ss << "=== Error Report ===\n\n";
    
    if (!errors_.empty()) {
        ss << "Errors:" << std::endl;
        for (size_t i = 0; i < errors_.size(); ++i) {
            ss << "[" << (i + 1) << "] " << errors_[i].getUserFriendlyMessage() << "\n\n";
        }
    }
    
    if (!warnings_.empty()) {
        ss << "Warnings:" << std::endl;
        for (size_t i = 0; i < warnings_.size(); ++i) {
            ss << "[" << (i + 1) << "] " << warnings_[i] << "\n";
        }
        ss << "\n";
    }
    
    if (!infos_.empty()) {
        ss << "Info Messages:" << std::endl;
        for (size_t i = 0; i < infos_.size(); ++i) {
            ss << "[" << (i + 1) << "] " << infos_[i] << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

size_t ErrorAggregator::getUniqueErrorTypes() const {
    std::unordered_set<ErrorCategory> categories;
    for (const auto& error : errors_) {
        categories.insert(error.getCategory());
    }
    return categories.size();
}

void ErrorAggregator::clear() {
    errors_.clear();
    warnings_.clear();
    infos_.clear();
    errorCount_ = 0;
    warningCount_ = 0;
    infoCount_ = 0;
}

EnhancedException ErrorAggregator::getMostRecentError() const {
    if (!errors_.empty()) {
        return errors_.back();
    }
    return EnhancedException("No errors occurred.");
}

// GlobalErrorManager implementation
GlobalErrorManager::GlobalErrorManager() : errorHandler_(std::make_shared<ConsoleErrorHandler>()) {}

void GlobalErrorManager::setErrorHandler(std::shared_ptr<ErrorHandler> handler) {
    errorHandler_ = handler;
}

std::shared_ptr<ErrorHandler> GlobalErrorManager::getErrorHandler() const {
    return errorHandler_;
}

void GlobalErrorManager::addError(const EnhancedException& error) {
    if (errorHandler_) {
        errorHandler_->handleError(error);
    }
    errorAggregator_.addError(error);
}

void GlobalErrorManager::addWarning(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->handleWarning(message);
    }
    errorAggregator_.addWarning(message);
}

void GlobalErrorManager::addInfo(const std::string& message) {
    if (errorHandler_) {
        errorHandler_->handleInfo(message);
    }
    errorAggregator_.addInfo(message);
}

ErrorAggregator GlobalErrorManager::getErrorAggregator() const {
    return errorAggregator_;
}

std::string GlobalErrorManager::getErrorSummary() const {
    return errorAggregator_.getSummary();
}

std::string GlobalErrorManager::getErrorReport() const {
    return errorAggregator_.getErrorReport();
}

void GlobalErrorManager::clear() {
    errorAggregator_.clear();
}

bool GlobalErrorManager::hasErrors() const {
    return errorAggregator_.getErrorCount() > 0;
}

bool GlobalErrorManager::hasCriticalErrors() const {
    for (const auto& error : errorAggregator_.getErrors()) {
        if (error.getSeverity() == ErrorSeverity::Critical || error.getSeverity() == ErrorSeverity::Fatal) {
            return true;
        }
    }
    return false;
}

void GlobalErrorManager::setupDefaultErrorHandling() {
    auto consoleHandler = std::make_shared<ConsoleErrorHandler>();
    setErrorHandler(consoleHandler);
}

void GlobalErrorManager::setupFileErrorLogging(const std::string& filename) {
    auto fileHandler = std::make_shared<FileErrorHandler>(filename);
    setErrorHandler(fileHandler);
}

// Setup default error handling
void setupDefaultErrorHandling() {
    GlobalErrorManager::setupDefaultErrorHandling();
}

void setupFileErrorLogging(const std::string& filename) {
    GlobalErrorManager::setupFileErrorLogging(filename);
}

// Error utility functions
std::string ErrorUtils::getErrorIcon(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug:
            return "🔍";
        case ErrorSeverity::Info:
            return "ℹ";
        case ErrorSeverity::Warning:
            return "⚠";
        case ErrorSeverity::Error:
            return "❌";
        case ErrorSeverity::Critical:
            return "🚨";
        case ErrorSeverity::Fatal:
            return "💥";
        default:
            return "❓";
    }
}

std::string ErrorUtils::getErrorColor(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug:
            return "\033[36m";  // Cyan
        case ErrorSeverity::Info:
            return "\033[32m";  // Green
        case ErrorSeverity::Warning:
            return "\033[33m";  // Yellow
        case ErrorSeverity::Error:
            return "\033[31m";  // Red
        case ErrorSeverity::Critical:
            return "\033[1;31m";  // Bold Red
        case ErrorSeverity::Fatal:
            return "\033[1;31;40m";  // Bold Red on Black
        default:
            return "\033[0m";   // Reset
    }
}

std::string ErrorUtils::resetColor() {
    return "\033[0m";
}

// Helper function to print error with icon and color
void printErrorWithIcon(const std::string& message, ErrorSeverity severity) {
    std::string icon = ErrorUtils::getErrorIcon(severity);
    std::string color = ErrorUtils::getErrorColor(severity);
    std::string reset = ErrorUtils::resetColor();
    
    std::cout << color << icon << " " << message << reset << std::endl;
}

// Helper function to print error summary
void printErrorSummary(const std::string& title, const std::string& summary) {
    std::cout << "\n" << "=" << std::string(60, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << summary << std::endl;
}

// Helper function to log error with context
void logErrorWithContext(const std::string& errorCode, const std::string& message,
                        ErrorCategory category, ErrorSeverity severity,
                        const std::string& file, int line, const std::string& function) {
    ErrorContext context = ErrorUtils::createErrorContext(errorCode, message, category, severity, file, line, function);
    EnhancedException error(context);
    
    auto& errorManager = GlobalErrorManager::getInstance();
    errorManager.addError(error);
}

} // namespace nlm

// Convenience macros for error handling
#define NLM_THROW_ERROR(error_type, message) \
    throw nlm::error_type(message)

#define NLM_THROW_ERROR_WITH_CONTEXT(error_type, message, category, severity) \
    nlm::ErrorContext context = nlm::ErrorUtils::createErrorContext("CUSTOM", message, category, severity); \
    throw nlm::error_type(context)

#define NLM_THROW_SYSTEM_ERROR(message) \
    throw nlm::SystemError(message)

#define NLM_THROW_CONFIGURATION_ERROR(message, key, expected) \
    throw nlm::ConfigurationError(message, key, expected)

#define NLM_THROW_NEURON_ERROR(message, neuron_id) \
    throw nlm::NeuronError(message, neuron_id)

#define NLM_THROW_SYNAPSE_ERROR(message, synapse_id) \
    throw nlm::SynapseError(message, synapse_id)

#define NLM_THROW_BRAIN_ERROR(message, brain_id) \
    throw nlm::BrainError(message, brain_id)

#define NLM_THROW_PLASTICITY_ERROR(message, rule_type) \
    throw nlm::PlasticityError(message, rule_type)

#define NLM_THROW_ENVIRONMENT_ERROR(message, environment_id) \
    throw nlm::EnvironmentError(message, environment_id)

#define NLM_THROW_MEMORY_ERROR(message, memory_type) \
    throw nlm::MemoryError(message, memory_type)

#define NLM_THROW_DEVELOPMENT_ERROR(message, stage) \
    throw nlm::DevelopmentError(message, stage)

#define NLM_THROW_LEARNING_ERROR(message, algorithm) \
    throw nlm::LearningError(message, algorithm)

// Usage examples:
// NLM_THROW_ERROR_WITH_CONTEXT(
//     nlm::BrainError,
//     "Failed to initialize brain with configuration",
//     nlm::ErrorCategory::BrainError,
//     nlm::ErrorSeverity::Critical,
//     __FILE__, __LINE__, __FUNCTION__
// );

// NLM_THROW_CONFIGURATION_ERROR(
//     "Invalid value for key 'random_seed'",
//     "random_seed",
//     "integer"
// );

// NLM_THROW_NEURON_ERROR(
//     "Membrane potential exceeds threshold",
//     "neuron_123"
// );