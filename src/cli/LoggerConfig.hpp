// NLM (熙然) - Neural Learning Machine CLI Logger Configuration
//
// This file implements logging configuration for the CLI, including
// console and file logging, verbosity control, and log level management.

#include "cli/LoggerConfig.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "core/Logger/FileLogger.hpp"

namespace nlm {
namespace cli {

LoggerConfig::LoggerConfig() : fileLoggingEnabled(false) {
    globalLogger = std::make_shared<Logger>();
    consoleLogger = nullptr;
    fileLogger = nullptr;
}

void LoggerConfig::configure(VerbosityLevel verbosity, const std::string& logFile, 
                           const std::string& outputFile) {
    NLM_LOG_INFO("Configuring logging system");
    
    // Clear existing loggers
    globalLogger->clearLoggers();
    
    // Set up console logger based on verbosity
    LogLevel logLevel;
    switch (verbosity) {
        case VerbosityLevel::Quiet:
            logLevel = LogLevel::Error;
            break;
        case VerbosityLevel::Error:
            logLevel = LogLevel::Error;
            break;
        case VerbosityLevel::Warning:
            logLevel = LogLevel::Warning;
            break;
        case VerbosityLevel::Info:
            logLevel = LogLevel::Info;
            break;
        case VerbosityLevel::Debug:
            logLevel = LogLevel::Debug;
            break;
        case VerbosityLevel::Trace:
            logLevel = LogLevel::Trace;
            break;
    }
    
    addConsoleLogger(logLevel);
    
    // Set up file logger if specified
    if (!logFile.empty()) {
        enableFileLogging(true);
        if (fileLogger) {
            fileLogger->setLevel(logLevel);
        }
    }
    
    NLM_LOG_INFO("Logging configured with verbosity level: " + std::to_string(static_cast<int>(verbosity)));
    if (!logFile.empty()) {
        NLM_LOG_INFO("Log file: " + logFile);
    }
}

void LoggerConfig::setLogLevel(LogLevel level) {
    if (consoleLogger) {
        consoleLogger->setLevel(level);
    }
    if (fileLogger) {
        fileLogger->setLevel(level);
    }
}

void LoggerConfig::addConsoleLogger(LogLevel level) {
    if (consoleLogger) {
        globalLogger->removeLogger(consoleLogger);
    }
    
    consoleLogger = std::make_shared<ConsoleLogger>(level);
    globalLogger->addLogger(consoleLogger);
}

void LoggerConfig::addFileLogger(const std::string& filename, LogLevel level) {
    if (fileLogger) {
        globalLogger->removeLogger(fileLogger);
    }
    
    fileLogger = std::make_shared<FileLogger>(filename, level);
    globalLogger->addLogger(fileLogger);
    fileLoggingEnabled = true;
}

void LoggerConfig::enableFileLogging(bool enable) {
    if (enable && !fileLoggingEnabled) {
        // Create a default file logger if one doesn't exist
        if (!fileLogger) {
            addFileLogger("nlm.log", LogLevel::Info);
        }
    }
    else if (!enable && fileLoggingEnabled) {
        if (fileLogger) {
            globalLogger->removeLogger(fileLogger);
        }
        fileLogger = nullptr;
    }
    
    fileLoggingEnabled = enable;
}

} // namespace cli
} // namespace nlm
