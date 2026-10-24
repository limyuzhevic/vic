#include "AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <map>

namespace nlm {

struct AdvancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance monitoring
    std::unordered_map<std::string, PerformanceMetrics> performanceMetrics;
    std::mutex perfMutex;
    
    // Timeline events
    std::unordered_map<std::string, TimelineEvent> timelineEvents;
    std::mutex timelineMutex;
    
    // Memory tracking
    std::vector<std::pair<size_t, std::string>> memoryUsageHistory;
    std::mutex memoryMutex;
    
    // Statistics
    std::unordered_map<std::string, size_t> operationCallCounts;
    std::unordered_map<std::string, double> operationTotalTimes;
};

AdvancedConsoleLogger::AdvancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

AdvancedConsoleLogger::AdvancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

AdvancedConsoleLogger::~AdvancedConsoleLogger() = default;

void AdvancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level && entry.level != LogLevel::Profile && 
        entry.level != LogLevel::Timeline) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    if (entry.elapsedTime > 0) {
        oss << " (+" << std::fixed << std::setprecision(2) << entry.elapsedTime << "ms)";
    }
    
    oss << " " << entry.message;
    
    if (!entry.category.empty()) {
        oss << " [" << entry.category << "]";
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
}

void AdvancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AdvancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void AdvancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool AdvancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void AdvancedConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void AdvancedConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void AdvancedConsoleLogger::startPerformanceMeasurement(const std::string& operationName,
                                                         const std::string& category,
                                                         const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    auto it = pImpl->performanceMetrics.find(operationName);
    if (it != pImpl->performanceMetrics.end()) {
        it->second.startTime = std::chrono::system_clock::now();
    } else {
        PerformanceMetrics metric;
        metric.operationName = operationName;
        metric.category = category;
        metric.memoryUsed = 0; // Could be enhanced with memory profiling
        metric.startTime = std::chrono::system_clock::now();
        metric.callsCount = 0;
        metric.minTime = std::numeric_limits<double>::max();
        metric.maxTime = 0.0;
        metric.averageTime = 0.0;
        
        for (const auto& meta : metadata) {
            metric.metadata[meta.first] = meta.second;
        }
        
        pImpl->performanceMetrics[operationName] = metric;
    }
}

PerformanceMetrics AdvancedConsoleLogger::stopPerformanceMeasurement(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    auto it = pImpl->performanceMetrics.find(operationName);
    if (it == pImpl->performanceMetrics.end()) {
        return PerformanceMetrics{}; // Return empty metric if not found
    }
    
    auto& metric = it->second;
    metric.endTime = std::chrono::system_clock::now();
    
    auto duration = std::chrono::duration<double, std::milli>(metric.endTime - metric.startTime).count();
    metric.elapsedTime = duration;
    
    // Update statistics
    metric.callsCount++;
    metric.minTime = std::min(metric.minTime, duration);
    metric.maxTime = std::max(metric.maxTime, duration);
    metric.averageTime = (metric.averageTime * (metric.callsCount - 1) + duration) / metric.callsCount;
    
    // Update global statistics
    pImpl->operationCallCounts[operationName] = metric.callsCount;
    pImpl->operationTotalTimes[operationName] = metric.totalTime = duration;
    
    return metric;
}

void AdvancedConsoleLogger::recordPerformanceMetric(const PerformanceMetrics& metric) {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    pImpl->performanceMetrics[metric.operationName] = metric;
    
    // Update global statistics
    pImpl->operationCallCounts[metric.operationName] = metric.callsCount;
    pImpl->operationTotalTimes[metric.operationName] = metric.totalTime;
}

std::vector<PerformanceMetrics> AdvancedConsoleLogger::getPerformanceMetrics(const std::string& category,
                                                                             const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    std::vector<PerformanceMetrics> result;
    
    for (const auto& pair : pImpl->performanceMetrics) {
        const auto& metric = pair.second;
        
        if (!category.empty() && metric.category != category) {
            continue;
        }
        
        if (!operationName.empty() && metric.operationName != operationName) {
            continue;
        }
        
        result.push_back(metric);
    }
    
    // Sort by elapsed time (descending)
    std::sort(result.begin(), result.end(),
              [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
                  return a.elapsedTime > b.elapsedTime;
              });
    
    return result;
}

void AdvancedConsoleLogger::clearPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    pImpl->performanceMetrics.clear();
    pImpl->operationCallCounts.clear();
    pImpl->operationTotalTimes.clear();
}

