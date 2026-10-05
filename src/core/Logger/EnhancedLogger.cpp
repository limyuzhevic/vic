#include "core/Logger/EnhancedLogger.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <unordered_set>

namespace nlm {

namespace {
    uint64_t globalSequenceNumber = 0;
    std::mutex sequenceMutex;
}

// EnhancedConsoleLogger implementation
struct EnhancedConsoleLogger::Impl {
    LogLevel level;
    bool useColors;
    bool structuredLogging;
    bool performanceTracking;
    std::string format;
    std::string outputFile;
    std::ofstream fileStream;
    
    // Filtering
    std::function<bool(const EnhancedLogEntry&)> filter;
    std::set<std::string> requiredTags;
    std::set<std::string> excludeTags;
    std::set<std::string> moduleFilter;
    std::set<std::string> experimentFilter;
    
    // Formatters
    std::function<std::string(const EnhancedLogEntry&)> messageFormatter;
    std::function<std::string(const EnhancedLogEntry&)> timestampFormatter;
    
    // Context
    std::string defaultModule;
    std::string defaultExperiment;
    std::unordered_map<std::string, std::string> defaultMetadata;
    
    // Statistics
    uint64_t totalLogs;
    std::unordered_map<LogLevel, uint64_t> logsByLevel;
    std::unordered_map<std::string, uint64_t> logsByModule;
    std::unordered_map<std::string, uint64_t> logsByTag;
    uint64_t errorCount;
    uint64_t warningCount;
    double totalProcessingTime;
    uint64_t logCount;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = LogLevel::Info;
    pImpl->useColors = true;
    pImpl->structuredLogging = false;
    pImpl->performanceTracking = false;
    pImpl->format = "[%timestamp%] [%level] [%module] %message%";
    pImpl->totalLogs = 0;
    pImpl->errorCount = 0;
    pImpl->warningCount = 0;
    pImpl->totalProcessingTime = 0.0;
    pImpl->logCount = 0;
}

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->useColors = true;
    pImpl->structuredLogging = false;
    pImpl->performanceTracking = false;
    pImpl->format = "[%timestamp%] [%level] [%module] %message%";
    pImpl->totalLogs = 0;
    pImpl->errorCount = 0;
    pImpl->warningCount = 0;
    pImpl->totalProcessingTime = 0.0;
    pImpl->logCount = 0;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() {
    flush();
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.close();
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setFilter(std::function<bool(const EnhancedLogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void EnhancedConsoleLogger::setRequiredTags(const std::set<std::string>& tags) {
    pImpl->requiredTags = tags;
}

void EnhancedConsoleLogger::setExcludeTags(const std::set<std::string>& tags) {
    pImpl->excludeTags = tags;
}

void EnhancedConsoleLogger::setModuleFilter(const std::set<std::string>& modules) {
    pImpl->moduleFilter = modules;
}

void EnhancedConsoleLogger::setExperimentFilter(const std::set<std::string>& experiments) {
    pImpl->experimentFilter = experiments;
}

void EnhancedConsoleLogger::setStructuredLogging(bool enabled) {
    pImpl->structuredLogging = enabled;
}

bool EnhancedConsoleLogger::getStructuredLogging() const {
    return pImpl->structuredLogging;
}

void EnhancedConsoleLogger::setPerformanceTracking(bool enabled) {
    pImpl->performanceTracking = enabled;
}

bool EnhancedConsoleLogger::getPerformanceTracking() const {
    return pImpl->performanceTracking;
}

void EnhancedConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string EnhancedConsoleLogger::getFormat() const {
    return pImpl->format;
}

void EnhancedConsoleLogger::log(const EnhancedLogEntry& entry) {
    // Apply filtering
    if (!shouldLog(entry)) {
        return;
    }
    
    // Update statistics
    pImpl->totalLogs++;
    pImpl->logsByLevel[entry.level]++;
    if (!entry.module.empty()) {
        pImpl->logsByModule[entry.module]++;
    }
    for (const auto& tag : entry.tags) {
        pImpl->logsByTag[tag]++;
    }
    if (entry.level == LogLevel::Error || entry.level == LogLevel::Critical || entry.level == LogLevel::Fatal) {
        pImpl->errorCount++;
    }
    if (entry.level == LogLevel::Warning) {
        pImpl->warningCount++;
    }
    if (entry.processingTime.count() > 0) {
        pImpl->totalProcessingTime += entry.processingTime.count();
        pImpl->logCount++;
    }
    
    // Format message
    std::string formatted = formatMessage(entry);
    
    // Output to console
    std::cout << formatted << std::endl;
    
    // Output to file if specified
    if (!pImpl->outputFile.empty()) {
        if (!pImpl->fileStream.is_open()) {
            pImpl->fileStream.open(pImpl->outputFile, std::ios::app);
            if (!pImpl->fileStream.is_open()) {
                std::cerr << "Failed to open log file: " << pImpl->outputFile << std::endl;
            }
        }
        if (pImpl->fileStream.is_open()) {
            pImpl->fileStream << formatted << std::endl;
        }
    }
}

void EnhancedConsoleLogger::flush() {
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.flush();
    }
}

struct EnhancedConsoleLogger::LogStatistics EnhancedConsoleLogger::getStatistics() const {
    LogStatistics stats;
    stats.totalLogs = pImpl->totalLogs;
    stats.byLevel = pImpl->logsByLevel;
    stats.byModule = pImpl->logsByModule;
    stats.byTag = pImpl->logsByTag;
    stats.errors = pImpl->errorCount;
    stats.warnings = pImpl->warningCount;
    stats.avgProcessingTime = pImpl->logCount > 0 ? 
        (pImpl->totalProcessingTime / pImpl->logCount) : 0.0;
    return stats;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->outputFile = filepath;
}

void EnhancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.close();
    }
}

void EnhancedConsoleLogger::setMessageFormatter(std::function<std::string(const EnhancedLogEntry&)> formatter) {
    pImpl->messageFormatter = std::move(formatter);
}

void EnhancedConsoleLogger::setTimestampFormatter(std::function<std::string(const EnhancedLogEntry&)> formatter) {
    pImpl->timestampFormatter = std::move(formatter);
}

void EnhancedConsoleLogger::setDefaultModule(const std::string& module) {
    pImpl->defaultModule = module;
}

void EnhancedConsoleLogger::setDefaultExperiment(const std::string& experiment) {
    pImpl->defaultExperiment = experiment;
}

void EnhancedConsoleLogger::addMetadata(const std::string& key, const std::string& value) {
    pImpl->defaultMetadata[key] = value;
}

std::string EnhancedConsoleLogger::formatMessage(const EnhancedLogEntry& entry) const {
    std::string formatted = pImpl->format;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = formatted.find("%timestamp%")) != std::string::npos) {
        formatted.replace(pos, 11, formatTimestamp(entry));
    }
    
    pos = 0;
    while ((pos = formatted.find("%level%")) != std::string::npos) {
        formatted.replace(pos, 8, levelToString(entry.level));
    }
    
    pos = 0;
    while ((pos = formatted.find("%module%")) != std::string::npos) {
        std::string module = entry.module.empty() ? pImpl->defaultModule : entry.module;
        formatted.replace(pos, 9, module);
    }
    
    pos = 0;
    while ((pos = formatted.find("%experiment%")) != std::string::npos) {
        std::string experiment = entry.experiment.empty() ? pImpl->defaultExperiment : entry.experiment;
        formatted.replace(pos, 11, experiment);
    }
    
    pos = 0;
    while ((pos = formatted.find("%message%")) != std::string::npos) {
        if (pImpl->messageFormatter) {
            formatted.replace(pos, 9, pImpl->messageFormatter(entry));
        } else {
            formatted.replace(pos, 9, entry.message);
        }
    }
    
    pos = 0;
    while ((pos = formatted.find("%thread%")) != std::string::npos) {
        formatted.replace(pos, 8, std::to_string(entry.threadId));
    }
    
    pos = 0;
    while ((pos = formatted.find("%sequence%")) != std::string::npos) {
        formatted.replace(pos, 10, std::to_string(entry.sequenceNumber));
    }
    
    pos = 0;
    while ((pos = formatted.find("%file%")) != std::string::npos) {
        formatted.replace(pos, 7, entry.file);
    }
    
    pos = 0;
    while ((pos = formatted.find("%line%")) != std::string::npos) {
        formatted.replace(pos, 7, std::to_string(entry.line));
    }
    
    // Add metadata if structured logging is enabled
    if (pImpl->structuredLogging && !entry.metadata.empty()) {
        std::string metadataStr = " [";
        for (const auto& [key, value] : entry.metadata) {
            metadataStr += key + "=" + value + ", ";
        }
        if (!metadataStr.empty()) {
            metadataStr.pop_back(); // Remove trailing comma
            metadataStr.pop_back(); // Remove trailing space
        }
        metadataStr += "]";
        formatted += metadataStr;
    }
    
    // Add performance info if enabled
    if (pImpl->performanceTracking && entry.processingTime.count() > 0) {
        formatted += " [" + std::to_string(entry.processingTime.count()) + "μs]";
    }
    
    // Add tags if any
    if (!entry.tags.empty()) {
        formatted += " [#";
        for (const auto& tag : entry.tags) {
            formatted += tag + ",";
        }
        if (!formatted.empty()) {
            formatted.back() = '#';
        }
        formatted += "]";
    }
    
    return formatted;
}

