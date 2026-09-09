// Implementation of Logger core functionality
namespace nlm {

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::vector<std::shared_ptr<IAsyncLogger>> asyncLoggers;
    std::mutex mutex;
    std::shared_mutex categoryMutex;
    std::map<std::string, bool> categoryFilters;
    std::map<std::string, std::mutex> categoryMutexes;
    LogEntryPool& entryPool;
    std::mutex poolMutex;
    LoggingConfig config;
    LoggerStats globalStats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default categories
    pImpl->categoryFilters[LogCategory::All] = true;
    pImpl->categoryFilters[LogCategory::Neural] = true;
    pImpl->categoryFilters[LogCategory::Memory] = true;
    pImpl->categoryFilters[LogCategory::Sensory] = true;
    pImpl->categoryFilters[LogCategory::Motor] = true;
    pImpl->categoryFilters[LogCategory::Cognition] = true;
    pImpl->categoryFilters[LogCategory::Plasticity] = true;
    pImpl->categoryFilters[LogCategory::Development] = true;
    pImpl->categoryFilters[LogCategory::Neuromodulation] = true;
    pImpl->categoryFilters[LogCategory::Experiment] = true;
    pImpl->categoryFilters[LogCategory::Performance] = true;
    pImpl->categoryFilters[LogCategory::Config] = true;
    pImpl->categoryFilters[LogCategory::System] = true;
    
    pImpl->entryPool = getEntryPool();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::addAsyncLogger(std::shared_ptr<IAsyncLogger> asyncLogger) {
    if (!asyncLogger) return;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->asyncLoggers.push_back(asyncLogger);
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

void Logger::setCategoryFilter(const std::string& category, bool enabled) {
    std::unique_lock lock(pImpl->categoryMutex);
    pImpl->categoryFilters[category] = enabled;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& category,
                 const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    // Check if level is enabled globally
    if (!isEnabled(level)) {
        std::atomic_fetch_add(&pImpl->globalStats.messagesFiltered, 1);
        return;
    }
    
    // Check category filter
    if (!isCategoryEnabled(category)) {
        std::atomic_fetch_add(&pImpl->globalStats.messagesFiltered, 1);
        return;
    }
    
    // Get log entry from pool
    LogEntry* entry = nullptr;
    {
        std::lock_guard<std::mutex> lock(pImpl->poolMutex);
        entry = pImpl->entryPool.acquire();
    }
    
    if (!entry) {
        // Fallback to stack allocation if pool is empty
        entry = new LogEntry;
        std::atomic_fetch_add(&pImpl->globalStats.entriesAllocated, 1);
    }
    
    // Initialize entry
    entry->level = level;
    entry->message = message;
    entry->file = file;
    entry->line = line;
    entry->function = function;
    entry->timestamp = std::chrono::system_clock::now();
    entry->threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry->category = category;
    entry->kvPairs = kvPairs;
    entry->neuronId.clear();
    entry->simulationStep.clear();
    entry->regionId.clear();
    entry->entryTime = std::chrono::high_resolution_clock::now();
    entry->messageSize = message.size() + kvPairs.size() * 50; // Approximate size
    
    // Update statistics
    std::atomic_fetch_add(&pImpl->globalStats.messagesLogged, 1);
    pImpl->globalStats.messagesByLevel[level]++;
    pImpl->globalStats.messagesByCategory[category]++;
    
    // Send to sync loggers
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            logger->log(*entry);
        }
    }
    
    // Send to async loggers
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->asyncLoggers) {
            logger->logAsync(*entry);
        }
    }
    
    // Release entry back to pool
    {
        std::lock_guard<std::mutex> lock(pImpl->poolMutex);
        pImpl->entryPool.release(entry);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& category,
                 const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(level, message, "", 0, "", category, kvPairs);
}

void Logger::logWithLocation(LogLevel level, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function,
                             const std::string& category,
                             const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(level, message, file, line, function, category, kvPairs);
}

// Forward declaration for global logger management
static std::shared_ptr<Logger> globalLogger;
static std::mutex globalLoggerMutex;
static LogEntryPool globalEntryPool;

Logger& Logger::getGlobal() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    if (!globalLogger) {
        globalLogger = std::make_shared<Logger>();
    }
    return *globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    globalLogger = logger;
}

LoggerStats Logger::getGlobalStats() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    LoggerStats stats;
    if (globalLogger) {
        stats = globalLogger->pImpl->globalStats;
    }
    return stats;
}

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    if (globalLogger) {
        for (auto& logger : globalLogger->pImpl->loggers) {
            logger->flush();
        }
    }
}

void Logger::waitForAsyncCompletion() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex);
    if (globalLogger) {
        for (auto& logger : globalLogger->pImpl->asyncLoggers) {
            logger->waitForCompletion();
        }
    }
}

LogEntryPool& Logger::getEntryPool() {
    return globalEntryPool;
}

void Logger::setEntryPoolSize(size_t maxEntries) {
    globalEntryPool.clear();
    globalEntryPool = LogEntryPool(maxEntries);
}

// Convenience implementations
void Logger::trace(const std::string& message, const std::string& category,
                  const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Trace, message, "", 0, "", category, kvPairs);
}

void Logger::traceWithLocation(const std::string& message, const std::string& file, int line,
                               const std::string& function, const std::string& category,
                               const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Trace, message, file, line, function, category, kvPairs);
}

void Logger::debug(const std::string& message, const std::string& category,
                   const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Debug, message, "", 0, "", category, kvPairs);
}

void Logger::debugWithLocation(const std::string& message, const std::string& file, int line,
                               const std::string& function, const std::string& category,
                               const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Debug, message, file, line, function, category, kvPairs);
}

void Logger::info(const std::string& message, const std::string& category,
                  const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Info, message, "", 0, "", category, kvPairs);
}

void Logger::infoWithLocation(const std::string& message, const std::string& file, int line,
                              const std::string& function, const std::string& category,
                              const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Info, message, file, line, function, category, kvPairs);
}

void Logger::warning(const std::string& message, const std::string& category,
                     const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Warning, message, "", 0, "", category, kvPairs);
}

void Logger::warningWithLocation(const std::string& message, const std::string& file, int line,
                                 const std::string& function, const std::string& category,
                                 const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Warning, message, file, line, function, category, kvPairs);
}

void Logger::error(const std::string& message, const std::string& category,
                   const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Error, message, "", 0, "", category, kvPairs);
}

void Logger::errorWithLocation(const std::string& message, const std::string& file, int line,
                               const std::string& function, const std::string& category,
                               const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Error, message, file, line, function, category, kvPairs);
}

void Logger::critical(const std::string& message, const std::string& category,
                      const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    log(LogLevel::Critical, message, "", 0, "", category, kvPairs);
}

void Logger::criticalWithLocation(const std::string& message, const std::string& file, int line,
                                  const std::string& function, const std::string& category,
                                  const std::vector<std::pair<std::string, std::string>>& kvPairs) {
    logWithLocation(LogLevel::Critical, message, file, line, function, category, kvPairs);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isLevelEnabled(level)) {
            return true;
        }
    }
    return false;
}

bool Logger::isCategoryEnabled(const std::string& category) const {
    std::shared_lock lock(pImpl->categoryMutex);
    auto it = pImpl->categoryFilters.find(category);
    if (it != pImpl->categoryFilters.end()) {
        return it->second;
    }
    return false;
}

} // namespace nlm