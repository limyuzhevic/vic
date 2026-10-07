#include "EnhancedLogger.hpp"
#include <thread>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {

// ConsoleDebugLogger implementation
struct ConsoleDebugLogger::Impl {
    LogLevel level;
    bool useColors;
    bool enablePerformanceMetrics;
    bool enableMemoryTracking;
    bool enableNeuralTracking;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Statistics
    size_t totalLogEntries;
    size_t performanceMetricsRecorded;
    size_t memoryAllocations;
    size_t neuralActivitiesRecorded;
    size_t simulationStatesRecorded;
    double cpuUsage;
    size_t memoryUsage;
    double averageFrameTime;
    double fps;
    
    // Real-time monitoring
    bool realTimeMonitoring;
    std::chrono::high_resolution_clock::time_point monitoringStart;
    std::thread monitoringThread;
    bool monitoringRunning;
    
    // Performance profiling
    std::chrono::high_resolution_clock::time_point profilingStart;
    std::string currentOperation;
    std::vector<PerformanceMetric> recentMetrics;
    
    // Memory tracking
    std::vector<MemoryAllocation> memoryAllocationsList;
    
    // Neural activity tracking
    std::vector<NeuralActivity> neuralActivitiesList;
    
    // Simulation state tracking
    std::vector<SimulationState> simulationStatesList;
    
    Impl() : level(LogLevel::Info), useColors(true), 
             enablePerformanceMetrics(false), enableMemoryTracking(false),
             enableNeuralTracking(false), totalLogEntries(0),
             performanceMetricsRecorded(0), memoryAllocations(0),
             neuralActivitiesRecorded(0), simulationStatesRecorded(0),
             cpuUsage(0.0), memoryUsage(0), averageFrameTime(0.0), fps(0.0),
             realTimeMonitoring(false), monitoringRunning(false) {
        // Get initial memory usage
        updateMemoryUsage();
    }
};

ConsoleDebugLogger::ConsoleDebugLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleDebugLogger::ConsoleDebugLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleDebugLogger::~ConsoleDebugLogger() = default;

void ConsoleDebugLogger::log(const EnhancedLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    formatEntry(oss, entry);
    
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
    
    pImpl->totalLogEntries++;
}

void ConsoleDebugLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleDebugLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleDebugLogger::recordPerformance(const PerformanceMetric& metric) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enablePerformanceMetrics) {
        pImpl->recentMetrics.push_back(metric);
        if (pImpl->recentMetrics.size() > 100) {
            pImpl->recentMetrics.erase(pImpl->recentMetrics.begin());
        }
        pImpl->performanceMetricsRecorded++;
    }
}

void ConsoleDebugLogger::recordMemoryUsage(size_t memoryBytes, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enableMemoryTracking) {
        MemoryAllocation alloc;
        alloc.address = nullptr; // Not tracked individually for usage
        alloc.size = memoryBytes;
        alloc.timestamp = std::chrono::system_clock::now();
        alloc.file = ""; // System memory usage
        alloc.line = 0;
        alloc.function = "memory_usage";
        alloc.allocated = true;
        
        pImpl->memoryAllocationsList.push_back(alloc);
        if (pImpl->memoryAllocationsList.size() > 100) {
            pImpl->memoryAllocationsList.erase(pImpl->memoryAllocationsList.begin());
        }
        
        pImpl->memoryAllocations++;
        updateMemoryUsage();
    }
}

void ConsoleDebugLogger::recordAllocation(const MemoryAllocation& alloc) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enableMemoryTracking) {
        pImpl->memoryAllocationsList.push_back(alloc);
        if (pImpl->memoryAllocationsList.size() > 100) {
            pImpl->memoryAllocationsList.erase(pImpl->memoryAllocationsList.begin());
        }
        
        pImpl->memoryAllocations++;
        updateMemoryUsage();
    }
}

void ConsoleDebugLogger::recordDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enableMemoryTracking) {
        // Find and mark the allocation as deallocated
        for (auto& alloc : pImpl->memoryAllocationsList) {
            if (alloc.address == address && alloc.allocated) {
                alloc.allocated = false;
                break;
            }
        }
    }
}

void ConsoleDebugLogger::recordNeuralActivity(const NeuralActivity& activity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->enableNeuralTracking) {
        pImpl->neuralActivitiesList.push_back(activity);
        if (pImpl->neuralActivitiesList.size() > 100) {
            pImpl->neuralActivitiesList.erase(pImpl->neuralActivitiesList.begin());
        }
        
        pImpl->neuralActivitiesRecorded++;
    }
}

