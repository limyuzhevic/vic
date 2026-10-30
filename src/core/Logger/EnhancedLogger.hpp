// Enhanced logging infrastructure for NLM

#include "Logger.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <map>
#include <atomic>
#include <ctime>

namespace nlm {

// Enhanced log entry with additional metadata
struct EnhancedLogEntry : public LogEntry {
    std::string module;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    
    EnhancedLogEntry() : LogEntry() {
        timestamp = std::chrono::system_clock::now();
        threadId = getCurrentThreadId();
    }
    
    static std::string getCurrentThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
};

// Log rotation configuration
struct LogRotationConfig {
    size_t maxSizeMB = 100;
    size_t maxFiles = 5;
    bool rotateDaily = false;
    bool compressOldLogs = true;
};

// Enhanced ConsoleLogger with log rotation and filtering
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file with rotation
    void setOutputFile(const std::string& filepath, const LogRotationConfig& config = LogRotationConfig());
    void closeOutputFile();
    
    // Filter by module
    void addModuleFilter(const std::string& module);
    void removeModuleFilter(const std::string& module);
    bool isModuleEnabled(const std::string& module) const;
    
    // Set minimum level per module
    void setModuleLevel(const std::string& module, LogLevel level);
    LogLevel getModuleLevel(const std::string& module) const;
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message, 
                      const std::map<std::string, std::string>& metadata,
                      const std::string& module = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const EnhancedLogEntry& entry) const;
    bool shouldLog(const EnhancedLogEntry& entry) const;
    void rotateFileIfNeeded();
    std::string getRotatedFilename(const std::string& basePath, int index) const;
};

// Logger with advanced features
class AdvancedLogger : public Logger {
public:
    AdvancedLogger();
    ~AdvancedLogger() override;
    
    // Component-based logging
    void setComponentEnabled(const std::string& component, bool enabled);
    bool isComponentEnabled(const std::string& component) const;
    
    // Performance monitoring
    void logPerformance(const std::string& operation, double duration,
                       const std::map<std::string, std::string>& metadata = {});
    
    // Memory tracking
    void logMemoryUsage(size_t allocated, size_t available);
    
    // Network logging
    void setNetworkLogEndpoint(const std::string& endpoint);
    void sendLogToNetwork(const EnhancedLogEntry& entry);
    
    // Log statistics
    struct LogStats {
        std::atomic<size_t> totalLogs{0};
        std::atomic<size_t> logsByLevel[5]{0}; // DEBUG, INFO, WARNING, ERROR, CRITICAL
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    LogStats getLogStats() const;
    void resetLogStats();
    
    // Log level filtering with thresholds
    void setLogLevelThreshold(LogLevel level);
    bool isLogLevelEnabled(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Python integration helpers
namespace python {
    // Create logger configuration from Python dict
    LogRotationConfig parseLogRotationConfig(const std::map<std::string, std::string>& config);
    
    // Get Python-friendly log statistics
    py::dict getPythonLogStats(const AdvancedLogger::LogStats& stats);
    
    // Python bindings for advanced logging
    void registerPythonLogging(py::module& m);
}

// Global advanced logger instance
inline AdvancedLogger& getGlobalAdvancedLogger() {
    static AdvancedLogger logger;
    return logger;
}

// Convenience macros for advanced logging
#define NLM_LOG_PERFORMANCE(operation, duration) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logPerformance(operation, duration); \
    } while(0)

#define NLM_LOG_MEMORY(allocated, available) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logMemoryUsage(allocated, available); \
    } while(0)

#define NLM_LOG_EVENT(event, metadata) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logStructured(nlm::LogLevel::Info, event, metadata, "APPLICATION"); \
    } while(0)

} // namespace nlm

// Implementation details
namespace nlm {

struct EnhancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Rotation configuration
    LogRotationConfig rotationConfig;
    size_t currentFileSize = 0;
    int currentFileIndex = 0;
    std::chrono::system_clock::time_point lastRotation;
    
    // Module filtering
    std::map<std::string, bool> moduleEnabled;
    std::map<std::string, LogLevel> moduleLevels;
    
