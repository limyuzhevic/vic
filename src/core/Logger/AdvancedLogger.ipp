// Include this file for the implementation of advanced logging
#include "AdvancedLogger.hpp"
#include <thread>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <ctime>

namespace nlm {

// ===============================================================
// CONCISE LOGGER IMPLEMENTATION
// ===============================================================

struct ConciseLogger::Impl {
    AdvancedLogLevel level = AdvancedLogLevel::Info;
    std::string outputFormat = "standard";
    bool useColors = true;
    bool useJson = false;
    bool asyncEnabled = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    
    std::string outputFile;
    std::ofstream fileOut;
    
    // Category management
    std::unordered_set<std::string> enabledCategories;
    std::mutex categoryMutex;
    
    // Performance monitoring
    bool performanceLoggingEnabled = false;
    double performanceSampleRate = 0.1; // 10% sample rate
    
    // Statistics
    size_t totalLogCount = 0;
    double totalLogTime = 0.0;
    std::unordered_map<std::string, size_t> categoryStats;
    
    // Filtering
    std::vector<std::function<bool(const StructuredLogEntry&)>> filters;
    
    // Async logging
    std::queue<StructuredLogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::thread asyncThread;
    std::atomic<bool> asyncRunning;
    
    // Thread info
    std::thread::id mainThreadId;
};

ConciseLogger::ConciseLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->mainThreadId = std::this_thread::get_id();
    pImpl->asyncRunning = false;
}

ConciseLogger::ConciseLogger(AdvancedLogLevel level) : ConciseLogger() {
    pImpl->level = level;
}

ConciseLogger::~ConciseLogger() {
    if (pImpl->asyncEnabled && pImpl->asyncRunning) {
        pImpl->asyncRunning = false;
        pImpl->queueCV.notify_one();
        if (pImpl->asyncThread.joinable()) {
            pImpl->asyncThread.join();
        }
    }
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConciseLogger::log(const StructuredLogEntry& entry) {
    // Apply filters
    for (const auto& filter : pImpl->filters) {
        if (!filter(entry)) {
            return;
        }
    }
    
    // Check level and category
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!pImpl->enabledCategories.empty() && 
        pImpl->enabledCategories.find(entry.category) == pImpl->enabledCategories.end()) {
        return;
    }
    
    // Performance sampling
    if (pImpl->performanceLoggingEnabled && pImpl->level >= AdvancedLogLevel::Performance) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) > pImpl->performanceSampleRate) {
            return;
        }
    }
    
    // Log time measurement
    auto start = std::chrono::high_resolution_clock::now();
    
    // Format entry
    std::string formattedEntry;
    if (pImpl->useJson) {
        formattedEntry = formatJsonEntry(entry);
    } else if (pImpl->outputFormat == "concise") {
        formattedEntry = formatConciseEntry(entry);
    } else {
        formattedEntry = formatEntry(entry);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    
    // Update statistics
    pImpl->totalLogCount++;
    pImpl->totalLogTime += duration;
    pImpl->categoryStats[entry.category]++;
    
    // Output
    if (pImpl->asyncEnabled) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->logQueue.push(entry);
        pImpl->queueCV.notify_one();
    } else {
        std::cout << formattedEntry << std::endl;
        writeToFile(entry);
    }
}

void ConciseLogger::log(AdvancedLogLevel level, const std::string& message) {
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = "general";
    entry.subcategory = "";
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void ConciseLogger::log(AdvancedLogLevel level, const std::string& category, 
                       const std::string& message) {
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = category;
    entry.subcategory = "";
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void ConciseLogger::log(AdvancedLogLevel level, const std::string& category,
                       const std::string& subcategory, const std::string& message) {
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = category;
    entry.subcategory = subcategory;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry);
}

void ConciseLogger::logWithMetadata(AdvancedLogLevel level, const std::string& message,
                                    const std::unordered_map<std::string, std::string>& metadata) {
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = "general";
    entry.subcategory = "";
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    log(entry);
}

void ConciseLogger::logWithMetrics(AdvancedLogLevel level, const std::string& message,
                                   const std::unordered_map<std::string, double>& metrics) {
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = "general";
    entry.subcategory = "";
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    entry.metrics = metrics;
    
    log(entry);
}

void ConciseLogger::setLevel(AdvancedLogLevel level) {
    pImpl->level = level;
}

AdvancedLogLevel ConciseLogger::getLevel() const {
    return pImpl->level;
}

void ConciseLogger::addCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->categoryMutex);
    pImpl->enabledCategories.insert(category);
}