void ConsoleDebugLogger::recordSimulationState(const SimulationState& state) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->simulationStatesList.push_back(state);
    if (pImpl->simulationStatesList.size() > 100) {
        pImpl->simulationStatesList.erase(pImpl->simulationStatesList.begin());
    }
    
    pImpl->simulationStatesRecorded++;
}

void ConsoleDebugLogger::startRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->realTimeMonitoring) {
        pImpl->realTimeMonitoring = true;
        pImpl->monitoringRunning = true;
        pImpl->monitoringStart = std::chrono::high_resolution_clock::now();
        
        // Start monitoring thread
        pImpl->monitoringThread = std::thread(&ConsoleDebugLogger::monitoringWorker, this);
    }
}

void ConsoleDebugLogger::stopRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->realTimeMonitoring = false;
    pImpl->monitoringRunning = false;
    
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
}

bool ConsoleDebugLogger::isRealTimeMonitoring() const {
    return pImpl->realTimeMonitoring;
}

void ConsoleDebugLogger::startProfiling(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->currentOperation = operation;
    pImpl->profilingStart = std::chrono::high_resolution_clock::now();
}

void ConsoleDebugLogger::stopProfiling() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->currentOperation.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->profilingStart);
        
        PerformanceMetric metric;
        metric.name = pImpl->currentOperation;
        metric.value = duration.count() * 1000.0; // Convert to milliseconds
        metric.timestamp = endTime;
        metric.unit = "ms";
        metric.context = "profile";
        
        recordPerformance(metric);
        
        pImpl->currentOperation.clear();
    }
}

std::chrono::duration<double> ConsoleDebugLogger::getLastProfileTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->recentMetrics.empty()) {
        return std::chrono::duration<double>(0.0);
    }
    
    auto lastMetric = pImpl->recentMetrics.back();
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(now - lastMetric.timestamp);
}

void ConsoleDebugLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleDebugLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleDebugLogger::setEnablePerformanceMetrics(bool enable) {
    pImpl->enablePerformanceMetrics = enable;
}

bool ConsoleDebugLogger::getEnablePerformanceMetrics() const {
    return pImpl->enablePerformanceMetrics;
}

void ConsoleDebugLogger::setEnableMemoryTracking(bool enable) {
    pImpl->enableMemoryTracking = enable;
}

bool ConsoleDebugLogger::getEnableMemoryTracking() const {
    return pImpl->enableMemoryTracking;
}

void ConsoleDebugLogger::setEnableNeuralTracking(bool enable) {
    pImpl->enableNeuralTracking = enable;
}

bool ConsoleDebugLogger::getEnableNeuralTracking() const {
    return pImpl->enableNeuralTracking;
}

void ConsoleDebugLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleDebugLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

ConsoleDebugLogger::DebugStats ConsoleDebugLogger::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    DebugStats stats;
    stats.totalLogEntries = pImpl->totalLogEntries;
    stats.performanceMetricsRecorded = pImpl->performanceMetricsRecorded;
    stats.memoryAllocations = pImpl->memoryAllocations;
    stats.neuralActivitiesRecorded = pImpl->neuralActivitiesRecorded;
    stats.simulationStatesRecorded = pImpl->simulationStatesRecorded;
    stats.cpuUsage = pImpl->cpuUsage;
    stats.memoryUsage = pImpl->memoryUsage;
    stats.averageFrameTime = pImpl->averageFrameTime;
    stats.fps = pImpl->fps;
    
    return stats;
}

std::string ConsoleDebugLogger::generatePerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    oss << "Total Performance Metrics: " << pImpl->performanceMetricsRecorded << "\n";
    oss << "Average FPS: " << std::fixed << std::setprecision(2) << pImpl->fps << "\n";
    oss << "Average Frame Time: " << std::fixed << std::setprecision(4) << pImpl->averageFrameTime * 1000.0 << " ms\n";
    oss << "Current CPU Usage: " << std::fixed << std::setprecision(2) << pImpl->cpuUsage << "%\n";
    oss << "Current Memory Usage: " << pImpl->memoryUsage << " bytes\n";
    
    oss << "\nRecent Performance Metrics:\n";
    for (const auto& metric : pImpl->recentMetrics) {
        oss << "  " << metric.name << ": " << std::fixed << std::setprecision(2) 
            << metric.value << " " << metric.unit;
        if (!metric.context.empty()) {
            oss << " (" << metric.context << ")";
        }
        oss << " at " << std::chrono::duration_cast<std::chrono::milliseconds>(
            metric.timestamp.time_since_epoch()).count() << "ms\n";
    }
    
    return oss.str();
}

