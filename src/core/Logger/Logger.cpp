// NLM (熙然) - Neural Learning Machine
// Logger Implementation
// Implementation of the enhanced Logger class

#include "core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>

using namespace nlm;

// Logger implementation
void ConsoleLogger::log(LogLevel level, const std::string& message) {
    if (level < this->level_) return;
    
    LogMessage logMsg(level, message);
    std::cout << formatter_->format(logMsg) << std::endl;
}

void ConsoleLogger::logWithMetadata(LogLevel level, const std::string& message,
                                     const std::map<std::string, std::string>& metadata) {
    if (level < this->level_) return;
    
    LogMessage logMsg(level, message);
    logMsg.metadata = metadata;
    std::cout << formatter_->format(logMsg) << std::endl;
}

// File logger implementation
void FileLogger::log(LogLevel level, const std::string& message) {
    if (level < this->level_) return;
    
    LogMessage logMsg(level, message);
    fileStream_ << formatter_->format(logMsg) << std::endl;
    fileStream_.flush();
}

void FileLogger::logWithMetadata(LogLevel level, const std::string& message,
                                 const std::map<std::string, std::string>& metadata) {
    if (level < this->level_) return;
    
    LogMessage logMsg(level, message);
    logMsg.metadata = metadata;
    fileStream_ << formatter_->format(logMsg) << std::endl;
    fileStream_.flush();
}

// Logger manager implementation
LoggerManager& LoggerManager::getInstance() {
    static LoggerManager instance;
    return instance;
}

void LoggerManager::addLogger(const std::shared_ptr<Logger>& logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.push_back(logger);
}

void LoggerManager::removeLogger(const std::shared_ptr<Logger>& logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(loggers_.begin(), loggers_.end(), logger);
    if (it != loggers_.end()) {
        loggers_.erase(it);
    }
}

void LoggerManager::clearLoggers() {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.clear();
}

void LoggerManager::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& logger : loggers_) {
        logger->log(level, message);
    }
}

void LoggerManager::logWithMetadata(LogLevel level, const std::string& message,
                                     const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& logger : loggers_) {
        logger->logWithMetadata(level, message, metadata);
    }
}

void LoggerManager::setGlobalLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& logger : loggers_) {
        logger->setLevel(level);
    }
}

std::vector<std::string> LoggerManager::getLoggerNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    for (const auto& logger : loggers_) {
        names.push_back(logger->getName());
    }
    return names;
}

void LoggerManager::setLogLevelByName(const std::string& loggerName, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& logger : loggers_) {
        if (logger->getName() == loggerName) {
            logger->setLevel(level);
            break;
        }
    }
}

} // namespace nlm
