#include "LoggerFactory.hpp"
#include "Logger.hpp"
#include "ErrorCodes.hpp"
#include <fstream>
#include <iomanip>
#include <chrono>
#include <filesystem>

namespace nlm {

struct ErrorAwareLogger::Impl {
    LoggerConfig config;
    LogLevel level;
    std::mutex mutex;
    std::string currentLogFile;
    
    // Error reporting
    std::shared_ptr<IErrorReporter> errorReporter;
    
    // Statistics
    size_t logCount;
    size_t errorCount;
    std::chrono::system_clock::time_point lastErrorTime;
    
    // Log file rotation
    std::vector<std::string> rotatedFiles;
    std::string baseLogPath;
    
    // File output streams
    std::ofstream currentFileStream;
    std::vector<std::ofstream> rotatedStreams;
    
    // Log directory management
    static constexpr size_t DEFAULT_MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB
    static constexpr size_t DEFAULT_MAX_FILE_COUNT = 5;
};

ErrorAwareLogger::ErrorAwareLogger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->level = LogLevel::Info;
    pImpl->logCount = 0;
    pImpl->errorCount = 0;
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    
    // Ensure log directory exists
    if (config.outputMode != LoggerConfig::OutputMode::None) {
        std::filesystem::path logDir = std::filesystem::path(config.filePath).parent_path();
        if (!logDir.empty()) {
            std::filesystem::create_directories(logDir);
        }
        
        // Initialize log file
        rotateLogs();
    }
}

ErrorAwareLogger::~ErrorAwareLogger() {
    if (pImpl->currentFileStream.is_open()) {
        pImpl->currentFileStream.close();
    }
    for (auto& stream : pImpl->rotatedStreams) {
        if (stream.is_open()) {
            stream.close();
        }
    }
}

void ErrorAwareLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if level is enabled
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Create log message
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with high precision
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto fraction = std::chrono::duration_cast<std::chrono::microseconds>(
        entry.timestamp.time_since_epoch()) % 1000000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(6) << fraction.count();
    oss << " " << entry.message;
    
    if (!entry.file.empty()) oss << " (" << entry.file << ":" << entry.line << "; " << entry.function << ")";
    
    std::string logMessage = oss.str();
    
    // Output to console if enabled
    if (pImpl->config.outputMode == LoggerConfig::OutputMode::Console || 
        pImpl->config.outputMode == LoggerConfig::OutputMode::Both) {
        
        if (pImpl->config.enableColors) {
            oss.str("");
            oss << levelToColor(entry.level) << logMessage << "\033[0m";
            std::cout << oss.str() << std::endl;
        } else {
            std::cout << logMessage << std::endl;
        }
    }
    
    // Output to file if enabled
    if (pImpl->config.outputMode == LoggerConfig::OutputMode::File || 
        pImpl->config.outputMode == LoggerConfig::OutputMode::Both) {
        
        // Check if we need to rotate logs
        if (pImpl->currentFileStream.tellp() > pImpl->config.maxFileSize) {
            rotateLogs();
        }
        
        if (pImpl->currentFileStream.is_open()) {
            pImpl->currentFileStream << logMessage << std::endl;
            pImpl->currentFileStream.flush();
        }
    }
    
    // Report as error if appropriate
    if (entry.level >= LogLevel::Error) {
        pImpl->errorCount++;
        pImpl->lastErrorTime = std::chrono::system_clock::now();
        
        if (pImpl->errorReporter) {
            ErrorDetails error;
            error.code = (entry.level == LogLevel::Error) ? ErrorCode::LoggerError : ErrorCode::Critical;
            error.message = entry.message;
            error.context = entry.file + ":" + std::to_string(entry.line);
            error.file = entry.file;
            error.line = entry.line;
            error.function = entry.function;
            error.timestamp = entry.timestamp;
            
            pImpl->errorReporter->report(error);
        }
    }
    
    pImpl->logCount++;
}

void ErrorAwareLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ErrorAwareLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ErrorAwareLogger::configure(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
    rotateLogs();
}

LoggerConfig ErrorAwareLogger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void ErrorAwareLogger::setErrorReporter(std::shared_ptr<IErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorReporter = reporter;
}

std::shared_ptr<IErrorReporter> ErrorAwareLogger::getErrorReporter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorReporter;
}

void ErrorAwareLogger::rotateLogs() {
    // Close current file
    if (pImpl->currentFileStream.is_open()) {
        pImpl->currentFileStream.close();
    }
    
    // Rotate existing files
    for (auto& stream : pImpl->rotatedStreams) {
        if (stream.is_open()) {
            stream.close();
        }
    }
    
    pImpl->rotatedStreams = std::move(pImpl->rotatedStreams);
    pImpl->rotatedFiles.clear();
    
    // Move current file to rotated
    if (!pImpl->currentLogFile.empty()) {
        std::string rotatedName = pImpl->currentLogFile + "." + 
            std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::filesystem::rename(pImpl->currentLogFile, rotatedName);
        pImpl->rotatedFiles.push_back(rotatedName);
        
        // Clean up old rotated files if we have too many
        while (pImpl->rotatedFiles.size() > pImpl->config.maxFileCount) {
            std::string oldest = pImpl->rotatedFiles.front();
            pImpl->rotatedFiles.erase(pImpl->rotatedFiles.begin());
            std::filesystem::remove(oldest);
        }
    }
    
    // Open new log file if needed
    if (pImpl->config.outputMode != LoggerConfig::OutputMode::None && 
        !pImpl->config.filePath.empty()) {
        
        std::filesystem::path logPath = pImpl->config.filePath;
        pImpl->currentLogFile = logPath.string();
        
        // Append mode to preserve existing logs
        pImpl->currentFileStream.open(pImpl->currentLogFile, std::ios::app);
    }
}

std::string ErrorAwareLogger::getLogFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentLogFile;
}

size_t ErrorAwareLogger::getLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logCount;
}

size_t ErrorAwareLogger::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

double ErrorAwareLogger::getErrorsPerSecond() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        now - pImpl->lastErrorTime).count();
    
    if (duration <= 0) return static_cast<double>(pImpl->errorCount);
    
    return static_cast<double>(pImpl->errorCount) / static_cast<double>(duration);
}

} // namespace nlm
