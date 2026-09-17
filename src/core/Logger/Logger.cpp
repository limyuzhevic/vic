// SimpleFormatter implementation
std::string SimpleFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    if (!entry.file.empty()) {
        oss << " [" << entry.file << ":" << entry.line << "]";
    }
    if (!entry.function.empty()) {
        oss << " [" << entry.function << "]";
    }
    if (!entry.category.empty()) {
        oss << " [" << entry.category << "]";
    }
    if (!entry.errorCode.empty()) {
        oss << " [Error: " << entry.errorCode << "]";
    }
    
    return oss.str();
}

// JsonFormatter implementation
std::string JsonFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\",\n";
    oss << "  \"level\": \"" << levelToString(entry.level) << "\",\n";
    oss << "  \"message\": \"" << entry.message << "\",\n";
    oss << "  \"file\": \"" << entry.file << "\",\n";
    oss << "  \"line\": " << entry.line << ",\n";
    oss << "  \"function\": \"" << entry.function << "\",\n";
    oss << "  \"category\": \"" << entry.category << "\",\n";
    oss << "  \"errorCode\": \"" << entry.errorCode << "\",\n";
    oss << "  \"stackTrace\": \"" << entry.stackTrace << "\",\n";
    oss << "  \"metadata\": {";
    
    bool first = true;
    for (const auto& pair : entry.metadata) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    oss << "}\n";
    oss << "}\n";
    return oss.str();
}

// AsyncLogger implementation
struct AsyncLogger::Impl {
    std::queue<LogEntry> queue_;
    std::mutex queueMutex_;
    std::condition_variable condVar_;
    std::atomic<bool> stop_;
    std::unique_ptr<std::thread> workerThread_;
    std::shared_ptr<ILogger> logger_;
    std::mutex loggerMutex_;
    size_t queueSize_;
    
    Impl(size_t queueSize = 10000) : stop_(false), queueSize_(queueSize) {}
};

AsyncLogger::AsyncLogger(size_t queueSize) : pImpl(std::make_unique<Impl>(queueSize)) {
    thread_ = std::make_unique<std::thread>(&AsyncLogger::workerThread, this);
}

AsyncLogger::~AsyncLogger() {
    stop();
}

void AsyncLogger::log(const LogEntry& entry) {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex_);
    
    // Block if queue is full
    pImpl->condVar_.wait(lock, [this] { 
        return pImpl->queue_.size() < pImpl->queueSize_ || pImpl->stop_; 
    });
    
    if (pImpl->stop_) {
        return;
    }
    
    pImpl->queue_.push(entry);
    lock.unlock();
    pImpl->condVar_.notify_one();
}

void AsyncLogger::stop() {
    pImpl->stop_ = true;
    pImpl->condVar_.notify_all();
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void AsyncLogger::setLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->loggerMutex_);
    pImpl->logger_ = logger;
}

void AsyncLogger::workerThread() {
    while (!pImpl->stop_) {
        LogEntry entry;
        {
            std::unique_lock<std::mutex> lock(pImpl->queueMutex_);
            
            // Wait for items or stop signal
            pImpl->condVar_.wait(lock, [this] { 
                return !pImpl->queue_.empty() || pImpl->stop_; 
            });
            
            if (pImpl->stop_ && pImpl->queue_.empty()) {
                break;
            }
            
            entry = pImpl->queue_.front();
            pImpl->queue_.pop();
        }
        
        // Process entry if we have a logger
        {
            std::lock_guard<std::mutex> lock(pImpl->loggerMutex_);
            if (pImpl->logger_) {
                try {
                    pImpl->logger_->log(entry);
                } catch (...) {
                    // Silently ignore logging errors to avoid crashing
                }
            }
        }
    }
}

// FileLogger implementation
struct FileLogger::Impl {
    std::string basePath;
    std::ofstream fileOut;
    std::mutex mutex;
    LogLevel level = LogLevel::Info;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxFiles = 5;
    uint64_t currentSize = 0;
    int fileNumber = 1;
    std::shared_ptr<LogFormatter> formatter;
};

FileLogger::FileLogger(const std::string& filepath, size_t maxFileSize, 
                       int maxFiles, LogLevel level,
                       std::shared_ptr<LogFormatter> formatter)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->basePath = filepath;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
    pImpl->level = level;
    pImpl->formatter = formatter ? formatter : std::make_shared<SimpleFormatter>();
    
    // Ensure directory exists
    try {
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    } catch (...) {
        // Create fallback directory if needed
        std::filesystem::create_directories("/tmp/nlm_logs");
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
}

FileLogger::~FileLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check if rotation is needed
        if (pImpl->currentSize >= pImpl->maxFileSize && pImpl->fileNumber > 1) {
            rotate();
        }
        
        std::string output;
        if (pImpl->formatter) {
            output = pImpl->formatter->format(entry);
        } else {
            output = SimpleFormatter().format(entry);
        }
        
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output;
            if (!output.empty() && output.back() != '\n') {
                pImpl->fileOut << "\n";
            }
            pImpl->fileOut.flush();
            pImpl->currentSize += output.size() + (output.empty() || output.back() != '\n' ? 1 : 0);
        }
        
    } catch (...) {
        // Silently fail for file logging to avoid breaking the application
    }
}

