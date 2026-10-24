#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <ctime>

namespace nlm {

// Helper functions for logging
namespace internal {
    std::string getCurrentThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }

    std::string getTimestamp(const std::chrono::system_clock::time_point& timestamp, const std::string& format) {
        auto time_t_now = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        if (format.empty()) {
            // Default format: YYYY-MM-DD HH:MM:SS.mmm
            ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
               << "." << std::setfill('0') << std::setw(3) << ms.count();
        } else {
            // Custom format (simplified)
            ss << std::put_time(std::localtime(&time_t_now), format.c_str())
               << "." << std::setfill('0') << std::setw(3) << ms.count();
        }
        return ss.str();
    }

    std::string formatMessage(const std::string& message, const std::string& category, 
                             const std::unordered_map<std::string, std::string>& metadata,
                             const std::string& threadId) {
        std::stringstream ss;
        if (!category.empty()) {
            ss << "[" << category << "] ";
        }
        ss << message;
        if (!threadId.empty()) {
            ss << " [" << threadId << "]";
        }
        if (!metadata.empty()) {
            ss << " {";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) ss << ", ";
                first = false;
                ss << pair.first << "=" << pair.second;
            }
            ss << "}";
        }
        return ss.str();
    }

    std::string escapeXML(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&apos;"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    std::string escapeJSON(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: 
                    if (c < 32 || c > 126) {
                        result += "\\u" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }

    std::string formatToJSON(const LogEntry& entry) {
        std::stringstream ss;
        ss << "{";
        ss << "\"timestamp\": \"" << escapeJSON(std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                entry.timestamp.time_since_epoch()).count())) << "\",";
        ss << "\"level\": \"" << logLevelToString(entry.level) << "\",";
        ss << "\"category\": \"" << escapeJSON(entry.category) << "\",";
        ss << "\"message\": \"" << escapeJSON(entry.message) << "\",";
        ss << "\"file\": \"" << escapeJSON(entry.file) << "\",";
        ss << "\"line\": " << entry.line << ",";
        ss << "\"function\": \"" << escapeJSON(entry.function) << "\"";

        if (!entry.threadId.empty()) {
            ss << ",\"threadId\": \"" << escapeJSON(entry.threadId) << "\"";
        }

        if (!entry.metadata.empty()) {
            ss << ",\"metadata\": {";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) ss << ", ";
                first = false;
                ss << "\"" << escapeJSON(pair.first) << "\": \"" << escapeJSON(pair.second) << "\"";
            }
            ss << "}";
        }

        ss << "}";
        return ss.str();
    }

    std::string formatToCSV(const LogEntry& entry) {
        std::stringstream ss;
        ss << "\"" << std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).count() << "\",";
        ss << "\"" << logLevelToString(entry.level) << "\",";
        ss << "\"" << escapeCSV(entry.category) << "\",";
        ss << "\"" << escapeCSV(entry.message) << "\",";
        ss << "\"" << escapeCSV(entry.file) << "\",";
        ss << entry.line << ",";
        ss << "\"" << escapeCSV(entry.function) << "\"";

        if (!entry.threadId.empty()) {
            ss << ",\"" << escapeCSV(entry.threadId) << "\"";
        }

        ss << "\n";
        return ss.str();
    }

    std::string formatToXML(const LogEntry& entry) {
        std::stringstream ss;
        ss << "<log>";
        ss << "<timestamp>" << std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()).count() << "</timestamp>";
        ss << "<level>" << logLevelToString(entry.level) << "</level>";
        ss << "<category>" << escapeXML(entry.category) << "</category>";
        ss << "<message>" << escapeXML(entry.message) << "</message>";
        ss << "<file>" << escapeXML(entry.file) << "</file>";
        ss << "<line>" << entry.line << "</line>";
        ss << "<function>" << escapeXML(entry.function) << "</function>";

        if (!entry.threadId.empty()) {
            ss << "<threadId>" << escapeXML(entry.threadId) << "</threadId>";
        }

        if (!entry.metadata.empty()) {
            ss << "<metadata>";
            for (const auto& pair : entry.metadata) {
                ss << "<" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">";
            }
            ss << "</metadata>";
        }

        ss << "</log>";
        return ss.str();
    }

    std::string escapeCSV(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '\"' || c == ',' || c == '\n' || c == '\r') {
                result += "\"" + escapeJSON(str) + "\"";
                return result;
            }
        }
        return str;
    }

    std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    bool shouldLogEntry(const LogEntry& entry, const LogFilter& filter) {
        if (entry.level < filter.minLevel) {
            return false;
        }

        if (!filter.categories.empty()) {
            bool inCategories = std::find(filter.categories.begin(), filter.categories.end(), entry.category) != filter.categories.end();
            if (filter.excludeCategories != inCategories) {
                return false;
            }
        }

        if (!filter.metadataFilters.empty()) {
            bool matchesFilters = true;
            for (const auto& pair : filter.metadataFilters) {
                auto it = entry.metadata.find(pair.first);
                if (it == entry.metadata.end() || it->second != pair.second) {
                    if (filter.excludeMetadata) {
                        matchesFilters = false;
                        break;
                    }
                } else {
                    if (!filter.excludeMetadata) {
                        matchesFilters = false;
                        break;
                    }
                }
            }
            if (!matchesFilters) {
                return false;
            }
        }

        return true;
    }

    std::string getNextLogFilename(const std::string& basePath) {
        std::string extension = "";
        size_t dotPos = basePath.find_last_of('.');
        if (dotPos != std::string::npos) {
            extension = basePath.substr(dotPos);
        }

        std::string baseName = basePath;
        if (!extension.empty()) {
            baseName = basePath.substr(0, dotPos);
        }

        if (!std::filesystem::exists(basePath)) {
            return basePath;
        }

        for (int i = 1; i < 1000; ++i) {
            std::stringstream ss;
            ss << baseName << "_" << i << extension;
            std::string rotatedName = ss.str();
            if (!std::filesystem::exists(rotatedName)) {
                return rotatedName;
            }
        }

        return basePath;
    }

    void compressFile(const std::string& filepath) {
        // Simplified compression - in practice, you would use a real compression library
        // This is a placeholder for future implementation
    }

    class AsyncLoggerWorker {
    public:
        AsyncLoggerWorker(size_t poolSize) : stop(true) {
            for (size_t i = 0; i < poolSize; ++i) {
                workers.emplace_back([this]() {
                    while (true) {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]() {
                            return !this->queue.empty() || this->stop;
                        });

                        if (this->stop && this->queue.empty()) {
                            break;
                        }

                        LogEntry entry = std::move(this->queue.front());
                        this->queue.pop();
                        lock.unlock();

                        processEntry(entry);
                    }
                });
            }
        }

        ~AsyncLoggerWorker() {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                stop = true;
            }
            condition.notify_all();
            for (auto& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        void log(const LogEntry& entry) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queue.push(entry);
            lock.unlock();
            condition.notify_one();
        }

        void setLevel(LogLevel level) {
            std::unique_lock<std::mutex> lock(levelMutex);
            currentLevel = level;
        }

        LogLevel getLevel() const {
            std::unique_lock<std::mutex> lock(levelMutex);
            return currentLevel;
        }

        void setFilter(const LogFilter& filter) {
            std::unique_lock<std::mutex> lock(filterMutex);
            currentFilter = filter;
        }

        LogFilter getFilter() const {
            std::unique_lock<std::mutex> lock(filterMutex);
            return currentFilter;
        }

        void setFormat(LogFormat format) {
            std::unique_lock<std::mutex> lock(formatMutex);
            currentFormat = format;
        }

        LogFormat getFormat() const {
            std::unique_lock<std::mutex> lock(formatMutex);
            return currentFormat;
        }

        void setOutput(LogOutput output) {
            std::unique_lock<std::mutex> lock(outputMutex);
            currentOutput = output;
        }

        LogOutput getOutput() const {
            std::unique_lock<std::mutex> lock(outputMutex);
            return currentOutput;
        }

        void setFilepath(const std::string& filepath) {
            std::unique_lock<std::mutex> lock(filepathMutex);
            currentFilepath = filepath;
        }

        std::string getFilepath() const {
            std::unique_lock<std::mutex> lock(filepathMutex);
            return currentFilepath;
        }

    private:
        void processEntry(const LogEntry& entry) {
            if (!shouldLogEntry(entry, currentFilter)) {
                return;
            }

            switch (currentOutput) {
                case LogOutput::CONSOLE:
                    logToConsole(entry);
                    break;
                case LogOutput::FILE:\n                    logToFile(entry);
                    break;
                case LogOutput::BOTH:
                    logToConsole(entry);
                    logToFile(entry);
                    break;
            }
        }

        void logToConsole(const LogEntry& entry) {
            // Simplified console logging - in practice, you would use a proper logger
            std::string formatted;
            switch (currentFormat) {
                case LogFormat::JSON:
                    formatted = formatToJSON(entry);
                    break;
                case LogFormat::CSV:
                    formatted = formatToCSV(entry);
                    break;
                case LogFormat::XML:
                    formatted = formatToXML(entry);
                    break;
                default:
                    formatted = formatMessage(entry.message, entry.category, entry.metadata, entry.threadId);
                    break;
            }

            std::cout << formatted << std::endl;
        }

        void logToFile(const LogEntry& entry) {
            std::ofstream file(currentFilepath, std::ios::app);
            if (file.is_open()) {
                switch (currentFormat) {
                    case LogFormat::JSON:
                        file << formatToJSON(entry) << std::endl;
                        break;
                    case LogFormat::CSV:
                        file << formatToCSV(entry);
                        break;
                    case LogFormat::XML:
                        file << formatToXML(entry) << std::endl;
                        break;
                    default:
                        file << formatMessage(entry.message, entry.category, entry.metadata, entry.threadId) << std::endl;
                        break;
                }
                file.flush();
            }
        }

        std::queue<LogEntry> queue;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::vector<std::thread> workers;
        std::mutex levelMutex;
        std::mutex filterMutex;
        std::mutex formatMutex;
        std::mutex outputMutex;
        std::mutex filepathMutex;
        LogLevel currentLevel = LogLevel::Info;
        LogFilter currentFilter;
        LogFormat currentFormat = LogFormat::TEXT;
        LogOutput currentOutput = LogOutput::CONSOLE;
        std::string currentFilepath = "log.txt";
        bool stop;
    };
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogFormat format = LogFormat::TEXT;
    LogFilter filter;
    ColorConfig colorConfig;
    std::string timestampFormat;
    bool includeThreadId = false;
    bool includeMetadata = false;
    std::string filepath;
    LogRotation rotation;
    bool rotationEnabled = false;
    std::string currentLogFile;
    std::time_t lastRotationTime;
    LogOutput output = LogOutput::CONSOLE;

    Impl() : lastRotationTime(std::time(nullptr)) {
        currentLogFile = filepath.empty() ? "log.txt" : filepath;
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->currentLogFile = pImpl->filepath.empty() ? "log.txt" : pImpl->filepath;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->currentLogFile = pImpl->filepath.empty() ? "log.txt" : pImpl->filepath;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (!internal::shouldLogEntry(entry, pImpl->filter)) {
        return;
    }

    if (entry.level < pImpl->level) {
        return;
    }

    std::lock_guard<std::mutex> lock(pImpl->mutex);

    // Check if rotation is needed
    if (pImpl->rotationEnabled) {
        auto now = std::chrono::system_clock::now();
        auto currentTime = std::chrono::system_clock::to_time_t(now);
        bool shouldRotate = false;

        if (pImpl->rotation.rotateDaily) {
            std::tm timeInfo = *std::localtime(&currentTime);
            if (timeInfo.tm_hour == 0 && timeInfo.tm_min == 0 && timeInfo.tm_sec == 0) {
                shouldRotate = true;
            }
        }

        if (!shouldRotate && pImpl->rotation.maxFileSize > 0) {
            // Check file size (simplified)
            std::error_code ec;
            auto fileSize = std::filesystem::file_size(pImpl->currentLogFile, ec);
            if (fileSize >= pImpl->rotation.maxFileSize) {
                shouldRotate = true;
            }
        }

        if (shouldRotate) {
            rotateLogFile();
        }
    }

    std::string formattedMessage;
    switch (pImpl->format) {
        case LogFormat::JSON:
            formattedMessage = internal::formatToJSON(entry);
            break;
        case LogFormat::CSV:
            formattedMessage = internal::formatToCSV(entry);
            break;
        case LogFormat::XML:
            formattedMessage = internal::formatToXML(entry);
            break;
        default:
            formattedMessage = formatMessage(entry, pImpl->format, pImpl->timestampFormat,
                                             pImpl->includeThreadId, pImpl->includeMetadata);
            break;
    }

    // Output to console
    if (pImpl->output == LogOutput::CONSOLE || pImpl->output == LogOutput::BOTH) {
        if (pImpl->useColors) {
            std::cout << internal::getColorCode(entry.level, pImpl->colorConfig) 
                     << formattedMessage << "\033[0m" << std::endl;
        } else {
            std::cout << formattedMessage << std::endl;
        }
    }

    // Output to file
    if (pImpl->output == LogOutput::FILE || pImpl->output == LogOutput::BOTH) {
        if (!pImpl->fileOut.is_open()) {
            pImpl->fileOut.open(pImpl->currentLogFile, std::ios::app);
            if (!pImpl->fileOut.is_open()) {
                std::cerr << "Failed to open log file: " << pImpl->currentLogFile << std::endl;
                return;
            }
        }
        pImpl->fileOut << formattedMessage << std::endl;
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    pImpl->filter = filter;
}

LogFilter ConsoleLogger::getFilter() const {
    return pImpl->filter;
}

void ConsoleLogger::setFormat(LogFormat format) {
    pImpl->format = format;
}

LogFormat ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setOutput(LogOutput output) {
    pImpl->output = output;
}

LogOutput ConsoleLogger::getOutput() const {
    return pImpl->output;
}

void ConsoleLogger::setFilepath(const std::string& filepath) {
    pImpl->filepath = filepath;
    pImpl->currentLogFile = filepath.empty() ? "log.txt" : filepath;
    closeOutputFile();
    setOutputFile(pImpl->currentLogFile);
}

std::string ConsoleLogger::getFilepath() const {
    return pImpl->filepath;
}

void ConsoleLogger::setColorConfig(const ColorConfig& config) {
    pImpl->colorConfig = config;
    pImpl->useColors = config.useColor;
}

ColorConfig ConsoleLogger::getColorConfig() const {
    return pImpl->colorConfig;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
    pImpl->colorConfig.useColor = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

std::string ConsoleLogger::getTimestampFormat() const {
    return pImpl->timestampFormat;
}

void ConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

bool ConsoleLogger::getIncludeThreadId() const {
    return pImpl->includeThreadId;
}

void ConsoleLogger::setIncludeMetadata(bool include) {
    pImpl->includeMetadata = include;
}

bool ConsoleLogger::getIncludeMetadata() const {
    return pImpl->includeMetadata;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    closeOutputFile();
    pImpl->filepath = filepath;
    pImpl->currentLogFile = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::getCurrentLogFile() const {
    return pImpl->currentLogFile;
}

void ConsoleLogger::setRotation(const LogRotation& rotation) {
    pImpl->rotation = rotation;
    pImpl->rotationEnabled = true;
}

LogRotation ConsoleLogger::getRotation() const {
    return pImpl->rotation;
}

void ConsoleLogger::enableRotation(bool enable) {
    pImpl->rotationEnabled = enable;
}

bool ConsoleLogger::isRotationEnabled() const {
    return pImpl->rotationEnabled;
}

std::vector<std::string> ConsoleLogger::getRotatedFiles() const {
    std::vector<std::string> rotatedFiles;
    std::string baseName = pImpl->currentLogFile;
    size_t dotPos = baseName.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string base = baseName.substr(0, dotPos);
        std::string ext = baseName.substr(dotPos);

        for (int i = 1; i <= pImpl->rotation.maxFiles; ++i) {
            std::stringstream ss;
            ss << base << "_" << i << ext;
            std::string rotatedName = ss.str();
            if (std::filesystem::exists(rotatedName)) {
                rotatedFiles.push_back(rotatedName);
            }
        }
    }
    return rotatedFiles;
}

void ConsoleLogger::rotateLogFile() {
    closeOutputFile();

    std::string rotatedName = getRotatedFilename();
    if (rotatedName != pImpl->currentLogFile) {
        std::rename(pImpl->currentLogFile.c_str(), rotatedName.c_str());
    }

    pImpl->currentLogFile = pImpl->filepath.empty() ? "log.txt" : pImpl->filepath;
    pImpl->fileOut.open(pImpl->currentLogFile, std::ios::app);

    if (pImpl->rotation.compressOldFiles) {
        for (const auto& file : getRotatedFiles()) {
            if (std::filesystem::exists(file)) {
                internal::compressFile(file);
            }
        }
    }
}

std::string ConsoleLogger::getRotatedFilename() const {
    std::string baseName = pImpl->currentLogFile;
    size_t dotPos = baseName.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string base = baseName.substr(0, dotPos);
        std::string ext = baseName.substr(dotPos);

        for (int i = pImpl->rotation.maxFiles - 1; i >= 1; --i) {
            std::stringstream ss;
            ss << base << "_" << i + 1 << ext;
            std::string nextName = ss.str();
            if (std::filesystem::exists(nextName)) {
                std::stringstream ss2;
                ss2 << base << "_" << i << ext;
                return ss2.str();
            }
        }

        return baseName;
    }
    return baseName;
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry, LogFormat format,
                                         const std::string& timestampFormat,
                                         bool includeThreadId, bool includeMetadata) const {
    std::stringstream ss;

    switch (format) {
        case LogFormat::JSON:
            return internal::formatToJSON(entry);
        case LogFormat::CSV:
            return internal::formatToCSV(entry);
        case LogFormat::XML:
            return internal::formatToXML(entry);
        default:
            // Text format
            ss << "[" << internal::logLevelToString(entry.level) << "] ";

            std::string timestamp = internal::getTimestamp(entry.timestamp, timestampFormat);
            ss << "[" << timestamp << "] ";

            if (includeThreadId && !entry.threadId.empty()) {
                ss << "[thread:" << entry.threadId << "] ";
            }

            if (!entry.category.empty()) {
                ss << "[" << entry.category << "] ";
            }

            ss << entry.message;

            if (includeMetadata && !entry.metadata.empty()) {
                ss << " {";
                bool first = true;
                for (const auto& pair : entry.metadata) {
                    if (!first) ss << ", ";
                    first = false;
                    ss << pair.first << "=" << pair.second;
                }
                ss << "}";
            }

            return ss.str();
    }
}

std::string internal::getColorCode(LogLevel level, const ColorConfig& config) {
    if (!config.useColor) return "";

    switch (level) {
        case LogLevel::Debug: return config.debugColor;
        case LogLevel::Info: return config.infoColor;
        case LogLevel::Warning: return config.warningColor;
        case LogLevel::Error: return config.errorColor;
        case LogLevel::Critical: return config.criticalColor;
        default: return "";
    }
}

// AsyncLogger implementation
struct AsyncLogger::Impl {
    std::shared_ptr<internal::AsyncLoggerWorker> worker;
    std::mutex mutex;
};

AsyncLogger::AsyncLogger(size_t poolSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->worker = std::make_shared<internal::AsyncLoggerWorker>(poolSize);
}

AsyncLogger::~AsyncLogger() = default;

void AsyncLogger::log(const LogEntry& entry) {
    pImpl->worker->log(entry);
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->worker->setLevel(level);
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->worker->getLevel();
}

void AsyncLogger::setFilter(const LogFilter& filter) {
    pImpl->worker->setFilter(filter);
}

LogFilter AsyncLogger::getFilter() const {
    return pImpl->worker->getFilter();
}

void AsyncLogger::setFormat(LogFormat format) {
    pImpl->worker->setFormat(format);
}

LogFormat AsyncLogger::getFormat() const {
    return pImpl->worker->getFormat();
}

void AsyncLogger::setOutput(LogOutput output) {
    pImpl->worker->setOutput(output);
}

LogOutput AsyncLogger::getOutput() const {
    return pImpl->worker->getOutput();
}

void AsyncLogger::setFilepath(const std::string& filepath) {
    pImpl->worker->setFilepath(filepath);
}

std::string AsyncLogger::getFilepath() const {
    return pImpl->worker->getFilepath();
}

void AsyncLogger::flush() {
    // In a real implementation, you would need a way to flush the async queue
    // This is a simplified version
}

// BatchLogger implementation
struct BatchLogger::Impl {
    std::vector<LogEntry> batch;
    std::mutex mutex;
    std::chrono::milliseconds flushInterval;
    std::chrono::time_point<std::chrono::steady_clock> lastFlush;
    size_t maxBatchSize;
    std::shared_ptr<ConsoleLogger> consoleLogger;

    Impl(size_t maxBatchSize, std::chrono::milliseconds flushInterval) 
        : maxBatchSize(maxBatchSize), flushInterval(flushInterval), 
          lastFlush(std::chrono::steady_clock::now()) {
        consoleLogger = std::make_shared<ConsoleLogger>();
    }
};

BatchLogger::BatchLogger(size_t batchSize, std::chrono::milliseconds flushInterval)
    : pImpl(std::make_unique<Impl>(batchSize, flushInterval)) {
    // Set up periodic flushing
    auto self = shared_from_this();
    flushTimer = std::thread([this]() {
        while (!stopped) {
            std::this_thread::sleep_for(pImpl->flushInterval);
            flush();
        }
    });
}

BatchLogger::~BatchLogger() {
    stopped = true;
    flushTimer.join();
    flush();
}

void BatchLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->batch.push_back(entry);

    if (pImpl->batch.size() >= pImpl->maxBatchSize) {
        flush();
    }
}