std::string ConsoleDebugLogger::generateMemoryReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Report ===\n";
    oss << "Total Memory Allocations: " << pImpl->memoryAllocations << "\n";
    oss << "Current Memory Usage: " << pImpl->memoryUsage << " bytes\n";
    
    size_t allocated = 0;
    size_t active = 0;
    for (const auto& alloc : pImpl->memoryAllocationsList) {
        allocated += alloc.size;
        if (alloc.allocated) {
            active += alloc.size;
        }
    }
    
    oss << "Allocated Memory: " << allocated << " bytes\n";
    oss << "Active Memory: " << active << " bytes\n";
    oss << "Memory Allocation Percentage: " << std::fixed << std::setprecision(2)
        << (allocated > 0 ? (active * 100.0 / allocated) : 0.0) << "%\n";
    
    oss << "\nRecent Allocations:\n";
    for (const auto& alloc : pImpl->memoryAllocationsList) {
        oss << "  " << alloc.size << " bytes at " << alloc.function << " (" << alloc.file << ":" << alloc.line << ")\n";
    }
    
    return oss.str();
}

std::string ConsoleDebugLogger::generateNeuralActivityReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Neural Activity Report ===\n";
    oss << "Total Neural Activities Recorded: " << pImpl->neuralActivitiesRecorded << "\n";
    
    if (!pImpl->neuralActivitiesList.empty()) {
        float totalFiringRate = 0.0f;
        float totalMembranePotential = 0.0f;
        size_t totalSpikes = 0;
        
        for (const auto& activity : pImpl->neuralActivitiesList) {
            totalFiringRate += activity.firingRate;
            totalMembranePotential += activity.membranePotential;
            totalSpikes += activity.spikeCount;
        }
        
        size_t count = pImpl->neuralActivitiesList.size();
        oss << "Average Firing Rate: " << std::fixed << std::setprecision(4) 
            << (count > 0 ? totalFiringRate / count : 0.0) << " Hz\n";
        oss << "Average Membrane Potential: " << std::fixed << std::setprecision(4)
            << (count > 0 ? totalMembranePotential / count : 0.0) << " mV\n";
        oss << "Total Spikes Recorded: " << totalSpikes << "\n";
        oss << "Average Spikes per Neuron: " << std::fixed << std::setprecision(2)
            << (count > 0 ? static_cast<double>(totalSpikes) / count : 0.0) << "\n";
    }
    
    return oss.str();
}

std::string ConsoleDebugLogger::generateSimulationReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Simulation Report ===\n";
    oss << "Total Simulation States Recorded: " << pImpl->simulationStatesRecorded << "\n";
    
    if (!pImpl->simulationStatesList.empty()) {
        auto latestState = pImpl->simulationStatesList.back();
        oss << "Current Step: " << latestState.step << "\n";
        oss << "Current Time: " << std::fixed << std::setprecision(4) << latestState.time << " seconds\n";
        oss << "Neuron Count: " << latestState.neuronCount << "\n";
        oss << "Active Neuron Count: " << latestState.activeNeuronCount << "\n";
        oss << "Total Spike Count: " << latestState.totalSpikeCount << "\n";
        oss << "Average Firing Rate: " << std::fixed << std::setprecision(4) 
            << latestState.averageFiringRate << " Hz\n";
        oss << "State Description: " << latestState.stateDescription << "\n";
        
        oss << "\nRecent Simulation States:\n";
        for (const auto& state : pImpl->simulationStatesList) {
            oss << "  Step " << state.step << ", Time: " << std::fixed << std::setprecision(4) 
                << state.time << ", " << state.activeNeuronCount << " active neurons\n";
        }
    }
    
    return oss.str();
}

void ConsoleDebugLogger::updateMemoryUsage() {
    // Get current memory usage (simplified version)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    pImpl->memoryUsage = usage.ru_maxrss * 1024; // Convert KB to bytes
    
    // Get CPU usage (simplified version)
    static auto lastTime = std::chrono::high_resolution_clock::now();
    static double lastCPU = 0.0;
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = currentTime - lastTime;
    
    double currentCPU = 0.0; // Would calculate actual CPU usage
    if (elapsed.count() > 0.0) {
        pImpl->cpuUsage = ((currentCPU - lastCPU) / elapsed.count()) * 100.0;
        lastCPU = currentCPU;
        lastTime = currentTime;
    }
}

