// NLM Advanced Features - Logging System Implementation
// Comprehensive logging framework with multiple destinations, rotation, and formatting

#include "LoggingSystem.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <filesystem>

namespace nlm {
namespace advanced {

// Implementation of LoggingSystem

struct LoggingSystem::Impl {
    LogLevel level;
    LogFormat format;
    std::vector<std::shared_ptr<LogDestinationHandler>> destinations;
    std::shared_ptr<LogFormatter> formatter;
    std::string sourceFilter;
    bool asyncLogging;
    size_t bufferSize;
    
    // Statistics
    LogStatistics stats;
    
    // Callbacks
    std::function<void(const LogEntry&)> logCallback;
    std::function<void(const LogStatistics&)> statsCallback;
    
    // Threading
    std::thread asyncWorker;
    std::queue<std::pair<LogEntry, std::string>> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    bool stopAsyncWorker;
    
    // File rotation
    std::string logFilepath;
    size_t maxFileSize;
    int maxBackupFiles;
    
    // Thread safety for main operations
    std::mutex configMutex;
    
    Impl() : level(LogLevel::Info), format(LogFormat::Text), asyncLogging(false),
             bufferSize(8192), stopAsyncWorker(false), maxFileSize(10 * 1024 * 1024), // 10MB
             maxBackupFiles(5) {
        formatter = std::make_shared<TextLogFormatter>();
        asyncWorker = std::thread(&Impl::asyncWorkerThread, this);
    }
    
    ~Impl() {
        stopAsyncWorker = true;
        queueCondVar.notify_one();
        if (asyncWorker.joinable()) {
            asyncWorker.join();
        }
    }
    
    void asyncWorkerThread() {
        while (!stopAsyncWorker) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondVar.wait_for(lock, std::chrono::milliseconds(100), 
                                 [this]() { return !logQueue.empty() || stopAsyncWorker; });
            
            if (stopAsyncWorker && logQueue.empty()) {
                break;
            }
            
            if (!logQueue.empty()) {
                auto entryPair = std::move(logQueue.front());
                logQueue.pop();
                lock.unlock();
                
                processLogEntry(entryPair.first, entryPair.second);
                lock.lock();
            }
        }
    }
    
    void processLogEntry(const LogEntry& entry, const std::string& formatted) {
        std::lock_guard<std::mutex> lock(configMutex);
        
        // Check if level is enabled
        if (entry.level < level) return;
        
        // Check source filter
        if (!sourceFilter.empty() && entry.source != sourceFilter) return;
        
        // Format the entry
        std::string formattedMessage;
        if (formatter) {
            formattedMessage = formatter->format(entry);
        } else {
            formattedMessage = formatted;
        }
        
        // Write to all destinations
        for (const auto& dest : destinations) {
            if (dest) {
                dest->write(entry, formattedMessage);
            }
        }
        
        // Call callback if set
        if (logCallback) {
            logCallback(entry);
        }
        
        // Update statistics
        updateStatistics(entry);
    }
    
    void updateStatistics(const LogEntry& entry) {
        stats.totalLogs++;
        stats.logsByLevel[static_cast<int>(entry.level)]++;
        
        // Hash source for statistics
        std::hash<std::string> hasher;
        size_t sourceHash = hasher(entry.source);
        if (sourceHash < 100) {
            stats.logsBySource[sourceHash]++;
        }
        
        auto now = std::chrono::system_clock::now();
        stats.lastLogTime = now;
        
        if (stats.totalLogs == 1) {
            stats.firstLogTime = now;
        }
        
        // Calculate log rate (per minute)
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.firstLogTime);
        if (duration.count() > 0) {
            stats.logRatePerMinute = (static_cast<double>(stats.totalLogs) / duration.count()) * 60.0;
        }
        
        // Update most frequent message
        stats.logFrequency[entry.message]++;
        if (stats.logFrequency[entry.message] > stats.mostFrequentMessageCount) {
            stats.mostFrequentMessageCount = stats.logFrequency[entry.message];
            stats.mostFrequentMessage = entry.message;
        }
        
