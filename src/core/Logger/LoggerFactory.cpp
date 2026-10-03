// LoggerFactory implementation

LoggerFactory::LoggerFactory() = default;

LoggerFactory::~LoggerFactory() = default;

std::shared_ptr<ILogger> LoggerFactory::createConsoleLogger(LogLevel level) {
    return std::make_shared<ConsoleLogger>(level);
}

std::shared_ptr<ILogger> LoggerFactory::createAsyncLogger(size_t threadCount, size_t queueSize) {
    return std::make_shared<AsyncLogger>(threadCount, queueSize);
}

std::shared_ptr<ILogger> LoggerFactory::createFileLogger(const std::string& filepath, const RotationConfig& rotation) {
    auto logger = std::make_shared<FileLogger>();
    if (logger->open(filepath, rotation)) {
        return logger;
    }
    return nullptr;
}

std::shared_ptr<ILogger> LoggerFactory::createMemoryLogger() {
    return std::make_shared<MemoryLogger>();
}

std::shared_ptr<ILogger> LoggerFactory::createStructLogger(OutputFormat format) {
    return std::make_shared<StructLogger>(format);
}

std::shared_ptr<ILogger> LoggerFactory::createNetworkLogger(const std::string& host, uint16_t port) {
    auto logger = std::make_shared<NetworkLogger>();
    if (logger->connect(host, port)) {
        return logger;
    }
    return nullptr;
}

std::shared_ptr<ILogger> LoggerFactory::createCustomLogger(const std::string& type, const std::unordered_map<std::string, std::string>& config) {
    // This is a placeholder for plugin-based logger creation
    // In a real implementation, this would use the plugin system
    if (type == "console") {
        return createConsoleLogger();
    } else if (type == "async") {
        return createAsyncLogger();
    } else if (type == "file") {
        auto it = config.find("filepath");
        if (it != config.end()) {
            RotationConfig rotation;
            auto sizeIt = config.find("max_size");
            if (sizeIt != config.end()) {
                rotation.maxFileSize = std::stoull(sizeIt->second);
            }
            auto filesIt = config.find("max_files");
            if (filesIt != config.end()) {
                rotation.maxFiles = std::stoi(filesIt->second);
            }
            return createFileLogger(it->second, rotation);
        }
    }
    return nullptr;
}

// Performance metrics implementation

PerformanceMetrics::Metrics PerformanceMetrics::metrics_ = PerformanceMetrics::Metrics();
std::mutex PerformanceMetrics::metricsMutex_;

void PerformanceMetrics::recordLog(const LogEntry& entry, bool dropped, double logTime) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    metrics_.logCount++;
    if (dropped) {
        metrics_.droppedLogs++;
    }
    if (metrics_.logCount > 0) {
        metrics_.averageLogTime = (metrics_.averageLogTime * (metrics_.logCount - 1) + logTime) / metrics_.logCount;
    }
    metrics_.logByCategory[entry.category]++;
    metrics_.logByLevel[entry.level]++;
}

PerformanceMetrics::Metrics PerformanceMetrics::getMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return metrics_;
}

void PerformanceMetrics::resetMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    metrics_ = Metrics();
}

std::string PerformanceMetrics::getMetricsReport() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    std::stringstream ss;
    ss << "=== Performance Metrics ===" << std::endl;
    ss << "Total logs: " << metrics_.logCount << std::endl;
    ss << "Dropped logs: " << metrics_.droppedLogs << std::endl;
    ss << "Average log time: " << metrics_.averageLogTime << " ns" << std::endl;
    ss << "\nLogs by category:" << std::endl;
    for (const auto& pair : metrics_.logByCategory) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    ss << "\nLogs by level:" << std::endl;
    for (int i = static_cast<int>(LogLevel::Debug); i <= static_cast<int>(LogLevel::Critical); ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        if (metrics_.logByLevel.find(level) != metrics_.logByLevel.end()) {
            ss << "  " << static_cast<int>(level) << ": " << metrics_.logByLevel[level] << std::endl;
        }
    }
    return ss.str();
}

// Global log manager implementation

std::mutex GlobalLogManager::mutex_;
LogLevel GlobalLogManager::defaultLevel_ = LogLevel::Info;
std::unordered_map<std::string, bool> GlobalLogManager::categoryEnabled_;
bool GlobalLogManager::synchronizedLogging_ = true;
std::string GlobalLogManager::logFile_;

void GlobalLogManager::setDefaultLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    defaultLevel_ = level;
}

LogLevel GlobalLogManager::getDefaultLevel() {
    std::lock_guard<std::mutex> lock(mutex_);
    return defaultLevel_;
}

void GlobalLogManager::setCategoryEnabled(const std::string& category, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    categoryEnabled_[category] = enabled;
}

bool GlobalLogManager::isCategoryEnabled(const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = categoryEnabled_.find(category);
    if (it != categoryEnabled_.end()) {
        return it->second;
    }
    return true; // Default: categories are enabled
}

void GlobalLogManager::enableSynchronizedLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    synchronizedLogging_ = enabled;
}

void GlobalLogManager::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    logFile_ = filepath;
}

void GlobalLogManager::initializeFromConfig() {
    // This would be implemented to read from config system
    // For now, it's a placeholder
}