void ConsoleDebugLogger::monitoringWorker() {
    while (pImpl->monitoringRunning) {
        // Update statistics
        updateMemoryUsage();
        
        // Sleep briefly
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// PerformanceProfiler implementation
struct PerformanceProfiler::Impl {
    std::string currentOperation;
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<PerformanceMetric> metrics;
    std::mutex mutex;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> threadStarts;
    std::mutex threadMutex;
    std::map<std::string, std::chrono::duration<double>> threadTimes;
    std::mutex threadTimeMutex;
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::startProfiling(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->currentOperation = operation;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->currentOperation.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->startTime);
        
        PerformanceMetric metric;
        metric.name = pImpl->currentOperation;
        metric.value = duration.count() * 1000.0;
        metric.timestamp = endTime;
        metric.unit = "ms";
        metric.context = "profile";
        
        recordMetric(metric.name, metric.value, metric.unit, metric.context);
        
        pImpl->currentOperation.clear();
    }
}

double PerformanceProfiler::getCurrentProfileTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->currentOperation.empty()) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->startTime);
    return duration.count() * 1000.0;
}

void PerformanceProfiler::recordMetric(const std::string& name, double value, 
                                       const std::string& unit,
                                       const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PerformanceMetric metric;
    metric.name = name;
    metric.value = value;
    metric.timestamp = std::chrono::system_clock::now();
    metric.unit = unit;
    metric.context = context;
    
    pImpl->metrics.push_back(metric);
    if (pImpl->metrics.size() > 1000) {
        pImpl->metrics.erase(pImpl->metrics.begin());
    }
}

std::vector<PerformanceMetric> PerformanceProfiler::getRecentMetrics(int count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (count > static_cast<int>(pImpl->metrics.size())) {
        count = static_cast<int>(pImpl->metrics.size());
    }
    
    std::vector<PerformanceMetric> recent;
    recent.reserve(count);
    
    auto startIt = pImpl->metrics.end() - count;
    for (auto it = startIt; it != pImpl->metrics.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

void PerformanceProfiler::clearMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.clear();
}

void PerformanceProfiler::startThreadProfiling(const std::string& threadName) {
    std::lock_guard<std::mutex> lock(pImpl->threadMutex);
    pImpl->threadStarts[threadName] = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopThreadProfiling() {
    std::lock_guard<std::mutex> lock(pImpl->threadMutex);
    
    if (!pImpl->threadStarts.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        
        for (auto& pair : pImpl->threadStarts) {
            auto duration = std::chrono::duration<double>(endTime - pair.second);
            
            std::lock_guard<std::mutex> timeLock(pImpl->threadTimeMutex);
            pImpl->threadTimes[pair.first] += duration;
        }
        
        pImpl->threadStarts.clear();
    }
}

std::string PerformanceProfiler::getThreadProfileSummary() const {
    std::ostringstream oss;
    oss << "=== Thread Profile Summary ===\n";
    
    std::lock_guard<std::mutex> timeLock(pImpl->threadTimeMutex);
    for (const auto& pair : pImpl->threadTimes) {
        oss << pair.first << ": " << std::fixed << std::setprecision(4) 
            << pair.second.count() * 1000.0 << " ms\n";
    }
    
    return oss.str();
}

// MemoryDebugger implementation
struct MemoryDebugger::Impl {
    std::vector<MemoryAllocation> allocations;
    std::mutex mutex;
    size_t totalAllocated;
    size_t peakUsage;
    size_t currentUsage;
};

MemoryDebugger::MemoryDebugger() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalAllocated = 0;
    pImpl->peakUsage = 0;
    pImpl->currentUsage = 0;
}

MemoryDebugger::~MemoryDebugger() = default;

void* MemoryDebugger::trackAllocation(size_t size, const std::string& file, int line, 
                                     const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // In a real implementation, this would allocate memory and track it
    // For now, we'll return a dummy pointer
    void* address = std::malloc(size);
    
    if (address) {
        MemoryAllocation alloc;
        alloc.address = address;
        alloc.size = size;
        alloc.timestamp = std::chrono::system_clock::now();
        alloc.file = file;
        alloc.line = line;
        alloc.function = function;
        alloc.allocated = true;
        
        pImpl->allocations.push_back(alloc);
        pImpl->totalAllocated += size;
        pImpl->currentUsage += size;
        
        if (pImpl->currentUsage > pImpl->peakUsage) {
            pImpl->peakUsage = pImpl->currentUsage;
        }
    }
    
    return address;
}

void MemoryDebugger::trackDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (address) {
        // Find and mark the allocation as deallocated
        for (auto& alloc : pImpl->allocations) {
            if (alloc.address == address && alloc.allocated) {
                alloc.allocated = false;
                pImpl->currentUsage -= alloc.size;
                std::free(address);
                break;
            }
        }
    }
}

