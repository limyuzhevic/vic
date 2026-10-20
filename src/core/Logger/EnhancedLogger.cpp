#include "EnhancedLogger.hpp"
#include <random>

namespace nlm {

// Configuration helper functions
static LogConfig createDefaultConfig() {
    LogConfig config;
    config.globalLevel = EnhancedLogLevel::Info;
    config.useEnhancedLevels = true;
    config.useAsyncLogging = true;
    config.asyncQueueSize = 10000;
    config.maxFileSize = 10 * 1024 * 1024;
    config.compressLogs = false;
    config.rotationTime = std::chrono::hours(1);
    config.maxRotatedFiles = 5;
    config.useLockFreeQueue = true;
    config.bufferPoolSize = 256;
    config.sampleRate = 1.0f;
    config.logSystemMetrics = true;
    config.logPerformanceMetrics = true;
    config.logGpuEvents = false;
    
    // Add default categories
    config.categories.push_back({"Brain", EnhancedLogLevel::Info, EnhancedLogLevel::Fatal, 1000});
    config.categories.push_back({"Memory", EnhancedLogLevel::Warning, EnhancedLogLevel::Fatal, 500});
    config.categories.push_back({"Neuromodulation", EnhancedLogLevel::Info, EnhancedLogLevel::Critical, 500});
    config.categories.push_back({"Cognition", EnhancedLogLevel::Debug, EnhancedLogLevel::Fatal, 2000});
    config.categories.push_back({"Development", EnhancedLogLevel::Info, EnhancedLogLevel::Warning, 200});
    config.categories.push_back({"Performance", EnhancedLogLevel::Debug, EnhancedLogLevel::Info, 100});
    config.categories.push_back({"System", EnhancedLogLevel::Info, EnhancedLogLevel::Fatal, 500});
    
    return config;
}

// Enhanced logging utilities
static bool shouldLogCategory(const std::string& category, EnhancedLogLevel level) {
    static std::unordered_map<std::string, CategoryFilter> categoryConfigs = {
        {"Brain", {"Brain", EnhancedLogLevel::Info, EnhancedLogLevel::Fatal, 1000}},
        {"Memory", {"Memory", EnhancedLogLevel::Warning, EnhancedLogLevel::Fatal, 500}},
        {"Neuromodulation", {"Neuromodulation", EnhancedLogLevel::Info, EnhancedLogLevel::Critical, 500}},
        {"Cognition", {"Cognition", EnhancedLogLevel::Debug, EnhancedLogLevel::Fatal, 2000}},
        {"Development", {"Development", EnhancedLogLevel::Info, EnhancedLogLevel::Warning, 200}},
        {"Performance", {"Performance", EnhancedLogLevel::Debug, EnhancedLogLevel::Info, 100}},
        {"System", {"System", EnhancedLogLevel::Info, EnhancedLogLevel::Fatal, 500}},
        {"Debug", {"Debug", EnhancedLogLevel::Trace, EnhancedLogLevel::Debug, 0}},
        {"All", {"All", EnhancedLogLevel::Trace, EnhancedLogLevel::Fatal, 0}}
    };
    
    auto it = categoryConfigs.find(category);
    if (it == categoryConfigs.end()) {
        return true;  // Unknown category - log it
    }
    
    const auto& cat = it->second;
    return level >= cat.minLevel && level <= cat.maxLevel;
}

static size_t getAvailableMemory() {
    // Implementation would depend on platform
    // This is a simplified placeholder
    return 0;
}

static int getCurrentThreadId() {
    std::hash<std::thread::id> hasher;
    return static_cast<int>(hasher(std::this_thread::get_id()));
}

// LogStream implementation
LogStream::LogStream(EnhancedLogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<AsyncLogger> asyncLogger;
    LogConfig logConfig;
    std::atomic<bool> useAsync = true;
    
    Impl() {
        logConfig = createDefaultConfig();
        asyncLogger = std::make_shared<AsyncLogger>(logConfig);
        useAsync = true;
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add the async logger as a sink
    pImpl->asyncLogger->setLevel(static_cast<LogLevel>(static_cast<int>(pImpl->logConfig.globalLevel)));
}

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
    if (pImpl->asyncLogger) {
        pImpl->asyncLogger->setLevel(level);
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
    
    // Determine category based on file path or caller
    std::string category = determineCategory(file);
    
    if (pImpl->useAsync && pImpl->asyncLogger) {
        // Use async logging for performance
        pImpl->asyncLogger->log(entry);
    } else {
        // Synchronous logging
        for (auto& logger : pImpl->loggers) {
            logger->log(entry);
        }
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
    if (pImpl->asyncLogger && pImpl->asyncLogger->getLevel() <= level) {
        return true;
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

std::string Logger::determineCategory(const std::string& file) {
    std::string lowerFile = file;
    std::transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), ::tolower);
    
    if (lowerFile.find("brain") != std::string::npos) return "Brain";
    if (lowerFile.find("memory") != std::string::npos) return "Memory";
    if (lowerFile.find("neuromod") != std::string::npos) return "Neuromodulation";
    if (lowerFile.find("cognition") != std::string::npos) return "Cognition";
    if (lowerFile.find("development") != std::string::npos) return "Development";
    if (lowerFile.find("performance") != std::string::npos) return "Performance";
    
    return "System";
}

// New enhanced logging functions
void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Trace, message, file, line, function);
}

void Logger::fatal(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Fatal, message, file, line, function);
}

