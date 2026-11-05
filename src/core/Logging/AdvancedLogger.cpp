// AdvancedLogger.cpp - Implementation of the advanced logging system

#include "core/Logging/AdvancedLogger.hpp"
#include <fstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// Implementation of PerformanceMonitor methods

// All methods are implemented in the header file for performance

// Implementation of AdvancedLogger methods

// All methods are implemented in the header file for performance

// Helper function to parse log level string to enum
AdvancedLogger::LogLevel stringToLogLevel(const std::string& level) {
    if (level == "DEBUG") return AdvancedLogger::LogLevel::DEBUG;
    if (level == "INFO") return AdvancedLogger::LogLevel::INFO;
    if (level == "WARNING") return AdvancedLogger::LogLevel::WARNING;
    if (level == "ERROR") return AdvancedLogger::LogLevel::ERROR;
    if (level == "CRITICAL") return AdvancedLogger::LogLevel::CRITICAL;
    return AdvancedLogger::LogLevel::INFO; // Default
}

// Helper function to convert LogLevel enum to string
std::string logLevelToString(AdvancedLogger::LogLevel level) {
    switch (level) {
        case AdvancedLogger::LogLevel::DEBUG: return "DEBUG";
        case AdvancedLogger::LogLevel::INFO: return "INFO";
        case AdvancedLogger::LogLevel::WARNING: return "WARNING";
        case AdvancedLogger::LogLevel::ERROR: return "ERROR";
        case AdvancedLogger::LogLevel::CRITICAL: return "CRITICAL";
        default: return "INFO";
    }
}

} // namespace nlm