std::string EnhancedConsoleLogger::formatTimestamp(const EnhancedLogEntry& entry) const {
    if (pImpl->timestampFormatter) {
        return pImpl->timestampFormatter(entry);
    }
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string EnhancedConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::NOTICE: return "NOTICE";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string EnhancedConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) return "";
    
    switch (level) {
        case LogLevel::TRACE: return "\033[37m"; // White
        case LogLevel::DEBUG: return "\033[36m"; // Cyan
        case LogLevel::INFO: return "\033[32m"; // Green
        case LogLevel::NOTICE: return "\033[33m"; // Yellow
        case LogLevel::WARNING: return "\033[33m"; // Yellow
        case LogLevel::ERROR: return "\033[31m"; // Red
        case LogLevel::CRITICAL: return "\033[35m"; // Magenta
        case LogLevel::FATAL: return "\033[41m\033[37m"; // Red background, white text
        default: return "";
    }
}

bool EnhancedConsoleLogger::shouldLog(const EnhancedLogEntry& entry) const {
    // Check level
    if (static_cast<int>(entry.level) < static_cast<int>(pImpl->level)) {
        return false;
    }
    
    // Apply custom filter
    if (pImpl->filter && !pImpl->filter(entry)) {
        return false;
    }
    
    // Check required tags
    if (!pImpl->requiredTags.empty()) {
        bool hasRequiredTag = false;
        for (const auto& tag : pImpl->requiredTags) {
            if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                hasRequiredTag = true;
                break;
            }
        }
        if (!hasRequiredTag) {
            return false;
        }
    }
    
    // Check excluded tags
    if (!pImpl->excludeTags.empty()) {
        for (const auto& tag : pImpl->excludeTags) {
            if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                return false;
            }
        }
    }
    
    // Check module filter
    if (!pImpl->moduleFilter.empty()) {
        if (entry.module.empty() || 
            pImpl->moduleFilter.find(entry.module) == pImpl->moduleFilter.end()) {
            return false;
        }
    }
    
    // Check experiment filter
    if (!pImpl->experimentFilter.empty()) {
        if (entry.experiment.empty() || 
            pImpl->experimentFilter.find(entry.experiment) == pImpl->experimentFilter.end()) {
            return false;
        }
    }
    
    return true;
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    std::vector<std::shared_ptr<IEnhancedLogger>> loggers;
    LogLevel level;
    bool consoleColors;
    bool structuredLogging;
    bool performanceLogging;
    
    // Statistics
    uint64_t totalLogs;
    std::unordered_map<LogLevel, uint64_t> logsByLevel;
    std::unordered_map<std::string, uint64_t> logsByModule;
    std::unordered_map<std::string, uint64_t> logsByTag;
    uint64_t errorCount;
    uint64_t warningCount;
    double totalProcessingTime;
    uint64_t logCount;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    
    std::mutex mutex;
    std::vector<EnhancedLogEntry> logHistory;
    size_t maxHistorySize;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = LogLevel::Info;
    pImpl->consoleColors = true;
    pImpl->structuredLogging = false;
    pImpl->performanceLogging = false;
    pImpl->totalLogs = 0;
    pImpl->errorCount = 0;
    pImpl->warningCount = 0;
    pImpl->totalProcessingTime = 0.0;
    pImpl->logCount = 0;
    pImpl->maxHistorySize = 10000;
}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::addEnhancedLogger(std::shared_ptr<IEnhancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(std::move(logger));
}

