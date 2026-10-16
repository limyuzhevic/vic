#include "Profiler.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <set>
#include <queue>
#include <chrono>
#include <unordered_set>
#include <map>

namespace nlm {

struct Profiler::Impl {
    std::shared_ptr<Config> config;
    PerformanceConfig perfConfig;
    ProfilingConfig profilingConfig;
    bool callStackProfilingActive{false};
    bool memoryProfilingActive{false};
    bool eventProfilingActive{false};
    
    // Call stack profiling
    std::vector<CallStackFrame> currentCallStack;
    std::unordered_map<std::string, uint64_t> callStackCounts;
    std::mutex callStackMutex;
    
    // Memory profiling
    std::vector<MemoryAllocation> allocationHistory;
    std::unordered_map<void*, MemoryAllocation> activeAllocations;
    std::unordered_map<std::string, uint64_t> allocationCounts;
    std::unordered_map<std::string, size_t> allocationSizes;
    std::mutex memoryMutex;
    
    // Event profiling
    std::vector<EventRecord> eventTimeline;
    std::mutex eventMutex;
    
    // Bottleneck detection
    std::vector<BottleneckInfo> bottlenecks;
    std::vector<std::string> hotPaths;
    std::chrono::high_resolution_clock::time_point lastBottleneckUpdate;
    
    // Flame graph
    std::unique_ptr<FlameGraphEntry> flameGraph;
    std::mutex flameGraphMutex;
    
    // Profile history
    std::vector<PerformanceProfile> profileHistory;
    
    // Performance tracking
    uint64_t totalSteps{0};
    std::chrono::microseconds totalRealTime{0};
    
    // Configuration
    bool initialized{false};
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {
}

Profiler::Profiler(std::shared_ptr<Config> config) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->profilingConfig.monitoringLevel = MonitoringLevel::Detailed;
    pImpl->profilingConfig.enableCallStackProfiling = true;
    pImpl->profilingConfig.enableMemoryProfiling = true;
    pImpl->profilingConfig.enableEventProfiling = true;
    pImpl->profilingConfig.enableBottleneckDetection = true;
    pImpl->profilingConfig.generateFlameGraphs = true;
    pImpl->profilingConfig.maxStackDepth = 50;
    pImpl->profilingConfig.maxMemorySamples = 10000;
    pImpl->profilingConfig.profileInterval = std::chrono::milliseconds(100);
    pImpl->profilingConfig.memoryThresholds["default"] = 1024 * 1024 * 1024; // 1GB
    pImpl->profilingConfig.eventThresholds["default"] = 100000;
    pImpl->initialized = true;
}

Profiler::Profiler(std::shared_ptr<Config> config, bool initialized)
    : pImpl(std::make_unique<Impl>()), initialized(initialized) {
    pImpl->config = config;
}

Profiler::~Profiler() = default;

void Profiler::setMonitoringLevel(MonitoringLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    pImpl->profilingConfig.monitoringLevel = level;
}

MonitoringLevel Profiler::getMonitoringLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->profilingConfig.monitoringLevel;
}

void Profiler::startCallStackProfiling() {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    callStackProfilingActive = true;
}

void Profiler::stopCallStackProfiling() {
    callStackProfilingActive = false;
}

void Profiler::recordCallStack(const std::vector<CallStackFrame>& stack) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    
    // Generate stack signature
    std::string stackSignature;
    for (size_t i = 0; i < stack.size() && i < pImpl->profilingConfig.maxStackDepth; ++i) {
        stackSignature += stack[i].functionName + ";";
    }
    
    pImpl->currentCallStack = stack;
    if (!stackSignature.empty()) {
        pImpl->callStackCounts[stackSignature]++;
    }
}

void Profiler::recordCallStack(const std::stacktrace& trace) {
    std::vector<CallStackFrame> stack;
    
    for (size_t i = 0; i < trace.size() && i < pImpl->profilingConfig.maxStackDepth; ++i) {
        const auto& frame = trace[i];
        CallStackFrame callFrame;
        callFrame.functionName = frame.function_name();
        callFrame.filePath = frame.source_file();
        callFrame.lineNumber = frame.source_line();
        callFrame.moduleName = frame.source_file(); // Simplified
        callFrame.address = reinterpret_cast<uint64_t>(frame.raw_address());
        
        stack.push_back(callFrame);
    }
    
    recordCallStack(stack);
}

