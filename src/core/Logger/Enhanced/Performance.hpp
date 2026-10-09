#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <shared_mutex>
#include <type_traits>
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"
#include "Enhanced/AsyncProcessor.hpp"
#include "Enhanced/Analysis.hpp"
#include "Enhanced/Management.hpp"

namespace nlm {

// Fast conditional log evaluation using constexpr if
class ConditionalLogEvaluator {
public:
    template<typename... Args>
    static bool evaluate(LogLevel level, const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "") {
        // Fast path: if level is higher than current threshold, skip entirely
        if (level >= getCurrentLogThreshold()) {
            return false;
        }
        
        // Fast path: if level is disabled, skip entirely
        if (!isLevelEnabled(level)) {
            return false;
        }
        
        // Check file path filter if enabled
        if (fileFilterEnabled_) {
            std::string normalizedFile = normalizeFilePath(file);
            if (!fileFilter_.empty() && fileFilter_.find(normalizedFile) == fileFilter_.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    static void setLogThreshold(LogLevel threshold) {
        thread_local static LogLevel currentThreshold = threshold;
        currentThreshold = threshold;
    }
    
    static LogLevel getCurrentLogThreshold() {
        thread_local static LogLevel currentThreshold = LogLevel::Info;
        return currentThreshold;
    }
    
    static void setLevelEnabled(LogLevel level, bool enabled) {
        std::lock_guard<std::mutex> lock(levelEnabledMutex_);
        levelEnabled_[level] = enabled;
    }
    
    static bool isLevelEnabled(LogLevel level) {
        std::lock_guard<std::mutex> lock(levelEnabledMutex_);
        auto it = levelEnabled_.find(level);
        return it == levelEnabled_.end() ? true : it->second;
    }
    
    static void setFileFilter(const std::set<std::string>& files) {
        std::lock_guard<std::mutex> lock(fileFilterMutex_);
        fileFilter_ = files;
        fileFilterEnabled_ = !files.empty();
    }
    
private:
    static std::string normalizeFilePath(const std::string& file) {
        std::string normalized = file;
        // Convert forward slashes to backslashes for Windows compatibility
        for (char& c : normalized) {
            if (c == '/') c = '\\';
        }
        return normalized;
    }
    
    static std::mutex levelEnabledMutex_;
    static std::mutex fileFilterMutex_;
    static std::unordered_map<LogLevel, bool> levelEnabled_;
    static std::set<std::string> fileFilter_;
    static bool fileFilterEnabled_;
};

// Memory-efficient log buffer for high-throughput scenarios
class MemoryEfficientLogBuffer {
public:
    MemoryEfficientLogBuffer(size_t capacity = 100000) : capacity_(capacity), size_(0), head_(0), tail_(0) {
        buffer_.reserve(capacity);
    }
    
    void add(const EnhancedLogEntry& entry) {
        // Use memory pool pattern for efficiency
        buffer_.emplace_back(entry);
        
        if (size_ == capacity_) {
            // Buffer is full, clear oldest entries to make room
            while (size_ > capacity_ / 2) {
                buffer_.erase(buffer_.begin());
                size_--;
            }
        }
        
        size_++;
    }
    
    std::vector<EnhancedLogEntry> getRecentLogs(size_t count) const {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        std::vector<EnhancedLogEntry> result;
        result.reserve(std::min(count, size_));
        
        size_t start = (tail_ + size_ - std::min(count, size_)) % size_;
        for (size_t i = 0; i < std::min(count, size_); ++i) {
            result.push_back(buffer_[start]);
            start = (start + 1) % size_;
        }
        
        return result;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        buffer_.clear();
        size_ = 0;
        head_ = 0;
        tail_ = 0;
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        return size_;
    }
    
    size_t getCapacity() const { return capacity_; }
    
    // Memory usage estimate
    size_t getEstimatedMemoryUsage() const {
        size_t totalSize = sizeof(EnhancedLogEntry) * size_;
        // Add overhead for buffer management
        return totalSize + sizeof(buffer_) + sizeof(size_) + sizeof(head_) + sizeof(tail_);
    }
    
private:
    std::vector<EnhancedLogEntry> buffer_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    mutable std::mutex bufferMutex_;
};

// Pre-compiled log format for performance
class PrecompiledLogFormat {
public:
    struct FormatTemplate {
        std::string format;
        std::vector<std::string> placeholders;
    };
    
    PrecompiledLogFormat(const std::string& format) {
        format_ = format;
        extractPlaceholders();
    }
    
    std::string format(const EnhancedLogEntry& entry) const {
        std::string result = format_;
        
        // Apply substitutions using pre-compiled replacements
        for (const auto& placeholder : placeholders_) {
            std::string value;
            if (placeholder == "{timestamp}") {
                value = formatTimestamp(entry.timestamp);
            } else if (placeholder == "{level}") {
                value = levelToString(entry.level);
            } else if (placeholder == "{message}") {
                value = entry.message;
            } else if (placeholder == "{file}") {
                value = entry.file;
            } else if (placeholder == "{line}") {
                value = std::to_string(entry.line);
            } else if (placeholder == "{function}") {
                value = entry.function;
            }
            
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
        
        return result;
    }
    
    bool hasPlaceholder(const std::string& placeholder) const {
        return std::find(placeholders_.begin(), placeholders_.end(), placeholder) != placeholders_.end();
    }
    
private:
    void extractPlaceholders() {
        size_t pos = 0;
        while ((pos = format_.find('{', pos)) != std::string::npos) {
            size_t end = format_.find('}', pos);
            if (end != std::string::npos) {
                std::string placeholder = format_.substr(pos, end - pos + 1);
                placeholders_.push_back(placeholder);
                pos = end + 1;
            } else {
                break;
            }
        }
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::string format_;
    std::vector<std::string> placeholders_;
};

// Fast log sink for high-performance scenarios
class FastLogSink {
public:
    FastLogSink() : enabled_(true), level_(LogLevel::Info) {}
    
    virtual ~FastLogSink() = default;
    
    virtual void log(const EnhancedLogEntry& entry) {
        if (!enabled_) return;
        if (!shouldLog(entry.level)) return;
        
        const std::string& formatted = preformat(entry);
        doOutput(formatted);
    }
    
    virtual void setEnabled(bool enabled) { enabled_ = enabled; }
    virtual bool isEnabled() const { return enabled_; }
    
    virtual void setLevel(LogLevel level) { level_ = level; }
    virtual LogLevel getLevel() const { return level_; }
    
protected:
    virtual const std::string& preformat(const EnhancedLogEntry& entry) = 0;
    virtual void doOutput(const std::string& formatted) = 0;
    
    virtual bool shouldLog(LogLevel level) const {
        return level >= level_;
    }
    
private:
    bool enabled_;
    LogLevel level_;
};

// Ultra-fast string-based logger for maximum performance
class UltraFastLogger {
public:
    UltraFastLogger() {
        // Pre-compile common formats
        commonFormats_[LogLevel::Debug] = "[{level}] {message}\n";
        commonFormats_[LogLevel::Info] = "[{level}] {message}\n";
        commonFormats_[LogLevel::Warning] = "[{level}] {message}\n";
        commonFormats_[LogLevel::Error] = "[{level}] {message}\n";
        commonFormats_[LogLevel::Critical] = "[{level}] {message}\n";
        
        // Pre-compile timestamp format
        timestampFormat_ = "%Y-%m-%d %H:%M:%S";
    }
    
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0,
             const std::string& function = "") {
        if (!ConditionalLogEvaluator::evaluate(level, message, file, line, function)) {
            return;
        }
        
        // Fast path for common formats
        const auto& it = commonFormats_.find(level);
        if (it != commonFormats_.end()) {
            std::string formatted = it->second;
            replaceAll(formatted, "{level}", levelToString(level));
            replaceAll(formatted, "{message}", message);
            
            // Add timestamp if format contains it
            if (it->second.find("{timestamp}") != std::string::npos) {
                auto time = std::chrono::system_clock::now();
                auto time_t = std::chrono::system_clock::to_time_t(time);
                char timeBuf[100];
                strftime(timeBuf, sizeof(timeBuf), timestampFormat_.c_str(), std::localtime(&time_t));
                
                size_t pos = formatted.find("{timestamp}");
                if (pos != std::string::npos) {
                    formatted.replace(pos, 12, timeBuf);
                }
            }
            
            output(formatted);
            return;
        }
        
        // Fallback to generic formatting
        genericLog(level, message, file, line, function);
    }
    
    void setLogThreshold(LogLevel threshold) {
        ConditionalLogEvaluator::setLogThreshold(threshold);
    }
    
    void setLevelEnabled(LogLevel level, bool enabled) {
        ConditionalLogEvaluator::setLevelEnabled(level, enabled);
    }
    
    void setFileFilter(const std::set<std::string>& files) {
        ConditionalLogEvaluator::setFileFilter(files);
    }
    
private:
    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    }
    
    void genericLog(LogLevel level, const std::string& message,
                    const std::string& file, int line,
                    const std::string& function) {
        std::string formatted;
        
        // Fast timestamp formatting
        auto time = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(time);
        char timeBuf[100];
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        
        formatted = "[" + levelToString(level) + "] " + timeBuf + " " + message + "\n";
        
        if (!file.empty() && line > 0) {
            formatted += "  File: " + file + ", Line: " + std::to_string(line) + "\n";
        }
        if (!function.empty()) {
            formatted += "  Function: " + function + "\n";
        }
        
        output(formatted);
    }
    
    void output(const std::string& formatted) {
        // Output to console
        std::cout << formatted;
    }
    
    std::map<LogLevel, std::string> commonFormats_;
    std::string timestampFormat_;
};

// Log level-based optimization for neural simulation workloads
class NeuralSimulationLogger {
public:
    NeuralSimulationLogger() : enabled_(true) {
        // Initialize neural simulation specific log levels
        neuralLogLevels_["neuron_update"] = LogLevel::Debug;
        neuralLogLevels_["synapse_update"] = LogLevel::Debug;
        neuralLogLevels_["plasticity"] = LogLevel::Info;
        neuralLogLevels_["learning"] = LogLevel::Info;
        neuralLogLevels_["error"] = LogLevel::Error;
        neuralLogLevels_["critical"] = LogLevel::Critical;
    }
    
    void logNeuronUpdate(const std::string& neuronId, double weight, 
                        double membranePotential, size_t timeStep) {
        if (!enabled_) return;
        
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["neuron_update"];
        entry.message = "Neuron " + neuronId + " updated: weight=" + 
                       std::to_string(weight) + ", Vmem=" + 
                       std::to_string(membranePotential) + ", t=" + 
                       std::to_string(timeStep);
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logNeuronUpdate";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "neuron_thread";
        entry.tags["neuron"] = neuronId;
        entry.tags["simulation"] = "true";
        entry.sequenceNumber = ++neuronLogCounter_;
        
        logEntry(entry);
    }
    
    void logSynapseUpdate(const std::string& sourceId, const std::string& targetId,
                         double weightChange, double newWeight) {
        if (!enabled_) return;
        
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["synapse_update"];
        entry.message = "Synapse " + sourceId + "->" + targetId + 
                       " updated: Δw=" + std::to_string(weightChange) + 
                       ", w=" + std::to_string(newWeight);
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logSynapseUpdate";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "synapse_thread";
        entry.tags["source"] = sourceId;
        entry.tags["target"] = targetId;
        entry.tags["simulation"] = "true";
        entry.sequenceNumber = ++synapseLogCounter_;
        
        logEntry(entry);
    }
    
    void logPlasticityEvent(const std::string& eventType, const std::string& details) {
        if (!enabled_) return;
        
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["plasticity"];
        entry.message = "Plasticity event: " + eventType + ": " + details;
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logPlasticityEvent";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "plasticity_thread";
        entry.tags["event_type"] = eventType;
        entry.tags["simulation"] = "true";
        entry.sequenceNumber = ++plasticityLogCounter_;
        
        logEntry(entry);
    }
    
    void logLearningEvent(const std::string& algorithm, double performance) {
        if (!enabled_) return;
        
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["learning"];
        entry.message = "Learning algorithm " + algorithm + 
                       " achieved performance: " + std::to_string(performance);
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logLearningEvent";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "learning_thread";
        entry.tags["algorithm"] = algorithm;
        entry.tags["simulation"] = "true";
        entry.sequenceNumber = ++learningLogCounter_;
        
        logEntry(entry);
    }
    
    void logError(const std::string& errorMessage, const std::string& context = "") {
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["error"];
        entry.message = "ERROR: " + errorMessage;
        if (!context.empty()) {
            entry.message += " (Context: " + context + ")";
        }
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logError";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "error_handler";
        entry.tags["simulation"] = "true";
        entry.metadata["severity"] = "high";
        entry.sequenceNumber = ++errorLogCounter_;
        
        logEntry(entry);
    }
    
    void logCritical(const std::string& message) {
        EnhancedLogEntry entry;
        entry.level = neuralLogLevels_["critical"];
        entry.message = "CRITICAL: " + message;
        entry.file = "neuro_log.cpp";
        entry.line = 0;
        entry.function = "logCritical";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "critical_handler";
        entry.tags["simulation"] = "true";
        entry.metadata["severity"] = "fatal";
        entry.sequenceNumber = ++criticalLogCounter_;
        
        logEntry(entry);
    }
    
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    void setLogLevel(const std::string& category, LogLevel level) {
        auto it = neuralLogLevels_.find(category);
        if (it != neuralLogLevels_.end()) {
            it->second = level;
        }
    }
    
    LogLevel getLogLevel(const std::string& category) const {
        auto it = neuralLogLevels_.find(category);
        return it != neuralLogLevels_.end() ? it->second : LogLevel::Info;
    }
    
private:
    void logEntry(const EnhancedLogEntry& entry) {
        // Route to appropriate log processor
        if (logProcessor_) {
            logProcessor_->log(entry);
        }
    }
    
    void setLogProcessor(std::unique_ptr<FastLogSink> processor) {
        logProcessor_ = std::move(processor);
    }
    
    std::map<std::string, LogLevel> neuralLogLevels_;
    bool enabled_;
    
    std::atomic<uint64_t> neuronLogCounter_ = 0;
    std::atomic<uint64_t> synapseLogCounter_ = 0;
    std::atomic<uint64_t> plasticityLogCounter_ = 0;
    std::atomic<uint64_t> learningLogCounter_ = 0;
    std::atomic<uint64_t> errorLogCounter_ = 0;
    std::atomic<uint64_t> criticalLogCounter_ = 0;
    
    std::unique_ptr<FastLogSink> logProcessor_;
};

} // namespace nlm