void EnhancedLogger::addConsoleLogger(LogLevel level) {
    auto consoleLogger = std::make_shared<EnhancedConsoleLogger>(level);
    consoleLogger->setUseColors(pImpl->consoleColors);
    consoleLogger->setStructuredLogging(pImpl->structuredLogging);
    consoleLogger->setPerformanceTracking(pImpl->performanceLogging);
    addEnhancedLogger(std::move(consoleLogger));
}

void EnhancedLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void EnhancedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void EnhancedLogger::log(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& module,
                         const std::string& experiment) {
    EnhancedLogEntry entry = createLogEntry(level, message, file, line, function, module, experiment);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update statistics
    pImpl->totalLogs++;
    pImpl->logsByLevel[level]++;
    if (!entry.module.empty()) {
        pImpl->logsByModule[entry.module]++;
    }
    for (const auto& tag : entry.tags) {
        pImpl->logsByTag[tag]++;
    }
    if (entry.level == LogLevel::Error || entry.level == LogLevel::Critical || entry.level == LogLevel::FATAL) {
        pImpl->errorCount++;
    }
    if (entry.level == LogLevel::Warning) {
        pImpl->warningCount++;
    }
    if (entry.processingTime.count() > 0) {
        pImpl->totalProcessingTime += entry.processingTime.count();
        pImpl->logCount++;
    }
    
    auto now = std::chrono::system_clock::now();
    if (pImpl->firstLog.time_since_epoch().count() == 0) {
        pImpl->firstLog = now;
    }
    pImpl->lastLog = now;
    
    // Add to history
    if (pImpl->logHistory.size() >= pImpl->maxHistorySize) {
        pImpl->logHistory.erase(pImpl->logHistory.begin());
    }
    pImpl->logHistory.push_back(entry);
    
    // Send to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logWithMetadata(LogLevel level, const std::string& message,
                                    const std::unordered_map<std::string, std::string>& metadata,
                                    const std::string& file, int line,
                                    const std::string& function,
                                    const std::string& module,
                                    const std::string& experiment) {
    EnhancedLogEntry entry = createLogEntry(level, message, file, line, function, module, experiment);
    entry.metadata = metadata;
    
    log(level, message, file, line, function, module, experiment);
}

