#include "Logger.hpp"
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <shared_mutex>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <set>

namespace nlm {

// Add new log levels for enhanced logging
enum class EnhancedLogLevel {
    Trace = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5,
    Count = 7  // Keep this as the last entry for convenience
};

// Log entry with enhanced fields for structured logging
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    std::string category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::chrono::nanoseconds timestampNs;  // For millisecond precision
    std::thread::id threadId;
    int cpuCore;
    size_t memoryUsage;
    uint64_t threadTimestamp;  // Unique timestamp for each log entry
    std::unordered_map<std::string, std::string> fields;  // For structured logging
};

// Category filter structure
struct CategoryFilter {
    std::string name;
    EnhancedLogLevel minLevel;
    EnhancedLogLevel maxLevel;
    bool enabled = true;
    size_t rateLimit = 0;  // 0 = no limit
    size_t rateCount = 0;
    std::chrono::steady_clock::time_point rateWindowStart;
    std::chrono::nanoseconds rateWindowDuration = std::chrono::seconds(1);
};

// Configuration structure for advanced logging
struct LogConfig {
    // Log levels
    EnhancedLogLevel globalLevel = EnhancedLogLevel::Info;
    bool useEnhancedLevels = true;
    
    // Categories
    std::vector<CategoryFilter> categories;
    
    // Asynchronous logging
    size_t asyncQueueSize = 10000;
    bool useAsyncLogging = true;
    
    // File output
    std::string logFile;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    bool compressLogs = false;
    std::chrono::hours rotationTime = std::chrono::hours(1);
    int maxRotatedFiles = 5;
    
    // Performance
    bool useLockFreeQueue = true;
    size_t bufferPoolSize = 256;
    
    // Filtering
    std::vector<std::regex> includePatterns;
    std::vector<std::regex> excludePatterns;
    
    // Sampling
    float sampleRate = 1.0f;  // 1.0 = 100%, 0.1 = 10%
    
    // Advanced features
    bool logSystemMetrics = true;
    bool logPerformanceMetrics = true;
    bool logGpuEvents = false;
    std::unordered_map<std::string, std::string> customFields;
};

// Custom formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const EnhancedLogEntry& entry) = 0;
};

// JSON formatter
class JsonFormatter : public ILogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) override {
        std::ostringstream oss;
        oss << "{\"timestamp\": \"" << formatTimestamp(entry) << "\",\"timestamp_ns\": " << entry.timestampNs.count() 
            << ",\"level\": \"" << enhancedLevelToString(entry.level) << "\",\"category\": \"" << entry.category << "\",\"message\": \"" << escapeJson(entry.message) << "\",\"thread_id\": \"" 
            << entry.threadId << "\",\"cpu_core\": " << entry.cpuCore << ",\"memory_usage\": " << entry.memoryUsage;
        
        for (const auto& field : entry.fields) {
            oss << ",\"" << field.first << "\": \"" << escapeJson(field.second) << "\"";
        }
        
        if (!entry.file.empty()) {
            oss << ",\"file\": \"" << escapeJson(entry.file) << "\",\"line\": " << entry.line 
                << ",\"function\": \"" << escapeJson(entry.function) << "\"";
        }
        
        oss << "}";
        return oss.str();
    }
    
