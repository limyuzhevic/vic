#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>
#include <ctime>

namespace nlm {

// Timer implementation
struct Timer::Impl {
    std::string name;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point pauseTime;
    double pausedDuration = 0.0;
    bool isPaused = false;
    LogLevel level;
    std::string correlationId;
    std::chrono::system_clock::time_point lastCheckpoint;
    double totalPaused = 0.0;
};

Timer::Timer(const std::string& name, LogLevel level, const std::string& correlationId)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->level = level;
    pImpl->correlationId = correlationId;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->lastCheckpoint = pImpl->startTime;
}

Timer::~Timer() {
    stop();
}

void Timer::stop() {
    if (!pImpl->isPaused) {
        auto now = std::chrono::system_clock::now();
        pImpl->pausedDuration += pImpl->totalPaused;
        pImpl->totalPaused = 0.0;
    }
}

void Timer::pause() {
    if (!pImpl->isPaused) {
        pImpl->isPaused = true;
        pImpl->pauseTime = std::chrono::system_clock::now();
    }
}

void Timer::resume() {
    if (pImpl->isPaused) {
        pImpl->isPaused = false;
        auto now = std::chrono::system_clock::now();
        pImpl->totalPaused += std::chrono::duration<double>(now - pImpl->pauseTime).count() * 1000.0; // Convert to ms
    }
}

double Timer::getDurationMs() const {
    auto now = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double>(now - pImpl->startTime).count() * 1000.0; // Convert to ms
    if (pImpl->isPaused) {
        duration -= std::chrono::duration<double>(now - pImpl->pauseTime).count() * 1000.0;
    }
    duration -= pImpl->pausedDuration;
    return duration;
}

const std::string& Timer::getName() const {
    return pImpl->name;
}

AdvancedLogEntry Timer::toLogEntry() const {
    AdvancedLogEntry entry;
    entry.level = pImpl->level;
    entry.message = "Timer: " + pImpl->name + " took " + std::to_string(getDurationMs()) + "ms";
    if (!pImpl->correlationId.empty()) {
        entry.correlationId = pImpl->correlationId;
    }
    entry.durationMs = getDurationMs();
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.timestamp = std::chrono::system_clock::now();
    return entry;
}

// EventTracer implementation
struct EventTracer::Impl {
    std::string name;
    std::vector<std::pair<std::string, std::string>> events;
    std::vector<std::string> checkpoints;
    std::unordered_map<std::string, std::string> context;
    std::string correlationId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastCheckpoint;
};

EventTracer::EventTracer(const std::string& name, const std::string& correlationId)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->correlationId = correlationId;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->lastCheckpoint = pImpl->startTime;
}

EventTracer::~EventTracer() = default;

void EventTracer::addEvent(const std::string& eventName, const std::string& details) {
    auto now = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double>(now - pImpl->lastCheckpoint).count() * 1000.0;
    
    std::ostringstream oss;
    oss << "[" << std::fixed << std::setprecision(3) << duration << "ms] " << eventName;
    if (!details.empty()) {
        oss << ": " << details;
    }
    
    pImpl->events.emplace_back(eventName, oss.str());
    pImpl->lastCheckpoint = now;
}

void EventTracer::addCheckpoint(const std::string& checkpointName) {
    pImpl->checkpoints.push_back(checkpointName);
    auto now = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double>(now - pImpl->startTime).count() * 1000.0;
    
    std::ostringstream oss;
    oss << "Checkpoint: " << checkpointName << " (" << duration << "ms total)";
    
    pImpl->events.emplace_back(checkpointName, oss.str());
}

void EventTracer::setContext(const std::string& key, const std::string& value) {
    pImpl->context[key] = value;
}

std::string EventTracer::getCorrelationId() const {
    return pImpl->correlationId;
}

std::vector<AdvancedLogEntry> EventTracer::toLogEntries() const {
    std::vector<AdvancedLogEntry> entries;
    
    // Add context as structured data
    AdvancedLogEntry contextEntry;
    contextEntry.level = LogLevel::Info;
    contextEntry.message = "Event trace: " + pImpl->name;
    contextEntry.correlationId = pImpl->correlationId;
    contextEntry.structuredData = pImpl->context;
    contextEntry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    contextEntry.timestamp = pImpl->startTime;
    entries.push_back(contextEntry);
    
    // Add events
    for (const auto& event : pImpl->events) {
        AdvancedLogEntry eventEntry;
        eventEntry.level = LogLevel::Info;
        eventEntry.message = event.second;
        eventEntry.correlationId = pImpl->correlationId;
        eventEntry.structuredData["event_name"] = event.first;
        eventEntry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        eventEntry.timestamp = std::chrono::system_clock::now();
        entries.push_back(eventEntry);
    }
    
    return entries;
}