void MemoryDebugger::recordMemoryUsage(size_t bytes, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Add a synthetic allocation record
    MemoryAllocation alloc;
    alloc.address = nullptr;
    alloc.size = bytes;
    alloc.timestamp = std::chrono::system_clock::now();
    alloc.file = "system";
    alloc.line = 0;
    alloc.function = context;
    alloc.allocated = true;
    
    pImpl->allocations.push_back(alloc);
    pImpl->totalAllocated += bytes;
    pImpl->currentUsage += bytes;
    
    if (pImpl->currentUsage > pImpl->peakUsage) {
        pImpl->peakUsage = pImpl->currentUsage;
    }
}

MemoryDebugger::MemoryInfo MemoryDebugger::getMemoryInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    MemoryInfo info;
    info.totalAllocated = pImpl->totalAllocated;
    info.currentUsage = pImpl->currentUsage;
    info.peakUsage = pImpl->peakUsage;
    info.allocationCount = pImpl->allocations.size();
    
    // Copy recent allocations
    if (info.allocationCount > 100) {
        info.recentAllocations.assign(pImpl->allocations.end() - 100, pImpl->allocations.end());
    } else {
        info.recentAllocations = pImpl->allocations;
    }
    
    return info;
}

std::string MemoryDebugger::getMemoryLeakReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Memory Leak Report ===\n";
    oss << "Total Allocations: " << pImpl->allocations.size() << "\n";
    oss << "Current Usage: " << pImpl->currentUsage << " bytes\n";
    oss << "Peak Usage: " << pImpl->peakUsage << " bytes\n";
    
    size_t leaked = 0;
    std::vector<MemoryAllocation> leaks;
    
    for (const auto& alloc : pImpl->allocations) {
        if (alloc.allocated) {
            leaked += alloc.size;
            leaks.push_back(alloc);
        }
    }
    
    oss << "Potentially Leaked Memory: " << leaked << " bytes\n";
    
    if (!leaks.empty()) {
        oss << "\nLeaked Allocations:\n";
        for (const auto& leak : leaks) {
            oss << "  " << leak.size << " bytes at " << leak.function << 
                " (" << leak.file << ":" << leak.line << ")\n";
        }
    }
    
    return oss.str();
}

void MemoryDebugger::clearTracking() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Free all allocated memory
    for (auto& alloc : pImpl->allocations) {
        if (alloc.allocated && alloc.address) {
            std::free(alloc.address);
        }
    }
    
    pImpl->allocations.clear();
    pImpl->totalAllocated = 0;
    pImpl->currentUsage = 0;
    pImpl->peakUsage = 0;
}

// NeuralActivityDebugger implementation
struct NeuralActivityDebugger::Impl {
    std::vector<NeuralActivity> activities;
    std::mutex mutex;
    std::unordered_map<uint64_t, float> neuronFiringRates;
    std::mutex ratesMutex;
};

NeuralActivityDebugger::NeuralActivityDebugger() : pImpl(std::make_unique<Impl>()) {}

NeuralActivityDebugger::~NeuralActivityDebugger() = default;

void NeuralActivityDebugger::recordActivity(const NeuralActivity& activity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->activities.push_back(activity);
    
    std::lock_guard<std::mutex> rateLock(pImpl->ratesMutex);
    pImpl->neuronFiringRates[activity.neuronId] = activity.firingRate;
    
    if (pImpl->activities.size() > 1000) {
        pImpl->activities.erase(pImpl->activities.begin());
    }
}

void NeuralActivityDebugger::recordSpike(uint64_t neuronId, double timestamp, float weight) {
    NeuralActivity activity;
    activity.neuronId = neuronId;
    activity.firingRate = 1.0f; // One spike
    activity.membranePotential = 0.0f; // Simplified
    activity.timestamp = std::chrono::system_clock::now();
    activity.spikeCount = 1;
    activity.averageWeight = weight;
    
    recordActivity(activity);
}

void NeuralActivityDebugger::recordFiringRate(uint64_t neuronId, float rate, double timestamp) {
    NeuralActivity activity;
    activity.neuronId = neuronId;
    activity.firingRate = rate;
    activity.membranePotential = 0.0f; // Simplified
    activity.timestamp = std::chrono::system_clock::now();
    activity.spikeCount = 0;
    activity.averageWeight = 0.0f;
    
    recordActivity(activity);
}

void NeuralActivityDebugger::recordConnectionChange(uint64_t source, uint64_t target, 
                                                  float oldWeight, float newWeight) {
    // Record connection changes as neural activities
    NeuralActivity activity;
    activity.neuronId = source;
    activity.firingRate = std::abs(newWeight - oldWeight);
    activity.membranePotential = newWeight;
    activity.timestamp = std::chrono::system_clock::now();
    activity.spikeCount = 0;
    activity.averageWeight = newWeight;
    
    recordActivity(activity);
}

