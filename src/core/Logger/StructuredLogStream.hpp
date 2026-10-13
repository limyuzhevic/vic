#pragma once

#include "core/Logger/Logger.hpp"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <chrono>

namespace nlm {

// Enhanced LogStream with structured logging support
class StructuredLogStream {
public:
    StructuredLogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~StructuredLogStream();
    
    // Basic logging operators
    template<typename T>
    StructuredLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Key-value pair logging
    StructuredLogStream& operator<<(const std::string& key);
    
    template<typename T>
    StructuredLogStream& operator<<(const T& value) {
        if (!currentKey_.empty()) {
            kvPairs_[currentKey_] = formatValue(value);
            currentKey_.clear();
        }
        oss_ << value;
        return *this;
    }
    
    // Context and correlation
    StructuredLogStream& correlationId(const std::string& id);
    StructuredLogStream& userId(const std::string& id);
    StructuredLogStream& sessionId(const std::string& id);
    
    // Performance timing
    StructuredLogStream& startTiming(const std::string& operation);
    StructuredLogStream& endTiming(const std::string& operation);
    
    // Custom context
    StructuredLogStream& context(const std::string& key, const std::string& value);
    
    // Log with structured data
    void logStructured(const std::string& message);
    
    // Get the formatted log message
    std::string getFormattedMessage() const;
    
    // Convert to JSON-like format
    std::string toJson() const;
    
    // Convert to YAML-like format
    std::string toYaml() const;
    
    // Convert to CSV-like format
    std::string toCsv() const;
    
    // Get log metadata
    const std::string& getMessage() const { return oss_.str(); }
    LogLevel getLevel() const { return level_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::string& getCorrelationId() const { return correlationId_; }
    const std::string& getSessionId() const { return sessionId_; }
    const std::string& getUserId() const { return userId_; }
    
private:
    std::string formatValue(const std::string& value) const;
    template<typename T>
    std::string formatValue(const T& value) const {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTimestamp() const;
    
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string correlationId_;
    std::string sessionId_;
    std::string userId_;
    std::string currentKey_;
    std::unordered_map<std::string, std::string> kvPairs_;
    std::unordered_map<std::string, double> timings_;
};

// Convenience macros for structured logging
#define NLM_LOG_STREAM_DEBUG nlm::StructuredLogStream(nlm::LogLevel::Debug, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_INFO nlm::StructuredLogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_WARNING nlm::StructuredLogStream(nlm::LogLevel::Warning, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_ERROR nlm::StructuredLogStream(nlm::LogLevel::Error, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_CRITICAL nlm::StructuredLogStream(nlm::LogLevel::Critical, __FILE__, __LINE__, __func__)

// Performance timing macros
#define NLM_PERF_SCOPE(operation) nlm::StructuredLogStream::ScopeTimer timer(operation, __func__)

// Structured logging convenience functions
void logStructured(LogLevel level, const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
void logWithContext(LogLevel level, const std::string& message, const std::unordered_map<std::string, std::string>& context,
                    const std::string& file = "", int line = 0, const std::string& function = "");

} // namespace nlm
