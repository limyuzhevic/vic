#include "ErrorHandling.hpp"
#include "Logger.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>

namespace nlm {

// ConsoleErrorReporter implementation
void ConsoleErrorReporter::report(const ErrorInfo& error) {
    if (!isEnabled(error.severity)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(error.severity) << "] ";
    
    // Add timestamp
    if (useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(error.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " ";
    }
    
    oss << error.message;
    
    if (!error.file.empty()) {
        oss << " (" << error.file << ":" << error.line << ")";
    }
    
    if (!error.context.empty()) {
        oss << " [Context: ";
        bool first = true;
        for (const auto& [key, value] : error.context) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        oss << "]";
    }
    
    std::string output = oss.str();
    
    // Console output with colors
    if (useColors) {
        std::cout << levelToColor(error.severity) << output << "\033[0m" << std::endl;
    } else {
        std::cout << output << std::endl;
    }
    
    // Also log to logger for consistency
    std::string logMessage = "Error: " + error.message;
    if (!error.file.empty()) {
        logMessage += " (" + error.file + ":" + std::to_string(error.line) + ")";
    }
    
    switch (error.severity) {
        case ErrorSeverity::Debug:
            Logger::debug(logMessage, error.file, error.line, error.function);
            break;
        case ErrorSeverity::Info:
            Logger::info(logMessage, error.file, error.line, error.function);
            break;
        case ErrorSeverity::Warning:
            Logger::warning(logMessage, error.file, error.line, error.function);
            break;
        case ErrorSeverity::Error:
            Logger::error(logMessage, error.file, error.line, error.function);
            break;
        case ErrorSeverity::Critical:
            Logger::critical(logMessage, error.file, error.line, error.function);
            break;
        case ErrorSeverity::Fatal:
            Logger::critical(logMessage, error.file, error.line, error.function);
            break;
    }
}

void ConsoleErrorReporter::report(const std::exception& ex) {
    ErrorInfo error(ErrorCode::UnknownError, ErrorCategory::System, 
                   ErrorSeverity::Error, ex.what());
    report(error);
}

// FileErrorReporter implementation
FileErrorReporter::FileErrorReporter(const std::string& path, bool appendMode, 
                                     ErrorSeverity severity)
    : filepath(path), append(appendMode) {
    std::ios::openmode mode = std::ios::app;
    if (!appendMode) {
        mode = std::ios::trunc | std::ios::out;
    }
    fileStream.open(path, mode);
}

FileErrorReporter::~FileErrorReporter() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void FileErrorReporter::report(const ErrorInfo& error) {
    if (!isEnabled(error.severity)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(error.severity) << "]";
    
    auto time = std::chrono::system_clock::to_time_t(error.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " " << error.message;
    
    if (!error.file.empty()) {
        oss << " (" << error.file << ":" << error.line << ")";
    }
    
    fileStream << oss.str() << std::endl;
    if (fileStream.fail()) {
        // Failed to write to file - can't report this error
    }
}

void FileErrorReporter::report(const std::exception& ex) {
    ErrorInfo error(ErrorCode::UnknownError, ErrorCategory::System,
                   ErrorSeverity::Error, ex.what());
    report(error);
}

// ErrorManager implementation
void ErrorManager::addReporter(std::shared_ptr<IErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(mutex);
    reporters.push_back(reporter);
}

void ErrorManager::removeReporter(const IErrorReporter* reporter) {
    std::lock_guard<std::mutex> lock(mutex);
    reporters.erase(std::remove_if(reporters.begin(), reporters.end(),
                                   [reporter](const std::shared_ptr<IErrorReporter>& r) {
                                       return r.get() == reporter;
                                   }),
                    reporters.end());
}

void ErrorManager::clearReporters() {
    std::lock_guard<std::mutex> lock(mutex);
    reporters.clear();
}

void ErrorManager::report(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if error should be reported based on global level
    if (error.severity < globalLevel) {
        return;
    }
    
    lastErrorMessage = error.message;
    
    // Report to all registered reporters
    for (const auto& reporter : reporters) {
        try {
            reporter->report(error);
        } catch (...) {
            // Silently ignore reporter failures
        }
    }
}

void ErrorManager::report(const std::exception& ex) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Try to extract NLM exception info if available
    try {
        const NLMException& nlmEx = dynamic_cast<const NLMException&>(ex);
        ErrorInfo error(nlmEx.code, nlmEx.category, nlmEx.severity, 
                       nlmEx.what(), nlmEx.file, nlmEx.line, nlmEx.function);
        report(error);
    } catch (...) {
        // Generic exception
        ErrorInfo error(ErrorCode::UnknownError, ErrorCategory::System,
                       ErrorSeverity::Error, ex.what());
        report(error);
    }
}

void ErrorManager::report(ErrorCode code, ErrorCategory category, ErrorSeverity severity,
                         const std::string& message, const std::string& file,
                         int line, const std::string& function,
                         const std::unordered_map<std::string, std::string>& context) {
    ErrorInfo error(code, category, severity, message, file, line, function);
    error.context = context;
    report(error);
}

void ErrorManager::setGlobalLevel(ErrorSeverity level) {
    std::lock_guard<std::mutex> lock(mutex);
    globalLevel = level;
}

// ValidationUtils implementation
bool ValidationUtils::isValidNeuronId(const NeuronId& id) {
    return id.isValid();
}

bool ValidationUtils::isValidSynapseId(const SynapseId& id) {
    return id.isValid();
}

bool ValidationUtils::isValidRegionId(const RegionId& id) {
    return id.isValid();
}

std::vector<std::string> ValidationUtils::validateVector(
    const std::vector<float>& values, float min, float max) {
    std::vector<std::string> errors;
    
    for (size_t i = 0; i < values.size(); ++i) {
        if (std::isnan(values[i]) || std::isinf(values[i])) {
            errors.push_back("NaN or infinite value at index " + std::to_string(i));
        }
        
        if (values[i] < min) {
            errors.push_back("Value at index " + std::to_string(i) + 
                           " is below minimum " + std::to_string(min));
        }
        
        if (values[i] > max) {
            errors.push_back("Value at index " + std::to_string(i) + 
                           " is above maximum " + std::to_string(max));
        }
    }
    
    return errors;
}

// Helper function implementations (these would typically be in a utility header)
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

void trim(std::string& str) {
    // Implementation would trim whitespace from both ends
}

bool isInteger(const std::string& str) {
    if (str.empty()) return false;
    char* end;
    std::strtol(str.c_str(), &end, 10);
    return *end == '\0';
}

bool isDouble(const std::string& str) {
    if (str.empty()) return false;
    char* end;
    std::strtod(str.c_str(), &end);
    return *end == '\0';
}

std::string levelToString(ErrorSeverity level) {
    switch (level) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARN";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRIT";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string levelToColor(ErrorSeverity level) {
    switch (level) {
        case ErrorSeverity::Debug: return "\033[36m";    // Cyan
        case ErrorSeverity::Info: return "\033[32m";      // Green
        case ErrorSeverity::Warning: return "\033[33m";   // Yellow
        case ErrorSeverity::Error: return "\033[31m";     // Red
        case ErrorSeverity::Critical: return "\033[35m"; // Magenta
        case ErrorSeverity::Fatal: return "\033[41m\033[30m"; // Red background
        default: return "\033[0m";
    }
}

} // namespace nlm