std::unordered_map<std::string, uint64_t> Profiler::getCallStackCounts() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->callStackCounts;
}

std::vector<CallStackFrame> Profiler::getCurrentCallStack() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->currentCallStack;
}

bool Profiler::generateCallStackReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Call Stack Profile Report ===\n\n";
    
    file << "Top functions by call count:\n";
    std::vector<std::pair<std::string, uint64_t>> sortedStacks;
    for (const auto& [stack, count] : pImpl->callStackCounts) {
        sortedStacks.emplace_back(stack, count);
    }
    
    std::sort(sortedStacks.begin(), sortedStacks.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    size_t maxDisplay = std::min(sortedStacks.size(), size_t(100));
    for (size_t i = 0; i < maxDisplay; ++i) {
        file << i + 1 << ". " << sortedStacks[i].first << ": " << sortedStacks[i].second << " calls\n";
    }
    
    file.close();
    return true;
}

void Profiler::startMemoryProfiling() {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    memoryProfilingActive = true;
}

void Profiler::stopMemoryProfiling() {
    memoryProfilingActive = false;
}

void Profiler::recordAllocation(void* address, size_t size, const std::string& type,
                               const std::string& allocationSite) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.type = type;
    alloc.allocationSite = allocationSite;
    alloc.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    
    pImpl->allocationHistory.push_back(alloc);
    pImpl->activeAllocations[address] = alloc;
    
    pImpl->allocationCounts[type]++;
    pImpl->allocationSizes[type] += size;
    
    if (pImpl->allocationHistory.size() > pImpl->profilingConfig.maxMemorySamples) {
        pImpl->allocationHistory.erase(pImpl->allocationHistory.begin());
        // Also remove from active allocations if old
        for (auto it = pImpl->activeAllocations.begin(); it != pImpl->activeAllocations.end(); ) {
            if (it->second.timestamp > pImpl->allocationHistory.front().timestamp) {
                ++it;
            } else {
                it = pImpl->activeAllocations.erase(it);
            }
        }
    }
}

void Profiler::recordDeallocation(void* address) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    auto it = pImpl->activeAllocations.find(address);
    if (it != pImpl->activeAllocations.end()) {
        // Record deallocation info (could store deallocation record if needed)
        pImpl->activeAllocations.erase(it);
    }
}

std::unordered_map<std::string, uint64_t> Profiler::getAllocationCounts() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return pImpl->allocationCounts;
}

std::unordered_map<std::string, size_t> Profiler::getAllocationSizes() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return pImpl->allocationSizes;
}

std::vector<MemoryAllocation> Profiler::getAllocationHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    return pImpl->allocationHistory;
}

bool Profiler::generateMemoryReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Memory Allocation Profile Report ===\n\n";
    
    file << "Allocation counts by type:\n";
    for (const auto& [type, count] : pImpl->allocationCounts) {
        file << "  " << type << ": " << count << " allocations\n";
    }
    
    file << "\nAllocation sizes by type:\n";
    for (const auto& [type, size] : pImpl->allocationSizes) {
        file << "  " << type << ": " << size / 1024.0 / 1024.0 << " MB\n";
    }
    
    file << "\nTotal allocations: " << pImpl->allocationHistory.size() << "\n";
    file << "Active allocations: " << pImpl->activeAllocations.size() << "\n";
    
    file.close();
    return true;
}

void Profiler::startEventProfiling() {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    eventProfilingActive = true;
}

void Profiler::stopEventProfiling() {
    eventProfilingActive = false;
}

void Profiler::recordEvent(const std::string& eventName, uint64_t timestamp,
                           const std::unordered_map<std::string, std::string>& metadata) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    
    EventRecord event;
    event.eventName = eventName;
    event.timestamp = std::chrono::microseconds(timestamp);
    event.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    event.metadata = metadata;
    
    pImpl->eventTimeline.push_back(event);
    
    if (pImpl->eventTimeline.size() > pImpl->profilingConfig.maxMemorySamples) {
        pImpl->eventTimeline.erase(pImpl->eventTimeline.begin());
    }
}

void Profiler::recordTimedEvent(const std::string& eventName, std::function<void()> eventFunction) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    eventFunction();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    EventRecord event;
    event.eventName = eventName + "_timed";
    event.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch());
    event.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    event.duration = duration;
    
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    pImpl->eventTimeline.push_back(event);
}