void BatchLogger::setLevel(LogLevel level) {
    pImpl->consoleLogger->setLevel(level);
}

LogLevel BatchLogger::getLevel() const {
    return pImpl->consoleLogger->getLevel();
}

void BatchLogger::setFilter(const LogFilter& filter) {
    pImpl->consoleLogger->setFilter(filter);
}

LogFilter BatchLogger::getFilter() const {
    return pImpl->consoleLogger->getFilter();
}

void BatchLogger::setFormat(LogFormat format) {
    pImpl->consoleLogger->setFormat(format);
}

LogFormat BatchLogger::getFormat() const {
    return pImpl->consoleLogger->getFormat();
}

void BatchLogger::setOutput(LogOutput output) {
    pImpl->consoleLogger->setOutput(output);
}

LogOutput BatchLogger::getOutput() const {
    return pImpl->consoleLogger->getOutput();
}

void BatchLogger::setFilepath(const std::string& filepath) {
    pImpl->consoleLogger->setFilepath(filepath);
}

std::string BatchLogger::getFilepath() const {
    return pImpl->consoleLogger->getFilepath();
}

void BatchLogger::flush() {
    if (pImpl->batch.empty()) {
        return;
    }

    std::vector<LogEntry> currentBatch;
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        currentBatch = std::move(pImpl->batch);
    }

    for (const auto& entry : currentBatch) {
        pImpl->consoleLogger->log(entry);
    }
}

