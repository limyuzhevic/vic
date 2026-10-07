#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <cmath>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance tracking
    bool trackPerformance = false;
    double performanceThreshold = 0.1; // 100ms
    std::vector<PerformanceSample> performanceSamples;
    
    // Memory tracking
    bool trackMemory = false;
    size_t memoryThreshold = 1024 * 1024; // 1MB
    std::vector<MemoryInfo> memoryInfo;
    
    // Neural debugging
    bool trackNeural = false;
    std::vector<NeuralDebugInfo> neuralDebugInfo;
    
    // Event debugging
    bool trackEvents = false;
    std::vector<EventDebugInfo> eventDebugInfo;
    
    // Configuration debugging
    bool trackConfig = false;
    std::vector<ConfigDebugInfo> configDebugInfo;
    
    // Plugin debugging
    bool trackPlugins = false;
    std::vector<PluginDebugInfo> pluginDebugInfo;
    
    // Log filters
    std::vector<std::function<bool(const LogEntry&)>> logFilters;
    
    // Global timers for performance tracking
    std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> timers;
    
    // Global memory tracking
    std::unordered_map<void*, MemoryInfo> memoryAllocations;
    
    // All log entries
    std::vector<LogEntry> logEntries;
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
    
    // Apply log filters
    bool shouldLog = true;
    for (const auto& filter : pImpl->logFilters) {
        if (!filter(entry)) {
            shouldLog = false;
            break;
        }
    }
    
    if (!shouldLog) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";
    
    // Add thread name
    oss << "[" << entry.threadName << "] ";
    
    // Add metadata
    if (!entry.metadata.empty()) {
        oss << "[META: ";
        for (const auto& meta : entry.metadata) {
            oss << meta.first << "=" << meta.second << " ";
        }
        oss << "] ";
    }
    
    oss << entry.message;
    
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
    
    // Store log entry
    pImpl->logEntries.push_back(entry);
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

void ConsoleLogger::setPerformanceThreshold(double threshold) {
    pImpl->performanceThreshold = threshold;
}

void ConsoleLogger::setMemoryThreshold(size_t threshold) {
    pImpl->memoryThreshold = threshold;
}

void ConsoleLogger::enablePerformanceTracking(bool enable) {
    pImpl->trackPerformance = enable;
}

void ConsoleLogger::enableMemoryTracking(bool enable) {
    pImpl->trackMemory = enable;
}

void ConsoleLogger::logPerformance(const PerformanceSample& sample) {
    if (pImpl->trackPerformance) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->performanceSamples.push_back(sample);
        
        // Log if exceeds threshold
        if (sample.duration.count() > pImpl->performanceThreshold) {
            LogEntry entry;
            entry.level = LogLevel::PerfTrace;
            entry.message = performanceToString(sample);
            entry.threadName = sample.threadName;
            entry.metadata["operation"] = sample.operation;
            entry.metadata["duration_ms"] = std::to_string(sample.duration.count() * 1000.0);
            
            log(entry);
        }
    }
}

void ConsoleLogger::logMemory(const MemoryInfo& info) {
    if (pImpl->trackMemory) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->memoryInfo.push_back(info);
        
        // Log memory warnings
        if (info.operation == "alloc" && info.size > pImpl->memoryThreshold) {
            LogEntry entry;
            entry.level = LogLevel::MemoryDebug;
            entry.message = memoryToString(info);
            entry.threadName = info.threadName;
            entry.metadata["operation"] = info.operation;
            entry.metadata["size_bytes"] = std::to_string(info.size);
            
            log(entry);
        }
    }
}

void ConsoleLogger::logNeural(const NeuralDebugInfo& info) {
    if (pImpl->trackNeural) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->neuralDebugInfo.push_back(info);
        
        // Log all neural debug info
        LogEntry entry;
        entry.level = LogLevel::NeuralDebug;
        entry.message = neuralToString(info);
        entry.threadName = info.threadName;
        entry.metadata["neuron_id"] = std::to_string(info.neuronId);
        entry.metadata["population_id"] = std::to_string(info.populationId);
        entry.metadata["region_id"] = std::to_string(info.regionId);
        
        log(entry);
    }
}

