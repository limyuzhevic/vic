// Legacy compatibility implementations for Logger class

namespace nlm {

// Legacy Logger::addLogger implementation for backward compatibility
void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->syncLoggers.push_back(logger);
}

// Legacy Logger::clearLoggers implementation for backward compatibility
void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->syncLoggers.clear();
}

// Legacy Logger::setLevel implementation for backward compatibility
void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->syncLoggers) {
        logger->setLevel(level);
    }
    if (pImpl->asyncCore) {
        // TODO: Implement async level setting
    }
}

// Legacy Logger::debug implementation for backward compatibility
void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

// Legacy Logger::info implementation for backward compatibility
void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

// Legacy Logger::warning implementation for backward compatibility
void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

// Legacy Logger::error implementation for backward compatibility
void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

// Legacy Logger::critical implementation for backward compatibility
void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

// Legacy Logger::isEnabled implementation for backward compatibility
bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->syncLoggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

// Legacy Logger::getGlobal implementation for backward compatibility
Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

// Legacy Logger::setGlobal implementation for backward compatibility
void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// Legacy Logger::log implementation for backward compatibility
void Logger::log(const LogEntry& entry) {
    // Convert enhanced LogEntry to legacy format for backward compatibility
    log(entry.level, entry.message, entry.file, entry.line, entry.function);
}

// Legacy Logger::getLogCount implementation for backward compatibility
uint64_t Logger::getLogCount() const {
    return pImpl->logCount.load();
}

// Legacy Logger::getErrorCount implementation for backward compatibility
uint64_t Logger::getErrorCount() const {
    return pImpl->errorCount.load();
}

// Legacy Logger::getDroppedCount implementation for backward compatibility
uint64_t Logger::getDroppedCount() const {
    return pImpl->droppedCount.load();
}

// Legacy Logger::configure implementation for backward compatibility
void Logger::configure(const std::string& configJson) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->configJson = configJson;
    
    // Parse configuration and update settings
    if (pImpl->asyncCore) {
        pImpl->asyncCore->configure(configJson);
    }
}

// Legacy Logger::getConfiguration implementation for backward compatibility
std::string Logger::getConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->configJson;
}

// Legacy Logger::reloadConfiguration implementation for backward compatibility
void Logger::reloadConfiguration() {
    // Force reload of configuration from source
    if (pImpl->asyncCore) {
        pImpl->asyncCore->configure(pImpl->configJson);
    }
}

// Legacy Logger::startAsync implementation for backward compatibility
void Logger::startAsync() {
    if (pImpl->asyncCore && !pImpl->asyncEnabled) {
        pImpl->asyncCore->start();
        pImpl->asyncEnabled = true;
    }
}

// Legacy Logger::stopAsync implementation for backward compatibility
void Logger::stopAsync() {
    if (pImpl->asyncCore && pImpl->asyncEnabled) {
        pImpl->asyncCore->stop();
        pImpl->asyncEnabled = false;
    }
}

// Legacy Logger::isAsyncRunning implementation for backward compatibility
bool Logger::isAsyncRunning() const {
    return pImpl->asyncEnabled && pImpl->asyncCore && pImpl->asyncCore->isRunning();
}

// Legacy Logger::isHealthy implementation for backward compatibility
bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    bool syncHealthy = std::all_of(pImpl->syncLoggers.begin(), pImpl->syncLoggers.end(),
        [](const std::shared_ptr<ILogger>& logger) { return logger->isHealthy(); });
    
    bool asyncHealthy = !pImpl->asyncEnabled || (pImpl->asyncCore && pImpl->asyncCore->isHealthy());
    
    return syncHealthy && asyncHealthy;
}

// Legacy Logger::getHealthStatus implementation for backward compatibility
std::string Logger::getHealthStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "LogCount: " << pImpl->logCount.load() << "\n"
        << "ErrorCount: " << pImpl->errorCount.load() << "\n"
        << "DroppedCount: " << pImpl->droppedCount.load() << "\n"
        << "AsyncRunning: " << (pImpl->asyncEnabled && pImpl->asyncCore ? "true" : "false") << "\n"
        << "SyncLoggers: " << pImpl->syncLoggers.size() << "\n"
        << "CorrelationId: " << pImpl->currentCorrelationId << "\n";
    return oss.str();
}

// Legacy Logger::createCorrelationId implementation for backward compatibility
std::string Logger::createCorrelationId() {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    pImpl->currentCorrelationId = utils::generateCorrelationId();
    return pImpl->currentCorrelationId;
}

// Legacy Logger::setCorrelationId implementation for backward compatibility
void Logger::setCorrelationId(const std::string& correlationId) {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    pImpl->currentCorrelationId = correlationId;
}

// Legacy Logger::getCorrelationId implementation for backward compatibility
std::string Logger::getCorrelationId() const {
    std::lock_guard<std::mutex> lock(pImpl->correlationMutex);
    return pImpl->currentCorrelationId;
}

} // namespace nlm