    // Performance tracking
    std::atomic<size_t> totalLogs{0};
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->firstLog = std::chrono::system_clock::now();
}

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : EnhancedConsoleLogger() {
    pImpl->level = level;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() = default;

void EnhancedConsoleLogger::log(const LogEntry& entry) {
    EnhancedLogEntry enhancedEntry;
    enhancedEntry.level = entry.level;
    enhancedEntry.message = entry.message;
    enhancedEntry.file = entry.file;
    enhancedEntry.line = entry.line;
    enhancedEntry.function = entry.function;
    enhancedEntry.timestamp = entry.timestamp;
    
    if (shouldLog(enhancedEntry)) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->totalLogs++;
        pImpl->lastLog = std::chrono::system_clock::now();
        
        rotateFileIfNeeded();
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Add module if present
        if (!enhancedEntry.module.empty()) {
            oss << " [" << enhancedEntry.module << "]";
        }
        
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
            pImpl->currentFileSize += output.size() + 1; // +1 for newline
        }
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath, const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->rotationConfig = config;
    pImpl->currentFileSize = pImpl->fileOut.tellp();
    pImpl->currentFileIndex = 0;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

void EnhancedConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void EnhancedConsoleLogger::addModuleFilter(const std::string& module) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->moduleEnabled[module] = true;
}

void EnhancedConsoleLogger::removeModuleFilter(const std::string& module) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->moduleEnabled.erase(module);
}

bool EnhancedConsoleLogger::isModuleEnabled(const std::string& module) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->moduleEnabled.find(module);
    return it == pImpl->moduleEnabled.end() || it->second;
}

void EnhancedConsoleLogger::setModuleLevel(const std::string& module, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->moduleLevels[module] = level;
}

LogLevel EnhancedConsoleLogger::getModuleLevel(const std::string& module) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->moduleLevels.find(module);
    return it == pImpl->moduleLevels.end() ? pImpl->level : it->second;
}

void EnhancedConsoleLogger::logStructured(LogLevel level, const std::string& message,
                                        const std::map<std::string, std::string>& metadata,
                                        const std::string& module) {
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.metadata = metadata;
    entry.module = module;
    
    log(entry);
}

bool EnhancedConsoleLogger::shouldLog(const EnhancedLogEntry& entry) const {
    // Check global level
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Check module level if module is specified
    if (!entry.module.empty()) {
        auto it = pImpl->moduleLevels.find(entry.module);
        if (it != pImpl->moduleLevels.end() && entry.level < it->second) {
            return false;
        }
    }
    
    // Check module enabled filter
    if (!entry.module.empty() && !isModuleEnabled(entry.module)) {
        return false;
    }
    
    return true;
}

void EnhancedConsoleLogger::rotateFileIfNeeded() {
    if (pImpl->rotationConfig.maxSizeMB > 0 && 
        pImpl->currentFileSize >= pImpl->rotationConfig.maxSizeMB * 1024 * 1024) {
        
        // Close current file
        pImpl->fileOut.close();
        
        // Find next available index
        int index = pImpl->currentFileIndex + 1;
        std::string basePath = "log.txt"; // Simplified - would need actual base path
        
        // Rotate files
        for (int i = pImpl->rotationConfig.maxFiles - 1; i > 0; --i) {
            std::string oldPath = getRotatedFilename(basePath, i - 1);
            std::string newPath = getRotatedFilename(basePath, i);
            std::rename(oldPath.c_str(), newPath.c_str());
        }
        
        // Create new file
        pImpl->fileOut.open(getRotatedFilename(basePath, 0), std::ios::trunc);
        pImpl->currentFileSize = 0;
        pImpl->currentFileIndex = 0;
        
        // Compress if requested
        if (pImpl->rotationConfig.compressOldLogs) {
            for (int i = 1; i <= pImpl->rotationConfig.maxFiles; ++i) {
                std::string filepath = getRotatedFilename(basePath, i);
                if (std::filesystem::exists(filepath)) {
                    // Compress file (simplified - would use actual compression)
                }
            }
        }
    }
    
    // Daily rotation
    if (pImpl->rotationConfig.rotateDaily) {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::system_clock::to_time_t(now);
        tm* tm_now = std::localtime(&today);
        
        auto lastToday = std::chrono::system_clock::to_time_t(pImpl->lastRotation);
        tm* tm_last = std::localtime(&lastToday);
        
        if (tm_now->tm_year != tm_last->tm_year || 
            tm_now->tm_mon != tm_last->tm_mon || 
            tm_now->tm_mday != tm_last->tm_mday) {
            
            // Close current file
            pImpl->fileOut.close();
            
            // Create new file with date
            std::stringstream ss;
            ss << "log_" << (tm_now->tm_year + 1900) << "-"
               << std::setw(2) << std::setfill('0') << (tm_now->tm_mon + 1) << "-"
               << std::setw(2) << std::setfill('0') << tm_now->tm_mday
               << ".txt";
            
            pImpl->fileOut.open(ss.str(), std::ios::trunc);
            pImpl->currentFileSize = 0;
            pImpl->lastRotation = now;
        }
    }
}