void ConsoleLogger::logEvent(const EventDebugInfo& info) {
    if (pImpl->trackEvents) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->eventDebugInfo.push_back(info);
        
        // Log all event debug info
        LogEntry entry;
        entry.level = LogLevel::EventDebug;
        entry.message = eventToString(info);
        entry.threadName = info.threadName;
        entry.metadata["event_name"] = info.eventName;
        entry.metadata["source"] = info.source;
        entry.metadata["target"] = info.target;
        
        log(entry);
    }
}

void ConsoleLogger::logConfig(const ConfigDebugInfo& info) {
    if (pImpl->trackConfig) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->configDebugInfo.push_back(info);
        
        // Log config changes
        LogEntry entry;
        entry.level = LogLevel::ConfigDebug;
        entry.message = configToString(info);
        entry.threadName = info.threadName;
        entry.metadata["parameter"] = info.parameterName;
        entry.metadata["old_value"] = info.oldValue;
        entry.metadata["new_value"] = info.newValue;
        
        log(entry);
    }
}

void ConsoleLogger::logPlugin(const PluginDebugInfo& info) {
    if (pImpl->trackPlugins) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->pluginDebugInfo.push_back(info);
        
        // Log plugin activities
        LogEntry entry;
        entry.level = LogLevel::PluginDebug;
        entry.message = pluginToString(info);
        entry.threadName = info.threadName;
        entry.metadata["plugin_name"] = info.pluginName;
        entry.metadata["plugin_version"] = info.pluginVersion;
        
        log(entry);
    }
}

std::vector<LogEntry> ConsoleLogger::getLogEntries(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (limit == 0 || limit >= pImpl->logEntries.size()) {
        return pImpl->logEntries;
    }
    
    size_t start = pImpl->logEntries.size() - limit;
    return std::vector<LogEntry>(pImpl->logEntries.begin() + start, pImpl->logEntries.end());
}

std::vector<PerformanceSample> ConsoleLogger::getPerformanceSamples(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (limit == 0 || limit >= pImpl->performanceSamples.size()) {
        return pImpl->performanceSamples;
    }
    
    size_t start = pImpl->performanceSamples.size() - limit;
    return std::vector<PerformanceSample>(pImpl->performanceSamples.begin() + start, pImpl->performanceSamples.end());
}

std::vector<MemoryInfo> ConsoleLogger::getMemoryInfo(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (limit == 0 || limit >= pImpl->memoryInfo.size()) {
        return pImpl->memoryInfo;
    }
    
    size_t start = pImpl->memoryInfo.size() - limit;
    return std::vector<MemoryInfo>(pImpl->memoryInfo.begin() + start, pImpl->memoryInfo.end());
}

void ConsoleLogger::addLogFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->logFilters.push_back(filter);
}

void ConsoleLogger::clearLogFilters() {
    pImpl->logFilters.clear();
}

