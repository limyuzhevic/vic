#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

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
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
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
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation with hierarchical logging
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<HierarchicalLogger> hierarchicalLogger;
    std::unordered_map<std::string, std::shared_ptr<ILogger>> legacyLoggers;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->hierarchicalLogger = std::make_shared<HierarchicalLogger>();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    pImpl->legacyLoggers[logger->getLevel() == LogLevel::Debug ? "legacy_debug" : 
                        logger->getLevel() == LogLevel::Info ? "legacy_info" :
                        logger->getLevel() == LogLevel::Warning ? "legacy_warning" :
                        logger->getLevel() == LogLevel::Error ? "legacy_error" : "legacy_critical"] = logger;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->legacyLoggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
    pImpl->hierarchicalLogger->setGlobalLevel(level);
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& component, const std::string& parentComponent) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create log entry with component information
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.component = component;
    entry.parentComponent = parentComponent;
    
    // First, log to legacy loggers for backward compatibility
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Then, log to hierarchical logger
    pImpl->hierarchicalLogger->log(entry);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function,
                   const std::string& component, const std::string& parentComponent) {
    log(LogLevel::Debug, message, file, line, function, component, parentComponent);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function,
                  const std::string& component, const std::string& parentComponent) {
    log(LogLevel::Info, message, file, line, function, component, parentComponent);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function,
                     const std::string& component, const std::string& parentComponent) {
    log(LogLevel::Warning, message, file, line, function, component, parentComponent);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function,
                   const std::string& component, const std::string& parentComponent) {
    log(LogLevel::Error, message, file, line, function, component, parentComponent);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function,
                      const std::string& component, const std::string& parentComponent) {
    log(LogLevel::Critical, message, file, line, function, component, parentComponent);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check legacy loggers
    for (auto& [name, logger] : pImpl->legacyLoggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    
    // Check hierarchical logger
    return pImpl->hierarchicalLogger->shouldLog("", LogEntry{level, "", "", 0, "", {}, "", ""});
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::enableComponent(const std::string& componentName, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->hierarchicalLogger->setComponentEnabled(componentName, enabled);
}

void Logger::setComponentLevel(const std::string& componentName, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto componentLogger = pImpl->hierarchicalLogger->getComponentLogger(componentName);
    if (componentLogger) {
        componentLogger->setLevel(level);
    }
}

void Logger::setComponentFilter(const std::string& componentName, 
                               const std::function<bool(const std::string&, const LogEntry&)>& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto componentLogger = pImpl->hierarchicalLogger->getComponentLogger(componentName);
    if (componentLogger) {
        componentLogger->setFilter(filter);
    }
}

void Logger::registerComponentLogger(const std::string& componentName,
                                     const std::string& parentComponent,
                                     LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->hierarchicalLogger->registerComponentLogger(componentName, parentComponent, level);
}

std::shared_ptr<ComponentLogger> Logger::getComponentLogger(const std::string& componentName,
                                                             const std::string& parentComponent) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->hierarchicalLogger->getComponentLogger(componentName, parentComponent);
}

std::shared_ptr<HierarchicalLogger> Logger::getHierarchicalLogger() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->hierarchicalLogger;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), component_("") {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, component_);
}

void LogStream::setComponent(const std::string& component) {
    component_ = component;
}

} // namespace nlm
