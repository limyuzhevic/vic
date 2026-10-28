#include "core/Error/Error.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlm {

ErrorHandler::ErrorHandler() : continue_on_error_(false) {}

ErrorHandler::~ErrorHandler() {}

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::addError(std::shared_ptr<NLMException> error) {
    if (error) {
        errors_.push_back(error);
        
        // Call callback if set
        if (errorReportCallback_) {
            errorReportCallback_(error);
        }
        
        // Log the error
        if (error->getSeverity() >= ErrorSeverity::Error) {
            NLM_LOG_ERROR(error->toString());
        } else if (error->getSeverity() == ErrorSeverity::Warning) {
            NLM_LOG_WARNING(error->toString());
        }
    }
}

void ErrorHandler::clearErrors() {
    errors_.clear();
}

std::vector<std::shared_ptr<NLMException>> ErrorHandler::getErrorsByCategory(ErrorCategory category) const {
    std::vector<std::shared_ptr<NLMException>> filteredErrors;
    for (const auto& error : errors_) {
        if (error->getCategory() == category) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

std::vector<std::shared_ptr<NLMException>> ErrorHandler::getErrorsBySeverity(ErrorSeverity severity) const {
    std::vector<std::shared_ptr<NLMException>> filteredErrors;
    for (const auto& error : errors_) {
        if (error->getSeverity() == severity) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

void ErrorHandler::reportErrors() {
    if (errors_.empty()) {
        return;
    }
    
    NLM_LOG_INFO("=== ERROR REPORT ===");
    NLM_LOG_INFO("Total errors: " + std::to_string(errors_.size()));
    
    // Report by category
    std::unordered_map<ErrorCategory, size_t> categoryCounts;
    for (const auto& error : errors_) {
        categoryCounts[error->getCategory()]++;
    }
    
    for (const auto& pair : categoryCounts) {
        std::string categoryStr;
        switch (pair.first) {
            case ErrorCategory::Configuration: categoryStr = "Configuration"; break;
            case ErrorCategory::FileSystem: categoryStr = "FileSystem"; break;
            case ErrorCategory::Memory: categoryStr = "Memory"; break;
            case ErrorCategory::Neural: categoryStr = "Neural"; break;
            case ErrorCategory::Integration: categoryStr = "Integration"; break;
            case ErrorCategory::Performance: categoryStr = "Performance"; break;
            case ErrorCategory::System: categoryStr = "System"; break;
            case ErrorCategory::Validation: categoryStr = "Validation"; break;
        }
        NLM_LOG_INFO("  " + categoryStr + ": " + std::to_string(pair.second));
    }
    
    // Report details
    for (const auto& error : errors_) {
        NLM_LOG_INFO("  " + error->toString());
    }
    
    NLM_LOG_INFO("=== END ERROR REPORT ===");
}

void ErrorHandler::setErrorReportCallback(std::function<void(std::shared_ptr<NLMException>)> callback) {
    errorReportCallback_ = callback;
}

size_t ErrorHandler::getErrorCount(ErrorCategory category) const {
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error->getCategory() == category) {
            count++;
        }
    }
    return count;
}

size_t ErrorHandler::getErrorCount(ErrorSeverity severity) const {
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error->getSeverity() == severity) {
            count++;
        }
    }
    return count;
}

std::string ErrorHandler::getErrorSummary() const {
    std::stringstream ss;
    ss << "Error Summary (" << errors_.size() << " total errors):" << std::endl;
    
    // Count by category
    std::unordered_map<ErrorCategory, size_t> categoryCounts;
    for (const auto& error : errors_) {
        categoryCounts[error->getCategory()]++;
    }
    
    for (const auto& pair : categoryCounts) {
        std::string categoryStr;
        switch (pair.first) {
            case ErrorCategory::Configuration: categoryStr = "Configuration"; break;
            case ErrorCategory::FileSystem: categoryStr = "FileSystem"; break;
            case ErrorCategory::Memory: categoryStr = "Memory"; break;
            case ErrorCategory::Neural: categoryStr = "Neural"; break;
            case ErrorCategory::Integration: categoryStr = "Integration"; break;
            case ErrorCategory::Performance: categoryStr = "Performance"; break;
            case ErrorCategory::System: categoryStr = "System"; break;
            case ErrorCategory::Validation: categoryStr = "Validation"; break;
        }
        ss << "  " << categoryStr << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

// Helper function to format time point to string
std::string formatTimePoint(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
