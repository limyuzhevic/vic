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
    
    // Add detailed timestamp with milliseconds
    auto time = std::chrono::system_clock::now();
    auto timeSinceEpoch = time.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeSinceEpoch);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceEpoch) % std::chrono::seconds(1);
    
    auto time_t_now = std::chrono::system_clock::to_time_t(time);
    auto tm_now = std::gmtime(&time_t_now);
    
    char timestampBuf[32];
    std::strftime(timestampBuf, sizeof(timestampBuf), "%Y-%m-%d %H:%M:%S", tm_now);
    oss << timestampBuf << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
    
    // Add thread ID for better debugging
    std::ostringstream threadOss;
    threadOss << std::this_thread::get_id();
    std::string threadId = threadOss.str();
    if (!threadId.empty()) {
        oss << " [Thread:" << threadId << "]";
    }
    
    // Add log level with better formatting
    oss << " [" << levelToString(entry.level) << "]";
    
    // Add source information
    if (!entry.file.empty()) {
        oss << " " << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    oss << " - " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with enhanced formatting
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            // If file write fails, log to stderr as backup
            std::cerr << "Error writing to log file: " << e.what() << std::endl;
            std::cerr << "Failed log entry: " << output << std::endl;
        }
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
    // Close existing file if open
    closeOutputFile();
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot set empty file path for logging");
        return;
    }
    
    // Create directory path if it doesn't exist
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string dirPath = filepath.substr(0, lastSlash);
        // Note: In a real implementation, you would create directories here
        // For now, we just try to open the file and handle errors
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("Cannot open log file for writing: " + filepath);
        return;
    }
    
    // Write a header to the log file
    pImpl->fileOut << "NLM Log File: " << filepath << "\n";
    pImpl->fileOut << "Log level: " << levelToString(pImpl->level) << "\n";
    pImpl->fileOut << "Started at: " << std::put_time(std::gmtime(&(std::time(nullptr))), "%Y-%m-%d %H:%M:%S UTC") << "\n";
    pImpl->fileOut << "==========================================\n";
    pImpl->fileOut.flush();
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << "Log file closed at: " << std::put_time(std::gmtime(&(std::time(nullptr))), "%Y-%m-%d %H:%M:%S UTC") << "\n";
            pImpl->fileOut.flush();
            pImpl->fileOut.close();
        } catch (const std::exception& e) {
            // If we can't close the file properly, log to stderr
            std::cerr << "Error closing log file: " << e.what() << std::endl;
        }
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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

Logger::Logger(Logger&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
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
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
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

} // namespace nlm