private:
    std::string formatTimestamp(const EnhancedLogEntry& entry) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(entry.timestampNs).count() % 1000;
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.") 
            << std::setfill('0') << std::setw(3) << ms;
        return oss.str();
    }
    
    std::string escapeJson(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string enhancedLevelToString(EnhancedLogLevel level) {
        switch (level) {
            case EnhancedLogLevel::Trace: return "TRACE";
            case EnhancedLogLevel::Debug: return "DEBUG";
            case EnhancedLogLevel::Info: return "INFO";
            case EnhancedLogLevel::Warning: return "WARN";
            case EnhancedLogLevel::Error: return "ERROR";
            case EnhancedLogLevel::Critical: return "CRITICAL";
            case EnhancedLogLevel::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

// Lock-free queue implementation (simplified for now)
template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T value;
        std::atomic<Node*> next;
        Node(const T& v) : value(v), next(nullptr) {}
    };
    
    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::atomic<size_t> size;
    
public:
    LockFreeQueue() {
        Node* dummy = new Node(T{});
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
        size.store(0, std::memory_order_relaxed);
    }
    
    ~LockFreeQueue() {
        Node* current = head.load(std::memory_order_relaxed);
        while (current != tail.load(std::memory_order_relaxed)) {
            Node* next = current->next.load(std::memory_order_relaxed);
            delete current;
            current = next;
        }
        delete current;
    }
    
    bool push(const T& value) {
        Node* node = new Node(value);
        Node* tailPrev = tail.load(std::memory_order_relaxed);
        
        // Try to atomically update tail
        if (!tail.compare_exchange_weak(tailPrev, node, std::memory_order_release, std::memory_order_relaxed)) {
            delete node;
            return false;
        }
        
        tailPrev->next.store(node, std::memory_order_release);
        size.fetch_add(1, std::memory_order_relaxed);
        return true;
    }
    
    bool pop(T& value) {
        Node* headPrev = head.load(std::memory_order_relaxed);
        Node* nextNode;
        
        if (headPrev == tail.load(std::memory_order_relaxed)) {
            return false;  // Queue is empty
        }
        
        nextNode = headPrev->next.load(std::memory_order_acquire);
        if (!head.compare_exchange_weak(headPrev, nextNode, std::memory_order_acquire, std::memory_order_relaxed)) {
            return false;
        }
        
        value = nextNode->value;
        delete headPrev;
        size.fetch_sub(1, std::memory_order_relaxed);
        return true;
    }
    
    size_t getSize() const {
        return size.load(std::memory_order_relaxed);
    }
};

// Asynchronous logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(const LogConfig& config)
        : config_(config)
        , stopRequested_(false)
        , currentThreadTimestamp_(0)
    {
        if (config_.useAsyncLogging) {
            queue_ = std::make_unique<LockFreeQueue<EnhancedLogEntry>>();
            workerThread_ = std::thread(&AsyncLogger::workerLoop, this);
        }
    }
    
    ~AsyncLogger() {
        stopRequested_ = true;
        if (workerThread_.joinable()) {
            workerThread_.join();
        }
    }
    
    void log(const LogEntry& entry) override {
        EnhancedLogEntry enhanced;
        convertLogEntry(entry, enhanced);
        
        if (config_.useAsyncLogging && queue_) {
            if (!queue_->push(enhanced)) {
                // Queue full, fallback to synchronous logging
                processEntry(enhanced);
            }
        } else {
            processEntry(enhanced);
        }
    }
    
    void setLevel(LogLevel level) override {
        // Convert old LogLevel to EnhancedLogLevel for backward compatibility
        EnhancedLogLevel enhancedLevel;
        switch (level) {
            case LogLevel::Debug: enhancedLevel = EnhancedLogLevel::Debug; break;
            case LogLevel::Info: enhancedLevel = EnhancedLogLevel::Info; break;
            case LogLevel::Warning: enhancedLevel = EnhancedLogLevel::Warning; break;
            case LogLevel::Error: enhancedLevel = EnhancedLogLevel::Error; break;
            case LogLevel::Critical: enhancedLevel = EnhancedLogLevel::Critical; break;
            default: enhancedLevel = EnhancedLogLevel::Info; break;
        }
        config_.globalLevel = enhancedLevel;
    }
    
    LogLevel getLevel() const override {
        // Convert back to old LogLevel for compatibility
        EnhancedLogLevel level = config_.globalLevel;
        switch (level) {
            case EnhancedLogLevel::Trace: return LogLevel::Debug;  // Map Trace to Debug
            case EnhancedLogLevel::Debug: return LogLevel::Debug;
            case EnhancedLogLevel::Info: return LogLevel::Info;
            case EnhancedLogLevel::Warning: return LogLevel::Warning;
            case EnhancedLogLevel::Error: return LogLevel::Error;
            case EnhancedLogLevel::Critical: return LogLevel::Critical;
            case EnhancedLogLevel::Fatal: return LogLevel::Critical;  // Map Fatal to Critical
            default: return LogLevel::Info;
        }
    }
    
    void setConfig(const LogConfig& config) {
        std::lock_guard<std::mutex> lock(configMutex_);
        config_ = config;
    }
    
    LogConfig getConfig() const {
        std::lock_guard<std::mutex> lock(configMutex_);
        return config_;
    }
    
    void addCategory(const std::string& name, EnhancedLogLevel minLevel = EnhancedLogLevel::Info,
                    EnhancedLogLevel maxLevel = EnhancedLogLevel::Fatal, size_t rateLimit = 0) {
        std::lock_guard<std::mutex> lock(configMutex_);
        CategoryFilter filter;
        filter.name = name;
        filter.minLevel = minLevel;
        filter.maxLevel = maxLevel;
        filter.rateLimit = rateLimit;
        filter.rateCount = 0;
        filter.rateWindowStart = std::chrono::steady_clock::now();
        config_.categories.push_back(filter);
    }
    
private:
    void workerLoop() {
        EnhancedLogEntry entry;
        while (!stopRequested_) {
            if (queue_ && queue_->pop(entry)) {
                processEntry(entry);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    
    void processEntry(const EnhancedLogEntry& entry) {
        // Check category filters
        if (!checkCategoryFilters(entry)) {
            return;
        }
        
        // Check rate limiting
        if (!checkRateLimit(entry.category)) {
            return;
        }
        
        // Check sampling
        if (!checkSampling()) {
            return;
        }
        
        // Check pattern filters
        if (!checkPatternFilters(entry.message)) {
            return;
        }
        
        // Check global level
        if (entry.level < config_.globalLevel) {
            return;
        }
        
        // Process based on formatter
        std::string formatted;
        if (config_.useEnhancedLevels) {
            formatted = formatEntry(entry);
        } else {
            formatted = formatLegacyEntry(entry);
        }
        
        // Output to file if configured
        if (!config_.logFile.empty()) {
            writeToFile(formatted);
        }
        
        // Output to console
        outputToConsole(formatted, entry.level);
    }
    
    bool checkCategoryFilters(const EnhancedLogEntry& entry) {
        for (const auto& category : config_.categories) {
            if (category.enabled && entry.category == category.name) {
                if (entry.level < category.minLevel || entry.level > category.maxLevel) {
                    return false;
                }
                break;
            }
        }
        return true;
    }
    
    bool checkRateLimit(const std::string& category) {
        for (auto& cat : config_.categories) {
            if (cat.enabled && cat.name == category && cat.rateLimit > 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - cat.rateWindowStart);
                
                if (elapsed >= cat.rateWindowDuration) {
                    cat.rateCount = 0;
                    cat.rateWindowStart = now;
                }
                
                if (cat.rateCount >= cat.rateLimit) {
                    return false;
                }
                
                cat.rateCount++;
                break;
            }
        }
        return true;
    }
    
    bool checkSampling() {
        return (config_.sampleRate >= 1.0f) || 
               ((rand() % 10000) / 10000.0f < config_.sampleRate);
    }
    
    bool checkPatternFilters(const std::string& message) {
        for (const auto& pattern : config_.excludePatterns) {
            if (std::regex_search(message, pattern)) {
                return false;
            }
        }
        
        if (!config_.includePatterns.empty()) {
            bool matched = false;
            for (const auto& pattern : config_.includePatterns) {
                if (std::regex_search(message, pattern)) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
        }
        
        return true;
    }
    
    void convertLogEntry(const LogEntry& legacy, EnhancedLogEntry& enhanced) {
        enhanced.level = legacy.level;
        enhanced.category = "System";  // Default category
        enhanced.message = legacy.message;
        enhanced.file = legacy.file;
        enhanced.line = legacy.line;
        enhanced.function = legacy.function;
        enhanced.timestamp = legacy.timestamp;
        enhanced.timestampNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
            legacy.timestamp.time_since_epoch());
        enhanced.threadId = std::this_thread::get_id();
        enhanced.cpuCore = getCurrentCpuCore();
        enhanced.memoryUsage = getMemoryUsage();
        enhanced.threadTimestamp = currentThreadTimestamp_.fetch_add(1, std::memory_order_relaxed);
        
        if (config_.logSystemMetrics) {
            enhanced.fields["memory_usage_kb"] = std::to_string(enhanced.memoryUsage / 1024);
            enhanced.fields["cpu_core"] = std::to_string(enhanced.cpuCore);
        }
    }
    
    std::string formatEntry(const EnhancedLogEntry& entry) {
        std::ostringstream oss;
        oss << "[" << enhancedLevelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(entry.timestampNs).count() % 1000;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.") 
            << std::setfill('0') << std::setw(3) << ms;
        
        oss << " [" << entry.threadTimestamp << "] [Core " << entry.cpuCore << "] "
            << entry.message;
        
        if (!entry.category.empty()) {
            oss << " [" << entry.category << "]";
        }
        
        return oss.str();
    }
    
    std::string formatLegacyEntry(const EnhancedLogEntry& entry) {
        std::ostringstream oss;
        oss << "[" << enhancedLevelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        return oss.str();
    }
    
    void outputToConsole(const std::string& message, EnhancedLogLevel level) {
        std::cout << message << "\n";
    }
    
    void writeToFile(const std::string& message) {
        std::lock_guard<std::mutex> lock(fileMutex_);
        
        if (!fileOut_.is_open()) {
            fileOut_.open(config_.logFile, std::ios::app);
            if (!fileOut_.is_open()) {
                std::cerr << "Failed to open log file: " << config_.logFile << "\n";
                return;
            }
        }
        
        fileOut_ << message << "\n";
        fileOut_.flush();
        
        // Check if file needs rotation
        checkFileRotation();
    }
    
    void checkFileRotation() {
        if (!fileOut_.is_open() || config_.maxFileSize == 0) return;
        
        auto currentPos = fileOut_.tellp();
        if (currentPos >= (std::streampos)config_.maxFileSize) {
            rotateFile();
        }
    }
    
    void rotateFile() {
        fileOut_.close();
        
        // Compress old file if requested
        if (config_.compressLogs && fileOut_.is_open()) {
            std::string compressedFile = config_.logFile + ".gz";
            // TODO: Implement compression
            fileOut_.close();
        }
        
        // Rename existing file
        std::string timestamp = getTimestampString();
        std::filesystem::path oldPath(config_.logFile);
        std::filesystem::path newPath = oldPath.parent_path() / (oldPath.stem().string() + "_" + timestamp + oldPath.extension().string());
        
        if (std::filesystem::exists(oldPath)) {
            std::filesystem::rename(oldPath, newPath);
        }
        
        // Clean up old rotated files
        cleanRotatedFiles(oldPath.parent_path(), oldPath.stem().string(), oldPath.extension().string());
        
        // Reopen file for new logs
        fileOut_.open(config_.logFile, std::ios::app);
    }
    
    void cleanRotatedFiles(const std::filesystem::path& directory, const std::string& baseName, const std::string& extension) {
        if (!std::filesystem::exists(directory)) return;
        
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.substr(0, baseName.size()) == baseName && 
                    filename.find(extension) != std::string::npos) {
                    files.push_back(entry.path());
                }
            }
        }
        
        if (files.size() > (size_t)config_.maxRotatedFiles) {
            // Sort by modification time
            std::sort(files.begin(), files.end(), 
                [](const std::filesystem::path& a, const std::filesystem::path& b) {
                    return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
                });
            
            // Delete oldest files
            for (size_t i = 0; i < files.size() - config_.maxRotatedFiles; ++i) {
                std::filesystem::remove(files[i]);
            }
        }
    }
    
    std::string getTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return oss.str();
    }
    
    int getCurrentCpuCore() {
        // Simplified implementation - in real code this would be more sophisticated
        return 0;
    }
    
    size_t getMemoryUsage() {
        // Simplified implementation - in real code this would be more sophisticated
        return 0;
    }
    
    std::string enhancedLevelToString(EnhancedLogLevel level) {
        switch (level) {
            case EnhancedLogLevel::Trace: return "TRACE";
            case EnhancedLogLevel::Debug: return "DEBUG";
            case EnhancedLogLevel::Info: return "INFO";
            case EnhancedLogLevel::Warning: return "WARN";
            case EnhancedLogLevel::Error: return "ERROR";
            case EnhancedLogLevel::Critical: return "CRITICAL";
            case EnhancedLogLevel::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
private:
    LogConfig config_;
    std::unique_ptr<LockFreeQueue<EnhancedLogEntry>> queue_;
    std::thread workerThread_;
    std::atomic<bool> stopRequested_;
    std::atomic<uint64_t> currentThreadTimestamp_;
    std::mutex configMutex_;
    std::mutex fileMutex_;
    std::ofstream fileOut_;
};

} // namespace nlm