bool ConsoleLogger::exportLogs(const std::string& filepath, 
                              const std::string& format) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"log_entries\": [" << std::endl;
            
            for (size_t i = 0; i < pImpl->logEntries.size(); ++i) {
                const auto& entry = pImpl->logEntries[i];
                file << "    {\"level\": \"" << levelToString(entry.level) << "\"," << std::endl;
                file << "     \"message\": \"" << entry.message << "\"," << std::endl;
                file << "     \"thread\": \"" << entry.threadName << "\"," << std::endl;
                file << "     \"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\"";
                if (i < pImpl->logEntries.size() - 1) {
                    file << "," << std::endl;
                } else {
                    file << std::endl;
                }
            }
            
            file << "  ]\n";
            file << "}" << std::endl;
        } else {
            // Default to text format
            for (const auto& entry : pImpl->logEntries) {
                std::ostringstream oss;
                oss << "[" << levelToString(entry.level) << "] ";
                oss << entry.message;
                file << oss.str() << std::endl;
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConsoleLogger::exportPerformance(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Performance Samples:" << std::endl;
        file << "====================" << std::endl << std::endl;
        
        for (const auto& sample : pImpl->performanceSamples) {
            file << performanceToString(sample) << std::endl << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConsoleLogger::exportMemory(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Memory Information:" << std::endl;
        file << "====================" << std::endl << std::endl;
        
        for (const auto& info : pImpl->memoryInfo) {
            file << memoryToString(info) << std::endl << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

void ConsoleLogger::startTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timers[operation] = std::chrono::high_resolution_clock::now();
}

void ConsoleLogger::stopTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->timers.find(operation);
    if (it != pImpl->timers.end()) {
        auto duration = std::chrono::high_resolution_clock::now() - it->second;
        
        PerformanceSample sample(operation, duration, "Main");
        logPerformance(sample);
        
        pImpl->timers.erase(it);
    }
}

void ConsoleLogger::memoryAlloc(size_t size, const std::string& file, int line) {
    if (pImpl->trackMemory) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        MemoryInfo info("alloc", size, reinterpret_cast<void*>(size), file, line, "Main");
        pImpl->memoryInfo.push_back(info);
        
        logMemory(info);
    }
}

void ConsoleLogger::memoryFree(void* ptr) {
    if (pImpl->trackMemory) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto it = pImpl->memoryAllocations.find(ptr);
        if (it != pImpl->memoryAllocations.end()) {
            MemoryInfo info = it->second;
            info.operation = "free";
            
            pImpl->memoryInfo.push_back(info);
            
            logMemory(info);
            
            pImpl->memoryAllocations.erase(it);
        }
    }
}

void ConsoleLogger::neuralActivity(const std::string& details, int neuronId) {
    if (pImpl->trackNeural) {
        NeuralDebugInfo info(NeuralDebugInfo::Type::SpikeEvent, details, neuronId);
        logNeural(info);
    }
}

void ConsoleLogger::eventActivity(const std::string& eventName, const std::string& source) {
    if (pImpl->trackEvents) {
        EventDebugInfo info(EventDebugInfo::Type::EventReceived, eventName, source);
        logEvent(info);
    }
}

void ConsoleLogger::configChange(const std::string& paramName, const std::string& oldValue,
                                const std::string& newValue, const std::string& source) {
    if (pImpl->trackConfig) {
        ConfigDebugInfo info(ConfigDebugInfo::Type::ParameterSet, paramName, oldValue, newValue, source);
        logConfig(info);
    }
}

std::string ConsoleLogger::performanceToString(const PerformanceSample& sample) {
    std::ostringstream oss;
    oss << "PERF: " << sample.operation << " took " 
        << std::fixed << std::setprecision(3) << sample.duration.count() * 1000.0 << "ms";
    
    if (!sample.threadName.empty()) {
        oss << " [Thread: " << sample.threadName << "]";
    }
    
    return oss.str();
}

std::string ConsoleLogger::memoryToString(const MemoryInfo& info) {
    std::ostringstream oss;
    oss << "MEM: " << info.operation << " ";
    
    if (info.operation == "alloc") {
        oss << std::to_string(info.size) << " bytes at " << info.pointer;
    } else if (info.operation == "free") {
        oss << "free " << info.pointer;
    }
    
    oss << " [File: " << info.file << ":" << info.line << "]";
    
    return oss.str();
}

std::string ConsoleLogger::neuralToString(const NeuralDebugInfo& info) {
    std::ostringstream oss;
    oss << "NEURAL: ";
    
    switch (info.type) {
        case NeuralDebugInfo::Type::NeuronCreated: oss << "Created neuron " << info.neuronId; break;
        case NeuralDebugInfo::Type::NeuronDestroyed: oss << "Destroyed neuron " << info.neuronId; break;
        case NeuralDebugInfo::Type::NeuronFired: oss << "Neuron " << info.neuronId << " fired"; break;
        case NeuralDebugInfo::Type::NeuronUpdated: oss << "Updated neuron " << info.neuronId << " to " << info.value; break;
        case NeuralDebugInfo::Type::SynapseCreated: oss << "Created synapse from " << info.neuronId << " to " << info.populationId; break;
        case NeuralDebugInfo::Type::SynapseDestroyed: oss << "Destroyed synapse from " << info.neuronId << " to " << info.populationId; break;
        case NeuralDebugInfo::Type::SynapseUpdated: oss << "Updated synapse from " << info.neuronId << " to " << info.populationId << " to " << info.value; break;
        case NeuralDebugInfo::Type::RegionCreated: oss << "Created region " << info.regionId; break;
        case NeuralDebugInfo::Type::RegionDestroyed: oss << "Destroyed region " << info.regionId; break;
        case NeuralDebugInfo::Type::PopulationCreated: oss << "Created population in region " << info.regionId; break;
        case NeuralDebugInfo::Type::PopulationUpdated: oss << "Updated population in region " << info.regionId << " to " << info.value; break;
        case NeuralDebugInfo::Type::SpikeEvent: oss << "Spike event in neuron " << info.neuronId; break;
        case NeuralDebugInfo::Type::ConnectionChanged: oss << "Connection changed in neuron " << info.neuronId; break;
        case NeuralDebugInfo::Type::PlasticityApplied: oss << "Plasticity applied to neuron " << info.neuronId; break;
        case NeuralDebugInfo::Type::DevelopmentStage: oss << "Development stage: " << info.value; break;
    }
    
    return oss.str();
}

std::string ConsoleLogger::eventToString(const EventDebugInfo& info) {
    std::ostringstream oss;
    oss << "EVENT: ";
    
    switch (info.type) {
        case EventDebugInfo::Type::EventReceived: oss << "Received event \"" << info.eventName << "\" from \"" << info.source << "\""; break;
        case EventDebugInfo::Type::EventProcessed: oss << "Processed event \"" << info.eventName << "\""; break;
        case EventDebugInfo::Type::EventDispatched: oss << "Dispatched event \"" << info.eventName << "\" to \"" << info.target << "\""; break;
        case EventDebugInfo::Type::EventCompleted: oss << "Completed event \"" << info.eventName << "\""; break;
        case EventDebugInfo::Type::EventError: oss << "Error in event \"" << info.eventName << "\": " << info.details; break;
        case EventDebugInfo::Type::EventQueued: oss << "Queued event \"" << info.eventName << "\""; break;
        case EventDebugInfo::Type::EventCancelled: oss << "Cancelled event \"" << info.eventName << "\""; break;
    }
    
    return oss.str();
}

std::string ConsoleLogger::configToString(const ConfigDebugInfo& info) {
    std::ostringstream oss;
    oss << "CONFIG: ";
    
    switch (info.type) {
        case ConfigDebugInfo::Type::ParameterSet: 
            oss << "Set parameter \"" << info.parameterName << "\" from \"" << info.oldValue << "\" to \"" << info.newValue << "\" (source: " << info.source << ")";
            break;
        case ConfigDebugInfo::Type::ParameterGet: oss << "Get parameter \"" << info.parameterName << "\""; break;
        case ConfigDebugInfo::Type::ParameterRemoved: oss << "Removed parameter \"" << info.parameterName << "\""; break;
        case ConfigDebugInfo::Type::ParameterValidated: 
            oss << "Validated parameter \"" << info.parameterName << "\": " << (info.validationPassed ? "PASSED" : "FAILED");
            break;
        case ConfigDebugInfo::Type::DefaultLoaded: oss << "Loaded default configuration"; break;
        case ConfigDebugInfo::Type::FileLoaded: oss << "Loaded configuration from file \"" << info.source << "\""; break;
        case ConfigDebugInfo::Type::CommandLineParsed: oss << "Parsed command line arguments"; break;
    }
    
    return oss.str();
}

std::string ConsoleLogger::pluginToString(const PluginDebugInfo& info) {
    std::ostringstream oss;
    oss << "PLUGIN: ";
    
    switch (info.type) {
        case PluginDebugInfo::Type::PluginLoaded: oss << "Loaded plugin \"" << info.pluginName << "\" version \"" << info.pluginVersion << "\""; break;
        case PluginDebugInfo::Type::PluginUnloaded: oss << "Unloaded plugin \"" << info.pluginName << "\""; break;
        case PluginDebugInfo::Type::PluginError: oss << "Plugin \"" << info.pluginName << "\" error: " << info.details; break;
        case PluginDebugInfo::Type::PluginCalled: oss << "Called plugin \"" << info.pluginName << "\""; break;
        case PluginDebugInfo::Type::PluginRegistered: oss << "Registered plugin \"" << info.pluginName << "\""; break;
        case PluginDebugInfo::Type::PluginUnregistered: oss << "Unregistered plugin \"" << info.pluginName << "\""; break;
        case PluginDebugInfo::Type::PluginEnabled: oss << "Enabled plugin \"" << info.pluginName << "\""; break;
        case PluginDebugInfo::Type::PluginDisabled: oss << "Disabled plugin \"" << info.pluginName << "\""; break;
    }
    
    return oss.str();
}

std::string ConsoleLogger::timerToString(const std::string& operation, 
                                        std::chrono::duration<double> duration) {
    std::ostringstream oss;
    oss << "TIMER: " << operation << " took " 
        << std::fixed << std::setprecision(3) << duration.count() * 1000.0 << "ms";
    return oss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add global console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    addLogger(consoleLogger);
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
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::string& thread) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadName = thread;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& thread) {
    log(LogLevel::Debug, message, file, line, function, thread);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                 const std::string& function, const std::string& thread) {
    log(LogLevel::Info, message, file, line, function, thread);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& thread) {
    log(LogLevel::Warning, message, file, line, function, thread);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& thread) {
    log(LogLevel::Error, message, file, line, function, thread);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& thread) {
    log(LogLevel::Critical, message, file, line, function, thread);
}

void Logger::performance(const std::string& operation, std::chrono::duration<double> duration,
                         const std::string& thread) {
    PerformanceSample sample(operation, duration, thread);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    performanceSamples_.push_back(sample);
    
    for (auto& logger : pImpl->loggers) {
        logger->logPerformance(sample);
    }
}

void Logger::memoryAlloc(size_t size, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    void* ptr = reinterpret_cast<void*>(size); // Simplified for demonstration
    MemoryInfo info("alloc", size, ptr, file, line, "Main");
    
    memoryAllocations_[ptr] = info;
    memoryInfo_.push_back(info);
    
    for (auto& logger : pImpl->loggers) {
        logger->logMemory(info);
    }
}

void Logger::memoryFree(void* ptr) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = memoryAllocations_.find(ptr);
    if (it != memoryAllocations_.end()) {
        MemoryInfo info = it->second;
        info.operation = "free";
        
        memoryInfo_.push_back(info);
        
        for (auto& logger : pImpl->loggers) {
            logger->logMemory(info);
        }
        
        memoryAllocations_.erase(it);
    }
}

