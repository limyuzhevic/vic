#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <sstream>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::map<LogLevel, std::string> colorScheme;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string timeFormat = "%Y-%m-%d %H:%M:%S.%f";
    bool includeThreadId = false;
    std::string pattern = "[%time%] [%level%] [%category%] [%tag%] %message";
    
    // Async logging
    bool asyncEnabled = false;
    size_t queueSize = 10000;
    int workerThreads = 1;
    std::queue<LogEntry> messageQueue;
    std::thread workerThread;
    std::atomic<bool> workerShouldStop{false};
    
    // File rotation
    std::string outputFilepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxBackupFiles = 5;
    std::atomic<uint64_t> currentFileSize{0};
    
    // Category and tag filters
    std::set<LogCategory> categoryFilter;
    std::set<std::string> tagFilter;
    
    Impl() {
        // Initialize color scheme
        colorScheme[LogLevel::Trace] = "\033[36m";    // Cyan
        colorScheme[LogLevel::Debug] = "\033[37m";    // White
        colorScheme[LogLevel::Info] = "\033[32m";     // Green
        colorScheme[LogLevel::Notice] = "\033[94m";  // Blue
        colorScheme[LogLevel::Warning] = "\033[33m";  // Yellow
        colorScheme[LogLevel::Error] = "\033[31m";    // Red
        colorScheme[LogLevel::Critical] = "\033[35m"; // Magenta
        colorScheme[LogLevel::Alert] = "\033[95m";    // Pink
        colorScheme[LogLevel::Emergency] = "\033[41m;37m"; // Red background, white text
        
        // Start worker thread if async logging enabled
        if (asyncEnabled) {
            workerShouldStop = false;
            workerThread = std::thread(&Impl::workerFunction, this);
        }
    }
    
    ~Impl() {
        workerShouldStop = true;
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    void workerFunction() {
        while (!workerShouldStop || !messageQueue.empty()) {
            std::queue<LogEntry> localQueue;
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::swap(localQueue, messageQueue);
            }
            
            while (!localQueue.empty()) {
                LogEntry entry = localQueue.front();
                localQueue.pop();
                logInternal(entry);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void log(const LogEntry& entry) {
        if (asyncEnabled) {
            std::lock_guard<std::mutex> lock(mutex);
            if (messageQueue.size() < queueSize) {
                messageQueue.push(entry);
            } else {
                // Queue full, log synchronously as fallback
                logInternal(entry);
            }
        } else {
            logInternal(entry);
        }
    }
    
    void logInternal(const LogEntry& entry) {
        // Apply filters
        if (!categoryFilter.empty() && categoryFilter.find(entry.category) == categoryFilter.end()) {
            return;
        }
        
        if (!tagFilter.empty() && !tagFilter.empty() && entry.tag.empty()) {
            return;
        }
        
        if (entry.level < level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        std::ostringstream oss;
        std::string message = formatMessage(entry);
        
        // Console output
        if (useColors) {
            std::string colorCode = levelToColor(entry.level);
            oss << colorCode << message << "\033[0m\n";
        } else {
            oss << message << "\n";
        }
        
        std::string output = oss.str();
        
        // Console output
        std::cout << output;
        
        // File output if set
        if (fileOut.is_open()) {
            fileOut << output;
            fileOut.flush();
            
            // Check file size for rotation
            if (currentFileSize >= maxFileSize) {
                rotateFile();
            }
        }
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Notice: return "NOTICE";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Alert: return "ALERT";
            case LogLevel::Emergency: return "EMERG";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) const {
        auto it = colorScheme.find(level);
        if (it != colorScheme.end()) {
            return it->second;
        }
        return "\033[0m";
    }
    
    std::string formatMessage(const LogEntry& entry) const {
        std::string message = entry.message;
        std::string formatted = pattern;
        
        // Replace placeholders
        size_t pos = 0;
        while ((pos = formatted.find("%level%")) != std::string::npos) {
            formatted.replace(pos, 7, levelToString(entry.level));
        }
        
        pos = 0;
        while ((pos = formatted.find("%time%")) != std::string::npos) {
            formatted.replace(pos, 7, entry.getFormattedTimestamp());
        }
        
        pos = 0;
        while ((pos = formatted.find("%category%")) != std::string::npos) {
            formatted.replace(pos, 10, categoryToString(entry.category));
        }
        
        pos = 0;
        while ((pos = formatted.find("%tag%")) != std::string::npos) {
            formatted.replace(pos, 6, entry.tag);
        }
        
        pos = 0;
        while ((pos = formatted.find("%message%")) != std::string::npos) {
            formatted.replace(pos, 9, message);
        }
        
        // Add thread ID if requested
        if (includeThreadId && !entry.threadId.empty()) {
            formatted += " [Thread: " + entry.threadId + "]";
        }
        
        return formatted;
    }
    
    std::string categoryToString(LogCategory category) const {
        switch (category) {
            case LogCategory::Core: return "CORE";
            case LogCategory::Brain: return "BRAIN";
            case LogCategory::Memory: return "MEMORY";
            case LogCategory::Plasticity: return "PLASTICITY";
            case LogCategory::Development: return "DEVELOPMENT";
            case LogCategory::Sensory: return "SENSORY";
            case LogCategory::Motor: return "MOTOR";
            case LogCategory::Cognition: return "COGNITION";
            case LogCategory::Prediction: return "PREDICTION";
            case LogCategory::Neuromodulation: return "NEUROMODULATION";
            case LogCategory::MemoryManagement: return "MEMORY_MGMT";
            case LogCategory::Performance: return "PERFORMANCE";
            case LogCategory::Configuration: return "CONFIG";
            case LogCategory::Python: return "PYTHON";
            case LogCategory::User: return "USER";
            case LogCategory::System: return "SYSTEM";
            default: return "CUSTOM";
        }
    }
    
    void rotateFile() {
        if (!fileOut.is_open()) return;
        
        fileOut.close();
        
        // Rotate existing files
        for (int i = maxBackupFiles - 1; i >= 0; --i) {
            std::string from = outputFilepath;
            if (i > 0) {
                from = outputFilepath + "." + std::to_string(i + 1);
            }
            std::string to = outputFilepath;
            if (i < maxBackupFiles - 1) {
                to = outputFilepath + "." + std::to_string(i + 2);
            } else if (i == maxBackupFiles - 1) {
                to = outputFilepath + ".bak";
            }
            
            if (std::filesystem::exists(from)) {
                std::filesystem::rename(from, to);
            }
        }
        
        // Reopen file
        fileOut.open(outputFilepath, std::ios::app);
        currentFileSize = 0;
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    pImpl->log(entry);
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setColorScheme(const std::map<LogLevel, std::string>& colors) {
    pImpl->colorScheme = colors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath, size_t maxFileSize, int maxFiles) {
    closeOutputFile();
    pImpl->outputFilepath = filepath;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxBackupFiles = maxFiles;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        // Get current file size
        pImpl->fileOut.seekp(0, std::ios::end);
        pImpl->currentFileSize = pImpl->fileOut.tellp();
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::getOutputFileInfo(std::string& filepath, uint64_t& fileSize, int64_t& lastModified) const {
    if (pImpl->fileOut.is_open()) {
        filepath = pImpl->outputFilepath;
        fileSize = pImpl->currentFileSize;
        
        // Get last modified time
        auto ftime = std::filesystem::last_write_time(pImpl->outputFilepath);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        lastModified = std::chrono::system_clock::to_time_t(sctp);
        
        return true;
    }
    return false;
}

void ConsoleLogger::setPattern(const std::string& pattern) {
    pImpl->pattern = pattern;
}

void ConsoleLogger::setTimeFormat(const std::string& format) {
    pImpl->timeFormat = format;
}

void ConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

void ConsoleLogger::enableAsyncLogging(size_t queueSize, int workerThreads) {
    if (!pImpl->asyncEnabled) {
        pImpl->asyncEnabled = true;
        pImpl->queueSize = queueSize;
        pImpl->workerThreads = workerThreads;
        // Note: worker thread will be started in constructor
    }
}

bool ConsoleLogger::isAsyncLoggingEnabled() const {
    return pImpl->asyncEnabled;
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;
    int maxBackupFiles = 5;
    std::string filepath;
    bool flushOnWrite = true;
    uint64_t currentFileSize = 0;
    
    Impl() {}
    ~Impl() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setFilepath(filepath);
    setLevel(level);
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::ostringstream oss;
    oss << "[" << pImpl->levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
    
    oss << " " << entry.message << "\n";
    
    std::string output = oss.str();
    
    fileOut << output;
    if (pImpl->flushOnWrite) {
        fileOut.flush();
    }
    
    pImpl->currentFileSize += output.size();
    
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        pImpl->rotateFile();
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFilepath(const std::string& filepath) {
    if (fileOut.is_open()) {
        fileOut.close();
    }
    
    pImpl->filepath = filepath;
    fileOut.open(filepath, std::ios::app);
    
    if (fileOut.is_open()) {
        fileOut.seekp(0, std::ios::end);
        pImpl->currentFileSize = fileOut.tellp();
    }
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxBackupFiles(int maxFiles) {
    pImpl->maxBackupFiles = maxFiles;
}

void FileLogger::setFlushOnWrite(bool flush) {
    pImpl->flushOnWrite = flush;
}

bool FileLogger::getFileInfo(std::string& filepath, uint64_t& fileSize, int64_t& lastModified) const {
    if (fileOut.is_open()) {
        filepath = pImpl->filepath;
        fileSize = pImpl->currentFileSize;
        
        auto ftime = std::filesystem::last_write_time(pImpl->filepath);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        lastModified = std::chrono::system_clock::to_time_t(sctp);
        
        return true;
    }
    return false;
}

void FileLogger::rotateFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!fileOut.is_open()) return;
    
    fileOut.close();
    
    // Rotate existing files
    for (int i = pImpl->maxBackupFiles - 1; i >= 0; --i) {
        std::string from = pImpl->filepath;
        if (i > 0) {
            from = pImpl->filepath + "." + std::to_string(i + 1);
        }
        std::string to = pImpl->filepath;
        if (i < pImpl->maxBackupFiles - 1) {
            to = pImpl->filepath + "." + std::to_string(i + 2);
        } else if (i == pImpl->maxBackupFiles - 1) {
            to = pImpl->filepath + ".bak";
        }
        
        if (std::filesystem::exists(from)) {
            std::filesystem::rename(from, to);
        }
    }
    
    // Reopen file
    fileOut.open(pImpl->filepath, std::ios::app);
    pImpl->currentFileSize = 0;
}

// RotatingFileLogger implementation
struct RotatingFileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string baseFilename;
    size_t maxFileSize = 10 * 1024 * 1024;
    int maxBackupFiles = 5;
    uint64_t currentFileSize = 0;
    int currentFileNumber = 0;
    
    Impl() {}
    ~Impl() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
    
    std::string getCurrentFilename() const {
        if (currentFileNumber == 0) {
            return baseFilename;
        } else {
            return baseFilename + "." + std::to_string(currentFileNumber);
        }
    }
};

RotatingFileLogger::RotatingFileLogger(const std::string& baseFilename, size_t maxFileSize, int maxBackupFiles)
    : pImpl(std::make_unique<Impl>()) {
    setBaseFilename(baseFilename);
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxBackupFiles = maxBackupFiles;
}

RotatingFileLogger::~RotatingFileLogger() = default;

void RotatingFileLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Check if we need to rotate
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotateImmediately();
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
    
    oss << " " << entry.message << "\n";
    
    std::string output = oss.str();
    
    fileOut << output;
    fileOut.flush();
    
    pImpl->currentFileSize += output.size();
}

void RotatingFileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel RotatingFileLogger::getLevel() const {
    return pImpl->level;
}

void RotatingFileLogger::setBaseFilename(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (fileOut.is_open()) {
        fileOut.close();
    }
    
    pImpl->baseFilename = filename;
    pImpl->currentFileNumber = 0;
    
    fileOut.open(pImpl->getCurrentFilename(), std::ios::app);
    if (fileOut.is_open()) {
        fileOut.seekp(0, std::ios::end);
        pImpl->currentFileSize = fileOut.tellp();
    }
}

std::string RotatingFileLogger::getCurrentFilename() const {
    return pImpl->getCurrentFilename();
}

bool RotatingFileLogger::rotateImmediately() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!fileOut.is_open()) {
        return false;
    }
    
    fileOut.close();
    
    // Increment file number
    pImpl->currentFileNumber++;
    
    // Rotate backup files
    for (int i = pImpl->maxBackupFiles - 1; i >= 0; --i) {
        std::string from = pImpl->baseFilename;
        if (i > 0) {
            from = pImpl->baseFilename + "." + std::to_string(i + 1);
        }
        std::string to = pImpl->baseFilename;
        if (i < pImpl->maxBackupFiles - 1) {
            to = pImpl->baseFilename + "." + std::to_string(i + 2);
        }
        
        if (std::filesystem::exists(from)) {
            std::filesystem::rename(from, to);
        }
    }
    
    // Reopen current file
    fileOut.open(pImpl->getCurrentFilename(), std::ios::app);
    pImpl->currentFileSize = 0;
    
    return true;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool asyncEnabled = false;
    size_t asyncQueueSize = 10000;
    int asyncWorkerThreads = 1;
    std::string defaultPattern = "[%time%] [%level%] [%category%] [%tag%] %message";
    std::string timeFormat = "%Y-%m-%d %H:%M:%S.%f";
    bool includeThreadId = false;
    
    std::atomic<size_t> totalLogCount{0};
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add global console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    addLogger(consoleLogger);
}

Logger::~Logger() = default;

bool Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Configure logger with default settings
    logger->setLevel(LogLevel::Info);
    logger->setPattern(pImpl->defaultPattern);
    
    pImpl->loggers.push_back(logger);
    return true;
}

bool Logger::removeLogger(ILogger* logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto it = pImpl->loggers.begin(); it != pImpl->loggers.end(); ++it) {
        if (it->get() == logger) {
            pImpl->loggers.erase(it);
            return true;
        }
    }
    return false;
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

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                LogCategory category, const std::string& tag) {
    if (!isEnabled(level)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.category = category;
    entry.tag = tag;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->totalLogCount++;
}

void Logger::logWithFields(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& fields,
                         const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.fields = fields;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    pImpl->totalLogCount++;
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function,
                   LogCategory category, const std::string& tag) {
    log(LogLevel::Trace, message, file, line, function, category, tag);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function,
                   LogCategory category, const std::string& tag) {
    log(LogLevel::Debug, message, file, line, function, category, tag);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function,
                  LogCategory category, const std::string& tag) {
    log(LogLevel::Info, message, file, line, function, category, tag);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function,
                    LogCategory category, const std::string& tag) {
    log(LogLevel::Notice, message, file, line, function, category, tag);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function,
                     LogCategory category, const std::string& tag) {
    log(LogLevel::Warning, message, file, line, function, category, tag);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function,
                   LogCategory category, const std::string& tag) {
    log(LogLevel::Error, message, file, line, function, category, tag);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function,
                      LogCategory category, const std::string& tag) {
    log(LogLevel::Critical, message, file, line, function, category, tag);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function,
                   LogCategory category, const std::string& tag) {
    log(LogLevel::Alert, message, file, line, function, category, tag);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function,
                       LogCategory category, const std::string& tag) {
    log(LogLevel::Emergency, message, file, line, function, category, tag);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static std::shared_ptr<Logger> instance = std::make_shared<Logger>();
    return *instance;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        static std::shared_ptr<Logger>& instance = *reinterpret_cast<std::shared_ptr<Logger>*>
            (&Logger::getGlobal());
        instance = logger;
    }
}

