// NLM Error Handler Implementation
// Enhanced error handling with context and recovery information

#include "core/Utilities/ErrorHandler.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace nlm;

std::vector<ErrorHandler::ErrorContext> ErrorHandler::errorHistory;
bool ErrorHandler::failOnError = false;
bool ErrorHandler::structuredLoggingEnabled = false;
LogLevel ErrorHandler::logLevel = LogLevel::Info;

void ErrorHandler::logError(const std::string& error) {
    logErrorWithContext(error, "");
}

void ErrorHandler::logErrorWithContext(const std::string& error, const std::string& context) {
    ErrorContext errorContext;
    errorContext.error = error;
    errorContext.context = context;
    errorContext.timestamp = std::chrono::system_clock::now();
    errorContext.suggestions = generateSuggestions(error);
    
    errorHistory.push_back(errorContext);
    
    std::stringstream logMessage;
    logMessage << "[ERROR] " << getTimestampString();
    if (!context.empty()) {
        logMessage << " Context: " << context;
    }
    logMessage << " Error: " << error;
    
    NLM_LOG_ERROR(logMessage.str());
    
    if (failOnError) {
        throw NLMError(error);
    }
}

void ErrorHandler::logWarning(const std::string& warning) {
    std::stringstream logMessage;
    logMessage << "[WARNING] " << getTimestampString() << " Warning: " << warning;
    NLM_LOG_WARNING(logMessage.str());
}

void ErrorHandler::logInfo(const std::string& info) {
    if (logLevel <= LogLevel::Info) {
        std::stringstream logMessage;
        logMessage << "[INFO] " << getTimestampString() << " Info: " << info;
        NLM_LOG_INFO(logMessage.str());
    }
}

void ErrorHandler::logMemoryError(const std::string& operation, size_t size) {
    std::string error = "Memory allocation failed for " + operation + " of size " + std::to_string(size) + " bytes";
    logErrorWithContext(error, "Memory Management");
}

void ErrorHandler::logFileError(const std::string& operation, const std::string& filepath) {
    std::string error = "File operation \"" + operation + "\" failed for file: " + filepath;
    logErrorWithContext(error, "File I/O");
}

void ErrorHandler::logConfigurationError(const std::string& key, const std::string& expectedType) {
    std::string error = "Configuration error: key \"" + key + "\" has invalid type or is missing. Expected type: " + expectedType;
    logErrorWithContext(error, "Configuration");
}

void ErrorHandler::clearErrorHistory() {
    errorHistory.clear();
}

std::vector<ErrorHandler::ErrorContext> ErrorHandler::getRecentErrors(size_t maxCount) {
    if (errorHistory.size() <= maxCount) {
        return errorHistory;
    }
    
    size_t startIndex = errorHistory.size() - maxCount;
    return std::vector<ErrorContext>(errorHistory.begin() + startIndex, errorHistory.end());
}

bool ErrorHandler::hasRecentErrors(double timeWindowSeconds) {
    auto now = std::chrono::system_clock::now();
    auto timeWindow = std::chrono::duration<double>(timeWindowSeconds);
    
    for (const auto& error : errorHistory) {
        auto timeSinceError = std::chrono::duration_cast<std::chrono::duration<double>>(now - error.timestamp);
        if (timeSinceError < timeWindow) {
            return true;
        }
    }
    return false;
}

std::string ErrorHandler::formatError(const ErrorContext& context) {
    std::stringstream formatted;
    formatted << "[" << getTimestampString() << "] ";
    formatted << "ERROR";
    if (!context.context.empty()) {
        formatted << " (" << context.context << ")";
    }
    formatted << ": " << context.error;
    if (!context.suggestions.empty()) {
        formatted << "\n  Suggestions:";
        for (const auto& suggestion : context.suggestions) {
            formatted << "\n    - " << suggestion;
        }
    }
    return formatted.str();
}

std::vector<std::string> ErrorHandler::generateSuggestions(const std::string& error) {
    std::vector<std::string> suggestions;
    
    if (error.find("memory") != std::string::npos || error.find("allocation") != std::string::npos) {
        suggestions.push_back("Check system memory availability");
        suggestions.push_back("Reduce requested memory size");
        suggestions.push_back("Free up memory by closing other applications");
    }
    else if (error.find("file") != std::string::npos || error.find("path") != std::string::npos) {
        suggestions.push_back("Verify file path exists and is accessible");
        suggestions.push_back("Check file permissions");
        suggestions.push_back("Use absolute path instead of relative path");
    }
    else if (error.find("configuration") != std::string::npos || error.find("config") != std::string::npos) {
        suggestions.push_back("Check configuration file syntax");
        suggestions.push_back("Verify required parameters are present");
        suggestions.push_back("Use default configuration");
    }
    else if (error.find("neuron") != std::string::npos || error.find("network") != std::string::npos) {
        suggestions.push_back("Check neuron count is reasonable");
        suggestions.push_back("Verify connection parameters");
        suggestions.push_back("Try with smaller network size");
    }
    
    if (suggestions.empty()) {
        suggestions.push_back("Review error message for details");
        suggestions.push_back("Check system logs for additional information");
        suggestions.push_back("Try running with --verbose flag for more information");
    }
    
    return suggestions;
}

std::string ErrorHandler::getTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void ErrorHandler::addError(const std::string& error, const std::string& context) {
    ErrorContext errorContext;
    errorContext.error = error;
    errorContext.context = context;
    errorContext.timestamp = std::chrono::system_clock::now();
    errorHistory.push_back(errorContext);
}

void ErrorHandler::exportErrorReport(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logErrorWithContext("Failed to open error report file: " + filepath, "Error Report Export");
        return;
    }
    
    file << "NLM Error Report" << std::endl;
    file << "=================" << std::endl;
    file << "Generated: " << getTimestampString() << std::endl;
    file << "Total errors: " << errorHistory.size() << std::endl;
    file << std::endl;
    
    for (const auto& error : errorHistory) {
        file << "Error: " << error.error << std::endl;
        if (!error.context.empty()) {
            file << "Context: " << error.context << std::endl;
        }
        file << "Timestamp: " << getTimestampString() << std::endl;
        file << "Suggestions:" << std::endl;
        for (const auto& suggestion : error.suggestions) {
            file << "  - " << suggestion << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
    logInfo("Error report exported to: " + filepath);
}