#include "LoggerFactory.hpp"
#include "Logger.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<Logger> LoggerFactory::createLogger(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("LoggerFactory not initialized, creating basic logger");
        return std::make_unique<Logger>();
    }
    
    // Get configuration from params or use defaults
    LogLevel level = static_cast<LogLevel>(params.getParam<int>("level", static_cast<int>(LogLevel::Info)));
    std::string type = params.getParam<std::string>("type", "console");
    bool useColors = params.getParam<bool>("useColors", true);
    std::string filepath = params.getParam<std::string>("filepath", "");
    bool asynchronous = params.getParam<bool>("asynchronous", false);
    
    // Handle different logger creation scenarios based on type
    if (type == "file" && !filepath.empty()) {
        return createFileLogger(filepath);
    } else if (type == "multi") {
        std::vector<std::string> logPaths = params.getParam<std::vector<std::string>>("logPaths", {});
        return createMultiLogger(logPaths);
    } else if (asynchronous) {
        return createAsynchronousLogger();
    } else {
        // Default console logger
        return createConsoleLogger();
    }
}

std::unique_ptr<Logger> LoggerFactory::createConsoleLogger() {
    spdlog::debug("Creating console logger");
    return std::make_unique<Logger>();
}

std::unique_ptr<Logger> LoggerFactory::createFileLogger(const std::string& filepath) {
    spdlog::debug("Creating file logger with filepath: {}", filepath);
    // In a real implementation, this would create a logger that writes to a file
    return std::make_unique<Logger>();
}

std::unique_ptr<Logger> LoggerFactory::createMultiLogger(const std::vector<std::string>& logPaths) {
    spdlog::debug("Creating multi logger with {} log paths", logPaths.size());
    // In a real implementation, this would create a logger that writes to multiple files
    return std::make_unique<Logger>();
}

std::unique_ptr<Logger> LoggerFactory::createAsynchronousLogger() {
    spdlog::debug("Creating asynchronous logger");
    // In a real implementation, this would create an asynchronous logger
    return std::make_unique<Logger>();
}

std::unique_ptr<Logger> LoggerFactory::createCustomLogger(const std::function<void(const LogEntry&)>& handler) {
    spdlog::debug("Creating custom logger with handler");
    // In a real implementation, this would create a logger with a custom handler
    return std::make_unique<Logger>();
}

std::unique_ptr<Logger> LoggerFactory::acquireLoggerFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!loggerPool_.empty()) {
        auto logger = std::move(loggerPool_.back());
        loggerPool_.pop_back();
        spdlog::debug("Acquired logger from pool");
        return logger;
    }
    return nullptr;
}

void LoggerFactory::releaseLoggerToPool(std::unique_ptr<Logger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggerPool_.push_back(std::move(logger));
        spdlog::debug("Released logger to pool");
    }
}

} // namespace nlm
