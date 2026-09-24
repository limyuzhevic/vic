#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <map>
#include <algorithm>
#include <filesystem>
#include <thread>

namespace nlm {

// LogEntry static members
int LogEntry::totalEntries = 0;
int LogEntry::entriesByLevel[8] = {0};
std::map<LogDomain, int> LogEntry::entriesByDomain;

std::string LogEntry::getFormattedTimestamp() const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
    return oss.str();
}

std::string LogEntry::getLevelString() const {
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

std::string LogEntry::getDomainString() const {
    switch (domain) {
        case LogDomain::Core: return "CORE";
        case LogDomain::Random: return "RANDOM";
        case LogDomain::Simulation: return "SIMULATION";
        case LogDomain::Brain: return "BRAIN";
        case LogDomain::Neuron: return "NEURON";
        case LogDomain::Synapse: return "SYNAPSE";
        case LogDomain::Plasticity: return "PLASTICITY";
        case LogDomain::Sensory: return "SENSORY";
        case LogDomain::Motor: return "MOTOR";
        case LogDomain::Environment: return "ENVIRONMENT";
        case LogDomain::Agent: return "AGENT";
        case LogDomain::Development: return "DEVELOPMENT";
        case LogDomain::Memory: return "MEMORY";
        case LogDomain::Prediction: return "PREDICTION";
        case LogDomain::Cognition: return "COGNITION";
        case LogDomain::Neuromodulation: return "NEUROMODULATION";
        case LogDomain::Experiments: return "EXPERIMENTS";
        case LogDomain::Performance: return "PERFORMANCE";
        case LogDomain::Error: return "ERROR";
        case LogDomain::System: return "SYSTEM";
        case LogDomain::All: return "ALL";
        default: return "UNKNOWN";
    }
}

std::string LogEntry::toJSON() const {
    json j;
    j["level"] = getLevelString();
    j["message"] = message;
    j["file"] = file;
    j["line"] = line;
    j["function"] = function;
    j["timestamp"] = getFormattedTimestamp();
    j["domain"] = getDomainString();
    
    if (!experimentId.empty()) j["experiment_id"] = experimentId;
    if (simulationStep > 0) j["simulation_step"] = simulationStep;
    if (simulationTime > 0.0) j["simulation_time"] = simulationTime;
    if (!threadId.empty()) j["thread_id"] = threadId;
    
    if (!context.empty()) {
        j["context"] = context;
    }
    
    j["processing_time_us"] = processingTime.count();
    j["memory_usage"] = memoryUsage;
    
    return j.dump(2);
}

std::string LogEntry::toCSV() const {
    std::ostringstream oss;
    oss << "\"" << getLevelString() << "\""."""";
    oss << "\"" << message << "\""."""";
    oss << "\"" << file << "\""."""";
    oss << line << "."""";
    oss << "\"" << function << "\""."""";
    oss << "\"" << getFormattedTimestamp() << "\""."""";
    oss << "\"" << getDomainString() << "\""."""";
    
    if (!experimentId.empty()) oss << "\"" << experimentId << "\""."""";
    else oss << """."""";
    
    oss << simulationStep << "."""";
    oss << simulationTime << "."""";
    oss << "\"" << threadId << "\""."""";
    
    // Add context fields
    bool first = true;
    for (const auto& pair : context) {
        if (!first) oss << ";";
        oss << "\"" << pair.first << "\""."""" \"" << pair.second << "\"";
        first = false;
    }
    
    oss << processingTime.count() << "."""";
    oss << memoryUsage << "."""";
    
    return oss.str();
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t outputCount = 0;
    std::chrono::system_clock::time_point startTime;
    
    Impl() : startTime(std::chrono::system_clock::now()) {}
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
    oss << "[" << entry.getLevelString() << "] ";
    
    oss << "[" << entry.getDomainString() << "] ";
    
    oss << entry.getFormattedTimestamp() << " ";
    
    oss << entry.message;
    
    if (!entry.experimentId.empty()) {
        oss << " [exp:" << entry.experimentId << "]";
    }
    
    if (entry.simulationStep > 0) {
        oss << " [step:" << entry.simulationStep << "]";
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
    
    pImpl->outputCount++;
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
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[32m";      // Green
        case LogLevel::Info: return "\033[34m";      // Blue
        case LogLevel::Notice: return "\033[37m";   // White
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";    // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<LogFilter> globalFilter;
    std::shared_ptr<LogAggregator> globalAggregator;
    bool useColors = true;
    std::string outputFile;
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

void Logger::setGlobalFilter(std::shared_ptr<LogFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
}

std::shared_ptr<LogFilter> Logger::getGlobalFilter() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalFilter;
}

void Logger::setGlobalAggregator(std::shared_ptr<LogAggregator> aggregator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalAggregator = aggregator;
}

std::shared_ptr<LogAggregator> Logger::getGlobalAggregator() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalAggregator;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = context;
    
    // Apply global filter if present
    if (pImpl->globalFilter && !pImpl->globalFilter->passesFilter(entry)) {
        return;
    }
    
    // Process through all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update global aggregator if present
    if (pImpl->globalAggregator) {
        pImpl->globalAggregator->processEntry(entry);
    }
    
    // Update static statistics
    LogEntry::totalEntries++;
    int levelIndex = static_cast<int>(level);
    if (levelIndex >= 0 && levelIndex < 8) {
        LogEntry::entriesByLevel[levelIndex]++;
    }
    LogEntry::entriesByDomain[entry.domain]++;
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::map<std::string, std::string>& context) {
    log(LogLevel::Debug, message, file, line, function, context);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::map<std::string, std::string>& context) {
    log(LogLevel::Info, message, file, line, function, context);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::map<std::string, std::string>& context) {
    log(LogLevel::Warning, message, file, line, function, context);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::map<std::string, std::string>& context) {
    log(LogLevel::Error, message, file, line, function, context);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::map<std::string, std::string>& context) {
    log(LogLevel::Critical, message, file, line, function, context);
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

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(getGlobal().pImpl->mutex);
    for (auto& logger : getGlobal().pImpl->loggers) {
        if (auto asyncLogger = std::dynamic_pointer_cast<AsyncLogger>(logger)) {
            asyncLogger->flush();
        }
    }
}

std::string Logger::generateReport() {
    if (auto aggregator = getGlobalAggregator()) {
        return aggregator->generateJSONReport();
    }
    return "No global aggregator available.";
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line,
                          const std::string& function, const std::map<std::string, std::string>& context)
    : level_(level), file_(file), line_(line), function_(function), context_(context) {}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, context_);
}

void Logger::LogStream::setContext(const std::string& key, const std::string& value) {
    context_[key] = value;
}

} // namespace nlm