std::string AdvancedConsoleLogger::startTimelineEvent(const std::string& eventName,
                                                      const std::string& category,
                                                      const std::string& parentEventId,
                                                      const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    
    std::string eventId = eventName + "_" + 
                         std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    TimelineEvent event;
    event.eventId = eventId;
    event.eventName = eventName;
    event.category = category;
    event.parentEventId = parentEventId;
    event.timestamp = std::chrono::system_clock::now();
    event.duration = 0.0; // Will be updated when stopped
    event.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.metadata = metadata;
    
    pImpl->timelineEvents[eventId] = event;
    
    return eventId;
}

void AdvancedConsoleLogger::stopTimelineEvent(const std::string& eventId) {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    
    auto it = pImpl->timelineEvents.find(eventId);
    if (it != pImpl->timelineEvents.end()) {
        auto& event = it->second;
        event.endTime = std::chrono::system_clock::now();
        event.duration = std::chrono::duration<double, std::milli>(event.endTime - event.timestamp).count();
        
        // Link to parent event if exists
        if (!event.parentEventId.empty()) {
            auto parentIt = pImpl->timelineEvents.find(event.parentEventId);
            if (parentIt != pImpl->timelineEvents.end()) {
                parentIt->second.childEventIds.push_back(eventId);
            }
        }
    }
}

TimelineEvent AdvancedConsoleLogger::getTimelineEvent(const std::string& eventId) const {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    
    auto it = pImpl->timelineEvents.find(eventId);
    if (it != pImpl->timelineEvents.end()) {
        return it->second;
    }
    
    return TimelineEvent{}; // Return empty event if not found
}

std::vector<TimelineEvent> AdvancedConsoleLogger::getTimelineEvents(const std::string& category,
                                                                    const std::string& parentEventId) const {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    
    std::vector<TimelineEvent> result;
    
    for (const auto& pair : pImpl->timelineEvents) {
        const auto& event = pair.second;
        
        if (!category.empty() && event.category != category) {
            continue;
        }
        
        if (!parentEventId.empty() && event.parentEventId != parentEventId) {
            continue;
        }
        
        result.push_back(event);
    }
    
    return result;
}

std::vector<TimelineEvent> AdvancedConsoleLogger::getTimelinePath(const std::string& eventId) const {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    
    std::vector<TimelineEvent> path;
    
    // Collect all events in the path from root to specified event
    std::unordered_map<std::string, bool> visited;
    std::function<void(const std::string&)> collectPath;
    
    collectPath = [&](const std::string& currentId) {
        auto it = pImpl->timelineEvents.find(currentId);
        if (it == pImpl->timelineEvents.end()) {
            return;
        }
        
        const auto& event = it->second;
        
        if (visited.find(currentId) != visited.end()) {
            return;
        }
        
        visited[currentId] = true;
        
        // Recursively process parent
        if (!event.parentEventId.empty()) {
            collectPath(event.parentEventId);
        }
        
        path.push_back(event);
    };
    
    collectPath(eventId);
    
    // Reverse to get from root to leaf
    std::reverse(path.begin(), path.end());
    
    return path;
}

void AdvancedConsoleLogger::clearTimelineEvents() {
    std::lock_guard<std::mutex> lock(pImpl->timelineMutex);
    pImpl->timelineEvents.clear();
}

void AdvancedConsoleLogger::recordMemoryUsage(size_t memoryBytes, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    memoryUsageHistory.emplace_back(memoryBytes, context);
    
    // Keep only last 1000 entries to prevent memory issues
    if (memoryUsageHistory.size() > 1000) {
        memoryUsageHistory.erase(memoryUsageHistory.begin());
    }
}

std::vector<std::pair<size_t, std::string>> AdvancedConsoleLogger::getMemoryUsageHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return memoryUsageHistory;
}

double AdvancedConsoleLogger::getAverageExecutionTime(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    auto it = pImpl->performanceMetrics.find(operationName);
    if (it != pImpl->performanceMetrics.end()) {
        return it->second.averageTime;
    }
    
    return 0.0;
}

size_t AdvancedConsoleLogger::getCallCount(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    auto it = pImpl->performanceMetrics.find(operationName);
    if (it != pImpl->performanceMetrics.end()) {
        return it->second.callsCount;
    }
    
    auto callIt = pImpl->operationCallCounts.find(operationName);
    if (callIt != pImpl->operationCallCounts.end()) {
        return callIt->second;
    }
    
    return 0;
}

