#ifdef _WIN32
#include <windows.h>
#endif

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxMessageLength = 10000;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check message length
    if (entry.message.size() > pImpl->maxMessageLength) {
        std::string truncated = entry.message.substr(0, pImpl->maxMessageLength - 3) + "...";
        // Log the truncated version
        logInternal(entry, truncated);
        return;
    }
    
    logInternal(entry, entry.message);
}

void ConsoleLogger::logInternal(const LogEntry& entry, const std::string& message) {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        // Use cross-platform color output
        #ifdef _WIN32
        // Windows console color codes (ANSI compatible via SetConsoleMode)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (entry.level == LogLevel::Debug) {
            SetConsoleTextAttribute(hConsole, 11); // Cyan
        } else if (entry.level == LogLevel::Info) {
            SetConsoleTextAttribute(hConsole, 10); // Green
        } else if (entry.level == LogLevel::Warning) {
            SetConsoleTextAttribute(hConsole, 14); // Yellow
        } else if (entry.level == LogLevel::Error) {
            SetConsoleTextAttribute(hConsole, 12); // Red
        } else if (entry.level == LogLevel::Critical) {
            SetConsoleTextAttribute(hConsole, 13); // Magenta
        }
        #endif
        std::cout << levelToColor(entry.level) << output << "\n";
        
        #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 7); // Reset to default
        #endif
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        
        // Check file size and rotate if needed
        checkAndRotateFile();
        
        // Use async write for better performance
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    closeOutputFile();
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::checkAndRotateFile() {
    if (!pImpl->fileOut.is_open()) return;
    
    // Get current file size
    std::streampos current_pos = pImpl->fileOut.tellp();
    if (current_pos >= static_cast<std::streampos>(pImpl->maxFileSize)) {
        // Rotate file
        closeOutputFile();
        std::string rotated_path = pImpl->fileOut._Getpath();
        if (!rotated_path.empty()) {
            // Add timestamp to rotated filename
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            char timestamp[20];
            std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&time_t));
            
            std::string rotated_filename = rotated_path + "." + std::string(timestamp);
            std::rename(rotated_path.c_str(), rotated_filename.c_str());
            
            // Reopen for writing
            pImpl->fileOut.open(rotated_path, std::ios::trunc);
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

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
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
    
    // Check message length before creating LogEntry
    if (message.size() > 10000) {
        return; // Skip very long messages
    }
    
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
    // Ensure the stream is flushed before logging
    oss_.flush();
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
