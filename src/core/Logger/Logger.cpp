#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// Performance monitoring and timing utilities
namespace {
    class PerformanceTimer {
    public:
        PerformanceTimer(const std::string& operation) : operation_(operation) {
            start_ = std::chrono::high_resolution_clock::now();
        }
        
        ~PerformanceTimer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
            NLM_LOG_DEBUG("Performance: " + operation_ + " took " + std::to_string(duration) + "μs");
        }
        
    private:
        std::string operation_;
        std::chrono::high_resolution_clock::time_point start_;
    };
}

// Enhanced logging with performance monitoring
void Logger::logWithPerformance(const std::string& operation, LogLevel level, 
                               const std::string& message, const std::string& file, 
                               int line, const std::string& function) {
    // Log performance timing for expensive operations
    PerformanceTimer timer("logWithPerformance");
    
    // Enhanced log entry with additional context
    log(level, "[" + operation + "] " + message, file, line, function);
}

void Logger::debugWithContext(const std::string& context, const std::string& message,
                             const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, "[" + context + "] " + message, file, line, function);
}

void Logger::infoWithContext(const std::string& context, const std::string& message,
                            const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, "[" + context + "] " + message, file, line, function);
}

void Logger::warningWithContext(const std::string& context, const std::string& message,
                               const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, "[" + context + "] " + message, file, line, function);
}

void Logger::errorWithContext(const std::string& context, const std::string& message,
                             const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, "[" + context + "] " + message, file, line, function);
}

void Logger::criticalWithContext(const std::string& context, const std::string& message,
                                const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, "[" + context + "] " + message, file, line, function);
}

// Performance statistics
struct Logger::PerfStats {
    int64_t totalTime_ = 0;     // Total time in microseconds
    int calls_ = 0;            // Number of calls
    int64_t minTime_ = LLONG_MAX; // Minimum time
    int64_t maxTime_ = 0;      // Maximum time
    
    void addSample(int64_t time) {
        totalTime_ += time;
        ++calls_;
        if (time < minTime_) minTime_ = time;
        if (time > maxTime_) maxTime_ = time;
    }
    
    double getAverageTime() const {
        return calls_ > 0 ? static_cast<double>(totalTime_) / calls_ : 0.0;
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Average: " << getAverageTime() << "μs, "
            << "Min: " << minTime_ << "μs, "
            << "Max: " << maxTime_ << "μs, "
            << "Calls: " << calls_;
        return oss.str();
    }
};

// LogStream implementation with enhanced debugging
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    oss_ << "[" << std::filesystem::path(file).filename() << ":" << line << "] ";
    
    // Add function name if available
    if (!function.empty()) {
        oss_ << function << "(); ";
    }
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