// MemoryProfiler implementation
struct MemoryProfiler::Impl {
    std::string name;
    std::vector<std::pair<size_t, std::string>> allocations;
    std::vector<std::pair<size_t, std::string>> deallocations;
    std::unordered_map<std::string, size_t> objectCounts;
    std::chrono::system_clock::time_point startTime;
    std::string correlationId;
    size_t peakMemory = 0;
    size_t currentMemory = 0;
};

MemoryProfiler::MemoryProfiler(const std::string& name, const std::string& correlationId)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->correlationId = correlationId;
    pImpl->startTime = std::chrono::system_clock::now();
}

MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::trackAllocation(size_t size, const std::string& allocationSite) {
    pImpl->allocations.emplace_back(size, allocationSite);
    pImpl->currentMemory += size;
    if (pImpl->currentMemory > pImpl->peakMemory) {
        pImpl->peakMemory = pImpl->currentMemory;
    }
}

void MemoryProfiler::trackDeallocation(size_t size) {
    pImpl->deallocations.emplace_back(size, "");
    if (pImpl->currentMemory >= size) {
        pImpl->currentMemory -= size;
    }
}

void MemoryProfiler::trackObjectCreation(const std::string& typeName, size_t count) {
    pImpl->objectCounts[typeName] += count;
}

void MemoryProfiler::trackObjectDestruction(const std::string& typeName, size_t count) {
    if (pImpl->objectCounts.count(typeName)) {
        pImpl->objectCounts[typeName] = std::max(0LL, static_cast<long long>(pImpl->objectCounts[typeName]) - count);
    }
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    return pImpl->currentMemory;
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    return pImpl->peakMemory;
}

size_t MemoryProfiler::getTotalAllocations() const {
    size_t total = 0;
    for (const auto& alloc : pImpl->allocations) {
        total += alloc.first;
    }
    return total;
}

size_t MemoryProfiler::getTotalDeallocations() const {
    size_t total = 0;
    for (const auto& dealloc : pImpl->deallocations) {
        total += dealloc.first;
    }
    return total;
}

std::vector<AdvancedLogEntry> MemoryProfiler::toLogEntries() const {
    std::vector<AdvancedLogEntry> entries;
    
    // Add memory profile summary
    AdvancedLogEntry profileEntry;
    profileEntry.level = LogLevel::Info;
    profileEntry.message = "Memory profile: " + pImpl->name;
    profileEntry.correlationId = pImpl->correlationId;
    profileEntry.memoryUsage = pImpl->peakMemory;
    profileEntry.structuredData["current_memory"] = std::to_string(pImpl->currentMemory);
    profileEntry.structuredData["peak_memory"] = std::to_string(pImpl->peakMemory);
    profileEntry.structuredData["total_allocations"] = std::to_string(getTotalAllocations());
    profileEntry.structuredData["total_deallocations"] = std::to_string(getTotalDeallocations());
    
    // Add object counts
    std::ostringstream oss;
    bool first = true;
    for (const auto& [type, count] : pImpl->objectCounts) {
        if (!first) oss << ", ";
        oss << type << ": " << count;
        first = false;
    }
    profileEntry.structuredData["object_types"] = oss.str();
    
    profileEntry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    profileEntry.timestamp = pImpl->startTime;
    entries.push_back(profileEntry);
    
    return entries;
}

// ConsoleLogger implementation
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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<IAdvancedLogger>> advancedLoggers;
    std::vector<std::shared_ptr<ILogger>> legacyLoggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<IAdvancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->advancedLoggers.push_back(logger);
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->legacyLoggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->advancedLoggers.clear();
    pImpl->legacyLoggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->advancedLoggers) {
        logger->setLevel(level);
    }
    for (auto& logger : pImpl->legacyLoggers) {
        logger->setLevel(level);
    }
}

