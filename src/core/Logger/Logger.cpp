#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <ctime>
#include <cmath>

namespace nlm {

struct PatternFilter::Impl {
    std::vector<std::regex> patterns;
    std::vector<std::regex> excludePatterns;
    bool excludeMode = false;
};

PatternFilter::PatternFilter(const std::vector<std::string>& patterns, bool exclude) : pImpl(std::make_unique<Impl>()) {
    pImpl->excludeMode = exclude;
    for (const auto& pattern : patterns) {
        try {
            pImpl->patterns.emplace_back(pattern, std::regex_constants::icase);
        } catch (const std::regex_error& e) {
            // Invalid pattern, skip
        }
    }
}

bool PatternFilter::shouldLog(const LogEntry& entry) const {
    if (pImpl->patterns.empty() && pImpl->excludePatterns.empty()) {
        return true;
    }
    
    std::string message = entry.message;
    if (message.empty()) {
        return true;
    }
    
    bool matchesAnyPattern = false;
    for (const auto& pattern : pImpl->patterns) {
        if (std::regex_search(message, pattern)) {
            matchesAnyPattern = true;
            break;
        }
    }
    
    bool matchesAnyExcludePattern = false;
    for (const auto& pattern : pImpl->excludePatterns) {
        if (std::regex_search(message, pattern)) {
            matchesAnyExcludePattern = true;
            break;
        }
    }
    
    if (pImpl->excludeMode) {
        return matchesAnyExcludePattern;
    } else {
        if (matchesAnyExcludePattern) {
            return false;
        }
        return matchesAnyPattern;
    }
}

void PatternFilter::addPattern(const std::string& pattern, bool exclude) {
    try {
        if (exclude) {
            pImpl->excludePatterns.emplace_back(pattern, std::regex_constants::icase);
        } else {
            pImpl->patterns.emplace_back(pattern, std::regex_constants::icase);
        }
    } catch (const std::regex_error& e) {
        // Invalid pattern, silently ignore
    }
}

void PatternFilter::clearPatterns() {
    pImpl->patterns.clear();
    pImpl->excludePatterns.clear();
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::function<std::string(const LogEntry&)> formatter;
    std::vector<std::shared_ptr<ILogFilter>> filters;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check filters first
    for (const auto& filter : pImpl->filters) {
        if (!filter->shouldLog(entry)) {
            return;
        }
    }
    
    // Check level
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output;
    
    if (pImpl->formatter) {
        output = pImpl->formatter(entry);
    } else {
        // Default formatting
        std::ostringstream oss;
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                entry.timestamp.time_since_epoch()).count() % 1000;
        oss << " ";
        
        // Add level
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add context if present
        if (!entry.context.empty()) {
            oss << "{";
            bool first = true;
            for (const auto& ctx : entry.context) {
                if (!first) oss << ", ";
                oss << ctx.first << "=" << ctx.second;
                first = false;
            }
            oss << "} ";
        }
        
        oss << entry.message;
        
        // Add location info
        if (!entry.file.empty() && entry.line > 0) {
            oss << " (" << entry.file << ":" << entry.line << ")";
        }
        
        output = oss.str();
    }
    
    // Output to console
    if (pImpl->useColors && entry.level <= LogLevel::Warning) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
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

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFormatter(std::function<std::string(const LogEntry&)> formatter) {
    pImpl->formatter = std::move(formatter);
}

void ConsoleLogger::addFilter(std::shared_ptr<ILogFilter> filter) {
    pImpl->filters.push_back(std::move(filter));
}

void ConsoleLogger::clearFilters() {
    pImpl->filters.clear();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
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

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Trace: return "\033[90m";    // Bright black
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";     // Green
        case LogLevel::Notice: return "\033[94m";   // Blue
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";    // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";    // Bright red
        case LogLevel::Emergency: return "\033[95m"; // Bright magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(std::move(logger));
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
                 const std::unordered_map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = context;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Trace, message, file, line, function, context);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Debug, message, file, line, function, context);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Info, message, file, line, function, context);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Notice, message, file, line, function, context);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Warning, message, file, line, function, context);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Error, message, file, line, function, context);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Critical, message, file, line, function, context);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Alert, message, file, line, function, context);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function, const std::unordered_map<std::string, std::string>& context) {
    log(LogLevel::Emergency, message, file, line, function, context);
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
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::addContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

// Logger::Timer implementation
Logger::Timer::Timer(const std::string& name, LogLevel level) : name_(name), level_(level) {
    start_ = std::chrono::system_clock::now();
}

Logger::Timer::~Timer() {
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    
    std::ostringstream oss;
    oss << "[" << name_ << "] elapsed: " << duration.count() << "ms";
    
    if (!context_.empty()) {
        oss << " (";
        bool first = true;
        for (const auto& ctx : context_) {
            if (!first) oss << ", ";
            oss << ctx.first << "=" << ctx.second;
            first = false;
        }
        oss << ")";
    }
    
    Logger::getGlobal().log(level_, oss.str(), "", 0, "");
}

Logger::Timer& Logger::Timer::setContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

void Logger::Timer::recordElapsed() {
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    
    std::ostringstream oss;
    oss << "[" << name_ << "] elapsed: " << duration.count() << "ms";
    
    if (!context_.empty()) {
        oss << " (";
        bool first = true;
        for (const auto& ctx : context_) {
            if (!first) oss << ", ";
            oss << ctx.first << "=" << ctx.second;
            first = false;
        }
        oss << ")";
    }
    
    Logger::getGlobal().log(level_, oss.str(), "", 0, "");
}

std::unordered_map<std::string, std::string> Logger::buildContext(std::initializer_list<std::pair<std::string, std::string>> pairs) {
    return std::unordered_map<std::string, std::string>(pairs.begin(), pairs.end());
}

} // namespace nlm
