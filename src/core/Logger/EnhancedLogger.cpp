// NLM (熙然) - Neural Learning Machine
// Phase 2 Enhanced: Enhanced Logging and Error Handling
//
// This phase implements advanced logging and error handling capabilities
// to help users debug and monitor NLM brain operation.

#include "core/Logger/Logger.hpp"
#include "core/Logger/Logger.h>"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>

using namespace nlm;

// Enhanced Console Logger with better error handling
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}
    explicit EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
        pImpl->level = level;
    }
    ~EnhancedConsoleLogger() override = default;
    
    void log(const LogEntry& entry) override {
        if (entry.level < pImpl->level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp with milliseconds
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).milliseconds() % 1000;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms;
        
        // Add source location information
        if (!entry.file.empty()) {
            oss << " [" << entry.file;
            if (entry.line > 0) {
                oss << ":" << entry.line;
                if (!entry.function.empty()) {
                    oss << " in " << entry.function;
                }
            }
            oss << "]";
        }
        
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        // Console output with enhanced formatting
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
        
        // Handle critical errors with additional context
        if (entry.level >= LogLevel::Error) {
            handleCriticalLogEntry(entry);
        }
    }
    
    void setLevel(LogLevel level) override {
        pImpl->level = level;
    }
    
    LogLevel getLevel() const override {
        return pImpl->level;
    }
    
    void setUseColors(bool useColors) {
        pImpl->useColors = useColors;
    }
    
    bool getUseColors() const {
        return pImpl->useColors;
    }
    
    void setOutputFile(const std::string& filepath) {
        pImpl->fileOut.open(filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            // Write header
            pImpl->fileOut << "=== NLM Log Session ===" << std::endl;
            pImpl->fileOut << "Start time: " << std::put_time(
                std::localtime(&pImpl->sessionStart), "%Y-%m-%d %H:%M:%S") 
                << std::endl;
            pImpl->fileOut.flush();
        }
    }
    
    void closeOutputFile() {
        if (pImpl->fileOut.is_open()) {
            // Write footer
            pImpl->fileOut << "=== End NLM Log Session ===" << std::endl;
            pImpl->fileOut.close();
        }
    }
    
    void setLogFileRotation(size_t maxSizeMB, size_t maxFiles) {
        pImpl->fileRotationMaxSize = maxSizeMB * 1024 * 1024;
        pImpl->fileRotationMaxFiles = maxFiles;
    }
    
    // Enhanced error handling functions
    void setErrorCallback(std::function<void(const LogEntry&)> callback) {
        pImpl->errorCallback = callback;
    }
    
    void setLogDirectory(const std::string& directory) {
        pImpl->logDirectory = directory;
        createLogDirectory();
    }
    
    std::string getLogFilePath(const std::string& baseName) {
        if (!pImpl->logDirectory.empty()) {
            return pImpl->logDirectory + "/" + baseName;
        }
        return baseName;
    }
    
    struct Impl {
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::ofstream fileOut;
        std::mutex mutex;
        
        // Log rotation settings
        size_t fileRotationMaxSize = 0;
        size_t fileRotationMaxFiles = 0;
        
        // Error handling
        std::function<void(const LogEntry&)> errorCallback;
        
        // Directory for log files
        std::string logDirectory;
        
        // Session tracking
        time_t sessionStart;
        
        Impl() {
            sessionStart = std::time(nullptr);
        }
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    void handleCriticalLogEntry(const LogEntry& entry) {
        if (pImpl->errorCallback) {
            pImpl->errorCallback(entry);
        }
        
        // Log critical errors to system error stream for better visibility
        if (entry.level >= LogLevel::Critical) {
            std::cerr << "[CRITICAL] " << entry.message << std::endl;
            if (!entry.file.empty()) {
                std::cerr << "  Source: " << entry.file;
                if (entry.line > 0) std::cerr << ":" << entry.line;
                std::cerr << std::endl;
            }
        }
    }
    
    void createLogDirectory() {
        if (pImpl->logDirectory.empty()) return;
        
        // Try to create directory
        if (system(("mkdir -p " + pImpl->logDirectory).c_str()) != 0) {
            // Directory creation failed
        }
    }
};