// Main Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<ConsoleLogger> globalLogger;
    LogLevel globalLevel;
    LogFormat globalFormat;
    LogFilter globalFilter;
    LogOutput globalOutput;
    std::string globalFilepath;
    bool stopped;
    std::thread flushTimer;

    Impl() : stopped(false), globalLevel(LogLevel::Info), globalFormat(LogFormat::TEXT),
             globalOutput(LogOutput::CONSOLE) {
        globalLogger = std::make_shared<ConsoleLogger>();
        globalLogger->setLevel(globalLevel);
        globalLogger->setFormat(globalFormat);

        // Set up periodic flushing
        auto self = shared_from_this();
        flushTimer = std::thread([this]() {
            while (!stopped) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                flush();
            }
        });
    }

    ~Impl() {
        stopped = true;
        if (flushTimer.joinable()) {
            flushTimer.join();
        }
    }

    void flush() {
        for (auto& logger : loggers) {
            if (auto batchLogger = std::dynamic_pointer_cast<BatchLogger>(logger)) {
                batchLogger->flush();
            }
        }
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->globalLogger->setLevel(pImpl->globalLevel);
    pImpl->globalLogger->setFormat(pImpl->globalFormat);
    pImpl->globalLogger->setOutput(pImpl->globalOutput);
    pImpl->globalLogger->setFilepath(pImpl->globalFilepath);
    pImpl->globalLogger->setFilter(pImpl->globalFilter);

    // Add global logger to the list
    pImpl->loggers.push_back(pImpl->globalLogger);
}

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
    pImpl->globalLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line,
                const std::string& function, const std::string& category) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.threadId = internal::getCurrentThreadId();
    entry.timestamp = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function, const std::string& category,
                             const std::unordered_map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.threadId = internal::getCurrentThreadId();
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;

    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
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

void Logger::setGlobalLevel(LogLevel level) {
    pImpl->globalLevel = level;
    pImpl->globalLogger->setLevel(level);
}

void Logger::setGlobalFormat(LogFormat format) {
    pImpl->globalFormat = format;
    pImpl->globalLogger->setFormat(format);
}

void Logger::setGlobalFilter(const LogFilter& filter) {
    pImpl->globalFilter = filter;
    pImpl->globalLogger->setFilter(filter);
}

void Logger::setGlobalOutput(LogOutput output) {
    pImpl->globalOutput = output;
    pImpl->globalLogger->setOutput(output);
}

void Logger::setGlobalFilepath(const std::string& filepath) {
    pImpl->globalFilepath = filepath;
    pImpl->globalLogger->setFilepath(filepath);
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line,
                           const std::string& function, const std::string& category)
    : level_(level), file_(file), line_(line), function_(function), category_(category) {
}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_);
}

} // namespace nlm