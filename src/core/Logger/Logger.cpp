#include "Logger.hpp"

namespace nlm {

// ConsoleLogger implementation
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    configure(defaultConfig);
}

ConsoleLogger::ConsoleLogger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    configure(config);
}

ConsoleLogger::~ConsoleLogger() {
    try {
        closeOutputFile();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Logger destructor error: %s\n", e.what());
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!pImpl->enabled || entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::string formattedMessage = formatMessage(entry);
        
        if (pImpl->enableConsole) {
            if (pImpl->useColors && !formattedMessage.empty()) {
                std::cout << formattedMessage;
            } else if (!formattedMessage.empty()) {
                std::cout << formattedMessage;
            }
        }
        
        if (pImpl->enableFile && pImpl->fileOut.is_open() && !formattedMessage.empty()) {
            pImpl->fileOut << formattedMessage;
            if (pImpl->fileOut.bad()) {
                throw FileOperationError("Failed to write to log file: " + pImpl->outputFile);
            }
            pImpl->fileOut.flush();
        }
        
    } catch (const std::exception& e) {
        try {
            std::cerr << "[LOGGER ERROR] Failed to log message: " << e.what() << std::endl;
        } catch (...) {
            std::fprintf(stderr, "[LOGGER ERROR] Critical logging failure\n");
        }
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

void ConsoleLogger::configure(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool needsReopen = false;
    
    if (config != pImpl->config) {
        pImpl->config = config;
        needsReopen = true;
    }
    
    if (config.useColors != pImpl->useColors) {
        pImpl->useColors = config.useColors;
    }
    
    if (needsReopen || config.outputFile != pImpl->outputFile) {
        closeOutputFile();
        if (config.enableFile && !config.outputFile.empty()) {
            setOutputFile(config.outputFile);
        }
    }
    
    pImpl->enabled = (config.level != LogLevel::Silent);
}

LoggerConfig ConsoleLogger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enableFile && pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
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
    
    closeOutputFile();
    
    if (filepath.empty()) {
        return;
    }
    
    std::error_code ec;
    uintmax_t fileSize = 0;
    if (std::filesystem::exists(filepath, ec)) {
        fileSize = std::filesystem::file_size(filepath, ec);
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        throw FileOperationError("Cannot open log file: " + filepath);
    }
    
    if (fileSize >= pImpl->config.maxFileSize) {
        std::string rotatedPath = filepath + ".1";
        std::rename(filepath.c_str(), rotatedPath.c_str());
    }
    
    pImpl->outputFile = filepath;
    pImpl->enableFile = true;
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut.close();
        } catch (const std::exception& e) {
            std::fprintf(stderr, "Warning: Failed to close log file %s: %s\n", 
                       pImpl->outputFile.c_str(), e.what());
        }
    }
    
    pImpl->fileOut.clear();
    pImpl->enableFile = false;
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    if (pImpl->useColors && entry.level != LogLevel::Silent) {
        oss << levelToColor(entry.level);
    }
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
    
    oss << "[" << getCurrentThreadId() << "] ";
    
    oss << "[" << levelToString(entry.level) << "] ";
    
    if (!entry.file.empty()) {
        oss << "(" << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        if (!entry.function.empty()) {
            oss << " " << entry.function << ")";
        } else {
            oss << ")";
        }
        oss << " ";
    }
    
    oss << entry.message;
    
    if (!entry.metadata.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& [key, value] : entry.metadata) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        oss << "}";
    }
    
    if (pImpl->useColors && entry.level != LogLevel::Silent) {
        oss << "\033[0m";
    }
    
    oss << "\n";
    return oss.str();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    static const std::unordered_map<LogLevel, std::string> levelStrings = {
        {LogLevel::Silent, "SILENT"},
        {LogLevel::Emergency, "EMERGENCY"},
        {LogLevel::Alert, "ALERT"},
        {LogLevel::Critical, "CRITICAL"},
        {LogLevel::Error, "ERROR"},
        {LogLevel::Warning, "WARN"},
        {LogLevel::Notice, "NOTICE"},
        {LogLevel::Info, "INFO"},
        {LogLevel::Debug, "DEBUG"},
        {LogLevel::Trace, "TRACE"},
        {LogLevel::All, "ALL"}
    };
    
    auto it = levelStrings.find(level);
    return it != levelStrings.end() ? it->second : "UNKNOWN";
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    static const std::unordered_map<LogLevel, std::string> levelColors = {
        {LogLevel::Emergency, "\033[95m"},
        {LogLevel::Alert, "\033[91m"},
        {LogLevel::Critical, "\033[91m"},
        {LogLevel::Error, "\033[31m"},
        {LogLevel::Warning, "\033[93m"},
        {LogLevel::Notice, "\033[94m"},
        {LogLevel::Info, "\033[92m"},
        {LogLevel::Debug, "\033[36m"},
        {LogLevel::Trace, "\033[90m"},
    };
    
    auto it = levelColors.find(level);
    return it != levelColors.end() ? it->second : "\033[0m";
}