// Enhanced Logger with error recovery
class EnhancedLogger : public Logger {
public:
    EnhancedLogger() : Logger() {
        // Initialize with enhanced console logger
        auto enhancedConsole = std::make_shared<EnhancedConsoleLogger>(LogLevel::Info);
        addLogger(enhancedConsole);
        
        // Add file logger if log directory is set
        if (!logDirectory.empty()) {
            auto fileLogger = std::make_shared<EnhancedConsoleLogger>(LogLevel::Info);
            fileLogger->setOutputFile(getLogFilePath("nlm.log"));
            addLogger(fileLogger);
        }
    }
    
    EnhancedLogger(const std::string& logDirectory) {
        this->logDirectory = logDirectory;
        auto enhancedConsole = std::make_shared<EnhancedConsoleLogger>(LogLevel::Info);
        enhancedConsole->setLogDirectory(logDirectory);
        addLogger(enhancedConsole);
        
        // Add file logger
        auto fileLogger = std::make_shared<EnhancedConsoleLogger>(LogLevel::Info);
        fileLogger->setOutputFile(getLogFilePath("nlm.log"));
        addLogger(fileLogger);
    }
    
    // Enhanced error handling methods
    void setErrorHandler(std::function<bool(const std::string&)> handler) {
        errorHandler = handler;
    }
    
    void setRecoveryMode(bool enabled) {
        recoveryMode = enabled;
    }
    
    bool isInRecoveryMode() const {
        return recoveryMode;
    }
    
    // Get log statistics
    struct LogStatistics {
        size_t debugCount = 0;
        size_t infoCount = 0;
        size_t warningCount = 0;
        size_t errorCount = 0;
        size_t criticalCount = 0;
        time_t startTime = 0;
        time_t endTime = 0;
    };
    
    LogStatistics getLogStatistics() const {
        LogStatistics stats;
        stats.startTime = std::time(nullptr);
        
        // This is a simplified implementation - in a real system,
        // you would track log counts over time
        for (auto& logger : pImpl->loggers) {
            if (auto enhancedLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger)) {
                // In a real implementation, you would access the logger's statistics
                // For now, we'll just initialize the structure
            }
        }
        
        return stats;
    }
    
    // Enable/disable logging to specific levels
    void enableLogging(LogLevel level) {
        setLevel(level);
    }
    
    void disableLogging(LogLevel level) {
        // This is a simplified implementation - in practice,
        // you might want more granular control
        if (level == LogLevel::Debug) {
            setLevel(static_cast<LogLevel>(static_cast<int>(getLevel()) + 1));
        }
    }
    
    // Log session start/end
    void startSession(const std::string& sessionName) {
        info("Starting NLM session: " + sessionName);
    }
    
    void endSession(const std::string& sessionName) {
        info("Ending NLM session: " + sessionName);
    }
    
private:
    std::string logDirectory;
    std::function<bool(const std::string&)> errorHandler;
    bool recoveryMode = false;
};

// Global enhanced logger
static std::shared_ptr<EnhancedLogger>& globalEnhancedLoggerPtr() {
    static std::shared_ptr<EnhancedLogger> ptr = std::make_shared<EnhancedLogger>();
    return ptr;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    return *globalEnhancedLoggerPtr();
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    if (logger) {
        globalEnhancedLoggerPtr() = logger;
    }
}

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG_ENHANCED(msg) EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_ENHANCED(msg) EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_ENHANCED(msg) EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_ENHANCED(msg) EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_ENHANCED(msg) EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Error handling function for NLM
bool NLMHandleError(const std::string& errorMessage, LogLevel severity = LogLevel::Error) {
    EnhancedLogger::getGlobal().error(errorMessage);
    
    // You could add additional error handling logic here
    // such as sending notifications, attempting recovery, etc.
    
    if (severity >= LogLevel::Critical) {
        // Critical errors might require immediate attention
        EnhancedLogger::getGlobal().critical("Critical error: " + errorMessage);
        return false; // Indicate error occurred
    }
    
    return true; // Continue despite error
}

// Initialize enhanced logging system
void InitializeEnhancedLogging(const std::string& logDirectory) {
    if (logDirectory.empty()) {
        EnhancedLogger::setGlobal(std::make_shared<EnhancedLogger>());
    } else {
        EnhancedLogger::setGlobal(std::make_shared<EnhancedLogger>(logDirectory));
    }
    
    EnhancedLogger::getGlobal().startSession("NLM Enhanced");
}