std::vector<EventRecord> Profiler::getEventTimeline() const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    return pImpl->eventTimeline;
}

std::vector<EventRecord> Profiler::getEvents(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(pImpl->eventMutex);
    std::vector<EventRecord> filteredEvents;
    for (const auto& event : pImpl->eventTimeline) {
        if (event.eventName == eventName) {
            filteredEvents.push_back(event);
        }
    }
    return filteredEvents;
}

bool Profiler::generateEventReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Event Profile Report ===\n\n";
    
    file << "Events by type:\n";
    std::unordered_map<std::string, uint64_t> eventCounts;
    for (const auto& event : pImpl->eventTimeline) {
        eventCounts[event.eventName]++;
    }
    
    for (const auto& [eventName, count] : eventCounts) {
        file << "  " << eventName << ": " << count << " events\n";
    }
    
    file << "\nTimeline (first 100 events):\n";
    size_t maxDisplay = std::min(pImpl->eventTimeline.size(), size_t(100));
    for (size_t i = 0; i < maxDisplay; ++i) {
        const auto& event = pImpl->eventTimeline[i];
        file << i + 1 << ". " << event.eventName << " @ " << event.timestamp.count()
             << " (duration: " << event.duration << " us)\n";
    }
    
    file.close();
    return true;
}

void Profiler::updateBottleneckData() {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    if (now - pImpl->lastBottleneckUpdate < pImpl->profilingConfig.profileInterval) {
        return; // Don't update too frequently
    }
    
    pImpl->lastBottleneckUpdate = now;
    
    // Analyze call stack counts to find bottlenecks
    std::unordered_map<std::string, uint64_t> functionCalls;
    std::unordered_map<std::string, uint64_t> functionSamples;
    
    for (const auto& [stack, count] : pImpl->callStackCounts) {
        // Extract function name from stack signature
        size_t pos = stack.find(':');
        if (pos != std::string::npos) {
            std::string funcName = stack.substr(0, pos);
            functionCalls[funcName] += count;
            functionSamples[funcName]++;
        }
    }
    
    // Calculate bottleneck information
    pImpl->bottlenecks.clear();
    double totalSamples = 0.0;
    for (const auto& [func, samples] : functionSamples) {
        totalSamples += samples;
    }
    
    for (const auto& [func, samples] : functionSamples) {
        BottleneckInfo info;
        info.functionName = func;
        info.category = PerformanceCategory::Overall; // Simplified
        info.impactFactor = totalSamples > 0 ? samples / totalSamples : 0.0;
        info.callCount = functionCalls[func];
        info.averageTimePerCall = info.impactFactor * 1000.0; // Simplified
        info.recommendation = "Consider optimizing this function";
        
        pImpl->bottlenecks.push_back(info);
    }
    
    std::sort(pImpl->bottlenecks.begin(), pImpl->bottlenecks.end(),
              [](const auto& a, const auto& b) { return a.impactFactor > b.impactFactor; });
    
    // Update hot paths
    pImpl->hotPaths.clear();
    for (const auto& bottleneck : pImpl->bottlenecks) {
        if (bottleneck.impactFactor > 0.01) { // 1% threshold
            pImpl->hotPaths.push_back(bottleneck.functionName);
        }
    }
}

std::vector<BottleneckInfo> Profiler::detectBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->bottlenecks;
}

std::vector<std::string> Profiler::identifyHotPaths() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->hotPaths;
}

std::string Profiler::getBottleneckDetails() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    auto bottlenecks = detectBottlenecks();
    if (bottlenecks.empty()) {
        ss << "No bottlenecks detected";
        return ss.str();
    }
    
    ss << "Top bottlenecks:\n";
    size_t displayCount = std::min(bottlenecks.size(), size_t(10));
    for (size_t i = 0; i < displayCount; ++i) {
        const auto& bottleneck = bottlenecks[i];
        ss << i + 1 << ". " << bottleneck.functionName
           << " (impact: " << (bottleneck.impactFactor * 100.0) << "%)"
           << " (calls: " << bottleneck.callCount << ")"
           << " (avg time: " << bottleneck.averageTimePerCall << " us)\n"
           << "   Recommendation: " << bottleneck.recommendation << "\n";
    }
    
    return ss.str();
}