        // Update recent messages
        stats.recentMessages.push_back(entry.message);
        if (stats.recentMessages.size() > 100) {
            stats.recentMessages.pop_front();
        }
        
        // Call statistics callback
        if (statsCallback) {
            statsCallback(stats);
        }
    }
};

LoggingSystem::LoggingSystem() : pImpl(std::make_unique<Impl>()) {
}

LoggingSystem::~LoggingSystem() {
    if (pImpl->asyncWorker.joinable()) {
        pImpl->stopAsyncWorker = true;
        pImpl->queueCondVar.notify_one();
        pImpl->asyncWorker.join();
    }
}

void LoggingSystem::configure(LogLevel level, LogFormat format,
                              const std::vector<LogDestination>& destinations) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    pImpl->level = level;
    pImpl->format = format;
    pImpl->destinations.clear();
    
    for (const auto& dest : destinations) {
        addDestination(dest);
    }
}

void LoggingSystem::addDestination(LogDestination destination, const std::string& param) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    switch (destination) {
        case LogDestination::Console:
            pImpl->destinations.push_back(std::make_shared<ConsoleLogDestination>());
            break;
        case LogDestination::File:
            if (!param.empty()) {
                pImpl->destinations.push_back(
                    std::make_shared<FileLogDestination>(param, pImpl->maxFileSize, pImpl->maxBackupFiles)
                );
            }
            break;
        case LogDestination::Network:
            // Network logging implementation would go here
            break;
        case LogDestination::Database:
            // Database logging implementation would go here
            break;
    }
}

void LoggingSystem::addDestination(std::shared_ptr<LogDestinationHandler> handler) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    if (handler) {
        pImpl->destinations.push_back(handler);
    }
}

void LoggingSystem::log(LogLevel level, const std::string& source, const std::string& message,
                        const std::string& file, int line, const std::string& function) {
    // Create log entry
    LogEntry entry;
    entry.level = level;
    entry.source = source;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    // Format message
    std::string formatted;
    if (pImpl->formatter) {
        formatted = pImpl->formatter->format(entry);
    }
    
    // Process log entry
    if (pImpl->asyncLogging) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->logQueue.push({entry, formatted});
        pImpl->queueCondVar.notify_one();
    } else {
        pImpl->processLogEntry(entry, formatted);
    }
}

void LoggingSystem::log(LogLevel level, const std::string& source, const std::string& message,
                        const std::map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.level = level;
    entry.source = source;
    entry.message = message;
    entry.metadata = metadata;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    std::string formatted;
    if (pImpl->formatter) {
        formatted = pImpl->formatter->format(entry);
    }
    
    if (pImpl->asyncLogging) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->logQueue.push({entry, formatted});
        pImpl->queueCondVar.notify_one();
    } else {
        pImpl->processLogEntry(entry, formatted);
    }
}

void LoggingSystem::debug(const std::string& source, const std::string& message,
                          const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, source, message, file, line, function);
}

void LoggingSystem::info(const std::string& source, const std::string& message,
                         const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, source, message, file, line, function);
}

void LoggingSystem::warning(const std::string& source, const std::string& message,
                            const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, source, message, file, line, function);
}

void LoggingSystem::error(const std::string& source, const std::string& message,
                          const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, source, message, file, line, function);
}

void LoggingSystem::critical(const std::string& source, const std::string& message,
                             const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, source, message, file, line, function);
}

LogStatistics LoggingSystem::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->stats;
}

void LoggingSystem::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->stats = LogStatistics();
}

void LoggingSystem::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->level = level;
}

void LoggingSystem::setFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->format = format;
    // Change formatter if needed
    switch (format) {
        case LogFormat::Text:
            pImpl->formatter = std::make_shared<TextLogFormatter>();
            break;
        case LogFormat::JSON:
            pImpl->formatter = std::make_shared<JsonLogFormatter>();
            break;
        case LogFormat::CSV:
            // CSV formatter would go here
            pImpl->formatter = std::make_shared<TextLogFormatter>();
            break;
        case LogFormat::XML:
            // XML formatter would go here
            pImpl->formatter = std::make_shared<TextLogFormatter>();
            break;
        case LogFormat::YAML:
            // YAML formatter would go here
            pImpl->formatter = std::make_shared<TextLogFormatter>();
            break;
    }
}

