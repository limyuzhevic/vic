// TestFramework.cpp
#include "TestFramework.hpp"

namespace nlm {
namespace test {

// Implementation of simple formatter for TestRunner
std::string SimpleFormatter::format(const LogEntry& entry) {
    std::string levelStr;
    switch (entry.level) {
        case LogLevel::Debug: levelStr = "DEBUG"; break;
        case LogLevel::Info: levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARNING"; break;
        case LogLevel::Error: levelStr = "ERROR"; break;
        case LogLevel::Critical: levelStr = "CRITICAL"; break;
    }

    std::stringstream ss;
    ss << "[" << entry.getFormattedTimestamp() << "] "
       << "[" << levelStr << "] "
       << "[" << entry.category << "] "
       << entry.message;

    return ss.str();
}

std::string SimpleFormatter::format(const std::string& message) {
    return message;
}

} // namespace test
} // namespace nlm