void ConciseLogger::removeCategory(const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->categoryMutex);
    pImpl->enabledCategories.erase(category);
}

bool ConciseLogger::isCategoryEnabled(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->categoryMutex);
    return pImpl->enabledCategories.empty() || 
           pImpl->enabledCategories.find(category) != pImpl->enabledCategories.end();
}

void ConciseLogger::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
    pImpl->useJson = (format == "json");
}

void ConciseLogger::enableColors(bool enable) {
    pImpl->useColors = enable;
}

bool ConciseLogger::getColorsEnabled() const {
    return pImpl->useColors;
}

void ConciseLogger::setOutputFile(const std::string& filepath) {
    pImpl->outputFile = filepath;
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConciseLogger::setLogRotation(size_t maxFileSize, size_t maxFiles) {
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
}

void ConciseLogger::enableAsynchronousLogging(bool enable) {
    if (enable && !pImpl->asyncRunning) {
        pImpl->asyncRunning = true;
        pImpl->asyncThread = std::thread(&ConciseLogger::asyncLogWorker, this);
    } else if (!enable && pImpl->asyncRunning) {
        pImpl->asyncRunning = false;
        pImpl->queueCV.notify_one();
        if (pImpl->asyncThread.joinable()) {
            pImpl->asyncThread.join();
        }
    }
    pImpl->asyncEnabled = enable;
}

void ConciseLogger::enablePerformanceLogging(bool enable) {
    pImpl->performanceLoggingEnabled = enable;
}

bool ConciseLogger::isPerformanceLoggingEnabled() const {
    return pImpl->performanceLoggingEnabled;
}

void ConciseLogger::setPerformanceSampleRate(double rate) {
    pImpl->performanceSampleRate = rate;
}

void ConciseLogger::addLogFilter(std::function<bool(const StructuredLogEntry&)> filter) {
    pImpl->filters.push_back(std::move(filter));
}

void ConciseLogger::clearLogFilters() {
    pImpl->filters.clear();
}

size_t ConciseLogger::getLogCount() const {
    return pImpl->totalLogCount;
}

double ConciseLogger::getAverageLogTime() const {
    return pImpl->totalLogCount > 0 ? pImpl->totalLogTime / pImpl->totalLogCount : 0.0;
}

std::unordered_map<std::string, size_t> ConciseLogger::getCategoryStats() const {
    return pImpl->categoryStats;
}

void ConciseLogger::writeToFile(const StructuredLogEntry& entry) {
    if (!pImpl->outputFile.empty() && pImpl->fileOut.is_open()) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        std::string formattedEntry;
        if (pImpl->useJson) {
            formattedEntry = formatJsonEntry(entry);
        } else {
            formattedEntry = formatEntry(entry);
        }
        pImpl->fileOut << formattedEntry << std::endl;
        
        // Check file rotation
        auto pos = pImpl->fileOut.tellp();
        if (pos >= static_cast<std::streampos>(pImpl->maxFileSize)) {
            pImpl->fileOut.close();
            // Rotate file
            for (size_t i = pImpl->maxFiles - 1; i > 0; --i) {
                std::string src = pImpl->outputFile + "." + std::to_string(i);
                std::string dst = pImpl->outputFile + "." + std::to_string(i + 1);
                if (std::filesystem::exists(src)) {
                    std::filesystem::rename(src, dst);
                }
            }
            std::string archive = pImpl->outputFile + ".1";
            if (std::filesystem::exists(pImpl->outputFile)) {
                std::filesystem::rename(pImpl->outputFile, archive);
            }
            pImpl->fileOut.open(pImpl->outputFile, std::ios::app);
        }
    }
}