std::string EnhancedConsoleLogger::getRotatedFilename(const std::string& basePath, int index) const {
    if (index == 0) {
        return basePath;
    }
    
    std::stringstream ss;
    ss << "log_" << index << ".txt";
    return ss.str();
}

// AdvancedLogger implementation
struct AdvancedLogger::Impl {
    std::mutex mutex;
    std::map<std::string, bool> componentEnabled;
    LogLevel levelThreshold = LogLevel::Info;
    std::string networkEndpoint;
    LogStats stats;
    std::map<std::string, double> performanceMetrics;
    size_t totalMemoryAllocated = 0;
};

AdvancedLogger::AdvancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->stats.firstLog = std::chrono::system_clock::now();
    
    // Enable default components
    setComponentEnabled("CORE", true);
    setComponentEnabled("BRAIN", true);
    setComponentEnabled("WORLD", true);
    setComponentEnabled("AGENT", true);
}

AdvancedLogger::~AdvancedLogger() = default;

void AdvancedLogger::setComponentEnabled(const std::string& component, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentEnabled[component] = enabled;
}

bool AdvancedLogger::isComponentEnabled(const std::string& component) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->componentEnabled.find(component);
    return it == pImpl->componentEnabled.end() || it->second;
}

void AdvancedLogger::logPerformance(const std::string& operation, double duration,
                                    const std::map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "PERFORMANCE: " + operation + " took " + std::to_string(duration) + "ms";
    entry.metadata = metadata;
    entry.metadata["duration_ms"] = std::to_string(duration);
    entry.module = "PERFORMANCE";
    
    // Log to console logger
    auto& consoleLogger = static_cast<ILogger&>(getGlobalAdvancedLogger().pImpl->componentLogger);
    consoleLogger.log(entry);
    
    // Update performance metrics
    pImpl->performanceMetrics[operation] = duration;
}

void AdvancedLogger::logMemoryUsage(size_t allocated, size_t available) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "MEMORY: Allocated " + std::to_string(allocated) + " bytes, " +
                   "Available " + std::to_string(available) + " bytes";
    entry.metadata["allocated_bytes"] = std::to_string(allocated);
    entry.metadata["available_bytes"] = std::to_string(available);
    entry.module = "MEMORY";
    
    // Log to console logger
    auto& consoleLogger = static_cast<ILogger&>(getGlobalAdvancedLogger().pImpl->componentLogger);
    consoleLogger.log(entry);
    
    pImpl->totalMemoryAllocated += allocated;
}

void AdvancedLogger::setNetworkLogEndpoint(const std::string& endpoint) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->networkEndpoint = endpoint;
}

void AdvancedLogger::sendLogToNetwork(const EnhancedLogEntry& entry) {
    if (!pImpl->networkEndpoint.empty()) {
        // Simplified - would implement actual network sending
        // For now, just log locally
        auto& consoleLogger = static_cast<ILogger&>(getGlobalAdvancedLogger().pImpl->componentLogger);
        consoleLogger.log(entry);
    }
}

AdvancedLogger::LogStats AdvancedLogger::getLogStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void AdvancedLogger::resetLogStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalLogs = 0;
    std::fill(std::begin(pImpl->stats.logsByLevel), std::end(pImpl->stats.logsByLevel), 0);
    pImpl->stats.firstLog = std::chrono::system_clock::now();
    pImpl->stats.lastLog = pImpl->stats.firstLog;
}

void AdvancedLogger::setLogLevelThreshold(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->levelThreshold = level;
}

bool AdvancedLogger::isLogLevelEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return level >= pImpl->levelThreshold;
}

// Python integration functions
namespace python {
    LogRotationConfig parseLogRotationConfig(const std::map<std::string, std::string>& config) {
        LogRotationConfig rotationConfig;
        
        auto it = config.find("max_size_mb");
        if (it != config.end()) {
            rotationConfig.maxSizeMB = std::stoul(it->second);
        }
        
        it = config.find("max_files");
        if (it != config.end()) {
            rotationConfig.maxFiles = std::stoul(it->second);
        }
        
        it = config.find("rotate_daily");
        if (it != config.end()) {
            rotationConfig.rotateDaily = (it->second == "true" || it->second == "1" || it->second == "yes");
        }
        
        it = config.find("compress_logs");
        if (it != config.end()) {
            rotationConfig.compressOldLogs = (it->second == "true" || it->second == "1" || it->second == "yes");
        }
        
        return rotationConfig;
    }
    