NeuralActivityDebugger::NeuralStats NeuralActivityDebugger::getNeuralStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    NeuralStats stats;
    stats.totalNeurons = pImpl->neuronFiringRates.size();
    stats.activeNeurons = 0;
    double totalFiringRate = 0.0;
    double totalMembranePotential = 0.0;
    size_t totalSpikes = 0;
    float maxFiringRate = 0.0f;
    float minFiringRate = std::numeric_limits<float>::max();
    
    for (const auto& activity : pImpl->activities) {
        if (activity.firingRate > 0.0f) {
            stats.activeNeurons++;
        }
        
        totalFiringRate += activity.firingRate;
        totalMembranePotential += activity.membranePotential;
        totalSpikes += activity.spikeCount;
        
        if (activity.firingRate > maxFiringRate) {
            maxFiringRate = activity.firingRate;
        }
        if (activity.firingRate < minFiringRate) {
            minFiringRate = activity.firingRate;
        }
        
        stats.neuronFiringRates[activity.neuronId] = activity.firingRate;
    }
    
    size_t count = pImpl->activities.size();
    if (count > 0) {
        stats.averageFiringRate = totalFiringRate / count;
        stats.averageMembranePotential = totalMembranePotential / count;
    }
    
    stats.totalSpikes = totalSpikes;
    stats.maxFiringRate = maxFiringRate;
    stats.minFiringRate = minFiringRate;
    
    return stats;
}

std::string NeuralActivityDebugger::getNeuralActivityReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Neural Activity Report ===\n";
    
    NeuralStats stats = getNeuralStats();
    
    oss << "Total Neurons: " << stats.totalNeurons << "\n";
    oss << "Active Neurons: " << stats.activeNeurons << "\n";
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(4) 
        << stats.averageFiringRate << " Hz\n";
    oss << "Average Membrane Potential: " << std::fixed << std::setprecision(4)
        << stats.averageMembranePotential << " mV\n";
    oss << "Total Spikes: " << stats.totalSpikes << "\n";
    oss << "Max Firing Rate: " << std::fixed << std::setprecision(4) 
        << stats.maxFiringRate << " Hz\n";
    oss << "Min Firing Rate: " << std::fixed << std::setprecision(4)
        << stats.minFiringRate << " Hz\n";
    
    oss << "\nRecent Neural Activities:\n";
    for (const auto& activity : pImpl->activities) {
        oss << "  Neuron " << activity.neuronId << ": " << std::fixed << std::setprecision(4)
            << activity.firingRate << " Hz, " << std::fixed << std::setprecision(2)
            << activity.membranePotential << " mV at "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                activity.timestamp.time_since_epoch()).count() << "ms\n";
    }
    
    return oss.str();
}

std::vector<NeuralActivity> NeuralActivityDebugger::getRecentActivities(int count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<NeuralActivity> recent;
    recent.reserve(static_cast<size_t>(count));
    
    if (count > static_cast<int>(pImpl->activities.size())) {
        count = static_cast<int>(pImpl->activities.size());
    }
    
    auto startIt = pImpl->activities.end() - count;
    for (auto it = startIt; it != pImpl->activities.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

void NeuralActivityDebugger::clearTracking() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->activities.clear();
    
    std::lock_guard<std::mutex> rateLock(pImpl->ratesMutex);
    pImpl->neuronFiringRates.clear();
}

// SimulationStateDebugger implementation
struct SimulationStateDebugger::Impl {
    std::vector<SimulationState> states;
    std::vector<std::string> errors;
    std::mutex mutex;
    size_t totalSteps;
    double totalTime;
    size_t errorCount;
};

SimulationStateDebugger::SimulationStateDebugger() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalSteps = 0;
    pImpl->totalTime = 0.0;
    pImpl->errorCount = 0;
}

SimulationStateDebugger::~SimulationStateDebugger() = default;

void SimulationStateDebugger::recordState(const SimulationState& state) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->states.push_back(state);
    pImpl->totalSteps++;
    pImpl->totalTime += state.time;
    
    if (pImpl->states.size() > 1000) {
        pImpl->states.erase(pImpl->states.begin());
    }
}

void SimulationStateDebugger::recordStepTransition(uint64_t fromStep, uint64_t toStep, 
                                                  double timeAdvance, const std::string& reason) {
    SimulationState state;
    state.step = toStep;
    state.time = timeAdvance;
    state.neuronCount = 0; // Would be obtained from brain
    state.activeNeuronCount = 0;
    state.totalSpikeCount = 0;
    state.averageFiringRate = 0.0f;
    state.timestamp = std::chrono::system_clock::now();
    state.stateDescription = "Transition from step " + std::to_string(fromStep) + 
                           " to step " + std::to_string(toStep) + ": " + reason;
    
    recordState(state);
}

