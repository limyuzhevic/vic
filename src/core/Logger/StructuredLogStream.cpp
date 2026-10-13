#include "StructuredLogStream.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <ctime>
#include <cmath>

namespace nlm {

StructuredLogStream::StructuredLogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level)
    , file_(file)
    , line_(line)
    , function_(function)
    , currentKey_("")
{
}

StructuredLogStream::~StructuredLogStream() {
    if (!oss_.str().empty()) {
        logStructured(oss_.str());
    }
}

StructuredLogStream& StructuredLogStream::operator<<(const std::string& key) {
    currentKey_ = key;
    oss_ << key << ": ";
    return *this;
}

StructuredLogStream& StructuredLogStream::correlationId(const std::string& id) {
    correlationId_ = id;
    return *this;
}

StructuredLogStream& StructuredLogStream::userId(const std::string& id) {
    userId_ = id;
    return *this;
}

StructuredLogStream& StructuredLogStream::sessionId(const std::string& id) {
    sessionId_ = id;
    return *this;
}

StructuredLogStream& StructuredLogStream::startTiming(const std::string& operation) {
    timings_[operation] = getCurrentTimestampDouble();
    return *this;
}

StructuredLogStream& StructuredLogStream::endTiming(const std::string& operation) {
    if (timings_.find(operation) != timings_.end()) {
        double startTime = timings_[operation];
        double endTime = getCurrentTimestampDouble();
        double duration = endTime - startTime;
        oss_ << "[" << operation << " took " << std::fixed << std::setprecision(3) << duration << "ms]";
    }
    return *this;
}

StructuredLogStream& StructuredLogStream::context(const std::string& key, const std::string& value) {
    kvPairs_[key] = value;
    oss_ << "{" << key << ": " << value << "}";
    return *this;
}

void StructuredLogStream::logStructured(const std::string& message) {
    Logger::getGlobal().log(level_, getFormattedMessage(), file_, line_, function_);
}

std::string StructuredLogStream::getFormattedMessage() const {
    std::ostringstream oss;
    
    // Add timestamp
    oss << "[" << getCurrentTimestamp() << "] ";
    
    // Add log level
    oss << "[" << levelToString(level_) << "] ";
    
    // Add correlation ID if present
    if (!correlationId_.empty()) {
        oss << "[corr:" << correlationId_ << "] ";
    }
    
    // Add session ID if present
    if (!sessionId_.empty()) {
        oss << "[session:" << sessionId_ << "] ";
    }
    
    // Add user ID if present
    if (!userId_.empty()) {
        oss << "[user:" << userId_ << "] ";
    }
    
    // Add message
    oss << message;
    
    // Add key-value pairs
    if (!kvPairs_.empty()) {
        oss << " | {";
        bool first = true;
        for (const auto& pair : kvPairs_) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

std::string StructuredLogStream::toJson() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"timestamp\": \"" << getCurrentTimestamp() << "\", ";
    oss << "\"level\": \"" << levelToString(level_) << "\", ";
    oss << "\"file\": \"" << file_ << "\", ";
    oss << "\"line\": " << line_ << ", ";
    oss << "\"function\": \"" << function_ << "\", ";
    
    if (!correlationId_.empty()) {
        oss << "\"correlation_id\": \"" << correlationId_ << "\", ";
    }
    if (!sessionId_.empty()) {
        oss << "\"session_id\": \"" << sessionId_ << "\", ";
    }
    if (!userId_.empty()) {
        oss << "\"user_id\": \"" << userId_ << "\", ";
    }
    
    oss << "\"message\": \"" << oss_.str() << "\", ";
    
    if (!kvPairs_.empty()) {
        oss << "\"context\": {";
        bool first = true;
        for (const auto& pair : kvPairs_) {
            if (!first) oss << ", ";
            oss << "\"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        oss << "}, ";
    }
    
    oss << "\"timings\": {";
    bool first = true;
    for (const auto& pair : timings_) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": " << std::fixed << std::setprecision(3) << pair.second;
        first = false;
    }
    oss << "}";
    
    oss << "}";
    return oss.str();
}

std::string StructuredLogStream::toYaml() const {
    std::ostringstream oss;
    oss << "timestamp: " << getCurrentTimestamp() << std::endl;
    oss << "level: " << levelToString(level_) << std::endl;
    oss << "file: " << file_ << std::endl;
    oss << "line: " << line_ << std::endl;
    oss << "function: " << function_ << std::endl;
    
    if (!correlationId_.empty()) {
        oss << "correlation_id: " << correlationId_ << std::endl;
    }
    if (!sessionId_.empty()) {
        oss << "session_id: " << sessionId_ << std::endl;
    }
    if (!userId_.empty()) {
        oss << "user_id: " << userId_ << std::endl;
    }
    
    oss << "message: " << oss_.str() << std::endl;
    
    if (!kvPairs_.empty()) {
        oss << "context:" << std::endl;
        for (const auto& pair : kvPairs_) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    if (!timings_.empty()) {
        oss << "timings:" << std::endl;
        for (const auto& pair : timings_) {
            oss << "  " << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second << "ms" << std::endl;
        }
    }
    
    return oss.str();
}

std::string StructuredLogStream::toCsv() const {
    std::ostringstream oss;
    oss << "timestamp,level,file,line,function,correlation_id,session_id,user_id,message";
    
    // Add context keys
    for (const auto& pair : kvPairs_) {
        oss << "," << pair.first;
    }
    
    // Add timing keys
    for (const auto& pair : timings_) {
        oss << "," << pair.first;
    }
    
    oss << std::endl;
    
    // Add values
    oss << "\"" << getCurrentTimestamp() << "\",\"" << levelToString(level_) << "\",\"" << file_ << "\"," << line_ << ",\"" << function_ << "\",\"" << correlationId_ << "\",\"" << sessionId_ << "\",\"" << userId_ << "\",\"" << oss_.str() << "\"";
    
    for (const auto& pair : kvPairs_) {
        oss << ",\"" << pair.second << "\"";
    }
    
    for (const auto& pair : timings_) {
        oss << "," << std::fixed << std::setprecision(3) << pair.second;
    }
    
    return oss.str();
}

std::string StructuredLogStream::formatValue(const std::string& value) const {
    // Escape special characters for different formats
    std::string result;
    for (char c : value) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string StructuredLogStream::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string StructuredLogStream::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

double StructuredLogStream::getCurrentTimestampDouble() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<double>(milliseconds) / 1000.0;
}

void logStructured(LogLevel level, const std::string& message, const std::string& file, int line, const std::string& function) {
    Logger::getGlobal().log(level, message, file, line, function);
}

void logWithContext(LogLevel level, const std::string& message, const std::unordered_map<std::string, std::string>& context,
                    const std::string& file, int line, const std::string& function) {
    std::ostringstream oss;
    oss << message;
    
    if (!context.empty()) {
        oss << " | Context: {";
        bool first = true;
        for (const auto& pair : context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    Logger::getGlobal().log(level, oss.str(), file, line, function);
}

} // namespace nlm
