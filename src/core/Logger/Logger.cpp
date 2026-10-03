#include "Logger.hpp"
#include <fstream>
#include <filesystem>

namespace nlm {

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
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
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
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
        case LogLevel::Debug: return "\033[36m";
        case LogLevel::Info: return "\033[32m";
        case LogLevel::Warning: return "\033[33m";
        case LogLevel::Error: return "\033[31m";
        case LogLevel::Critical: return "\033[35m";
        default: return "\033[0m";
    }
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;
    size_t maxFileCount = 5;
    std::chrono::system_clock::time_point rotationTime;
    std::string baseFilename;
    size_t currentFileSize = 0;
};

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    pImpl->baseFilename = filepath;
    pImpl->rotationTime = std::chrono::system_clock::now();
    rotate();
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->baseFilename = filepath;
    pImpl->rotationTime = std::chrono::system_clock::now();
    rotate();
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1;
    
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
    }
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFileCount(size_t maxCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileCount = maxCount;
}

void FileLogger::setRotationTime(const std::chrono::system_clock::time_point& time) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationTime = time;
}

void FileLogger::rotate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string newFilename = rotateFilename(pImpl->baseFilename, timestamp) + ".log";
    
    pImpl->fileOut.open(newFilename, std::ios::app);
    pImpl->currentFileSize = 0;
    pImpl->rotationTime = std::chrono::system_clock::now();
}

std::string FileLogger::getLogFilePath() const {
    return "";
}

std::string FileLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// Helper functions for FileLogger
namespace {
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    std::string rotateFilename(const std::string& original, const std::string& timestamp) {
        size_t dotPos = original.find_last_of('.');
        if (dotPos == std::string::npos) {
            return original + "_" + timestamp;
        }
        return original.substr(0, dotPos) + "_" + timestamp + 
               original.substr(dotPos);
    }
}

// NetworkLogger implementation (placeholder)
struct NetworkLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::string host;
    int port;
    std::mutex mutex;
};

NetworkLogger::NetworkLogger(const std::string& host, int port) : pImpl(std::make_unique<Impl>()) {
    pImpl->host = host;
    pImpl->port = port;
}

NetworkLogger::~NetworkLogger() = default;

void NetworkLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::cout << "[NETWORK] " << levelToString(entry.level) << ": " << entry.message << std::endl;
}

void NetworkLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel NetworkLogger::getLevel() const {
    return pImpl->level;
}

// DatabaseLogger implementation (placeholder)
struct DatabaseLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::string connectionString;
    std::mutex mutex;
};

DatabaseLogger::DatabaseLogger(const std::string& connectionString) : pImpl(std::make_unique<Impl>()) {
    pImpl->connectionString = connectionString;
}

DatabaseLogger::~DatabaseLogger() = default;

void DatabaseLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::cout << "[DATABASE] " << levelToString(entry.level) << ": " << entry.message << std::endl;
}

void DatabaseLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel DatabaseLogger::getLevel() const {
    return pImpl->level;
}

// AsyncLogger implementation (placeholder)
struct AsyncLogger::Impl {
    std::mutex queueMutex_;
    std::vector<std::pair<std::string, LogLevel>> logQueue_;
    size_t maxQueueSize_;
    std::shared_ptr<Logger> currentLogger_;
};

AsyncLogger::AsyncLogger(size_t maxQueueSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxQueueSize_ = maxQueueSize;
}

AsyncLogger::~AsyncLogger() = default;

void AsyncLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex_);
    pImpl->logQueue_.push_back({entry.message, entry.level});
    if (pImpl->logQueue_.size() >= pImpl->maxQueueSize_) {
        flush();
    }
}

void AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex_);
    if (!pImpl->logQueue_.empty() && pImpl->currentLogger_) {
        for (const auto& logEntry : pImpl->logQueue_) {
            std::thread([this, logEntry]() {
                pImpl->currentLogger_->log(LogLevel::Info, logEntry.first);
            }).detach();
        }
        pImpl->logQueue_.clear();
    }
}

void AsyncLogger::setCurrentLogger(std::shared_ptr<Logger> logger) {
    pImpl->currentLogger_ = logger;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
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

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
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

Logger& Logger::getGlobal() {
    std::lock_guard<std::mutex> lock(globalMutex_);
    if (!globalLogger_) {
        initializeDefaultGlobalLogger();
    }
    return *globalLogger_;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (!logger) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(globalMutex_);
    globalLogger_ = logger;
}

void Logger::initializeDefaultGlobalLogger() {
    auto logger = std::make_shared<Logger>();
    logger->addLogger(std::make_shared<ConsoleLogger>());
    setGlobal(logger);
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm

int main() {
    return 0;
}