std::string AdvancedConsoleLogger::getMostUsedOperation() const {
    std::lock_guard<std::mutex> lock(pImpl->perfMutex);
    
    if (pImpl->operationCallCounts.empty()) {
        return "";
    }
    
    auto it = std::max_element(pImpl->operationCallCounts.begin(), pImpl->operationCallCounts.end(),
                               [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return it->first;
}

std::string AdvancedConsoleLogger::formatTimelineEvent(const TimelineEvent& event) const {
    std::ostringstream oss;
    oss << "Event: " << event.eventName;
    if (!event.eventId.empty() && event.eventId != event.eventName) {
        oss << " (" << event.eventId << ")";
    }
    oss << "\n  Duration: " << std::fixed << std::setprecision(2) << event.duration << "ms";
    if (!event.category.empty()) {
        oss << "\n  Category: " << event.category;
    }
    if (!event.threadId.empty()) {
        oss << "\n  Thread: " << event.threadId;
    }
    oss << "\n  Started: " << std::chrono::system_clock::to_time_t(event.timestamp);
    
    return oss.str();
}

std::string AdvancedConsoleLogger::formatPerformanceMetric(const PerformanceMetrics& metric) const {
    std::ostringstream oss;
    oss << "Operation: " << metric.operationName;
    if (!metric.category.empty()) {
        oss << " [" << metric.category << "]";
    }
    oss << "\n  Calls: " << metric.callsCount;
    oss << "\n  Total time: " << std::fixed << std::setprecision(2) << metric.elapsedTime << "ms";
    oss << "\n  Average: " << std::fixed << std::setprecision(2) << metric.averageTime << "ms";
    oss << "\n  Min: " << std::fixed << std::setprecision(2) << metric.minTime << "ms";
    oss << "\n  Max: " << std::fixed << std::setprecision(2) << metric.maxTime << "ms";
    
    return oss.str();
}

std::string AdvancedConsoleLogger::generateProfilingReport() const {
    std::ostringstream oss;
    
    oss << "=== NLM Advanced Profiling Report ===\n";
    oss << "Generated: " << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d %H:%M:%S") << "\n\n";
    
    // Performance metrics section
    oss << "=== Performance Metrics ===\n";
    auto perfMetrics = getPerformanceMetrics();
    for (const auto& metric : perfMetrics) {
        oss << formatPerformanceMetric(metric) << "\n\n";
    }
    
    // Timeline events section
    oss << "=== Timeline Events ===\n";
    auto timelineEvents = getTimelineEvents();
    for (const auto& event : timelineEvents) {
        oss << formatTimelineEvent(event) << "\n\n";
    }
    
    // Memory usage section
    oss << "=== Memory Usage ===\n";
    auto memoryHistory = getMemoryUsageHistory();
    if (!memoryHistory.empty()) {
        oss << "Last " << memoryHistory.size() << " memory measurements:\n";
        for (size_t i = 0; i < memoryHistory.size(); ++i) {
            oss << "  " << i+1 << ". " << memoryHistory[i].first << " bytes";
            if (!memoryHistory[i].second.empty()) {
                oss << " (" << memoryHistory[i].second << ")";
            }
            oss << "\n";
        }
    } else {
        oss << "No memory usage data available.\n";
    }
    
    // Summary
    oss << "\n=== Summary ===\n";
    oss << "Total operations profiled: " << perfMetrics.size() << "\n";
    oss << "Total timeline events: " << timelineEvents.size() << "\n";
    oss << "Memory measurements: " << memoryHistory.size() << "\n";
    
    if (!perfMetrics.empty()) {
        oss << "Most used operation: " << getMostUsedOperation() << "\n";
    }
    
    return oss.str();
}

bool AdvancedConsoleLogger::exportProfilingData(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateProfilingReport();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

void AdvancedConsoleLogger::clearAllData() {
    clearPerformanceMetrics();
    clearTimelineEvents();
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    memoryUsageHistory.clear();
}

// AdvancedLogger class implementation
struct AdvancedLogger::Impl {
    std::vector<std::shared_ptr<IAdvancedLogger>> loggers;
    std::mutex mutex;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeMeasurements;
    std::mutex measurementsMutex;
};

AdvancedLogger::AdvancedLogger() : pImpl(std::make_unique<Impl>()) {}

AdvancedLogger::~AdvancedLogger() = default;

void AdvancedLogger::addLogger(std::shared_ptr<IAdvancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void AdvancedLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void AdvancedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void AdvancedLogger::log(LogLevel level, const std::string& message,
                         const std::string& file, int line, const std::string& function,
                         const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.timestamp = std::chrono::system_clock::now();
    entry.elapsedTime = 0.0;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void AdvancedLogger::debug(const std::string& message, const std::string& file, int line,
                          const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void AdvancedLogger::info(const std::string& message, const std::string& file, int line,
                         const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void AdvancedLogger::warning(const std::string& message, const std::string& file, int line,
                            const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void AdvancedLogger::error(const std::string& message, const std::string& file, int line,
                          const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void AdvancedLogger::critical(const std::string& message, const std::string& file, int line,
                             const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

void AdvancedLogger::profile(const std::string& operationName, std::function<void()> func,
                            const std::string& category, const std::string& description) {
    // Start performance measurement
    startPerformanceMeasurement(operationName, category, {{ "description", description }});
    
    // Execute function and measure time
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Stop performance measurement with actual elapsed time
    PerformanceMetrics metric = stopPerformanceMeasurement(operationName);
    metric.elapsedTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    metric.description = description; // Store description for reference
    
    // Record the metric
    recordPerformanceMetric(metric);
}

void AdvancedLogger::profileWithReturn(const std::string& operationName, std::function<void*()> func,
                                       const std::string& category, const std::string& description) {
    startPerformanceMeasurement(operationName, category, {{ "description", description }});
    
    auto startTime = std::chrono::high_resolution_clock::now();
    void* result = func();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    PerformanceMetrics metric = stopPerformanceMeasurement(operationName);
    metric.elapsedTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    metric.description = description;
    
    // Store result pointer in metric for later retrieval
    metric.resultPtr = result;
    
    recordPerformanceMetric(metric);
}

std::string AdvancedLogger::timelineStart(const std::string& eventName,
                                          const std::string& category,
                                          const std::string& parentEventId) {
    return getGlobal().startTimelineEvent(eventName, category, parentEventId);
}

void AdvancedLogger::timelineEnd(const std::string& eventId) {
    getGlobal().stopTimelineEvent(eventId);
}

AdvancedLogger& AdvancedLogger::getGlobal() {
    static AdvancedLogger globalLogger;
    return globalLogger;
}

void AdvancedLogger::setGlobal(std::shared_ptr<AdvancedLogger> logger) {
    // Placeholder - we rely on getGlobal() which is already static
}

std::string AdvancedLogger::getProfilingSummary() const {
    std::ostringstream oss;
    
    oss << "=== NLM Advanced Logger Summary ===\n";
    oss << "Loggers: " << pImpl->loggers.size() << "\n";
    
    // Get metrics from first available logger
    for (const auto& logger : pImpl->loggers) {
        auto metrics = logger->getPerformanceMetrics();
        if (!metrics.empty()) {
            oss << "Performance metrics tracked: " << metrics.size() << "\n";
            oss << "Timeline events: " << logger->getTimelineEvents().size() << "\n";
            break;
        }
    }
    
    return oss.str();
}

std::vector<PerformanceMetrics> AdvancedLogger::getTopPerformanceMetrics(size_t limit) const {
    std::vector<PerformanceMetrics> allMetrics;
    
    for (const auto& logger : pImpl->loggers) {
        auto metrics = logger->getPerformanceMetrics();
        allMetrics.insert(allMetrics.end(), metrics.begin(), metrics.end());
    }
    
    // Sort by elapsed time (descending)
    std::sort(allMetrics.begin(), allMetrics.end(),
              [](const PerformanceMetrics& a, const PerformanceMetrics& b) {
                  return a.elapsedTime > b.elapsedTime;
              });
    
    if (limit > allMetrics.size()) {
        limit = allMetrics.size();
    }
    
    return std::vector<PerformanceMetrics>(allMetrics.begin(), allMetrics.begin() + limit);
}

std::vector<TimelineEvent> AdvancedLogger::getTimelineReport() const {
    std::vector<TimelineEvent> allEvents;
    
    for (const auto& logger : pImpl->loggers) {
        auto events = logger->getTimelineEvents();
        allEvents.insert(allEvents.end(), events.begin(), events.end());
    }
    
    return allEvents;
}

bool AdvancedLogger::exportProfilingReport(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << getProfilingSummary();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

// Initialize with default console logger
void initializeDefaultAdvancedLogger() {
    auto logger = std::make_shared<AdvancedConsoleLogger>();
    logger->setLevel(LogLevel::Info);
    AdvancedLogger::getGlobal().addLogger(logger);
}

// Global initialization
namespace {
    struct LoggerInitializer {
        LoggerInitializer() {
            initializeDefaultAdvancedLogger();
        }
    } loggerInitializer;
}

} // namespace nlm

// Export functions for easy inclusion
#ifdef NLM_EXPORT_LOG
extern "C" {
    void nlm_init_profiling() {
        nlm::initializeDefaultAdvancedLogger();
    }
    
    void nlm_start_profile(const char* operationName, const char* category) {
        nlm::AdvancedLogger::getGlobal().startPerformanceMeasurement(
            std::string(operationName), std::string(category));
    }
    
    void nlm_stop_profile(const char* operationName) {
        nlm::AdvancedLogger::getGlobal().stopPerformanceMeasurement(std::string(operationName));
    }
    
    const char* nlm_get_profiling_report() {
        static std::string report = nlm::AdvancedLogger::getGlobal().getProfilingSummary();
        return report.c_str();
    }
}
#endif