std::string ConsoleLogger::getCurrentThreadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

const LoggerConfig ConsoleLogger::defaultConfig = {
    .level = LogLevel::Info,
    .outputFile = "",
    .useColors = true,
    .enableConsole = true,
    .enableFile = false,
    .maxFileSize = 10 * 1024 * 1024,
    .maxFiles = 5,
    .colorTheme = "default",
    .asyncLogging = false,
    .bufferSize = 4096
};

void Logger::configureFromConfig(const Config& config) {
    std::lock_guard<std::shared_mutex> lock(pImpl->configMutex);
    
    if (config.has("logger.level")) {
        auto levelStr = config.get<std::string>("logger.level");
        pImpl->config.level = getLogLevelFromString(*levelStr);
    }
    
    if (config.has("logger.output_file")) {
        auto filePath = config.get<std::string>("logger.output_file");
        pImpl->config.outputFile = *filePath;
        pImpl->config.enableFile = !filePath->empty();
    }
    
    if (config.has("logger.use_colors")) {
        auto useColors = config.get<bool>("logger.use_colors");
        pImpl->config.useColors = *useColors;
    }
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->configure(pImpl->config);
        } catch (const std::exception& e) {
            throw ConfigurationError("Failed to apply logger configuration: " + std::string(e.what()));
        }
    }
}

LogLevel Logger::getLogLevelFromString(const std::string& levelStr) {
    static const std::unordered_map<std::string, LogLevel> levelMap = {
        {"silent", LogLevel::Silent},
        {"emergency", LogLevel::Emergency},
        {"alert", LogLevel::Alert},
        {"critical", LogLevel::Critical},
        {"error", LogLevel::Error},
        {"warning", LogLevel::Warning},
        {"notice", LogLevel::Notice},
        {"info", LogLevel::Info},
        {"debug", LogLevel::Debug},
        {"trace", LogLevel::Trace},
        {"all", LogLevel::All}
    };
    
    std::string level = levelStr;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    
    auto it = levelMap.find(level);
    return it != levelMap.end() ? it->second : LogLevel::Info;
}

std::string Logger::getLogLevelString(LogLevel level) const {
    static const std::unordered_map<LogLevel, std::string> levelStrings = {
        {LogLevel::Silent, "silent"},
        {LogLevel::Emergency, "emergency"},
        {LogLevel::Alert, "alert"},
        {LogLevel::Critical, "critical"},
        {LogLevel::Error, "error"},
        {LogLevel::Warning, "warning"},
        {LogLevel::Notice, "notice"},
        {LogLevel::Info, "info"},
        {LogLevel::Debug, "debug"},
        {LogLevel::Trace, "trace"},
        {LogLevel::All, "all"}
    };
    
    auto it = levelStrings.find(level);
    return it != levelStrings.end() ? it->second : "unknown";
}

bool Logger::isLevelEnabled(LogLevel level) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
    return level != LogLevel::Silent && level >= pImpl->config.level;
}

} // namespace nlm