    py::dict getPythonLogStats(const AdvancedLogger::LogStats& stats) {
        py::dict result;
        result["total_logs"] = static_cast<long>(stats.totalLogs);
        
        py::dict byLevel;
        byLevel["debug"] = static_cast<long>(stats.logsByLevel[0]);
        byLevel["info"] = static_cast<long>(stats.logsByLevel[1]);
        byLevel["warning"] = static_cast<long>(stats.logsByLevel[2]);
        byLevel["error"] = static_cast<long>(stats.logsByLevel[3]);
        byLevel["critical"] = static_cast<long>(stats.logsByLevel[4]);
        
        result["by_level"] = byLevel;
        
        if (stats.totalLogs > 0) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.firstLog).count();
            result["logs_per_second"] = static_cast<double>(stats.totalLogs) / duration;
        }
        
        return result;
    }
    
    void registerPythonLogging(py::module& m) {
        py::class_<EnhancedConsoleLogger, ILogger>(m, "EnhancedConsoleLogger")
            .def(py::init<>())
            .def(py::init<LogLevel>(), py::arg("level"))
            .def("setLevel", &EnhancedConsoleLogger::setLevel)
            .def("getLevel", &EnhancedConsoleLogger::getLevel)
            .def("setUseColors", &EnhancedConsoleLogger::setUseColors)
            .def("getUseColors", &EnhancedConsoleLogger::getUseColors)
            .def("setOutputFile", &EnhancedConsoleLogger::setOutputFile)
            .def("closeOutputFile", &EnhancedConsoleLogger::closeOutputFile)
            .def("addModuleFilter", &EnhancedConsoleLogger::addModuleFilter)
            .def("removeModuleFilter", &EnhancedConsoleLogger::removeModuleFilter)
            .def("isModuleEnabled", &EnhancedConsoleLogger::isModuleEnabled)
            .def("setModuleLevel", &EnhancedConsoleLogger::setModuleLevel)
            .def("getModuleLevel", &EnhancedConsoleLogger::getModuleLevel)
            .def("logStructured", &EnhancedConsoleLogger::logStructured)
            .def("set_rotation_config", [](EnhancedConsoleLogger& logger, const std::map<std::string, std::string>& config) {
                LogRotationConfig rotationConfig = parseLogRotationConfig(config);
                // Simplified - would need to access private implementation
            });
        
        py::class_<AdvancedLogger, Logger>(m, "AdvancedLogger")
            .def(py::init<>())
            .def("setComponentEnabled", &AdvancedLogger::setComponentEnabled)
            .def("isComponentEnabled", &AdvancedLogger::isComponentEnabled)
            .def("logPerformance", &AdvancedLogger::logPerformance)
            .def("logMemoryUsage", &AdvancedLogger::logMemoryUsage)
            .def("setNetworkLogEndpoint", &AdvancedLogger::setNetworkLogEndpoint)
            .def("getLogStats", [](const AdvancedLogger& logger) {
                return getPythonLogStats(logger.getLogStats());
            })
            .def("resetLogStats", &AdvancedLogger::resetLogStats)
            .def("setLogLevelThreshold", &AdvancedLogger::setLogLevelThreshold)
            .def("isLogLevelEnabled", &AdvancedLogger::isLogLevelEnabled);
        
        // Set up global advanced logger
        m.def("get_global_advanced_logger", &getGlobalAdvancedLogger, "Get the global advanced logger instance");
        
        // Log level enum
        py::enum_<LogLevel>(m, "LogLevel")
            .value("DEBUG", LogLevel::Debug)
            .value("INFO", LogLevel::Info)
            .value("WARNING", LogLevel::Warning)
            .value("ERROR", LogLevel::Error)
            .value("CRITICAL", LogLevel::Critical);
        
        // Log rotation config
        py::class_<LogRotationConfig>(m, "LogRotationConfig")
            .def(py::init<>())
            .def_readwrite("max_size_mb", &LogRotationConfig::maxSizeMB)
            .def_readwrite("max_files", &LogRotationConfig::maxFiles)
            .def_readwrite("rotate_daily", &LogRotationConfig::rotateDaily)
            .def_readwrite("compress_old_logs", &LogRotationConfig::compressOldLogs);
    }
}

} // namespace nlm