void Logger::logAdvanced(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& correlationId,
                         const std::unordered_map<std::string, std::string>& structuredData) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    AdvancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.correlationId = correlationId;
    entry.structuredData = structuredData;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->advancedLoggers) {
        if (logger->shouldLog(entry)) {
            logger->log(entry);
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
    
    for (auto& logger : pImpl->legacyLoggers) {
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
    for (auto& logger : pImpl->legacyLoggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

static std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

// AdvancedLogger implementation
struct AdvancedLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::function<bool(const AdvancedLogEntry&)> filter;
};

AdvancedLogger::AdvancedLogger() : pImpl(std::make_unique<Impl>()) {}

AdvancedLogger::~AdvancedLogger() = default;

void AdvancedLogger::log(const AdvancedLogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::cout << formatConsole(entry) << std::endl;
}

void AdvancedLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AdvancedLogger::getLevel() const {
    return pImpl->level;
}

std::string AdvancedLogger::format(const AdvancedLogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(entry.timestamp)), "%Y-%m-%d %H:%M:%S");
    oss << " " << entry.message;
    return oss.str();
}

std::string AdvancedLogger::formatJson(const AdvancedLogEntry& entry) const {
    nlohmann::json json;
    json["level"] = levelToString(entry.level);
    json["message"] = entry.message;
    json["file"] = entry.file;
    json["line"] = entry.line;
    json["function"] = entry.function;
    json["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
    json["correlation_id"] = entry.correlationId;
    json["duration_ms"] = entry.durationMs;
    json["memory_usage"] = entry.memoryUsage;
    json["thread_id"] = entry.threadId;
    
    for (const auto& [key, value] : entry.structuredData) {
        json["structured_data"][key] = value;
    }
    
    return json.dump();
}

std::string AdvancedLogger::formatConsole(const AdvancedLogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(entry.timestamp)), "%Y-%m-%d %H:%M:%S");
    oss << " " << entry.message;
    return oss.str();
}

bool AdvancedLogger::shouldLog(const AdvancedLogEntry& entry) const {
    if (entry.level < pImpl->level) {
        return false;
    }
    
    if (pImpl->filter) {
        return pImpl->filter(entry);
    }
    
    return true;
}

void AdvancedLogger::setFilter(std::function<bool(const AdvancedLogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void AdvancedLogger::logAdvanced(const AdvancedLogEntry& entry) {
    log(entry);
}

void AdvancedLogger::logTimer(const Timer& timer) {
    log(timer.toLogEntry());
}

void AdvancedLogger::logEventTrace(const EventTracer& tracer) {
    for (const auto& entry : tracer.toLogEntries()) {
        log(entry);
    }
}

void AdvancedLogger::logMemoryProfile(const MemoryProfiler& profiler) {
    for (const auto& entry : profiler.toLogEntries()) {
        log(entry);
    }
}

// Global logger convenience methods
void Logger::logTimer(const Timer& timer) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->advancedLoggers) {
        if (auto advancedLogger = std::dynamic_pointer_cast<AdvancedLogger>(logger)) {
            advancedLogger->logTimer(timer);
        }
    }
}

void Logger::logEventTrace(const EventTracer& tracer) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->advancedLoggers) {
        if (auto advancedLogger = std::dynamic_pointer_cast<AdvancedLogger>(logger)) {
            advancedLogger->logEventTrace(tracer);
        }
    }
}

void Logger::logMemoryProfile(const MemoryProfiler& profiler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->advancedLoggers) {
        if (auto advancedLogger = std::dynamic_pointer_cast<AdvancedLogger>(logger)) {
            advancedLogger->logMemoryProfile(profiler);
        }
    }
}

void Logger::logPerformance(const std::string& operation, double durationMs,
                            const std::string& correlationId) {
    AdvancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Performance: " + operation + " took " + std::to_string(durationMs) + "ms";
    entry.correlationId = correlationId;
    entry.durationMs = durationMs;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.timestamp = std::chrono::system_clock::now();
    
    logAdvanced(entry.level, entry.message, "", 0, "", entry.correlationId, entry.structuredData);
}

void Logger::logMemoryAllocation(size_t size, const std::string& allocationSite,
                                const std::string& correlationId) {
    AdvancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Memory allocation: " + std::to_string(size) + " bytes";
    entry.correlationId = correlationId;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.timestamp = std::chrono::system_clock::now();
    entry.structuredData["size"] = std::to_string(size);
    entry.structuredData["allocation_site"] = allocationSite;
    
    logAdvanced(entry.level, entry.message, "", 0, "", entry.correlationId, entry.structuredData);
}

void Logger::logEvent(const std::string& eventName, const std::string& details,
                      const std::string& correlationId) {
    AdvancedLogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Event: " + eventName + (details.empty() ? "" : (" - " + details));
    entry.correlationId = correlationId;
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.timestamp = std::chrono::system_clock::now();
    entry.structuredData["event_name"] = eventName;
    
    logAdvanced(entry.level, entry.message, "", 0, "", entry.correlationId, entry.structuredData);
}

EventTracer Logger::traceEvent(const std::string& name, const std::string& correlationId) {
    return EventTracer(name, correlationId);
}

MemoryProfiler Logger::profileMemory(const std::string& name, const std::string& correlationId) {
    return MemoryProfiler(name, correlationId);
}

} // namespace nlm