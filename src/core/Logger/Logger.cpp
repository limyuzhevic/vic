#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    ErrorHandler errorHandler;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    NLM_VALIDATE(static_cast<int>(level) >= static_cast<int>(LogLevel::Debug) && 
                 static_cast<int>(level) <= static_cast<int>(LogLevel::Critical),
                 "Invalid log level");
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    try {
        // Validate log entry
        NLM_VALIDATE(static_cast<int>(entry.level) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(entry.level) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level in entry");
        
        // Thread safety
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->errorHandler.clear();
        
        // Check log level filtering
        if (entry.level < pImpl->level) {
            return;
        }
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp with error handling
        try {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        } catch (const std::exception& e) {
            oss << "[TIMESTAMP_ERROR]";
            pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                "Failed to format timestamp: " + std::string(e.what()), "Logger"));
        }
        
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        // Thread-safe console output
        try {
            if (pImpl->useColors) {
                std::cout << levelToColor(entry.level) << output << "\033[0m\n";
            } else {
                std::cout << output << "\n";
            }
            std::cout.flush();
        } catch (const std::exception& e) {
            pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                "Failed to output to console: " + std::string(e.what()), "Logger"));
        }
        
        // Thread-safe file output with validation
        if (pImpl->fileOut.is_open()) {
            try {
                pImpl->fileOut << output << "\n";
                pImpl->fileOut.flush();
                
                if (!pImpl->fileOut.good()) {
                    pImpl->fileOut.close();
                    pImpl->errorHandler.addError(std::make_shared<IOError>(
                        "Failed to write to log file: error state set", "Logger"));
                }
            } catch (const std::exception& e) {
                pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                    "Failed to write to log file: " + std::string(e.what()), "Logger"));
            }
        }
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        // Log error to stderr but don't throw
        std::cerr << "[Logger] Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[Logger] Unexpected error: " << e.what() << std::endl;
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    try {
        NLM_VALIDATE(static_cast<int>(level) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(level) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->level = level;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to set log level: " + std::string(e.what()), "Logger"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

LogLevel ConsoleLogger::getLevel() const {
    try {
        return pImpl->level;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
        return LogLevel::Info;
    }
}

void ConsoleLogger::setUseColors(bool useColors) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->useColors = useColors;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

bool ConsoleLogger::getUseColors() const {
    try {
        return pImpl->useColors;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
        return true;
    }
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    try {
        // Validate file path
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Create directory if needed
        std::filesystem::path path(filepath);
        std::filesystem::path parentDir = path.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            if (!std::filesystem::create_directories(parentDir)) {
                NLM_THROW_IO_ERROR("Cannot create directory for log file: " + parentDir.string());
            }
        }
        
        // Open file safely
        pImpl->fileOut.open(filepath, std::ios::app | std::ios::binary);
        if (!pImpl->fileOut.is_open()) {
            NLM_THROW_IO_ERROR("Cannot open log file for writing: " + filepath);
        }
        
        pImpl->fileOut.flush();
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<IOError>(
            "Failed to set output file: " + std::string(e.what()), "Logger"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void ConsoleLogger::closeOutputFile() {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    ErrorHandler errorHandler;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    try {
        NLM_VALIDATE(logger != nullptr, "Logger cannot be null");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to add logger: " + std::string(e.what()), "Logger"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::clearLoggers() {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.clear();
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::setLevel(LogLevel level) {
    try {
        NLM_VALIDATE(static_cast<int>(level) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(level) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            try {
                logger->setLevel(level);
            } catch (const std::exception& e) {
                pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                    "Failed to set level on logger: " + std::string(e.what()), "Logger"));
            }
        }
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to set global logger level: " + std::string(e.what()), "Logger"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    try {
        NLM_VALIDATE(static_cast<int>(level) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(level) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->errorHandler.clear();
        
        // Validate inputs
        NLM_VALIDATE(!message.empty(), "Log message cannot be empty");
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                    "Failed to log to logger: " + std::string(e.what()), "Logger"));
            }
        }
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to log message: " + std::string(e.what()), "Logger"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    try {
        log(LogLevel::Debug, message, file, line, function);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    try {
        log(LogLevel::Info, message, file, line, function);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    try {
        log(LogLevel::Warning, message, file, line, function);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    try {
        log(LogLevel::Error, message, file, line, function);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    try {
        log(LogLevel::Critical, message, file, line, function);
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

bool Logger::isEnabled(LogLevel level) const {
    try {
        NLM_VALIDATE(static_cast<int>(level) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(level) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level");
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            if (logger->getLevel() <= level) {
                return true;
            }
        }
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
        return false;
    }
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    try {
        // This is a placeholder for advanced global logger management
        // In practice, we just use getGlobal() which is already static
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Logger"));
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    try {
        NLM_VALIDATE(static_cast<int>(level_) >= static_cast<int>(LogLevel::Debug) && 
                     static_cast<int>(level_) <= static_cast<int>(LogLevel::Critical),
                     "Invalid log level in LogStream");
    } catch (const NLMError& e) {
        std::cerr << "[Logger] Error: " << e.what() << std::endl;
    }
}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "[Logger] Error in LogStream destructor: " << e.what() << std::endl;
    }
}

} // namespace nlm