void Logger::enableAsyncLogging(size_t queueSize, int workerThreads) {
    pImpl->asyncEnabled = true;
    pImpl->asyncQueueSize = queueSize;
    pImpl->asyncWorkerThreads = workerThreads;
    
    // Configure existing loggers for async
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->enableAsyncLogging(queueSize, workerThreads);
        }
    }
}

bool Logger::isAsyncLoggingEnabled() const {
    return pImpl->asyncEnabled;
}

void Logger::setDefaultPattern(const std::string& pattern) {
    pImpl->defaultPattern = pattern;
    for (auto& logger : pImpl->loggers) {
        logger->setPattern(pattern);
    }
}

void Logger::setTimeFormat(const std::string& format) {
    pImpl->timeFormat = format;
}

void Logger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

size_t Logger::getLoggerCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loggers.size();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                     LogCategory category, const std::string& tag)
    : level_(level), file_(file), line_(line), function_(function),
      category_(category), tag_(tag) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_, tag_);
}

LogStream& LogStream::operator<<(const std::pair<std::string, std::string>& field) {
    // Store field in a local map for the log entry
    // Note: This is a simplified implementation - in a full implementation,
    // you'd need to modify the LogEntry class to handle structured fields
    return *this;
}

// Helper function to convert LogLevel to string
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

// Version function
std::string Logger::getLoggerVersion() {
    return "Enhanced Logger v2.0.0";
}

} // namespace nlm

// Version function for backwards compatibility
namespace nlm {
    std::string getLoggerVersion() {
        return Logger::getLoggerVersion();
    }
}