void LoggingSystem::setSourceFilter(const std::string& sourceFilter) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->sourceFilter = sourceFilter;
}

void LoggingSystem::setAsyncLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->asyncLogging = enabled;
}

void LoggingSystem::setFileRotation(const std::string& filepath, size_t maxSize, int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->logFilepath = filepath;
    pImpl->maxFileSize = maxSize;
    pImpl->maxBackupFiles = maxFiles;
    
    // Add or update file destination
    addDestination(LogDestination::File, filepath);
}

void LoggingSystem::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->maxFileSize = maxSize;
}

void LoggingSystem::setMaxBackupFiles(int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->maxBackupFiles = maxFiles;
}

void LoggingSystem::setLogCallback(const std::function<void(const LogEntry&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->logCallback = callback;
}

void LoggingSystem::setStatisticsCallback(const std::function<void(const LogStatistics&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->statsCallback = callback;
}

void LoggingSystem::setLogFile(const std::string& filepath) {
    setFileRotation(filepath, pImpl->maxFileSize, pImpl->maxBackupFiles);
}

void LoggingSystem::closeLogFile() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    // Remove existing file destinations
    auto it = pImpl->destinations.begin();
    while (it != pImpl->destinations.end()) {
        // Check if it's a file destination (simplified check)
        if ((*it)->getType() == "File") { // Would need virtual method in base class
            it = pImpl->destinations.erase(it);
        } else {
            ++it;
        }
    }
}

std::string LoggingSystem::getLogFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return pImpl->logFilepath;
}

void LoggingSystem::setCustomFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->formatter = formatter;
}

void LoggingSystem::setBufferSize(size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->bufferSize = bufferSize;
}

void LoggingSystem::flushBuffers() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    for (const auto& dest : pImpl->destinations) {
        if (dest) {
            dest->flush();
        }
    }
}

void LoggingSystem::lock() {
    pImpl->configMutex.lock();
}

void LoggingSystem::unlock() {
    pImpl->configMutex.unlock();
}

bool LoggingSystem::tryLock() {
    return pImpl->configMutex.try_lock();
}

LoggingSystem& LoggingSystem::getInstance() {
    static LoggingSystem instance;
    return instance;
}

void LoggingSystem::resetInstance() {
    // This is primarily for testing
    // In a real implementation, you would properly reset the singleton
}

// Additional helper methods
std::string LoggingSystem::createLogEntry(const std::string& source, const std::string& message,
                                          LogLevel level, const std::string& file,
                                          int line, const std::string& function) {
    LogEntry entry;
    entry.source = source;
    entry.message = message;
    entry.level = level;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    if (pImpl->formatter) {
        return pImpl->formatter->format(entry);
    }
    
    return ""; // Fallback
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& source,
                     const std::string& file, int line, const std::string& function)
    : level_(level), source_(source), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    // Auto-log the accumulated message
    nlm::advanced::LoggingSystem::getInstance()
        .log(level_, source_, oss_.str(), file_, line_, function_);
}

} // namespace advanced
} // namespace nlm

// Add virtual method to LogDestinationHandler for type checking
namespace nlm {
    namespace advanced {
        class LogDestinationHandler {
        public:
            virtual ~LogDestinationHandler() = default;
            virtual bool write(const LogEntry& entry, const std::string& formatted) = 0;
            virtual void flush() = 0;
            virtual void close() = 0;
            virtual std::string getType() const = 0;
        };
        
        // Update ConsoleLogDestination to inherit from LogDestinationHandler
        class ConsoleLogDestination : public LogDestinationHandler {
        public:
            bool write(const LogEntry& entry, const std::string& formatted) override {
                std::cout << formatted << std::endl;
                return true;
            }
            