void SimulationStateDebugger::recordError(const std::string& error, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->errors.push_back("Error at " + context + ": " + error);
    pImpl->errorCount++;
}

SimulationStateDebugger::SimulationStats SimulationStateDebugger::getSimulationStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    SimulationStats stats;
    stats.currentStep = pImpl->states.empty() ? 0 : pImpl->states.back().step;
    stats.currentTime = pImpl->states.empty() ? 0.0 : pImpl->states.back().time;
    stats.totalSteps = pImpl->totalSteps;
    stats.errorCount = pImpl->errorCount;
    
    if (!pImpl->states.empty()) {
        double totalStepTime = 0.0;
        for (const auto& state : pImpl->states) {
            totalStepTime += state.time;
        }
        stats.averageStepTime = totalStepTime / pImpl->states.size();
        
        if (stats.averageStepTime > 0.0) {
            stats.simulationSpeed = pImpl->states.back().time / stats.averageStepTime;
        }
    }
    
    stats.recentStates = pImpl->states;
    stats.errors = pImpl->errors;
    
    return stats;
}

std::string SimulationStateDebugger::getSimulationReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Simulation Report ===\n";
    
    SimulationStats stats = getSimulationStats();
    
    oss << "Current Step: " << stats.currentStep << "\n";
    oss << "Current Time: " << std::fixed << std::setprecision(4) << stats.currentTime << " seconds\n";
    oss << "Total Steps: " << stats.totalSteps << "\n";
    oss << "Average Step Time: " << std::fixed << std::setprecision(4) 
        << stats.averageStepTime << " seconds\n";
    oss << "Simulation Speed: " << std::fixed << std::setprecision(2) 
        << stats.simulationSpeed << "x\n";
    oss << "Error Count: " << stats.errorCount << "\n";
    
    oss << "\nRecent Simulation States:\n";
    for (const auto& state : stats.recentStates) {
        oss << "  Step " << state.step << ", Time: " << std::fixed << std::setprecision(4) 
            << state.time << ", " << state.activeNeuronCount << " active neurons, "
            << state.totalSpikeCount << " spikes\n";
    }
    
    if (!stats.errors.empty()) {
        oss << "\nErrors:\n";
        for (const auto& error : stats.errors) {
            oss << "  " << error << "\n";
        }
    }
    
    return oss.str();
}

std::vector<SimulationState> SimulationStateDebugger::getRecentStates(int count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<SimulationState> recent;
    recent.reserve(static_cast<size_t>(count));
    
    if (count > static_cast<int>(pImpl->states.size())) {
        count = static_cast<int>(pImpl->states.size());
    }
    
    auto startIt = pImpl->states.end() - count;
    for (auto it = startIt; it != pImpl->states.end(); ++it) {
        recent.push_back(*it);
    }
    
    return recent;
}

void SimulationStateDebugger::clearTracking() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->states.clear();
    pImpl->errors.clear();
    pImpl->totalSteps = 0;
    pImpl->totalTime = 0.0;
    pImpl->errorCount = 0;
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    std::vector<std::shared_ptr<IDebugLogger>> debugLoggers;
    std::mutex mutex;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::addDebugLogger(std::shared_ptr<IDebugLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugLoggers.push_back(logger);
}

void EnhancedLogger::clearDebugLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugLoggers.clear();
}

void EnhancedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->debugLoggers) {
        logger->setLevel(level);
    }
}

