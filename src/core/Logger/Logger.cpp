// Comprehensive Logging and Error Handling Implementation
// Phase 6: Advanced Logging and Safety Improvements

#include "Logger.hpp"
#include <stdexcept>
#include <ctime>
#include <algorithm>
#include <filesystem>

namespace nlm {

namespace {
    // Get current timestamp string
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
}

// ConsoleLogSink implementation
void ConsoleLogSink::write(const LogEntry& entry) {
    if (entry.level < minLevel) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    auto timePoint = entry.timestamp;
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timePoint.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count()
        << "] [" << getLevelString(entry.level) << "]"
        << " [" << getCategoryString(entry.category) << "]"
        << " " << entry.message;
    
    if (!entry.sourceFile.empty()) {
        oss << " (" << entry.sourceFile;
        if (!entry.sourceFunction.empty()) {
            oss << "::" << entry.sourceFunction;
        }
        oss << ":" << entry.sourceLine << ")";
    }
    
    std::cout << getColorCode(entry.level) << oss.str() << getResetColor() << std::endl;
    
    // Output metadata if present
    for (const auto& [key, value] : entry.metadata) {
        std::cout << "    Metadata: " << key << " = " << value << std::endl;
    }
}

void ConsoleLogSink::flush() {
    std::cout.flush();
}

// FileLogSink implementation
void FileLogSink::write(const LogEntry& entry) {
    if (entry.level < minLevel) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check file rotation
    if (useRotation && currentSize >= maxFileSize) {
        rotateFile();
    }
    
    // Write entry to file
    std::ostringstream oss;
    
    auto timePoint = entry.timestamp;
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timePoint.time_since_epoch()) % 1000;
    
    oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count()
        << "] [" << static_cast<int>(entry.level) << "]"
        << " [" << static_cast<int>(entry.category) << "]"
        << " " << entry.message << std::endl;
    
    file << oss.str();
    currentSize += oss.str().size();
}

void FileLogSink::flush() {
    std::lock_guard<std::mutex> lock(mutex);
    file.flush();
}

void FileLogSink::rotateFile() {
    if (!useRotation) return;
    
    file.close();
    
    // Close current file
    while (std::filesystem::exists(filepath + "." + std::to_string(fileNumber))) {
        fileNumber++;
    }
    
    // Rename current file
    std::filesystem::rename(filepath, filepath + "." + std::to_string(fileNumber));
    
    // Create new file
    file.open(filepath, std::ios::app);
    currentSize = 0;
}

// EnhancedErrorHandler implementation
void EnhancedErrorHandler::handleError(const std::string& message, 
                                     const std::string& source,
                                     int line) {
    std::string enhancedMessage = message;
    
    if (captureStackTrace) {
        enhancedMessage += "\nStack trace:\n";
        enhancedMessage += getStackTrace();
    }
    
    if (!source.empty()) {
        enhancedMessage += "\nSource: " + source;
    }
    
    if (line > 0) {
        enhancedMessage += "\nLine: " + std::to_string(line);
    }
    
    // Handle through all registered handlers
    for (const auto& handler : handlers) {
        handler->handleError(enhancedMessage, "", 0);
    }
}

std::string EnhancedErrorHandler::getStackTrace() {
    std::ostringstream oss;
    oss << "  Stack trace not implemented (consider using backtrace library)";
    return oss.str();
}

} // namespace nlm

// Initialize global logger instance
nlm::GlobalLogger::getInstance();

// Test the enhanced logging system
#ifdef TEST_LOGGING
#include <iostream>

void testLogging() {
    std::cout << "=== Testing Enhanced Logging System ===" << std::endl;
    
    // Test different log levels
    nlm::GlobalLogger::logger().info("This is an info message");
    nlm::GlobalLogger::logger().warning("This is a warning message");
    nlm::GlobalLogger::logger().error("This is an error message");
    
    // Test categories
    nlm::GlobalLogger::logger().log(
        nlm::LogLevel::Debug,
        nlm::LogCategory::Brain,
        "Debug message for brain simulation"
    );
    
    nlm::GlobalLogger::logger().log(
        nlm::LogLevel::Info,
        nlm::LogCategory::Memory,
        "Memory system status update"
    );
    
    // Test error handling
    try {
        NLM_LOG_ERROR("Test error message");
        throw std::runtime_error("Simulated error for testing");
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
    
    std::cout << "=== Logging test completed ===" << std::endl;
}
#endif

// Convenience function for easy logging setup
void setupDefaultLogging(const std::string& name, nlm::LogLevel level) {
    nlm::GlobalLogger::getInstance().setLevel(level);
    nlm::GlobalLogger::getInstance().addConsoleSink(level);
    std::cout << "Logging initialized for: " << name << std::endl;
}

void setupFileLogging(const std::string& filepath, nlm::LogLevel level) {
    nlm::GlobalLogger::getInstance().setLevel(level);
    nlm::GlobalLogger::getInstance().addFileSink(filepath, level);
    std::cout << "File logging initialized: " << filepath << std::endl;
}