void Logger::neuralActivity(const std::string& details, int neuronId) {
    NeuralDebugInfo info(NeuralDebugInfo::Type::SpikeEvent, details, neuronId);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    neuralDebugInfo_.push_back(info);
    
    for (auto& logger : pImpl->loggers) {
        logger->logNeural(info);
    }
}

void Logger::eventActivity(const std::string& eventName, const std::string& source) {
    EventDebugInfo info(EventDebugInfo::Type::EventReceived, eventName, source);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    eventDebugInfo_.push_back(info);
    
    for (auto& logger : pImpl->loggers) {
        logger->logEvent(info);
    }
}

void Logger::configChange(const std::string& paramName, const std::string& oldValue,
                         const std::string& newValue, const std::string& source) {
    ConfigDebugInfo info(ConfigDebugInfo::Type::ParameterSet, paramName, oldValue, newValue, source);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    configDebugInfo_.push_back(info);
    
    for (auto& logger : pImpl->loggers) {
        logger->logConfig(info);
    }
}

void Logger::pluginActivity(const std::string& pluginName, const std::string& version,
                           const std::string& details) {
    PluginDebugInfo info(PluginDebugInfo::Type::PluginCalled, pluginName, version, details);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pluginDebugInfo_.push_back(info);
    
    for (auto& logger : pImpl->loggers) {
        logger->logPlugin(info);
    }
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

void Logger::startTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    timers_[operation] = std::chrono::high_resolution_clock::now();
}

