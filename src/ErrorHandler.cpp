// Error Handling and Validation for NLM
// Comprehensive error handling, validation, and exception management

#include "ErrorHandler.hpp"
#include "Validation.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>

using namespace nlm;

namespace {
    // Global error handler instance
    std::shared_ptr<ErrorHandler> g_errorHandler;
}

// Error Handler Implementation
void ErrorHandler::setLevel(ErrorLevel level) {
    currentLevel = level;
}

ErrorLevel ErrorHandler::getLevel() const {
    return currentLevel;
}

void ErrorHandler::logError(const std::string& message, const std::string& context) {
    logMessage(ErrorLevel::Error, message, context);
}

void ErrorHandler::logWarning(const std::string& message, const std::string& context) {
    logMessage(ErrorLevel::Warning, message, context);
}

void ErrorHandler::logInfo(const std::string& message, const std::string& context) {
    logMessage(ErrorLevel::Info, message, context);
}

void ErrorHandler::logMessage(ErrorLevel level, const std::string& message, const std::string& context) {
    if (level < currentLevel) return;
    
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = errorLevelToString(level);
    
    std::ostringstream oss;
    oss << "[" << timestamp << "] [" << levelStr << "] ";
    
    if (!context.empty()) {
        oss << "(" << context << ") ";
    }
    
    oss << message;
    
    std::cout << oss.str() << std::endl;
    
    // Store error for reporting
    ErrorInfo errorInfo;
    errorInfo.level = level;
    errorInfo.message = message;
    errorInfo.context = context;
    errorInfo.timestamp = timestamp;
    
    errorHistory.push_back(errorInfo);
}

void ErrorHandler::throwException(ErrorLevel level, const std::string& message, const std::string& context) {
    logMessage(level, message, context);
    
    if (level >= ErrorLevel::Error) {
        throw NLMException(message, level, context);
    }
}

const std::vector<ErrorInfo>& ErrorHandler::getErrorHistory() const {
    return errorHistory;
}

void ErrorHandler::clearErrorHistory() {
    errorHistory.clear();
}

void ErrorHandler::setOutputStream(std::ostream& stream) {
    outputStream = &stream;
}

void ErrorHandler::setFileOutput(const std::string& filepath) {
    logFile.open(filepath, std::ios::out | std::ios::app);
}

std::string ErrorHandler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch() % std::chrono::seconds(1))
        .count();
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

std::string ErrorHandler::errorLevelToString(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::Debug: return "DEBUG";
        case ErrorLevel::Info: return "INFO";
        case ErrorLevel::Warning: return "WARNING";
        case ErrorLevel::Error: return "ERROR";
        case ErrorLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// Validation Implementation
void Validation::setStrictMode(bool strict) {
    isStrict = strict;
}

bool Validation::isStrictMode() const {
    return isStrict;
}

void Validation::setAbortOnError(bool abort) {
    abortOnError = abort;
}

bool Validation::shouldAbortOnError() const {
    return abortOnError;
}

void Validation::addCheck(const std::string& name, std::function<bool()> checkFunc, const std::string& errorMessage) {
    validationChecks.push_back({name, checkFunc, errorMessage});
}

void Validation::addRangeCheck(const std::string& name, double value, double min, double max, const std::string& errorMessage) {
    validationChecks.push_back({name, 
        [value, min, max, errorMessage]() {
            if (value < min || value > max) {
                std::cerr << "VALIDATION ERROR: " << errorMessage << std::endl;
                return false;
            }
            return true;
        },
        errorMessage});
}

void Validation::addFileExistsCheck(const std::string& name, const std::string& filepath, const std::string& errorMessage) {
    validationChecks.push_back({name,
        [filepath, errorMessage]() {
            std::ifstream file(filepath);
            if (!file.good()) {
                std::cerr << "VALIDATION ERROR: " << errorMessage << std::endl;
                return false;
            }
            return true;
        },
        errorMessage});
}

void Validation::addRangeCheck(const std::string& name, int value, int min, int max, const std::string& errorMessage) {
    validationChecks.push_back({name,
        [value, min, max, errorMessage]() {
            if (value < min || value > max) {
                std::cerr << "VALIDATION ERROR: " << errorMessage << std::endl;
                return false;
            }
            return true;
        },
        errorMessage});
}

void Validation::addRangeCheck(const std::string& name, size_t value, size_t min, size_t max, const std::string& errorMessage) {
    validationChecks.push_back({name,
        [value, min, max, errorMessage]() {
            if (value < min || value > max) {
                std::cerr << "VALIDATION ERROR: " << errorMessage << std::endl;
                return false;
            }
            return true;
        },
        errorMessage});
}

bool Validation::runAllChecks() {
    bool allPassed = true;
    
    for (const auto& check : validationChecks) {
        try {
            if (!check.func()) {
                allPassed = false;
                if (isStrictMode() && abortOnError) {
                    std::cerr << "Validation check failed: " << check.name << std::endl;
                    std::exit(1);
                }
            } else {
                if (isStrictMode()) {
                    std::cout << "VALIDATION PASSED: " << check.name << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "VALIDATION EXCEPTION in " << check.name << ": " << e.what() << std::endl;
            allPassed = false;
            
            if (isStrictMode() && abortOnError) {
                std::exit(1);
            }
        }
    }
    
    return allPassed;
}

void Validation::clearChecks() {
    validationChecks.clear();
}

void Validation::setAbortOnError(bool abort) {
    abortOnError = abort;
}

void Validation::setStrictMode(bool strict) {
    isStrict = strict;
}

// Exception Classes
NLMException::NLMException(const std::string& message, ErrorLevel level, const std::string& context)
    : message(message), level(level), context(context), timestamp(getCurrentTimestamp()) {}

const char* NLMException::what() const noexcept {
    return message.c_str();
}

ErrorLevel NLMException::getLevel() const {
    return level;
}

const std::string& NLMException::getContext() const {
    return context;
}

const std::string& NLMException::getTimestamp() const {
    return timestamp;
}

std::string NLMException::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Global Error Handler Functions
define_global_error_handler(ErrorHandler& handler) {
    g_errorHandler = std::shared_ptr<ErrorHandler>(&handler, [](ErrorHandler*) {});
}

ErrorHandler& get_global_error_handler() {
    if (!g_errorHandler) {
        g_errorHandler = std::make_shared<ErrorHandler>();
    }
    return *g_errorHandler;
}

void log_error(const std::string& message, const std::string& context) {
    if (g_errorHandler) {
        g_errorHandler->logError(message, context);
    }
}

void log_warning(const std::string& message, const std::string& context) {
    if (g_errorHandler) {
        g_errorHandler->logWarning(message, context);
    }
}

void log_info(const std::string& message, const std::string& context) {
    if (g_errorHandler) {
        g_errorHandler->logInfo(message, context);
    }
}

void throw_error(const std::string& message, ErrorLevel level, const std::string& context) {
    if (g_errorHandler) {
        g_errorHandler->throwException(level, message, context);
    }
}