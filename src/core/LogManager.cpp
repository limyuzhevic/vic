#include "LogManager.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

struct LogManager::Impl {
    std::map<std::string, std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool initialized = false;
};

LogManager::LogManager() : pImpl(std::make_unique<Impl>()) {}

LogManager::~LogManager() = default;

LogManager& LogManager::getInstance() {
    static LogManager instance;
    return instance;
}

void LogManager::addLogger(const std::string& name, std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers[name] = logger;
}

std::shared_ptr<ILogger> LogManager::getLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->loggers.find(name);
    return (it != pImpl->loggers.end()) ? it->second : nullptr;
}

void LogManager::removeLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(name);
}

std::vector<std::string> LogManager::getLoggerNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->loggers.size());
    for (const auto& pair : pImpl->loggers) {
        names.push_back(pair.first);
    }
    return names;
}

void LogManager::setLoggerLevel(const std::string& name, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->loggers.find(name);
    if (it != pImpl->loggers.end()) {
        it->second->setLevel(level);
    }
}

LogLevel LogManager::getLoggerLevel(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->loggers.find(name);
    if (it != pImpl->loggers.end()) {
        return it->second->getLevel();
    }
    return LogLevel::Info; // Default level
}

void LogManager::logConfigChange(const std::string& key, const std::string& oldValue, const std::string& newValue) {
    std::ostringstream oss;
    oss << "Configuration changed: " << key;
    if (!oldValue.empty()) {
        oss << " (" << oldValue << " -> " << newValue << ")";
    } else {
        oss << " set to: " << newValue;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second) {
            LogEntry entry;
            entry.level = LogLevel::Info;
            entry.message = oss.str();
            entry.file = "";
            entry.line = 0;
            entry.function = "LogManager::logConfigChange";
            entry.timestamp = std::chrono::system_clock::now();
            pair.second->log(entry);
        }
    }
}

void LogManager::logNeuralOperation(const std::string& operation, int neuronId, float value, bool success) {
    std::ostringstream oss;
    oss << "Neural " << operation << " - Neuron ID: " << neuronId << ", Value: " << value;
    if (success) {
        oss << ", Status: SUCCESS";
    } else {
        oss << ", Status: FAILED";
    }
    
    LogLevel level = success ? LogLevel::Info : LogLevel::Warning;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second) {
            LogEntry entry;
            entry.level = level;
            entry.message = oss.str();
            entry.file = "";
            entry.line = 0;
            entry.function = "LogManager::logNeuralOperation";
            entry.timestamp = std::chrono::system_clock::now();
            pair.second->log(entry);
        }
    }
}

void LogManager::logPerformance(const std::string& metric, double value, const std::string& unit) {
    std::ostringstream oss;
    oss << "Performance: " << metric << " = " << std::fixed << std::setprecision(6) << value << " " << unit;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second) {
            LogEntry entry;
            entry.level = LogLevel::Debug;
            entry.message = oss.str();
            entry.file = "";
            entry.line = 0;
            entry.function = "LogManager::logPerformance";
            entry.timestamp = std::chrono::system_clock::now();
            pair.second->log(entry);
        }
    }
}

void LogManager::logMemoryUsage(size_t bytes, size_t peakBytes) {
    std::ostringstream oss;
    oss << "Memory Usage: " << bytes << " bytes (Peak: " << peakBytes << " bytes)";
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second) {
            LogEntry entry;
            entry.level = LogLevel::Debug;
            entry.message = oss.str();
            entry.file = "";
            entry.line = 0;
            entry.function = "LogManager::logMemoryUsage";
            entry.timestamp = std::chrono::system_clock::now();
            pair.second->log(entry);
        }
    }
}

void LogManager::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second && dynamic_cast<ConsoleLogger*>(pair.second.get())) {
            // Type-specific flush for ConsoleLogger
            static_cast<ConsoleLogger*>(pair.second.get())->closeOutputFile();
        }
    }
}

void LogManager::closeAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second && dynamic_cast<ConsoleLogger*>(pair.second.get())) {
            static_cast<ConsoleLogger*>(pair.second.get())->closeOutputFile();
        }
    }
    pImpl->loggers.clear();
}

bool LogManager::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second && pair.second->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

void LogManager::clearAllLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

} // namespace nlm