void Logger::stopTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    
    auto it = timers_.find(operation);
    if (it != timers_.end()) {
        auto duration = std::chrono::high_resolution_clock::now() - it->second;
        
        performance(duration, operation);
        
        timers_.erase(it);
    }
}

std::chrono::duration<double> Logger::getLastTimerDuration(const std::string& operation) {
    std::lock_guard<std::mutex> lock(timer_mutex_);
    
    auto it = timers_.find(operation);
    if (it != timers_.end()) {
        return std::chrono::high_resolution_clock::now() - it->second;
    }
    
    return std::chrono::duration<double>(0.0);
}

void Logger::trackMemoryAlloc(size_t size, const std::string& file, int line) {
    memoryAlloc(size, file, line);
}

void Logger::trackMemoryFree(void* ptr) {
    memoryFree(ptr);
}

void Logger::analyzePerformance(const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return;
    }
    
    file << "Performance Analysis:" << std::endl;
    file << "====================" << std::endl << std::endl;
    
    double totalTime = 0.0;
    size_t sampleCount = performanceSamples_.size();
    
    for (const auto& sample : performanceSamples_) {
        file << performanceToString(sample) << std::endl;
        totalTime += sample.duration.count();
    }
    
    file << std::endl;
    file << "Statistics:" << std::endl;
    file << "  Total operations: " << sampleCount << std::endl;
    file << "  Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    file << "  Average time per operation: " << (sampleCount > 0 ? totalTime / sampleCount : 0.0) * 1000.0 << "ms" << std::endl;
    
    file.close();
}