void Logger::logEnhanced(const EnhancedLogEntry& entry) {
    // Add enhanced logging functionality
    LogEntry legacyEntry;
    legacyEntry.level = static_cast<LogLevel>(static_cast<int>(entry.level));
    legacyEntry.message = entry.message;
    legacyEntry.file = entry.file;
    legacyEntry.line = entry.line;
    legacyEntry.function = entry.function;
    legacyEntry.timestamp = entry.timestamp;
    
    log(legacyEntry.level, legacyEntry.message, legacyEntry.file, 
        legacyEntry.line, legacyEntry.function);
}

void Logger::setCategoryLevel(const std::string& category, LogLevel level) {
    EnhancedLogLevel enhancedLevel;
    switch (level) {
        case LogLevel::Debug: enhancedLevel = EnhancedLogLevel::Debug; break;
        case LogLevel::Info: enhancedLevel = EnhancedLogLevel::Info; break;
        case LogLevel::Warning: enhancedLevel = EnhancedLogLevel::Warning; break;
        case LogLevel::Error: enhancedLevel = EnhancedLogLevel::Error; break;
        case LogLevel::Critical: enhancedLevel = EnhancedLogLevel::Critical; break;
        default: enhancedLevel = EnhancedLogLevel::Info; break;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& categoryFilter : pImpl->logConfig.categories) {
        if (categoryFilter.name == category) {
            categoryFilter.minLevel = enhancedLevel;
            categoryFilter.maxLevel = enhancedLevel;
            break;
        }
    }
}

void Logger::enableCategory(const std::string& category, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& catFilter : pImpl->logConfig.categories) {
        if (catFilter.name == category) {
            catFilter.enabled = enabled;
            break;
        }
    }
}

void Logger::setLogConfig(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logConfig = config;
    if (pImpl->asyncLogger) {
        pImpl->asyncLogger->setConfig(config);
    }
}

LogConfig Logger::getLogConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logConfig;
}

void Logger::enableAsyncLogging(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useAsync = enable;
    if (pImpl->asyncLogger) {
        // TODO: Update async logger configuration
    }
}

// Convenience macros for enhanced logging
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FATAL(msg) nlm::Logger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__)

// Macro for category-specific logging
#define NLM_LOG_CATEGORY(category, level, msg) do { \
    auto& logger = nlm::Logger::getGlobal(); \
    logger.setCategoryLevel(category, level); \
    logger.enableCategory(category, true); \
    logger.log(level, msg, __FILE__, __LINE__, __func__); \
} while(0)

} // namespace nlm