void EnhancedLogger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::TRACE, message, file, line, function);
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::DEBUG, message, file, line, function);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::INFO, message, file, line, function);
}

void EnhancedLogger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::NOTICE, message, file, line, function);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::WARNING, message, file, line, function);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::ERROR, message, file, line, function);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::CRITICAL, message, file, line, function);
}

void EnhancedLogger::fatal(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::FATAL, message, file, line, function);
}

void EnhancedLogger::logWithPerformance(LogLevel level, const std::string& message,
                                       const std::chrono::microseconds& processingTime,
                                       const std::string& file, int line,
                                       const std::string& function) {
    EnhancedLogEntry entry = createLogEntry(level, message, file, line, function);
    entry.processingTime = processingTime;
    
    log(level, message, file, line, function);
}

bool EnhancedLogger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return static_cast<int>(level) >= static_cast<int>(pImpl->level);
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger instance;
    return instance;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    static std::shared_ptr<EnhancedLogger> globalLogger;
    globalLogger = std::move(logger);
}

void EnhancedLogger::exportLogs(const std::string& filepath, LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file for writing: " + filepath);
    }
    
    file << "Log export from " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
         << std::endl << std::endl;
    
    for (const auto& entry : pImpl->logHistory) {
        if (static_cast<int>(entry.level) < static_cast<int>(minLevel)) {
            continue;
        }
        
        // Write entry in structured format
        file << "TIMESTAMP: " << std::chrono::system_clock::to_time_t(entry.timestamp)
             << "\nLEVEL: " << levelToString(entry.level)
             << "\nMODULE: " << entry.module
             << "\nEXPERIMENT: " << entry.experiment
             << "\nTHREAD: " << entry.threadId
             << "\nSEQUENCE: " << entry.sequenceNumber
             << "\nMESSAGE: " << entry.message
             << "\nFILE: " << entry.file
             << "\nLINE: " << entry.line
             << "\nFUNCTION: " << entry.function << std::endl;
        
        if (!entry.metadata.empty()) {
            file << "METADATA:";
            for (const auto& [key, value] : entry.metadata) {
                file << " " << key << "=" << value;
            }
            file << std::endl;
        }
        
        if (entry.processingTime.count() > 0) {
            file << "PROCESSING_TIME: " << entry.processingTime.count() << "μs" << std::endl;
        }
        
        if (!entry.tags.empty()) {
            file << "TAGS:";
            for (const auto& tag : entry.tags) {
                file << " " << tag;
            }
            file << std::endl;
        }
        
        file << "---" << std::endl;
    }
}

