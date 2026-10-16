// Convenience functions for logger using dependency injection
#include "LoggerFactory.hpp"
#include "Logger.hpp"
#include <memory>
#include <mutex>

namespace nlm {
namespace LoggerFactoryUtils {

// Static variables for singleton pattern replacement
static std::mutex initializationMutex;
static std::shared_ptr<ILogger> defaultLogger = nullptr;
static bool initialized = false;
static LoggerConfig globalConfig;

void initialize(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(initializationMutex);
    globalConfig = config;
    defaultLogger = LoggerFactory::createLogger(config);
    initialized = true;
}

std::shared_ptr<ILogger> getDefaultLogger() {
    std::lock_guard<std::mutex> lock(initializationMutex);
    if (!initialized) {
        initialize(); // Initialize with default config
    }
    return defaultLogger;
}

std::shared_ptr<ILogger> getLogger(const std::string& name) {
    return LoggerFactory::getLogger(name);
}

void logStructured(ErrorCode code, const std::string& message,
                  const std::string& context,
                  const std::string& file,
                  int line,
                  const std::string& function) {
    auto logger = getDefaultLogger();
    if (!logger) return;

    LogEntry entry;
    entry.level = errorCodeToLogLevel(code);
    entry.message = message + " [Error: " + std::to_string(static_cast<int>(code)) + "]";
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();

    logger->log(entry);
}

ErrorCode logLevelToErrorCode(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return ErrorCode::Success;
        case LogLevel::Info: return ErrorCode::Success;
        case LogLevel::Warning: return ErrorCode::PermissionDenied;
        case LogLevel::Error: return ErrorCode::LoggerError;
        case LogLevel::Critical: return ErrorCode::Critical;
        default: return ErrorCode::UnknownError;
    }
}

LogLevel errorCodeToLogLevel(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success:
        case ErrorCode::NotImplemented:
        case ErrorCode::InvalidArgument:
        case ErrorCode::OutOfRange:
        case ErrorCode::Timeout:
        case ErrorCode::NotFound:
        case ErrorCode::AlreadyExists:
            return LogLevel::Info;

        case ErrorCode::FileNotFound:
        case ErrorCode::FileAccessDenied:
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::ConfigParseError:
        case ErrorCode::ConfigTypeMismatch:
        case ErrorCode::MemoryNotInitialized:
        case ErrorCode::NeuronNotFound:
        case ErrorCode::SynapseNotFound:
            return LogLevel::Warning;

        case ErrorCode::PermissionDenied:
        case ErrorCode::LoggerNotInitialized:
        case ErrorCode::LogFileError:
            return LogLevel::Error;

        case ErrorCode::MemoryAllocationFailed:
        case ErrorCode::MemoryAccessViolation:
        case ErrorCode::OutOfMemory:
        case ErrorCode::ConnectionFailed:
        case ErrorCode::NetworkTimeout:
        case ErrorCode::OperationAborted:
        case ErrorCode::RecoveryFailed:
            return LogLevel::Critical;

        default:
            return LogLevel::Error;
    }
}

} // namespace LoggerFactoryUtils
} // namespace nlm
