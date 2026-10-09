#include "Enhanced/EnhancedLogger.hpp"
#include <fstream>
#include <iostream>

namespace nlm {

EnhancedLogger::EnhancedLogger() : asyncProcessor_(std::make_unique<AsyncLogProcessor>(std::thread::hardware_concurrency())),
                                  logAnalyzer_(std::make_unique<LogAnalyzer>(100000)),
                                  logManager_(std::make_unique<LogManager>()),
                                  performanceLogger_(std::make_unique<PerformanceLogger>()),
                                  sequenceCounter_(0), parseCounter_(0) {
    
    // Set up default formatters
    asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("JSON"));
    asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("CSV"));
    asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("XML"));
    
    // Set up async processor callback
    asyncProcessor_->setOutputCallback([this](const std::string& formatted) {
        // Store for analysis
        EnhancedLogEntry entry;
        parseLogEntry(entry, formatted);
        logAnalyzer_->addLog(entry);
    });
    
    // Initialize log manager
    logManager_->addLogFilePattern("*.log");
    logManager_->setMaxLogFiles(10);
}

EnhancedLogger::~EnhancedLogger() {
    // Wait for async processing to complete
    asyncProcessor_->waitForCompletion();
    
    // Stop monitoring
    enableMonitoring(false);
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void EnhancedLogger::log(const LogEntry& entry) {
    // Convert to enhanced log entry
    EnhancedLogEntry enhancedEntry;
    enhancedEntry.level = entry.level;
    enhancedEntry.message = entry.message;
    enhancedEntry.file = entry.file;
    enhancedEntry.line = entry.line;
    enhancedEntry.function = entry.function;
    enhancedEntry.timestamp = entry.timestamp;
    enhancedEntry.sequenceNumber = ++sequenceCounter_;
    
    // Add to async processor
    asyncProcessor_->addLog(std::make_unique<EnhancedLogEntry>(enhancedEntry));
    
    // Add to analyzer
    logAnalyzer_->addLog(enhancedEntry);
    
    // Log performance metrics
    performanceLogger_->logPerformance("log_processing", 0.0, sizeof(EnhancedLogEntry));
}

void EnhancedLogger::setLevel(LogLevel level) {
    currentLevel_ = level;
    asyncProcessor_->setFilter(std::make_unique<LevelFilter>(level));
}

LogLevel EnhancedLogger::getLevel() const {
    return currentLevel_;
}

void EnhancedLogger::addFormatter(std::unique_ptr<LogFormatter> formatter) {
    asyncProcessor_->addFormatter(std::move(formatter));
}

void EnhancedLogger::removeFormatter(const std::string& name) {
    // Implementation would remove formatter by name
    // For now, just clear and add back
    std::lock_guard<std::mutex> lock(formatterMutex_);
    formatters_.clear();
}

void EnhancedLogger::addFilter(std::unique_ptr<LogFilter> filter) {
    std::lock_guard<std::mutex> lock(filterMutex_);
    filter_ = std::move(filter);
    asyncProcessor_->setFilter(filter_);
}

void EnhancedLogger::clearFilters() {
    std::lock_guard<std::mutex> lock(filterMutex_);
    filter_ = nullptr;
    asyncProcessor_->setFilter(nullptr);
}

void EnhancedLogger::setOutputDirectory(const std::string& directory) {
    logManager_->setLogDirectory(directory);
}

void EnhancedLogger::rotateLogs() {
    if (rotationPolicy_) {
        logManager_->rotateLogs(*rotationPolicy_);
    }
}

std::string EnhancedLogger::generateReport() const {
    std::stringstream report;
    report << "=== Enhanced Logger Report ===\n";
    
    // Add statistics
    LogStatistics stats = logAnalyzer_->getStatistics();
    report << "Statistics:\n";
    report << "  Total logs: " << stats.totalCount << "\n";
    report << "  Errors: " << stats.errorCount << "\n";
    report << "  Warnings: " << stats.warningCount << "\n";
    report << "  Info: " << stats.infoCount << "\n";
    report << "  Debug: " << stats.debugCount << "\n";
    report << "  Critical: " << stats.criticalCount << "\n";
    
    // Add performance metrics
    std::string perfReport = performanceLogger_->getPerformanceReport();
    report << "\nPerformance Metrics:\n" << perfReport << "\n";
    
    return report.str();
}

bool EnhancedLogger::exportLogs(const std::string& filename, const std::string& format) const {
    // Get logs from analyzer
    std::vector<EnhancedLogEntry> logs = getAllLogs();
    
    // Export using log manager
    return logManager_->exportLogs(filename, logs, format);
}

bool EnhancedLogger::importLogs(const std::string& filename, const std::string& format) {
    // Import using log manager
    std::vector<EnhancedLogEntry> logs = logManager_->importLogs(filename, format);
    
    // Add logs to analyzer
    for (const auto& log : logs) {
        logAnalyzer_->addLog(log);
    }
    
    return true;
}

std::vector<EnhancedLogEntry> EnhancedLogger::searchLogs(const std::string& pattern,
                                                        const std::string& filePattern,
                                                        LogLevel minLevel,
                                                        const std::chrono::system_clock::time_point& startTime,
                                                        const std::chrono::system_clock::time_point& endTime) const {
    return logManager_->searchLogs(pattern, filePattern, minLevel, startTime, endTime);
}

std::vector<EnhancedLogEntry> EnhancedLogger::getAllLogs() const {
    // This would need to be implemented to retrieve logs from the analyzer
    // For now, return empty vector
    return std::vector<EnhancedLogEntry>();
}

void EnhancedLogger::enableMonitoring(bool enabled) {
    monitoringEnabled_ = enabled;
    if (enabled) {
        monitoringThread_ = std::thread(&EnhancedLogger::monitoringThread, this);
    } else {
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
}

void EnhancedLogger::setLogDirectory(const std::string& directory) {
    logManager_->setLogDirectory(directory);
}

void EnhancedLogger::setMaxLogFiles(size_t maxFiles) {
    logManager_->setMaxLogFiles(maxFiles);
}

void EnhancedLogger::setRotationPolicy(std::unique_ptr<RotationPolicy> policy) {
    rotationPolicy_ = std::move(policy);
}

void EnhancedLogger::monitoringThread() {
    while (monitoringEnabled_) {
        std::this_thread::sleep_for(std::chrono::minutes(5));
        
        // Generate and log monitoring report
        std::string report = generateReport();
        std::cout << "[Monitoring] " << report << std::endl;
    }
}

void EnhancedLogger::parseLogEntry(EnhancedLogEntry& entry, const std::string& formattedLog) const {
    // Parse formatted log entry back to EnhancedLogEntry
    // This is a simplified implementation
    entry.level = LogLevel::Info;
    entry.message = formattedLog;
    entry.sequenceNumber = ++parseCounter_;
}

// LogFormatterFactory implementation
std::unique_ptr<LogFormatter> LogFormatterFactory::createFormatter(const std::string& name, const std::string& config) {
    if (name == "Template") {
        return std::make_unique<TemplateLogFormatter>(config);
    } else if (name == "JSON") {
        return std::make_unique<JsonLogFormatter>();
    } else if (name == "CSV") {
        return std::make_unique<CsvLogFormatter>();
    } else if (name == "XML") {
        return std::make_unique<XmlLogFormatter>();
    } else if (name == "Theme") {
        return std::make_unique<ThemeLogFormatter>(AnsiColorTheme());
    } else {
        return std::make_unique<TemplateLogFormatter>(config);
    }
}

// LogFilterFactory implementation
std::unique_ptr<LogFilter> LogFilterFactory::createFilter(const std::string& type, const std::string& config) {
    if (type == "Level") {
        LogLevel level = LogLevel::Info;
        if (config == "Debug") level = LogLevel::Debug;
        else if (config == "Info") level = LogLevel::Info;
        else if (config == "Warning") level = LogLevel::Warning;
        else if (config == "Error") level = LogLevel::Error;
        else if (config == "Critical") level = LogLevel::Critical;
        return std::make_unique<LevelFilter>(level);
    } else if (type == "Exclusion") {
        std::set<LogLevel> excludedLevels;
        if (config.find("Debug") != std::string::npos) excludedLevels.insert(LogLevel::Debug);
        if (config.find("Info") != std::string::npos) excludedLevels.insert(LogLevel::Info);
        return std::make_unique<ExclusionFilter>(excludedLevels);
    } else if (type == "Tag") {
        std::set<std::string> tags;
        size_t pos = 0;
        while ((pos = config.find(',', pos)) != std::string::npos) {
            std::string tag = config.substr(0, pos);
            tags.insert(tag);
            config.erase(0, pos + 1);
        }
        tags.insert(config);
        return std::make_unique<TagFilter>(tags);
    } else if (type == "Regex") {
        bool matchMessage = true;
        bool matchFile = false;
        bool matchFunction = false;
        if (config.find("file") != std::string::npos) matchFile = true;
        if (config.find("function") != std::string::npos) matchFunction = true;
        return std::make_unique<RegexFilter>(config, matchMessage, matchFile, matchFunction);
    } else if (type == "CompositeAND" || type == "CompositeOR") {
        CompositeFilter::Operation op = (type == "CompositeAND") ? 
            CompositeFilter::Operation::AND : CompositeFilter::Operation::OR;
        auto composite = std::make_unique<CompositeFilter>(op);
        return composite;
    } else {
        return std::make_unique<LevelFilter>(LogLevel::Info);
    }
}

// ConditionalLogEvaluator implementation
std::mutex ConditionalLogEvaluator::levelEnabledMutex_;
std::mutex ConditionalLogEvaluator::fileFilterMutex_;
std::unordered_map<LogLevel, bool> ConditionalLogEvaluator::levelEnabled_;
std::set<std::string> ConditionalLogEvaluator::fileFilter_;
bool ConditionalLogEvaluator::fileFilterEnabled_ = false;

bool ConditionalLogEvaluator::evaluate(LogLevel level, const std::string& message,
                                       const std::string& file, int line,
                                       const std::string& function) {
    if (level >= getCurrentLogThreshold()) {
        return false;
    }
    
    if (!isLevelEnabled(level)) {
        return false;
    }
    
    if (fileFilterEnabled_) {
        std::string normalizedFile = normalizeFilePath(file);
        if (!fileFilter_.empty() && fileFilter_.find(normalizedFile) == fileFilter_.end()) {
            return false;
        }
    }
    
    return true;
}

void ConditionalLogEvaluator::setLogThreshold(LogLevel threshold) {
    thread_local static LogLevel currentThreshold = threshold;
    currentThreshold = threshold;
}

LogLevel ConditionalLogEvaluator::getCurrentLogThreshold() {
    thread_local static LogLevel currentThreshold = LogLevel::Info;
    return currentThreshold;
}

void ConditionalLogEvaluator::setLevelEnabled(LogLevel level, bool enabled) {
    std::lock_guard<std::mutex> lock(levelEnabledMutex_);
    levelEnabled_[level] = enabled;
}

bool ConditionalLogEvaluator::isLevelEnabled(LogLevel level) {
    std::lock_guard<std::mutex> lock(levelEnabledMutex_);
    auto it = levelEnabled_.find(level);
    return it == levelEnabled_.end() ? true : it->second;
}

void ConditionalLogEvaluator::setFileFilter(const std::set<std::string>& files) {
    std::lock_guard<std::mutex> lock(fileFilterMutex_);
    fileFilter_ = files;
    fileFilterEnabled_ = !files.empty();
}

std::string ConditionalLogEvaluator::normalizeFilePath(const std::string& file) {
    std::string normalized = file;
    for (char& c : normalized) {
        if (c == '/') c = '\\';
    }
    return normalized;
}

// Level conversion function implementation
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