std::string ConciseLogger::formatEntry(const StructuredLogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << LoggerFactory::levelToString(entry.level) << "] ";
    
    if (!entry.category.empty()) {
        oss << "[" << entry.category << "] ";
    }
    if (!entry.subcategory.empty()) {
        oss << "[" << entry.subcategory << "] ";
    }
    
    oss << entry.message;
    
    if (!entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    return oss.str();
}

std::string ConciseLogger::formatJsonEntry(const StructuredLogEntry& entry) const {
    nlohmann::json json;
    
    json["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
    json["level"] = LoggerFactory::levelToString(entry.level);
    json["message"] = entry.message;
    json["category"] = entry.category;
    
    if (!entry.subcategory.empty()) {
        json["subcategory"] = entry.subcategory;
    }
    
    if (!entry.file.empty()) {
        json["file"] = entry.file;
        json["line"] = entry.line;
    }
    
    if (!entry.function.empty()) {
        json["function"] = entry.function;
    }
    
    if (!entry.metadata.empty()) {
        json["metadata"] = entry.metadata;
    }
    
    if (!entry.metrics.empty()) {
        json["metrics"] = entry.metrics;
    }
    
    if (!entry.tags.empty()) {
        json["tags"] = entry.tags;
    }
    
    if (entry.executionTime > 0.0) {
        json["execution_time_ms"] = entry.executionTime;
    }
    
    if (entry.memoryUsage > 0) {
        json["memory_usage_bytes"] = entry.memoryUsage;
    }
    
    return json.dump();
}

std::string ConciseLogger::formatConciseEntry(const StructuredLogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << LoggerFactory::levelToString(entry.level) << "]";
    
    if (!entry.category.empty()) {
        oss << "[" << entry.category << "]";
    }
    
    oss << entry.message;
    
    if (!entry.metrics.empty()) {
        oss << " | ";
        bool first = true;
        for (const auto& pair : entry.metrics) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
    }
    
    return oss.str();
}

void ConciseLogger::asyncLogWorker() {
    while (pImpl->asyncRunning) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        pImpl->queueCV.wait(lock, [this]() {
            return !pImpl->asyncRunning || !pImpl->logQueue.empty();
        });
        
        if (!pImpl->asyncRunning && pImpl->logQueue.empty()) {
            break;
        }
        
        StructuredLogEntry entry = std::move(pImpl->logQueue.front());
        pImpl->logQueue.pop();
        lock.unlock();
        
        // Process entry
        std::string formattedEntry;
        if (pImpl->useJson) {
            formattedEntry = formatJsonEntry(entry);
        } else if (pImpl->outputFormat == "concise") {
            formattedEntry = formatConciseEntry(entry);
        } else {
            formattedEntry = formatEntry(entry);
        }
        
        std::cout << formattedEntry << std::endl;
        writeToFile(entry);
        
        lock.lock();
    }
}

// ===============================================================
// LOGGER FACTORY IMPLEMENTATION
// ===============================================================

std::shared_ptr<IAdvancedLogger> LoggerFactory::createConsoleLogger(AdvancedLogLevel level) {
    return std::make_shared<ConciseLogger>(level);
}

std::shared_ptr<IAdvancedLogger> LoggerFactory::createFileLogger(const std::string& filepath, AdvancedLogLevel level) {
    auto logger = std::make_shared<ConciseLogger>(level);
    logger->setOutputFile(filepath);
    return logger;
}

std::shared_ptr<IAdvancedLogger> LoggerFactory::createRotatingFileLogger(const std::string& filepath, 
                                                                       size_t maxFileSize,
                                                                       size_t maxFiles,
                                                                       AdvancedLogLevel level) {
    auto logger = std::make_shared<ConciseLogger>(level);
    logger->setOutputFile(filepath);
    logger->setLogRotation(maxFileSize, maxFiles);
    return logger;
}

std::shared_ptr<IAdvancedLogger> LoggerFactory::createAsynchronousLogger(std::shared_ptr<IAdvancedLogger> backend, size_t queueSize) {
    // This is a simplified implementation
    // In a full implementation, you would create a wrapper logger that queues messages
    return backend;
}

std::string LoggerFactory::levelToString(AdvancedLogLevel level) {
    switch (level) {
        case AdvancedLogLevel::Trace: return "TRACE";
        case AdvancedLogLevel::Debug: return "DEBUG";
        case AdvancedLogLevel::Info: return "INFO";
        case AdvancedLogLevel::Warning: return "WARN";
        case AdvancedLogLevel::Error: return "ERROR";
        case AdvancedLogLevel::Critical: return "CRIT";
        case AdvancedLogLevel::Performance: return "PERF";
        case AdvancedLogLevel::Memory: return "MEM";
        case AdvancedLogLevel::Neural: return "NEURAL";
        case AdvancedLogLevel::Integration: return "INTEGRATION";
        case AdvancedLogLevel::Development: return "DEVELOPMENT";
        case AdvancedLogLevel::Experiment: return "EXPERIMENT";
        default: return "UNKNOWN";
    }
}

std::string LoggerFactory::levelToColor(AdvancedLogLevel level) {
    switch (level) {
        case AdvancedLogLevel::Trace: return "\033[36m";    // Cyan
        case AdvancedLogLevel::Debug: return "\033[32m";    // Green
        case AdvancedLogLevel::Info: return "\033[32m";     // Green
        case AdvancedLogLevel::Warning: return "\033[33m";  // Yellow
        case AdvancedLogLevel::Error: return "\033[31m";     // Red
        case AdvancedLogLevel::Critical: return "\033[35m"; // Magenta
        case AdvancedLogLevel::Performance: return "\033[90m"; // Bright Black
        case AdvancedLogLevel::Memory: return "\033[34m";  // Blue
        case AdvancedLogLevel::Neural: return "\033[35m";   // Magenta
        case AdvancedLogLevel::Integration: return "\033[33m"; // Yellow
        case AdvancedLogLevel::Development: return "\033[36m"; // Cyan
        case AdvancedLogLevel::Experiment: return "\033[91m"; // Bright Red
        default: return "\033[0m";
    }
}

std::string LoggerFactory::timestampToString(const std::chrono::system_clock::time_point& timestamp) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LoggerFactory::setupDevelopmentLogger(const std::string& logFile) {
    auto logger = createRotatingFileLogger(logFile, 5 * 1024 * 1024, 10, AdvancedLogLevel::Debug);
    logger->enableColors(true);
    logger->setLevel(AdvancedLogLevel::Debug);
    
    // Add common categories for development
    logger->addCategory("brain");
    logger->addCategory("memory");
    logger->addCategory("neuromodulation");
    logger->addCategory("prediction");
    logger->addCategory("cognition");
    logger->addCategory("development");
    logger->addCategory("experiment");
    
    AdvancedLoggerManager::getInstance().initialize(logger);
}

void LoggerFactory::setupProductionLogger(const std::string& logFile) {
    auto logger = createRotatingFileLogger(logFile, 50 * 1024 * 1024, 20, AdvancedLogLevel::Warning);
    logger->enableColors(false);
    logger->setLevel(AdvancedLogLevel::Warning);
    
    // Only essential categories for production
    logger->addCategory("brain");
    logger->addCategory("error");
    logger->addCategory("integration");
    
    AdvancedLoggerManager::getInstance().initialize(logger);
}

void LoggerFactory::setupPerformanceLogger(const std::string& logFile) {
    auto logger = createRotatingFileLogger(logFile, 10 * 1024 * 1024, 10, AdvancedLogLevel::Performance);
    logger->enableColors(true);
    logger->setLevel(AdvancedLogLevel::Performance);
    logger->enablePerformanceLogging(true);
    
    // Performance-specific categories
    logger->addCategory("performance");
    logger->addCategory("neural");
    logger->addCategory("memory");
    logger->addCategory("integration");
    
    AdvancedLoggerManager::getInstance().initialize(logger);
}

void LoggerFactory::setupIntegrationLogger(const std::string& logFile) {
    auto logger = createRotatingFileLogger(logFile, 20 * 1024 * 1024, 15, AdvancedLogLevel::Info);
    logger->enableColors(true);
    logger->setLevel(AdvancedLogLevel::Info);
    
    // Integration-specific categories
    logger->addCategory("brain");
    logger->addCategory("memory");
    logger->addCategory("neuromodulation");
    logger->addCategory("prediction");
    logger->addCategory("cognition");
    logger->addCategory("development");
    logger->addCategory("checkpoint");
    logger->addCategory("replay");
    
    AdvancedLoggerManager::getInstance().initialize(logger);
}

// ===============================================================
// ADVANCED LOGGER MANAGER IMPLEMENTATION
// ===============================================================

AdvancedLoggerManager& AdvancedLoggerManager::getInstance() {
    static AdvancedLoggerManager instance;
    return instance;
}

void AdvancedLoggerManager::initialize(std::shared_ptr<IAdvancedLogger> logger) {
    pImpl->logger = logger;
}

std::shared_ptr<IAdvancedLogger> AdvancedLoggerManager::getLogger() {
    return pImpl->logger;
}

void AdvancedLoggerManager::log(AdvancedLogLevel level, const std::string& message) {
    if (pImpl->logger) {
        pImpl->logger->log(level, message);
    }
}

void AdvancedLoggerManager::log(AdvancedLogLevel level, const std::string& category, const std::string& message) {
    if (pImpl->logger) {
        pImpl->logger->log(level, category, message);
    }
}

void AdvancedLoggerManager::log(AdvancedLogLevel level, const std::string& category,
                               const std::string& subcategory, const std::string& message) {
    if (pImpl->logger) {
        pImpl->logger->log(level, category, subcategory, message);
    }
}

void AdvancedLoggerManager::debug(const std::string& message) {
    log(AdvancedLogLevel::Debug, message);
}

void AdvancedLoggerManager::info(const std::string& message) {
    log(AdvancedLogLevel::Info, message);
}

void AdvancedLoggerManager::warning(const std::string& message) {
    log(AdvancedLogLevel::Warning, message);
}

void AdvancedLoggerManager::error(const std::string& message) {
    log(AdvancedLogLevel::Error, message);
}

void AdvancedLoggerManager::critical(const std::string& message) {
    log(AdvancedLogLevel::Critical, message);
}

void AdvancedLoggerManager::debug(const std::string& category, const std::string& message) {
    log(AdvancedLogLevel::Debug, category, message);
}

void AdvancedLoggerManager::info(const std::string& category, const std::string& message) {
    log(AdvancedLogLevel::Info, category, message);
}

void AdvancedLoggerManager::warning(const std::string& category, const std::string& message) {
    log(AdvancedLogLevel::Warning, category, message);
}

void AdvancedLoggerManager::error(const std::string& category, const std::string& message) {
    log(AdvancedLogLevel::Error, category, message);
}

void AdvancedLoggerManager::critical(const std::string& category, const std::string& message) {
    log(AdvancedLogLevel::Critical, category, message);
}

void AdvancedLoggerManager::logPerformance(const std::string& operation, double duration,
                                          const std::unordered_map<std::string, double>& metrics) {
    std::ostringstream message;
    message << "Performance: " << operation << " took " << duration << " ms";
    
    if (!metrics.empty()) {
        message << " | Metrics: ";
        bool first = true;
        for (const auto& pair : metrics) {
            if (!first) message << ", ";
            message << pair.first << "=" << pair.second;
            first = false;
        }
    }
    
    log(AdvancedLogLevel::Performance, "performance", message.str());
}

void AdvancedLoggerManager::logMemoryUsage(size_t memoryBytes) {
    std::ostringstream message;
    message << "Memory usage: " << memoryBytes << " bytes ("
            << (memoryBytes / 1024.0 / 1024.0) << " MB)";
    
    log(AdvancedLogLevel::Memory, "memory", message.str());
}

void AdvancedLoggerManager::logNeuralActivity(int neuronCount, int spikeCount, float firingRate) {
    std::ostringstream message;
    message << "Neural activity: " << neuronCount << " neurons, "
            << spikeCount << " spikes, firing rate: " << firingRate << " Hz";
    
    log(AdvancedLogLevel::Neural, "neural", message.str());
}

void AdvancedLoggerManager::logIntegrationEvent(const std::string& stage, const std::string& component) {
    std::ostringstream message;
    message << "Integration: " << stage << " - " << component << " completed";
    
    log(AdvancedLogLevel::Integration, "integration", message.str());
}

void AdvancedLoggerManager::setLogLevel(AdvancedLogLevel level) {
    if (pImpl->logger) {
        pImpl->logger->setLevel(level);
    }
}

void AdvancedLoggerManager::enablePerformanceMonitoring(bool enable) {
    if (pImpl->logger) {
        pImpl->logger->enablePerformanceLogging(enable);
    }
}

void AdvancedLoggerManager::setLogCategoryFilter(const std::vector<std::string>& categories) {
    if (pImpl->logger) {
        pImpl->logger->clearLogFilters();
        for (const auto& category : categories) {
            pImpl->logger->addCategory(category);
        }
    }
}

size_t AdvancedLoggerManager::getTotalLogCount() {
    if (pImpl->logger) {
        return pImpl->logger->getLogCount();
    }
    return 0;
}

std::unordered_map<std::string, size_t> AdvancedLoggerManager::getLogStatistics() {
    if (pImpl->logger) {
        return pImpl->logger->getCategoryStats();
    }
    return std::unordered_map<std::string, size_t>();
}

void AdvancedLoggerManager::setLogDirectory(const std::string& directory) {
    // Implementation would handle directory creation and file rotation
}

void AdvancedLoggerManager::rotateLogs() {
    if (pImpl->logger) {
        // This would trigger file rotation in the underlying logger
    }
}

// ===============================================================
// ADVANCED LOG STREAM IMPLEMENTATION
// ===============================================================

AdvancedLogStream::AdvancedLogStream(AdvancedLogLevel level, const std::string& category,
                                   const std::string& subcategory, const std::string& file,
                                   int line, const std::string& function)
    : level_(level), category_(category), subcategory_(subcategory), file_(file),
      line_(line), function_(function) {
}

AdvancedLogStream::~AdvancedLogStream() {
    // Create structured log entry
    StructuredLogEntry entry;
    entry.level = level_;
    entry.message = oss_.str();
    entry.category = category_;
    entry.subcategory = subcategory_;
    entry.file = file_;
    entry.line = line_;
    entry.function = function_;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata_;
    entry.metrics = metrics_;
    entry.tags = std::unordered_map<std::string, std::vector<std::string>>();
    
    for (const auto& tag : tags_) {
        entry.tags["tags"].push_back(tag);
    }
    
    entry.executionTime = executionTime_;
    entry.memoryUsage = memoryUsage_;
    entry.systemLoad = systemLoad_;
    entry.neuronCount = neuronCount_;
    entry.synapseCount = synapseCount_;
    entry.spikeCount = spikeCount_;
    entry.firingRate = firingRate_;
    entry.workingMemorySize = workingMemorySize_;
    entry.episodicMemorySize = episodicMemorySize_;
    entry.totalMemoryUsage = totalMemoryUsage_;
    entry.developmentalStage = developmentalStage_;
    entry.plasticityRate = plasticityRate_;
    entry.integrationStage = integrationStage_;
    entry.componentName = componentName_;
    entry.componentVersion = componentVersion_;
    
    // Log the entry
    AdvancedLoggerManager::getInstance().getLogger()->log(entry);
}

AdvancedLogStream& AdvancedLogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withMetric(const std::string& key, double value) {
    metrics_[key] = value;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withTag(const std::string& tag) {
    tags_.push_back(tag);
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withExecutionTime(double time) {
    executionTime_ = time;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withMemoryUsage(size_t bytes) {
    memoryUsage_ = bytes;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withSystemLoad(int load) {
    systemLoad_ = load;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withNeuralStats(int neuronCount, int synapseCount, int spikeCount, float firingRate) {
    neuronCount_ = neuronCount;
    synapseCount_ = synapseCount;
    spikeCount_ = spikeCount;
    firingRate_ = firingRate;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withMemoryStats(int workingMemSize, int episodicMemSize, int totalMemUsage) {
    workingMemorySize_ = workingMemSize;
    episodicMemorySize_ = episodicMemSize;
    totalMemoryUsage_ = totalMemUsage;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withDevelopmentStage(const std::string& stage) {
    developmentalStage_ = stage;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withPlasticityRate(float rate) {
    plasticityRate_ = rate;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withIntegrationStage(const std::string& stage) {
    integrationStage_ = stage;
    return *this;
}

AdvancedLogStream& AdvancedLogStream::withComponentInfo(const std::string& name, const std::string& version) {
    componentName_ = name;
    componentVersion_ = version;
    return *this;
}

// Thread-local storage definitions
thread_local std::string currentLogCategory = "";
thread_local std::string currentLogSubcategory = "";

} // namespace nlm