std::vector<EnhancedLogEntry> EnhancedLogger::searchLogs(
    std::function<bool(const EnhancedLogEntry&)> predicate) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<EnhancedLogEntry> results;
    for (const auto& entry : pImpl->logHistory) {
        if (predicate(entry)) {
            results.push_back(entry);
        }
    }
    
    return results;
}

EnhancedLogger::Statistics EnhancedLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Statistics stats;
    stats.totalLogs = pImpl->totalLogs;
    stats.byLevel = pImpl->logsByLevel;
    stats.byModule = pImpl->logsByModule;
    stats.byTag = pImpl->logsByTag;
    stats.errors = pImpl->errorCount;
    stats.warnings = pImpl->warningCount;
    stats.avgProcessingTime = pImpl->logCount > 0 ? 
        (pImpl->totalProcessingTime / pImpl->logCount) : 0.0;
    stats.firstLog = pImpl->firstLog;
    stats.lastLog = pImpl->lastLog;
    
    return stats;
}

void EnhancedLogger::clearStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->totalLogs = 0;
    pImpl->logsByLevel.clear();
    pImpl->logsByModule.clear();
    pImpl->logsByTag.clear();
    pImpl->errorCount = 0;
    pImpl->warningCount = 0;
    pImpl->totalProcessingTime = 0.0;
    pImpl->logCount = 0;
    pImpl->firstLog = std::chrono::system_clock::time_point();
    pImpl->lastLog = std::chrono::system_clock::time_point();
    pImpl->logHistory.clear();
}

void EnhancedLogger::setupConsoleColors(bool enabled) {
    pImpl->consoleColors = enabled;
    for (auto& logger : pImpl->loggers) {
        if (auto* enhancedLogger = dynamic_cast<EnhancedConsoleLogger*>(logger.get())) {
            enhancedLogger->setUseColors(enabled);
        }
    }
}

void EnhancedLogger::setupStructuredLogging(bool enabled) {
    pImpl->structuredLogging = enabled;
    for (auto& logger : pImpl->loggers) {
        if (auto* enhancedLogger = dynamic_cast<EnhancedConsoleLogger*>(logger.get())) {
            enhancedLogger->setStructuredLogging(enabled);
        }
    }
}

void EnhancedLogger::setupPerformanceLogging(bool enabled) {
    pImpl->performanceLogging = enabled;
    for (auto& logger : pImpl->loggers) {
        if (auto* enhancedLogger = dynamic_cast<EnhancedConsoleLogger*>(logger.get())) {
            enhancedLogger->setPerformanceTracking(enabled);
        }
    }
}

void EnhancedLogger::addTag(EnhancedLogEntry& entry, const std::string& tag) {
    entry.tags.push_back(tag);
}

EnhancedLogEntry EnhancedLogger::createLogEntry(LogLevel level, const std::string& message,
                                                const std::string& file, int line,
                                                const std::string& function,
                                                const std::string& module,
                                                const std::string& experiment) {
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    
    {
        std::lock_guard<std::mutex> lock(sequenceMutex);
        entry.sequenceNumber = ++globalSequenceNumber;
    }
    
    entry.module = module.empty() ? "" : module;
    entry.experiment = experiment.empty() ? "" : experiment;
    
    return entry;
}

} // namespace nlm