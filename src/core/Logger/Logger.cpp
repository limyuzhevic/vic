#include <thread>
#include <chrono>
#include <filesystem>
#include <system_error>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    PerformanceMetrics metrics;
    LogConfig config;
    uint64_t filteredCount = 0;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::ConsoleLogger(const LogConfig& config) : pImpl(std::make_unique<Impl>()) {
    configureFromConfig(config);
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::configureFromConfig(const LogConfig& config) {
    pImpl->config = config;
    pImpl->level = config.level;
    pImpl->useColors = config.useColors;
    if (config.logToFile && !config.filePath.empty()) {
        setOutputFile(config.filePath);
    }
}

bool ConsoleLogger::loadConfigFromFile(const std::string& filepath) {
    try {
        nlm::Config config;
        if (!config.loadFromFile(filepath)) {
            return false;
        }
        
        LogConfig logConfig;
        std::string levelStr = config.getOr<std::string>("logging.level", "INFO");
        std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
        
        if (levelStr == "DEBUG") logConfig.level = LogLevel::Debug;
        else if (levelStr == "INFO") logConfig.level = LogLevel::Info;
        else if (levelStr == "WARNING") logConfig.level = LogLevel::Warning;
        else if (levelStr == "ERROR") logConfig.level = LogLevel::Error;
        else if (levelStr == "CRITICAL") logConfig.level = LogLevel::Critical;
        
        logConfig.useColors = config.getOr<bool>("logging.use_colors", true);
        logConfig.filePath = config.getOr<std::string>("logging.filename", "");
        logConfig.logToFile = config.getOr<bool>("logging.to_file", false);
        logConfig.enablePerformanceMonitoring = true;
        logConfig.enableStructuredLogging = config.getOr<bool>("logging.structured", false);
        
        configureFromConfig(logConfig);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading logging config: " << e.what() << std::endl;
        return false;
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Performance timing
    auto startTime = std::chrono::system_clock::now();
    
    // Check if entry should be logged
    if (entry.level < pImpl->level) {
        pImpl->filteredCount++;
        pImpl->metrics.filteredLogs++;
        return;
    }
    
    // Check filters
    bool shouldLog = shouldLog(entry);
    if (!shouldLog) {
        pImpl->filteredCount++;
        pImpl->metrics.filteredLogs++;
        return;
    }
    
    // Get processing time
    auto endTime = std::chrono::system_clock::now();
    auto processingTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update metrics
    pImpl->metrics.totalLogs++;
    pImpl->metrics.lastLogTime = std::chrono::system_clock::now();
    pImpl->metrics.totalProcessingTime += std::chrono::system_clock::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (pImpl->config.enablePerformanceMonitoring) {
        pImpl->metrics.averageProcessingTime = 
            static_cast<double>(pImpl->metrics.totalProcessingTime.count()) / pImpl->metrics.totalLogs;
    }
    
    // Build log message
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add file and line info for non-structured logging
    if (!pImpl->config.enableStructuredLogging && !entry.file.empty()) {
        oss << " " << entry.file << ":" << entry.line;
        if (!entry.function.empty()) {
            oss << " (" << entry.function << ")";
        }
    }
    
    oss << " " << entry.message;
    
    // Add structured fields
    if (pImpl->config.enableStructuredLogging && !entry.fields.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& field : entry.fields) {
            if (!first) oss << ", ";
            oss << field.first << "=" << field.second;
            first = false;
        }
        oss << "}";
    }
    
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
    
    // Critical event tracking
    if (entry.level >= LogLevel::Error) {
        pImpl->metrics.criticalEvents++;
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

void ConsoleLogger::setOutputFile(const std::string& filepath, int maxRetries, std::chrono::milliseconds retryDelay) {
    // Close existing file if open
    closeOutputFile();
    
    // Validate filepath
    if (filepath.empty()) {
        std::cerr << "ERROR: Empty file path provided" << std::endl;
        return;
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path filePath(filepath);
    std::error_code ec;
    if (!std::filesystem::exists(filePath.parent_path(), ec)) {
        std::cerr << "WARNING: Log directory does not exist: " << filePath.parent_path() << std::endl;
        try {
            std::filesystem::create_directories(filePath.parent_path());
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Could not create log directory " << filePath.parent_path() << ": " << e.what() << std::endl;
            return;
        }
    }
    
    // Try to open file with retry logic
    std::ofstream newFile(filepath, std::ios::app);
    
    // Attempt opening with retry logic
    for (int attempt = 0; attempt <= maxRetries && !newFile.is_open(); ++attempt) {
        if (attempt > 0) {
            std::cerr << "WARNING: Retry attempt " << (attempt + 1) << " of " << (maxRetries + 1) << std::endl;
            std::this_thread::sleep_for(retryDelay);
            
            // Try to create directory again on retry
            if (!std::filesystem::exists(filePath.parent_path(), ec)) {
                try {
                    std::filesystem::create_directories(filePath.parent_path());
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Could not create log directory " << filePath.parent_path() << " (retry): " << e.what() << std::endl;
                    continue;
                }
            }
            
            newFile.open(filepath, std::ios::app);
        }
    }
    
    if (newFile.is_open()) {
        // Successfully opened, transfer ownership to pImpl
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        pImpl->fileOut.swap(newFile);
        std::cerr << "INFO: Log file opened successfully: " << filepath << std::endl;
    } else {
        // All attempts failed
        std::cerr << "ERROR: Could not open log file after " << (maxRetries + 1) << " attempts: " << filepath << std::endl;
        
        // Try fallback to stdout
        std::cerr << "WARNING: Falling back to console output only" << std::endl;
    }
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

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level
    if (entry.level < pImpl->level) return false;
    
    // Check filters
    for (const auto& filter : pImpl->config.filters) {
        if (!matchesFilter(entry, filter.first)) {
            return false;
        }
    }
    
    // Additional filter logic could be added here
    return true;
}

bool ConsoleLogger::matchesFilter(const LogEntry& entry, const std::string& filter) const {
    // Simple filter matching - can be extended
    if (filter == "debug" && entry.level == LogLevel::Debug) return true;
    if (filter == "info" && entry.level == LogLevel::Info) return true;
    if (filter == "warning" && entry.level == LogLevel::Warning) return true;
    if (filter == "error" && entry.level >= LogLevel::Error) return true;
    if (filter == "performance" && entry.structuredData.find("performance") != entry.structuredData.end()) return true;
    if (filter == "error_details" && entry.structuredData.find("error") != entry.structuredData.end()) return true;
    
    return false;
}

void ConsoleLogger::updatePerformanceMetrics(double processingTime, bool filtered) {
    pImpl->metrics.averageProcessingTime = processingTime;
}

void ConsoleLogger::logStructured(const std::string& message, const std::unordered_map<std::string, std::string>& fields) {
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = message;
    entry.fields = fields;
    log(entry);
}

void ConsoleLogger::logStructured(const std::string& message, const std::unordered_map<std::string, std::variant<int, double, std::string, bool>>& data) {
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = message;
    entry.structuredData = data;
    log(entry);
}

double ConsoleLogger::getLastProcessingTime() const {
    return pImpl->metrics.averageProcessingTime;
}

uint64_t ConsoleLogger::getTotalLogCount() const {
    return pImpl->metrics.totalLogs;
}

uint64_t ConsoleLogger::getFilteredCount(const std::string& filter) const {
    return pImpl->filteredCount;
}

void ConsoleLogger::resetPerformanceMetrics() {
    pImpl->metrics = PerformanceMetrics();
    pImpl->filteredCount = 0;
}

std::string ConsoleLogger::getPerformanceSummary() const {
    std::ostringstream oss;
    oss << "=== Performance Summary ===" << std::endl;
    oss << "Total logs: " << pImpl->metrics.totalLogs << std::endl;
    oss << "Filtered logs: " << pImpl->metrics.filteredLogs << std::endl;
    oss << "Average processing time: " << pImpl->metrics.averageProcessingTime << "μs" << std::endl;
    oss << "Critical events (Error/Critical): " << pImpl->metrics.criticalEvents << std::endl;
    oss << "Last log: " << std::chrono::system_clock::to_time_t(pImpl->metrics.lastLogTime) << std::endl;
    
    if (pImpl->fileOut.is_open()) {
        oss << "Output file: " << "[open]" << std::endl;
    }
    
    return oss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    PerformanceMetrics metrics;
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

void Logger::loadConfigFromFile(const std::string& filepath) {
    for (auto& logger : pImpl->loggers) {
        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
        if (consoleLogger) {
            consoleLogger->loadConfigFromFile(filepath);
        }
    }
}

void Logger::configureFromGlobalConfig(const nlm::Config& config) {
    // Configure all console loggers from the global config
    for (auto& logger : pImpl->loggers) {
        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
        if (consoleLogger) {
            LogConfig logConfig;
            
            // Parse config values
            std::string levelStr = config.getOr<std::string>("logging.level", "INFO");
            std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
            
            if (levelStr == "DEBUG") logConfig.level = LogLevel::Debug;
            else if (levelStr == "INFO") logConfig.level = LogLevel::Info;
            else if (levelStr == "WARNING") logConfig.level = LogLevel::Warning;
            else if (levelStr == "ERROR") logConfig.level = LogLevel::Error;
            else if (levelStr == "CRITICAL") logConfig.level = LogLevel::Critical;
            
            logConfig.useColors = config.getOr<bool>("logging.use_colors", true);
            logConfig.filePath = config.getOr<std::string>("logging.filename", "");
            logConfig.logToFile = config.getOr<bool>("logging.to_file", false);
            logConfig.enablePerformanceMonitoring = true;
            logConfig.enableStructuredLogging = config.getOr<bool>("logging.structured", false);
            
            consoleLogger->configureFromConfig(logConfig);
        }
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

void Logger::resetPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
        if (consoleLogger) {
            consoleLogger->resetPerformanceMetrics();
        }
    }
}

std::string Logger::getPerformanceSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "=== Global Logger Performance Summary ===" << std::endl;
    
    for (auto& logger : pImpl->loggers) {
        oss << logger->getPerformanceSummary() << std::endl;
    }
    
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::operator<<(const std::pair<std::string, std::string>& field) {
    // For now, just add to the stream
    oss_ << " [" << field.first << "=" << field.second << "]";
    return *this;
}

} // namespace nlm