void FileLogger::logStructured(const LogEntry& entry, const std::string& format) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::string output;
        if (format == "json" || pImpl->formatter == nullptr || 
            dynamic_cast<JsonFormatter*>(pImpl->formatter.get()) != nullptr) {
            output = JsonFormatter().format(entry);
        } else {
            output = SimpleFormatter().format(entry);
        }
        
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output;
            if (!output.empty() && output.back() != '\n') {
                pImpl->fileOut << "\n";
            }
            pImpl->fileOut.flush();
        }
        
    } catch (...) {
        // Fallback to standard logging
        log(entry, "Structured logging failed: " + format);
    }
}

void FileLogger::rotate() {
    try {
        pImpl->fileOut.close();
        
        // Move current file
        std::string currentPath = pImpl->basePath;
        if (pImpl->fileNumber >= 1 && pImpl->fileNumber < pImpl->maxFiles) {
            std::string rotatedPath = pImpl->basePath;
            size_t dotPos = rotatedPath.rfind('.');
            if (dotPos != std::string::npos) {
                rotatedPath.insert(dotPos, "_" + std::to_string(pImpl->fileNumber));
            } else {
                rotatedPath += "_" + std::to_string(pImpl->fileNumber);
            }
            
            if (std::filesystem::exists(rotatedPath)) {
                std::filesystem::remove(rotatedPath);
            }
            std::filesystem::rename(pImpl->basePath, rotatedPath);
        }
        
        pImpl->fileNumber = 1;
        pImpl->currentSize = 0;
        pImpl->fileOut.open(pImpl->basePath, std::ios::app);
        
    } catch (...) {
        // Ignore rotation failures
    }
}

std::string FileLogger::getCurrentFilePath() const {
    return pImpl->basePath;
}

void FileLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatter = formatter;
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::shared_ptr<LogFormatter> formatter;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->formatter = std::make_shared<SimpleFormatter>();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->formatter = std::make_shared<SimpleFormatter>();
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::string output;
        if (pImpl->formatter) {
            output = pImpl->formatter->format(entry);
        } else {
            output = SimpleFormatter().format(entry);
        }
        
        // Console output with error handling
        if (pImpl->useColors) {
            try {
                std::cout << levelToColor(entry.level) << output;
                if (!output.empty() && output.back() != '\n') {
                    std::cout << "\n";
                }
                std::cout << "\033[0m";
            } catch (...) {
                std::cerr << "[ERROR] Failed to write colored log output\n";
                std::cout << output;
                if (!output.empty() && output.back() != '\n') {
                    std::cout << "\n";
                }
            }
        } else {
            std::cout << output;
            if (!output.empty() && output.back() != '\n') {
                std::cout << "\n";
            }
        }
        
        // File output if set with error handling
        if (pImpl->fileOut.is_open()) {
            try {
                pImpl->fileOut << output;
                if (!output.empty() && output.back() != '\n') {
                    pImpl->fileOut << "\n";
                }
                pImpl->fileOut.flush();
            } catch (...) {
                std::cerr << "[ERROR] Failed to write log to file\n";
                // Try to recover by attempting to reopen the file
                pImpl->fileOut.close();
                try {
                    pImpl->fileOut.open("/tmp/fallback.log", std::ios::app);
                    if (pImpl->fileOut.is_open()) {
                        pImpl->fileOut << "[RECOVERY] Log recovery successful\n";
                        pImpl->fileOut << output;
                        if (!output.empty() && output.back() != '\n') {
                            pImpl->fileOut << "\n";
                        }
                    }
                } catch (...) {
                    std::cerr << "[ERROR] Failed to recover log file\n";
                }
            }
        }
        
    } catch (...) {
        std::cerr << "[CRITICAL] Unhandled exception in ConsoleLogger::log\n";
    }
}

void ConsoleLogger::logStructured(const LogEntry& entry, const std::string& format) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::string output;
        if (format == "json") {
            output = JsonFormatter().format(entry);
        } else if (pImpl->formatter) {
            output = pImpl->formatter->format(entry);
        } else {
            output = SimpleFormatter().format(entry);
        }
        
        // Output to console
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level);
        }
        std::cout << output;
        if (pImpl->useColors) {
            std::cout << "\033[0m";
        }
        
        // Output to file if set
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output;
            pImpl->fileOut.flush();
        }
        
    } catch (...) {
        // Fallback to standard logging
        log(entry, "Structured logging failed: " + format);
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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

void ConsoleLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatter = formatter;
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

void Logger::error(const std::string& message, const std::string& errorCode,
                   const std::string& file, int line,
                   const std::string& function, const std::string& stackTrace) {
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = message;
    entry.errorCode = errorCode;
    entry.stackTrace = stackTrace;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry.level, message, file, line, function);
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                             const std::unordered_map<std::string, std::string>& metadata,
                             const std::string& file, int line,
                             const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    log(entry.level, message, file, line, function);
}

void Logger::logWithCategory(LogLevel level, const std::string& message,
                             const std::string& category,
                             const std::string& file, int line,
                             const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.category = category;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    log(entry.level, message, file, line, function);
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::metadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::errorCode(const std::string& code) {
    errorCode_ = code;
    return *this;
}

LogStream& LogStream::stackTrace(const std::string& trace) {
    stackTrace_ = trace;
    return *this;
}

LogStream& LogStream::category(const std::string& category) {
    category_ = category;
    return *this;
}

} // namespace nlm