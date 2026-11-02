#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace nlm {

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    int fileRotationCount = 5;
    size_t currentFileSize = 0;
    
    std::shared_ptr<LogFormatter> formatter = std::make_shared<ColoredFormatter>();
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the message
    std::string formattedMessage = pImpl->formatter->format(entry);
    
    // Write to console
    writeToConsole(formattedMessage);
    
    // Write to file if set
    writeToFile(formattedMessage);
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    if (formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->formatter = formatter;
    }
}

std::shared_ptr<LogFormatter> ConsoleLogger::getFormatter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->formatter;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    ensureFileOpen();
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::ensureFileOpen() {
    if (!pImpl->filepath.empty() && !pImpl->fileOut.is_open()) {
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            // Get file size for rotation check
            pImpl->fileOut.seekp(0, std::ios::end);
            pImpl->currentFileSize = pImpl->fileOut.tellp();
        }
    }
}

void ConsoleLogger::writeToConsole(const std::string& formattedMessage) {
    if (pImpl->useColors) {
        std::cout << formattedMessage << "\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
}

void ConsoleLogger::writeToFile(const std::string& formattedMessage) {
    if (!pImpl->filepath.empty() && pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->currentFileSize += formattedMessage.size() + 1;  // +1 for newline
        
        // Rotate if needed
        rotateIfNeeded();
        
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::rotateIfNeeded() {
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        closeOutputFile();
        
        // Rotate files
        std::error_code ec;
        for (int i = pImpl->fileRotationCount - 1; i >= 0; --i) {
            std::string from = pImpl->filepath;
            std::string to = pImpl->filepath;
            
            if (i < pImpl->fileRotationCount - 1) {
                from += "." + std::to_string(i + 1);
            } else {
                from += "." + std::to_string(i);
            }
            
            if (i < pImpl->fileRotationCount - 1) {
                to += "." + std::to_string(i + 2);
            } else {
                to += "." + std::to_string(i + 1);
            }
            
            std::filesystem::rename(from, to, ec);
        }
        
        // Create new file
        ensureFileOpen();
        pImpl->currentFileSize = 0;
    }
}

// FileLogger implementation
struct FileLogger::Impl {
    std::ofstream fileOut;
    LogLevel level = LogLevel::Info;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    int maxFiles = 5;
    size_t currentFileSize = 0;
    std::mutex mutex;
    
    std::shared_ptr<LogFormatter> formatter = std::make_shared<TextFormatter>();
    std::atomic<bool> shouldRotate{false};
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setOutputFile(filepath);
    pImpl->level = level;
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the message
    std::string formattedMessage = pImpl->formatter->format(entry);
    
    // Write to file
    fileOut << formattedMessage << "\n";
    currentFileSize += formattedMessage.size() + 1;  // +1 for newline
    
    // Rotate if needed
    rotateIfNeeded();
    
    fileOut.flush();
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    if (formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->formatter = formatter;
    }
}

std::shared_ptr<LogFormatter> FileLogger::getFormatter() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->formatter;
}

void FileLogger::setMaxFileSize(size_t maxSizeBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSizeBytes;
}

void FileLogger::setMaxFiles(int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFiles = maxFiles;
}

std::string FileLogger::getCurrentLogFile() const {
    return pImpl->filepath;
}

void FileLogger::rotateIfNeeded() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (currentFileSize >= pImpl->maxFileSize) {
        if (fileOut.is_open()) {
            fileOut.close();
        }
        
        // Rotate files
        std::error_code ec;
        for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
            std::string from = pImpl->filepath;
            std::string to = pImpl->filepath;
            
            if (i < pImpl->maxFiles - 1) {
                from += "." + std::to_string(i + 1);
            } else {
                from += "." + std::to_string(i);
            }
            
            if (i < pImpl->maxFiles - 1) {
                to += "." + std::to_string(i + 2);
            } else {
                to += "." + std::to_string(i + 1);
            }
            
            std::filesystem::rename(from, to, ec);
        }
        
        // Create new file
        fileOut.open(pImpl->filepath, std::ios::trunc);
        currentFileSize = 0;
    }
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<LogFormatter> globalFormatter;
    std::atomic<bool> shouldRotate{false};
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    if (formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->globalFormatter = formatter;
        for (auto& logger : pImpl->loggers) {
            logger->setFormatter(formatter);
        }
    }
}

void Logger::log(LogLevel level, const std::string& message, 
                 const std::string& file, int line, 
                 const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (pImpl->globalFormatter) {
            logger->setFormatter(pImpl->globalFormatter);
        }
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Global logger singleton
static std::shared_ptr<Logger> globalLoggerPtr;
static std::mutex globalLoggerMutex;

Logger& Logger::getGlobal() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    if (!globalLoggerPtr) {
        globalLoggerPtr = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        globalLoggerPtr->addLogger(consoleLogger);
    }
    return *globalLoggerPtr;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(globalLoggerMutex);
        globalLoggerPtr = logger;
    }
}

} // namespace nlm