            void flush() override {
                std::cout.flush();
            }
            
            void close() override {
                // Nothing to close for console
            }
            
            std::string getType() const override {
                return "Console";
            }
        };
        
        // Update FileLogDestination to inherit from LogDestinationHandler
        class FileLogDestination : public LogDestinationHandler {
        public:
            FileLogDestination(const std::string& filepath, size_t maxSize, int maxFiles) 
                : filepath_(filepath), maxSize_(maxSize), maxFiles_(maxFiles) {
                rotation_ = 0;
                currentFile_.open(filepath_, std::ios::app | std::ios::binary);
                if (currentFile_.is_open()) {
                    currentFile_.seekg(0, std::ios::end);
                    currentSize_ = currentFile_.tellg();
                }
            }
            
            ~FileLogDestination() {
                close();
            }
            
            bool write(const LogEntry& entry, const std::string& formatted) override {
                if (!currentFile_.is_open()) {
                    return false;
                }
                
                if (currentSize_ + formatted.size() + 1 > maxSize_) {
                    rotateFiles();
                }
                
                currentFile_ << formatted << std::endl;
                if (!currentFile_.good()) {
                    return false;
                }
                
                currentSize_ += formatted.size() + 1;
                return true;
            }
            
            void flush() override {
                if (currentFile_.is_open()) {
                    currentFile_.flush();
                }
            }
            
            void close() override {
                if (currentFile_.is_open()) {
                    currentFile_.close();
                }
            }
            
            std::string getType() const override {
                return "File";
            }
            
        private:
            void rotateFiles() {
                if (currentFile_.is_open()) {
                    currentFile_.close();
                }
                
                // Close current file and rotate if needed
                std::string currentPath = filepath_ + "." + std::to_string(rotation_);
                if (std::filesystem::exists(currentPath)) {
                    // Remove oldest file if we have too many
                    if (rotation_ >= static_cast<size_t>(maxFiles_) - 1) {
                        std::filesystem::remove(currentPath);
                        for (int i = maxFiles_ - 2; i >= 0; --i) {
                            std::string src = filepath_ + "." + std::to_string(i);
                            std::string dst = filepath_ + "." + std::to_string(i + 1);
                            if (std::filesystem::exists(src)) {
                                std::filesystem::rename(src, dst);
                            }
                        }
                        rotation_ = maxFiles_ - 2;
                    } else {
                        rotation_++;
                    }
                }
                
                // Reopen with new name
                currentPath = filepath_ + "." + std::to_string(rotation_);
                currentFile_.open(currentPath, std::ios::trunc | std::ios::binary);
                if (currentFile_.is_open()) {
                    currentSize_ = 0;
                }
            }
            
            std::string filepath_;
            size_t maxSize_;
            int maxFiles_;
            int rotation_;
            std::fstream currentFile_;
            size_t currentSize_;
        };
        
    } // namespace advanced
} // namespace nlm

// Include necessary headers
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <map>
#include <deque>
#include <functional>

namespace nlm {
namespace advanced {

// Initialize async worker thread
void LoggingSystem::Impl::asyncWorkerThread() {
    while (!stopAsyncWorker) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait_for(lock, std::chrono::milliseconds(100),
                             [this]() { return !logQueue.empty() || stopAsyncWorker; });
        
        if (stopAsyncWorker && logQueue.empty()) {
            break;
        }
        
        if (!logQueue.empty()) {
            auto entryPair = std::move(logQueue.front());
            logQueue.pop();
            lock.unlock();
            
            processLogEntry(entryPair.first, entryPair.second);
            lock.lock();
        }
    }
}

// Implementation of processLogEntry, updateStatistics, etc.
// (These are defined above in the class definition)

} // namespace advanced
} // namespace nlm

// Initialize the async worker thread in the constructor
LoggingSystem::LoggingSystem() : pImpl(std::make_unique<Impl>()) {
    // The async worker thread is started in the Impl constructor
}