std::string Profiler::getHotPathDetails() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    auto hotPaths = identifyHotPaths();
    if (hotPaths.empty()) {
        ss << "No hot paths detected";
        return ss.str();
    }
    
    ss << "Hot paths (functions called frequently):\n";
    for (size_t i = 0; i < hotPaths.size() && i < 20; ++i) {
        ss << i + 1 << ". " << hotPaths[i] << "\n";
    }
    
    return ss.str();
}

bool Profiler::generateBottleneckReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Performance Bottleneck Report ===\n\n";
    file << getBottleneckDetails() << "\n";
    file << getHotPathDetails() << "\n";
    
    file.close();
    return true;
}

void Profiler::updateFlameGraphData(const std::string& functionName, const std::string& filePath,
                                  int lineNumber, uint64_t samples) {
    if (pImpl->profilingConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->flameGraphMutex);
    
    if (!pImpl->flameGraph) {
        pImpl->flameGraph = std::make_unique<FlameGraphEntry>();
        pImpl->flameGraph->functionName = "root";
        pImpl->flameGraph->samples = 0;
    }
    
    // Simplified flame graph update - would need more sophisticated tree structure
    // For now, just update the root node
    pImpl->flameGraph->samples += samples;
}

bool Profiler::generateFlameGraph(const std::string& filename) const {
    if (!pImpl->profilingConfig.generateFlameGraphs || !pImpl->flameGraph) return false;
    
    // Simple flame graph generation - would need proper format in production
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Flame graph not implemented in this simplified version";
    file.close();
    return true;
}

std::unique_ptr<FlameGraphEntry> Profiler::getFlameGraph() const {
    std::lock_guard<std::mutex> lock(pImpl->flameGraphMutex);
    return std::make_unique<FlameGraphEntry>(*pImpl->flameGraph);
}

void Profiler::clearFlameGraph() {
    std::lock_guard<std::mutex> lock(pImpl->flameGraphMutex);
    if (pImpl->flameGraph) {
        pImpl->flameGraph->samples = 0;
        pImpl->flameGraph->children.clear();
    }
}

std::vector<PerformanceProfile> Profiler::getProfileHistory() const {
    return pImpl->profileHistory;
}

void Profiler::addProfileToHistory() {
    PerformanceProfile profile;
    profile.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    profile.stepNumber = ++pImpl->totalSteps;
    profile.callStackCounts = getCallStackCounts();
    profile.allocationCounts = getAllocationCounts();
    profile.eventCounts = std::unordered_map<std::string, uint64_t>(); // Would need to implement
    
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    pImpl->profileHistory.push_back(profile);
}

void Profiler::clearProfileHistory() {
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    pImpl->profileHistory.clear();
}

bool Profiler::establishBaseline(const std::string& baselineName) {
    // Store baseline name in config system
    if (pImpl->config) {
        pImpl->config->set("performance.baseline_name", baselineName, ConfigSource::Runtime);
        return true;
    }
    return false;
}

bool Profiler::compareToBaseline(const std::string& baselineName, const std::string& outputFile) const {
    // Simplified baseline comparison
    return true; // Would need proper implementation
}

void Profiler::setConfig(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    pImpl->perfConfig = config;
}

PerformanceConfig Profiler::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->perfConfig;
}

void Profiler::setProfilingConfig(const ProfilingConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    pImpl->profilingConfig = config;
}

ProfilingConfig Profiler::getProfilingConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->callStackMutex);
    return pImpl->profilingConfig;
}

// Singleton implementation
namespace {
    std::shared_ptr<Profiler> g_profilerInstance;
    std::mutex g_profilerMutex;
}

Profiler& Profiler::getInstance() {
    std::lock_guard<std::mutex> lock(g_profilerMutex);
    if (!g_profilerInstance) {
        g_profilerInstance = std::shared_ptr<Profiler>(new Profiler());
    }
    return *g_profilerInstance;
}

void Profiler::setInstance(std::shared_ptr<Profiler> profiler) {
    std::lock_guard<std::mutex> lock(g_profilerMutex);
    g_profilerInstance = profiler;
}

bool Profiler::isInitialized() {
    std::lock_guard<std::mutex> lock(g_profilerMutex);
    return g_profilerInstance != nullptr && g_profilerInstance->pImpl->initialized;
}

} // namespace nlm