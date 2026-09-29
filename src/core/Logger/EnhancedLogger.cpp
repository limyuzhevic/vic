#include "EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <ctime>

namespace nlm {

struct EnhancedConsoleLogger::Impl {
    EnhancedLogLevel level = EnhancedLogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    bool includeThreadInfo = false;
    std::string format = "[LEVEL] [CATEGORY] TIMESTAMP: MESSAGE\n";
    std::ofstream fileOut;
    std::mutex mutex;
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<LogCategory, bool> categoryFilters;
    
    Impl() {
        // Initialize all categories as enabled by default
        for (int i = 0; i < static_cast<int>(LogCategory::Performance); ++i) {
            categoryFilters[static_cast<LogCategory>(i)] = true;
        }
    }
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

EnhancedConsoleLogger::EnhancedConsoleLogger(EnhancedLogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

void EnhancedConsoleLogger::log(const EnhancedLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check category filter
    auto categoryIt = pImpl->categoryFilters.find(entry.category);
    if (categoryIt == pImpl->categoryFilters.end() || !categoryIt->second) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output = formatEntry(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m";
    } else {
        std::cout << output;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output;
        pImpl->fileOut.flush();
    }
}

void EnhancedConsoleLogger::setLevel(EnhancedLogLevel level) {
    pImpl->level = level;
}

EnhancedLogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setCategoryFilter(LogCategory category, bool enabled) {
    pImpl->categoryFilters[category] = enabled;
}

bool EnhancedConsoleLogger::isCategoryEnabled(LogCategory category) const {
    auto it = pImpl->categoryFilters.find(category);
    return it != pImpl->categoryFilters.end() && it->second;
}

void EnhancedConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string EnhancedConsoleLogger::getFormat() const {
    return pImpl->format;
}

void EnhancedConsoleLogger::setMetadata(const std::string& key, const std::string& value) {
    pImpl->metadata[key] = value;
}

std::string EnhancedConsoleLogger::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    return it != pImpl->metadata.end() ? it->second : "";
}

void EnhancedConsoleLogger::clearMetadata() {
    pImpl->metadata.clear();
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setUseTimestamps(bool useTimestamps) {
    pImpl->useTimestamps = useTimestamps;
}

bool EnhancedConsoleLogger::getUseTimestamps() const {
    return pImpl->useTimestamps;
}

void EnhancedConsoleLogger::setIncludeThreadInfo(bool includeInfo) {
    pImpl->includeThreadInfo = includeInfo;
}

bool EnhancedConsoleLogger::getIncludeThreadInfo() const {
    return pImpl->includeThreadInfo;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void EnhancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool EnhancedConsoleLogger::isFileOutputOpen() const {
    return pImpl->fileOut.is_open();
}

std::string EnhancedConsoleLogger::levelToString(EnhancedLogLevel level) {
    switch (level) {
        case EnhancedLogLevel::Trace: return "TRACE";
        case EnhancedLogLevel::Debug: return "DEBUG";
        case EnhancedLogLevel::Info: return "INFO";
        case EnhancedLogLevel::Notice: return "NOTICE";
        case EnhancedLogLevel::Warning: return "WARN";
        case EnhancedLogLevel::Error: return "ERROR";
        case EnhancedLogLevel::Critical: return "CRIT";
        case EnhancedLogLevel::Alert: return "ALERT";
        case EnhancedLogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

std::string EnhancedConsoleLogger::levelToColor(EnhancedLogLevel level) {
    switch (level) {
        case EnhancedLogLevel::Trace: return "\033[90m";    // Bright Black
        case EnhancedLogLevel::Debug: return "\033[36m";    // Cyan
        case EnhancedLogLevel::Info: return "\033[32m";      // Green
        case EnhancedLogLevel::Notice: return "\033[94m";   // Bright Blue
        case EnhancedLogLevel::Warning: return "\033[33m";   // Yellow
        case EnhancedLogLevel::Error: return "\033[31m";     // Red
        case EnhancedLogLevel::Critical: return "\033[35m"; // Magenta
        case EnhancedLogLevel::Alert: return "\033[91m";    // Bright Red
        case EnhancedLogLevel::Emergency: return "\033[95m";// Bright Magenta
        default: return "\033[0m";
    }
}

std::string EnhancedConsoleLogger::categoryToString(LogCategory category) {
    switch (category) {
        case LogCategory::All: return "ALL";
        case LogCategory::Core: return "CORE";
        case LogCategory::Brain: return "BRAIN";
        case LogCategory::Agent: return "AGENT";
        case LogCategory::Memory: return "MEMORY";
        case LogCategory::Prediction: return "PREDICTION";
        case LogCategory::Cognition: return "COGNITION";
        case LogCategory::Development: return "DEVELOPMENT";
        case LogCategory::Neuromodulation: return "NEUROMODULATION";
        case LogCategory::Plasticity: return "PLASTICITY";
        case LogCategory::Sensory: return "SENSORY";
        case LogCategory::Motor: return "MOTOR";
        case LogCategory::Environment: return "ENVIRONMENT";
        case LogCategory::Experiment: return "EXPERIMENT";
        case LogCategory::Performance: return "PERFORMANCE";
        default: return "UNKNOWN";
    }
}

std::string EnhancedConsoleLogger::formatEntry(const EnhancedLogEntry& entry) const {
    std::ostringstream oss;
    
    // Replace placeholders in format string
    std::string formatted = pImpl->format;
    
    // Basic replacements
    size_t pos = 0;
    while ((pos = formatted.find("[LEVEL]", pos)) != std::string::npos) {
        formatted.replace(pos, 8, levelToString(entry.level));
        pos += levelToString(entry.level).size();
    }
    
    pos = 0;
    while ((pos = formatted.find("[CATEGORY]", pos)) != std::string::npos) {
        formatted.replace(pos, 10, categoryToString(entry.category));
        pos += categoryToString(entry.category).size();
    }
    
    pos = 0;
    while ((pos = formatted.find("[TIMESTAMP]", pos)) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        formatted.replace(pos, 11, timeBuf);
        pos += strlen(timeBuf);
    }
    
    pos = 0;
    while ((pos = formatted.find("[MESSAGE]", pos)) != std::string::npos) {
        formatted.replace(pos, 9, entry.message);
        pos += entry.message.size();
    }
    
    // Add file, line, function info
    if (!entry.file.empty()) {
        size_t lastSlash = entry.file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) ? entry.file.substr(lastSlash + 1) : entry.file;
        
        pos = 0;
        while ((pos = formatted.find("[FILE]", pos)) != std::string::npos) {
            formatted.replace(pos, 6, filename);
            pos += filename.size();
        }
        
        pos = 0;
        while ((pos = formatted.find("[LINE]", pos)) != std::string::npos) {
            formatted.replace(pos, 6, std::to_string(entry.line));
            pos += std::to_string(entry.line).size();
        }
        
        pos = 0;
        while ((pos = formatted.find("[FUNCTION]", pos)) != std::string::npos) {
            formatted.replace(pos, 10, entry.function);
            pos += entry.function.size();
        }
    }
    
    // Add metadata
    for (const auto& [key, value] : entry.metadata) {
        pos = 0;
        std::string placeholder = "[" + key + "]";
        while ((pos = formatted.find(placeholder, pos)) != std::string::npos) {
            formatted.replace(pos, placeholder.size(), value);
            pos += value.size();
        }
    }
    
    // Add thread info if requested
    if (pImpl->includeThreadInfo) {
        pos = 0;
        while ((pos = formatted.find("[THREAD]", pos)) != std::string::npos) {
            formatted.replace(pos, 8, entry.threadName.empty() ? "main" : entry.threadName);
            pos += (entry.threadName.empty() ? 5 : entry.threadName.size());
        }
    }
    
    // Ensure newline
    if (!formatted.empty() && formatted.back() != '\n') {
        formatted += '\n';
    }
    
    return formatted;
}

struct EnhancedLogger::Impl {
    EnhancedLogLevel level = EnhancedLogLevel::Info;
    std::vector<std::shared_ptr<IEnhancedLogger>> loggers;
    std::mutex mutex;
    std::unordered_map<LogCategory, bool> categoryFilters;
    std::string format = "[LEVEL] [CATEGORY] TIMESTAMP: MESSAGE\n";
    std::unordered_map<std::string, std::string> metadata;
    
    Impl() {
        for (int i = 0; i < static_cast<int>(LogCategory::Performance); ++i) {
            categoryFilters[static_cast<LogCategory>(i)] = true;
        }
    }
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {}

void EnhancedLogger::addLogger(std::shared_ptr<IEnhancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void EnhancedLogger::removeLogger(IEnhancedLogger* logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
            [logger](const std::shared_ptr<IEnhancedLogger>& lg) {
                return lg.get() == logger;
            }),
        pImpl->loggers.end()
    );
}

void EnhancedLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void EnhancedLogger::setLevel(EnhancedLogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

EnhancedLogLevel EnhancedLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedLogger::setGlobalLevel(EnhancedLogLevel level) {
    setLevel(level);
}

void EnhancedLogger::setCategoryFilter(LogCategory category, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilters[category] = enabled;
    for (auto& logger : pImpl->loggers) {
        logger->setCategoryFilter(category, enabled);
    }
}

bool EnhancedLogger::isCategoryEnabled(LogCategory category) const {
    auto it = pImpl->categoryFilters.find(category);
    return it != pImpl->categoryFilters.end() && it->second;
}

void EnhancedLogger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
    for (auto& logger : pImpl->loggers) {
        logger->setFormat(format);
    }
}

std::string EnhancedLogger::getFormat() const {
    return pImpl->format;
}

void EnhancedLogger::log(EnhancedLogLevel level, LogCategory category, const std::string& message,
                         const std::string& file, int line, const std::string& function) {
    // Check if this level is enabled for the category
    if (!isCategoryEnabledForLevel(level, category)) {
        return;
    }
    
    EnhancedLogEntry entry(level, category, message);
    entry.file = file;
    entry.line = line;
    entry.function = function;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::setMetadata(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metadata[key] = value;
    for (auto& logger : pImpl->loggers) {
        logger->setMetadata(key, value);
    }
}

std::string EnhancedLogger::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    return it != pImpl->metadata.end() ? it->second : "";
}

void EnhancedLogger::clearMetadata() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metadata.clear();
    for (auto& logger : pImpl->loggers) {
        logger->clearMetadata();
    }
}

bool EnhancedLogger::isEnabled(EnhancedLogLevel level) const {
    return level >= pImpl->level;
}

bool EnhancedLogger::isCategoryEnabledForLevel(EnhancedLogLevel level, LogCategory category) const {
    return isEnabled(level) && isCategoryEnabled(category);
}

void EnhancedLogger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Trace, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Debug, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Info, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Notice, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Warning, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Error, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Critical, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Alert, LogCategory::All, message, file, line, function);
}

void EnhancedLogger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Emergency, LogCategory::All, message, file, line, function);
}

EnhancedLogger::LogStream::LogStream(EnhancedLogLevel level, LogCategory category,
                                    const std::string& file, int line, const std::string& function)
    : level_(level), category_(category), file_(file), line_(line), function_(function) {}

EnhancedLogger::LogStream::~LogStream() {
    EnhancedLogger::getGlobal().log(level_, category_, stream_.str(), file_, line_, function_);
}

EnhancedLogger::LogStream EnhancedLogger::trace(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Trace, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::debug(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Debug, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::info(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Info, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::notice(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Notice, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::warning(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Warning, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::error(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Error, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::critical(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Critical, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::alert(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Alert, LogCategory::All, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::emergency(const std::string& file, int line, const std::string& function) {
    return LogStream(EnhancedLogLevel::Emergency, LogCategory::All, file, line, function);
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

} // namespace nlm