void Logger::analyzeMemory(const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return;
    }
    
    file << "Memory Analysis:" << std::endl;
    file << "================" << std::endl << std::endl;
    
    size_t allocCount = 0;
    size_t freeCount = 0;
    size_t totalAllocSize = 0;
    
    for (const auto& info : memoryInfo_) {
        file << memoryToString(info) << std::endl;
        if (info.operation == "alloc") {
            allocCount++;
            totalAllocSize += info.size;
        } else if (info.operation == "free") {
            freeCount++;
        }
    }
    
    file << std::endl;
    file << "Statistics:" << std::endl;
    file << "  Allocations: " << allocCount << std::endl;
    file << "  Frees: " << freeCount << std::endl;
    file << "  Total allocated: " << totalAllocSize << " bytes" << std::endl;
    
    file.close();
}

void Logger::analyzeNeural(const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return;
    }
    
    file << "Neural Activity Analysis:" << std::endl;
    file << "=========================" << std::endl << std::endl;
    
    // Count different types of neural events
    std::map<NeuralDebugInfo::Type, size_t> eventCounts;
    for (const auto& info : neuralDebugInfo_) {
        eventCounts[info.type]++;
    }
    
    file << "Neural Events:" << std::endl;
    for (const auto& pair : eventCounts) {
        file << "  " << static_cast<int>(pair.first) << ": " << pair.second << " events" << std::endl;
    }
    
    file << std::endl;
    file << "Sample Neural Events:" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(10, neuralDebugInfo_.size()); ++i) {
        file << "  " << neuralToString(neuralDebugInfo_[i]) << std::endl;
    }
    
    file.close();
}

void Logger::enableAllDebugModes() {
    auto globalLogger = std::dynamic_pointer_cast<ConsoleLogger>(getGlobal().pImpl->loggers[0]);
    if (globalLogger) {
        globalLogger->enablePerformanceTracking(true);
        globalLogger->enableMemoryTracking(true);
        globalLogger->enableNeuralDebug(true);
        globalLogger->enableEventDebug(true);
        globalLogger->enableConfigDebug(true);
        globalLogger->enablePluginDebug(true);
    }
}

void Logger::disableAllDebugModes() {
    auto globalLogger = std::dynamic_pointer_cast<ConsoleLogger>(getGlobal().pImpl->loggers[0]);
    if (globalLogger) {
        globalLogger->enablePerformanceTracking(false);
        globalLogger->enableMemoryTracking(false);
        globalLogger->enableNeuralDebug(false);
        globalLogger->enableEventDebug(false);
        globalLogger->enableConfigDebug(false);
        globalLogger->enablePluginDebug(false);
    }
}

void Logger::setDebugLevel(LogLevel level) {
    getGlobal().setLevel(level);
}

// Global static variables
std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> Logger::timers_;
std::mutex Logger::timer_mutex_;

std::unordered_map<void*, MemoryInfo> Logger::memoryAllocations_;
std::mutex Logger::memory_mutex_;

std::vector<PerformanceSample> Logger::performanceSamples_;
std::mutex Logger::performance_mutex_;

std::vector<NeuralDebugInfo> Logger::neuralDebugInfo_;
std::mutex Logger::neural_mutex_;

std::vector<EventDebugInfo> Logger::eventDebugInfo_;
std::mutex Logger::event_mutex_;

std::vector<ConfigDebugInfo> Logger::configDebugInfo_;
std::mutex Logger::config_mutex_;

std::vector<PluginDebugInfo> Logger::pluginDebugInfo_;
std::mutex Logger::plugin_mutex_;

} // namespace nlm