void EnhancedLogger::log(LogLevel level, const std::string& message, 
                         const std::string& file, int line, 
                         const std::string& function,
                         const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = context;
    
    for (auto& logger : pImpl->debugLoggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line, 
                           const std::string& function, const std::string& context) {
    log(LogLevel::Debug, message, file, line, function, context);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line, 
                          const std::string& function, const std::string& context) {
    log(LogLevel::Info, message, file, line, function, context);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line, 
                             const std::string& function, const std::string& context) {
    log(LogLevel::Warning, message, file, line, function, context);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line, 
                           const std::string& function, const std::string& context) {
    log(LogLevel::Error, message, file, line, function, context);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line, 
                              const std::string& function, const std::string& context) {
    log(LogLevel::Critical, message, file, line, function, context);
}

void EnhancedLogger::performance(const std::string& message, const PerformanceMetric& metric,
                                 const std::string& file, int line, 
                                 const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Performance;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.performanceMetrics.push_back(metric);
    
    for (auto& logger : pImpl->debugLoggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::memory(const std::string& message, const MemoryAllocation& alloc,
                            const std::string& file, int line, 
                            const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Memory;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.memoryAllocations.push_back(alloc);
    
    for (auto& logger : pImpl->debugLoggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::neural(const std::string& message, const NeuralActivity& activity,
                            const std::string& file, int line, 
                            const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Neural;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.neuralActivities.push_back(activity);
    
    for (auto& logger : pImpl->debugLoggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::simulation(const std::string& message, const SimulationState& state,
                                const std::string& file, int line, 
                                const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogEntry entry;
    entry.level = LogLevel::Simulation;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.simulationStates.push_back(state);
    
    for (auto& logger : pImpl->debugLoggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logPerformance(const std::string& name, double value, 
                                    const std::string& unit,
                                    const std::string& context,
                                    const std::string& file, int line, 
                                    const std::string& function) {
    PerformanceMetric metric;
    metric.name = name;
    metric.value = value;
    metric.timestamp = std::chrono::system_clock::now();
    metric.unit = unit;
    metric.context = context;
    
    performance("Performance: " + name, metric, file, line, function);
}

void EnhancedLogger::logMemory(void* address, size_t size,
                               const std::string& file, int line, 
                               const std::string& function) {
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.timestamp = std::chrono::system_clock::now();
    alloc.file = file;
    alloc.line = line;
    alloc.function = function;
    alloc.allocated = true;
    
    memory("Memory allocation", alloc, file, line, function);
}

void EnhancedLogger::logNeuralActivity(uint64_t neuronId, float firingRate, float membranePotential,
                                       double timestamp,
                                       const std::string& file, int line, 
                                       const std::string& function) {
    NeuralActivity activity;
    activity.neuronId = neuronId;
    activity.firingRate = firingRate;
    activity.membranePotential = membranePotential;
    activity.timestamp = std::chrono::system_clock::now();
    activity.spikeCount = 0;
    activity.averageWeight = 0.0f;
    
    neural("Neural activity", activity, file, line, function);
}

void EnhancedLogger::logSimulationState(uint64_t step, double time, size_t neuronCount,
                                        size_t activeNeuronCount, float averageFiringRate,
                                        const std::string& file, int line, 
                                        const std::string& function) {
    SimulationState state;
    state.step = step;
    state.time = time;
    state.neuronCount = neuronCount;
    state.activeNeuronCount = activeNeuronCount;
    state.totalSpikeCount = 0; // Would be obtained from brain
    state.averageFiringRate = averageFiringRate;
    state.timestamp = std::chrono::system_clock::now();
    state.stateDescription = "Step " + std::to_string(step) + ", " + 
                           std::to_string(activeNeuronCount) + " active neurons";
    
    simulation("Simulation state", state, file, line, function);
}

bool EnhancedLogger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->debugLoggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    static std::shared_ptr<EnhancedLogger> globalLogger = nullptr;
    globalLogger = logger;
}

PerformanceProfiler& EnhancedLogger::getProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

MemoryDebugger& EnhancedLogger::getMemoryDebugger() {
    static MemoryDebugger memoryDebugger;
    return memoryDebugger;
}

NeuralActivityDebugger& EnhancedLogger::getNeuralDebugger() {
    static NeuralActivityDebugger neuralDebugger;
    return neuralDebugger;
}

SimulationStateDebugger& EnhancedLogger::getSimulationDebugger() {
    static SimulationStateDebugger simulationDebugger;
    return simulationDebugger;
}

void EnhancedLogger::enableRealTimeDebugging(bool enable) {
    if (enable) {
        ConsoleDebugLogger::getGlobal().startRealTimeMonitoring();
    } else {
        ConsoleDebugLogger::getGlobal().stopRealTimeMonitoring();
    }
}

bool EnhancedLogger::isRealTimeDebuggingEnabled() {
    return ConsoleDebugLogger::getGlobal().isRealTimeMonitoring();
}

std::string EnhancedLogger::generateDebugReport() {
    std::ostringstream oss;
    oss << "=== Comprehensive Debug Report ===\n\n";
    
    oss << ConsoleDebugLogger::getGlobal().generatePerformanceReport() << "\n";
    oss << ConsoleDebugLogger::getGlobal().generateMemoryReport() << "\n";
    oss << ConsoleDebugLogger::getGlobal().generateNeuralActivityReport() << "\n";
    oss << ConsoleDebugLogger::getGlobal().generateSimulationReport() << "\n";
    
    return oss.str();
}

} // namespace nlm
