// Performance profiling and optimization tools for NLM
// Provides comprehensive profiling capabilities for performance analysis and optimization

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <thread>
#include <future>

namespace nlm {

// Performance metric types
enum class MetricType {
    Counter,
    Timer,
    Memory,
    CPU,
    Custom
};

// Performance metric data
struct PerformanceMetric {
    std::string name;
    MetricType type;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> tags;
    
    PerformanceMetric(const std::string& name, MetricType type, double value, 
                     const std::string& unit = "", const std::string& tags = "")
        : name(name), type(type), value(value), unit(unit), timestamp(std::chrono::system_clock::now()) {
        if (!tags.empty()) {
            parseTags(tags);
        }
    }
    
private:
    void parseTags(const std::string& tagsStr) {
        size_t pos = 0;
        while ((pos = tagsStr.find('=', pos)) != std::string::npos) {
            size_t keyEnd = pos;
            pos = tagsStr.find(',', pos);
            size_t valueEnd = pos == std::string::npos ? tagsStr.length() : pos;
            
            std::string key = tagsStr.substr(0, keyEnd);
            std::string value = tagsStr.substr(keyEnd + 1, valueEnd - keyEnd - 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            tags[key] = value;
        }
    }
};

// Performance profile data
struct PerformanceProfile {
    std::string name;
    std::string description;
    std::chrono::duration<double> startTime;
    std::chrono::duration<double> endTime;
    std::vector<PerformanceMetric> metrics;
    std::vector<std::string> systemInfo;
    std::string platformInfo;
    std::string compilerInfo;
    
    PerformanceProfile(const std::string& name, const std::string& description = "")
        : name(name), description(description), startTime(0.0) {}
    
    double getDuration() const {
        return (endTime - startTime).count();
    }
    
    size_t getMetricCount() const {
        return metrics.size();
    }
    
    PerformanceMetric* getMetric(const std::string& name) {
        for (auto& metric : metrics) {
            if (metric.name == name) {
                return &metric;
            }
        }
        return nullptr;
    }
    
    const PerformanceMetric* getMetric(const std::string& name) const {
        for (const auto& metric : metrics) {
            if (metric.name == name) {
                return &metric;
            }
        }
        return nullptr;
    }
};

// CPU performance counter
class CPUMonitor {
public:
    CPUMonitor() = default;
    
    struct CPUUsage {
        double user;
        double system;
        double idle;
        double nice;
        double iowait;
        double irq;
        double softirq;
        double steal;
        double total;
    };
    
    CPUUsage getUsage() {
        // Platform-specific implementation would go here
        // For now, return dummy data
        CPUUsage usage;
        usage.user = 10.0;
        usage.system = 5.0;
        usage.idle = 80.0;
        usage.nice = 0.0;
        usage.iowait = 2.0;
        usage.irq = 0.5;
        usage.softirq = 1.0;
        usage.steal = 0.0;
        usage.total = 100.0;
        return usage;
    }
    
    double getCpuUsage() {
        return getUsage().total;
    }
    
    std::string getCpuInfo() {
        // Platform-specific CPU info
        return "CPU Info: (Platform-specific data would go here)";
    }
    
    int getCoreCount() {
        // Platform-specific core count
        return std::thread::hardware_concurrency();
    }
    
    double getCacheInfo() {
        // Platform-specific cache info
        return 0.0;
    }
    
    std::string getCacheHierarchy() {
        // Platform-specific cache hierarchy
        return "L1: 32KB, L2: 256KB, L3: 8192KB (Platform-specific data)";
    }
};

// Memory performance monitor
class MemoryMonitor {
public:
    MemoryMonitor() = default;
    
    struct MemoryStats {
        size_t total;
        size_t used;
        size_t free;
        size_t cached;
        size_t buffers;
        size_t swapTotal;
        size_t swapUsed;
        double usagePercentage;
        double swapPercentage;
    };
    
    MemoryStats getMemoryStats() {
        // Platform-specific implementation
        MemoryStats stats;
        stats.total = 8589934592; // 8GB
        stats.used = 4294967296; // 4GB
        stats.free = 2147483648; // 2GB
        stats.cached = 1073741824; // 1GB
        stats.buffers = 536870912; // 512MB
        stats.swapTotal = 4294967296; // 4GB
        stats.swapUsed = 1073741824; // 1GB
        stats.usagePercentage = (double)stats.used / stats.total * 100.0;
        stats.swapPercentage = (double)stats.swapUsed / stats.swapTotal * 100.0;
        return stats;
    }
    
    double getMemoryUsage() {
        auto stats = getMemoryStats();
        return stats.usagePercentage;
    }
    
    size_t getAvailableMemory() {
        auto stats = getMemoryStats();
        return stats.free + stats.cached + stats.buffers;
    }
    
    size_t getUsedMemory() {
        auto stats = getMemoryStats();
        return stats.used;
    }
    
    std::string getMemoryInfo() {
        auto stats = getMemoryStats();
        std::stringstream ss;
        ss << "Memory: " << stats.used / (1024 * 1024) << "MB used / "
           << stats.total / (1024 * 1024) << "MB total ("
           << stats.usagePercentage << "% used)";
        return ss.str();
    }
    
    std::string getPageInfo() {
        // Platform-specific page info
        return "Page size: 4096 bytes (Platform-specific data)";
    }
    
    std::string getSwapInfo() {
        auto stats = getMemoryStats();
        std::stringstream ss;
        ss << "Swap: " << stats.swapUsed / (1024 * 1024) << "MB used / "
           << stats.swapTotal / (1024 * 1024) << "MB total ("
           << stats.swapPercentage << "% used)";
        return ss.str();
    }
    
    std::vector<std::string> getMemoryRegions() {
        // Platform-specific memory regions
        return {"Region 1: 0x0000-0x1000", "Region 2: 0x1000-0x2000"};
    }
};

// System performance profiler
class SystemProfiler {
public:
    SystemProfiler() = default;
    
    PerformanceProfile startProfile(const std::string& name, const std::string& description = "") {
        PerformanceProfile profile(name, description);
        profile.startTime = std::chrono::steady_clock::now();
        
        // Record system info
        profile.systemInfo = getSystemInfo();
        profile.platformInfo = getPlatformInfo();
        profile.compilerInfo = getCompilerInfo();
        
        return profile;
    }
    
    void endProfile(PerformanceProfile& profile) {
        profile.endTime = std::chrono::steady_clock::now();
    }
    
    void recordMetric(PerformanceProfile& profile, const PerformanceMetric& metric) {
        profile.metrics.push_back(metric);
    }
    
    std::vector<PerformanceProfile> startMultiProfile(const std::vector<std::string>& names) {
        std::vector<PerformanceProfile> profiles;
        for (const auto& name : names) {
            profiles.push_back(startProfile(name));
        }
        return profiles;
    }
    
    void endMultiProfile(std::vector<PerformanceProfile>& profiles) {
        for (auto& profile : profiles) {
            endProfile(profile);
        }
    }
    
    // Profiling system utilities
    std::vector<std::string> getSystemInfo() {
        std::vector<std::string> info;
        
        // Get CPU info
        CPUMonitor cpu;
        info.push_back(cpu.getCpuInfo());
        
        // Get memory info
        MemoryMonitor memory;
        info.push_back(memory.getMemoryInfo());
        
        // Get platform info
        info.push_back(getPlatformInfo());
        
        // Get compiler info
        info.push_back(getCompilerInfo());
        
        // Get core count
        CPUMonitor cpuMonitor;
        info.push_back("Cores: " + std::to_string(cpuMonitor.getCoreCount()));
        
        return info;
    }
    
    std::string getPlatformInfo() {
        // Platform-specific implementation
        return "Platform: Linux (Platform-specific data)";
    }
    
    std::string getCompilerInfo() {
        // Compiler info
        return "Compiler: GNU GCC (Platform-specific data)";
    }
    
    std::vector<std::string> getProcessInfo() {
        // Platform-specific process info
        return {"Process ID: 12345", "Parent Process: 1234", "Thread Count: 4"};
    }
    
    std::vector<std::string> getThreadInfo() {
        // Platform-specific thread info
        return {"Thread 0: Active", "Thread 1: Sleeping", "Thread 2: Ready"};
    }
    
    std::vector<std::string> getFileDescriptorInfo() {
        // Platform-specific file descriptor info
        return {"FD 0: stdin", "FD 1: stdout", "FD 2: stderr"};
    }
    
    std::vector<std::string> getNetworkInfo() {
        // Platform-specific network info
        return {"Interface: eth0", "Address: 192.168.1.100", "Netmask: 255.255.255.0"};
    }
    
    // Performance analysis
    struct PerformanceAnalysis {
        std::string name;
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        double diskUsage;
        double responseTime;
        double throughput;
        std::string bottlenecks;
        std::string recommendations;
    };
    
    PerformanceAnalysis analyzeProfile(const PerformanceProfile& profile) {
        PerformanceAnalysis analysis;
        analysis.name = profile.name;
        
        // Calculate metrics
        double totalTime = profile.getDuration();
        
        // Find timing metrics
        double maxTime = 0.0;
        double totalTimeMetrics = 0.0;
        size_t timeMetricCount = 0;
        
        for (const auto& metric : profile.metrics) {
            if (metric.type == MetricType::Timer) {
                if (metric.value > maxTime) maxTime = metric.value;
                totalTimeMetrics += metric.value;
                timeMetricCount++;
            } else if (metric.type == MetricType::Counter) {
                totalTimeMetrics += metric.value;
                timeMetricCount++;
            }
        }
        
        if (timeMetricCount > 0) {
            analysis.responseTime = totalTimeMetrics / timeMetricCount;
        } else {
            analysis.responseTime = totalTime;
        }
        
        // Find throughput metrics
        double totalCounters = 0.0;
        double maxCounter = 0.0;
        
        for (const auto& metric : profile.metrics) {
            if (metric.type == MetricType::Counter) {
                totalCounters += metric.value;
                if (metric.value > maxCounter) maxCounter = metric.value;
            }
        }
        
        if (totalTime > 0.0) {
            analysis.throughput = totalCounters / totalTime;
        } else {
            analysis.throughput = 0.0;
        }
        
        // Calculate resource usage
        CPUMonitor cpu;
        analysis.cpuUsage = cpu.getCpuUsage();
        
        MemoryMonitor memory;
        analysis.memoryUsage = memory.getMemoryUsage();
        
        // Identify bottlenecks
        analysis.bottlenecks = "Analysis would identify bottlenecks here (CPU, memory, I/O, etc.)";
        
        // Generate recommendations
        analysis.recommendations = "Recommendations for optimization would go here";
        
        return analysis;
    }
    
    // Export profile to different formats
    std::string exportProfileToJSON(const PerformanceProfile& profile) {
        // JSON export implementation
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"name\": \"" << profile.name << "\",\n";
        ss << "  \"description\": \"" << profile.description << "\",\n";
        ss << "  \"duration\": " << profile.getDuration() << ",\n";
        ss << "  \"metrics\": [\n";
        
        for (size_t i = 0; i < profile.metrics.size(); ++i) {
            const auto& metric = profile.metrics[i];
            ss << "    {\n";
            ss << "      \"name\": \"" << metric.name << "\",\n";
            ss << "      \"type\": \"" << static_cast<int>(metric.type) << "\",\n";
            ss << "      \"value\": " << metric.value << ",\n";
            ss << "      \"unit\": \"" << metric.unit << "\",\n";
            ss << "      \"timestamp\": \"" << metric.timestamp.time_since_epoch().count() << "\"\n";
            
            if (i < profile.metrics.size() - 1) {
                ss << "    },\n";
            } else {
                ss << "    }\n";
            }
        }
        
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string exportProfileToCSV(const PerformanceProfile& profile) {
        // CSV export implementation
        std::stringstream ss;
        ss << "Metric,Value,Unit,Timestamp\n";
        
        for (const auto& metric : profile.metrics) {
            ss << metric.name << "," << metric.value << "," << metric.unit << ","
               << std::chrono::system_clock::to_time_t(metric.timestamp) << "\n";
        }
        
        return ss.str();
    }
    
    std::string exportProfileToText(const PerformanceProfile& profile) {
        // Text export implementation
        std::stringstream ss;
        ss << "Performance Profile: " << profile.name << "\n";
        ss << "Description: " << profile.description << "\n";
        ss << "Duration: " << profile.getDuration() << " seconds\n\n";
        ss << "Metrics:\n";
        
        for (const auto& metric : profile.metrics) {
            ss << "  " << metric.name << ": " << metric.value << " " << metric.unit << "\n";
        }
        
        return ss.str();
    }
    
    // Profile comparison
    std::string compareProfiles(const PerformanceProfile& profile1, const PerformanceProfile& profile2) {
        // Comparison implementation
        std::stringstream ss;
        ss << "Profile Comparison: " << profile1.name << " vs " << profile2.name << "\n\n";
        
        // Compare duration
        double duration1 = profile1.getDuration();
        double duration2 = profile2.getDuration();
        
        ss << "Duration Comparison:\n";
        ss << "  " << profile1.name << ": " << duration1 << " seconds\n";
        ss << "  " << profile2.name << ": " << duration2 << " seconds\n";
        
        if (duration2 > 0.0) {
            double improvement = (duration1 - duration2) / duration2 * 100.0;
            ss << "  Improvement: " << improvement << "%\n";
        }
        
        return ss.str();
    }
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler() = default;
    
    struct AllocationInfo {
        void* address;
        size_t size;
        std::string type;
        std::string stackTrace;
        std::chrono::system_clock::time_point timestamp;
    };
    
    struct MemorySnapshot {
        std::chrono::system_clock::time_point timestamp;
        std::vector<AllocationInfo> allocations;
        std::vector<AllocationInfo> deallocations;
        std::vector<AllocationInfo> leaks;
    };
    
    void startTracking() {
        // Start memory tracking
        tracking_ = true;
        snapshotInterval_ = 1.0f; // 1 second
    }
    
    void stopTracking() {
        tracking_ = false;
    }
    
    MemorySnapshot takeSnapshot() {
        MemorySnapshot snapshot;
        snapshot.timestamp = std::chrono::system_clock::now();
        // Take memory snapshot (platform-specific implementation)
        return snapshot;
    }
    
    std::vector<MemorySnapshot> takeSnapshots(size_t count) {
        std::vector<MemorySnapshot> snapshots;
        for (size_t i = 0; i < count; ++i) {
            snapshots.push_back(takeSnapshot());
            std::this_thread::sleep_for(std::chrono::seconds(snapshotInterval_));
        }
        return snapshots;
    }
    
    std::string analyzeSnapshots(const std::vector<MemorySnapshot>& snapshots) {
        // Analyze memory snapshots for leaks and issues
        std::stringstream ss;
        ss << "Memory Analysis Report\n\n";
        
        if (snapshots.size() < 2) {
            ss << "Insufficient snapshots for analysis.\n";
            return ss.str();
        }
        
        size_t totalAllocations = 0;
        size_t totalDeallocations = 0;
        
        for (const auto& snapshot : snapshots) {
            totalAllocations += snapshot.allocations.size();
            totalDeallocations += snapshot.deallocations.size();
        }
        
        ss << "Total Allocations: " << totalAllocations << "\n";
        ss << "Total Deallocations: " << totalDeallocations << "\n";
        
        size_t leaks = 0;
        for (const auto& snapshot : snapshots) {
            leaks += snapshot.leaks.size();
        }
        
        ss << "Total Leaks: " << leaks << "\n";
        
        if (leaks > 0) {
            ss << "\nPotential Memory Leaks:\n";
            for (size_t i = 0; i < snapshots.size() && i < leaks; ++i) {
                ss << "  Leak " << i << ": " << snapshots[i].leaks.size() << " bytes\n";
            }
        }
        
        return ss.str();
    }
    
    std::string getMemoryUsageReport() {
        MemoryMonitor memory;
        auto stats = memory.getMemoryStats();
        
        std::stringstream ss;
        ss << "Memory Usage Report\n\n";
        ss << "Total Memory: " << stats.total / (1024 * 1024) << "MB\n";
        ss << "Used Memory: " << stats.used / (1024 * 1024) << "MB\n";
        ss << "Free Memory: " << stats.free / (1024 * 1024) << "MB\n";
        ss << "Cache: " << stats.cached / (1024 * 1024) << "MB\n";
        ss << "Buffers: " << stats.buffers / (1024 * 1024) << "MB\n";
        ss << "Swap Total: " << stats.swapTotal / (1024 * 1024) << "MB\n";
        ss << "Swap Used: " << stats.swapUsed / (1024 * 1024) << "MB\n";
        ss << "Usage Percentage: " << stats.usagePercentage << "%\n";
        ss << "Swap Percentage: " << stats.swapPercentage << "%\n";
        
        return ss.str();
    }
    
    std::vector<std::string> getAllocationPatterns() {
        // Analyze allocation patterns
        return {"Pattern 1: Frequent small allocations", "Pattern 2: Large single allocations"};
    }
    
    std::vector<std::string> getDeallocationPatterns() {
        // Analyze deallocation patterns
        return {"Pattern 1: Regular deallocations", "Pattern 2: Delayed deallocations"};
    }
    
    std::string getMemoryOptimizationSuggestions() {
        // Generate memory optimization suggestions
        return "1. Use memory pools for frequently allocated objects\n"
               "2. Implement object pooling for reusable resources\n"
               "3. Use smart pointers for automatic memory management\n"
               "4. Profile memory usage to identify bottlenecks\n"
               "5. Use static analysis tools to detect memory issues";
    }

private:
    bool tracking_ = false;
    float snapshotInterval_ = 1.0f;
};

// Code profiler
class CodeProfiler {
public:
    CodeProfiler() = default;
    
    struct CodeProfile {
        std::string functionName;
        std::string fileName;
        int lineNumber;
        double executionTime;
        double cpuTime;
        double memoryUsage;
        size_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
        std::string module;
    };
    
    struct ProfileSession {
        std::string name;
        std::chrono::steady_clock::time_point startTime;
        std::vector<CodeProfile> profiles;
        std::vector<std::string> tags;
    };
    
    ProfileSession startSession(const std::string& name) {
        ProfileSession session;
        session.name = name;
        session.startTime = std::chrono::steady_clock::now();
        return session;
    }
    
    void endSession(ProfileSession& session) {
        // End the profiling session
        session.profiles.clear();
    }
    
    void profileFunction(const std::string& functionName, const std::string& fileName, 
                        int lineNumber, double executionTime, double cpuTime,
                        double memoryUsage, size_t callCount) {
        CodeProfile profile;
        profile.functionName = functionName;
        profile.fileName = fileName;
        profile.lineNumber = lineNumber;
        profile.executionTime = executionTime;
        profile.cpuTime = cpuTime;
        profile.memoryUsage = memoryUsage;
        profile.callCount = callCount;
        profile.averageTime = executionTime / callCount;
        profile.minTime = executionTime; // Would need to track min/max
        profile.maxTime = executionTime;
        profile.module = extractModuleName(fileName);
        
        // Add to current session (implementation would need session tracking)
        // For now, just print to console
        std::cout << "Profile: " << functionName << " in " << fileName << ":"
                  << lineNumber << " - " << executionTime << "ms\n";
    }
    
    std::string extractModuleName(const std::string& fileName) {
        size_t lastSlash = fileName.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            return fileName.substr(lastSlash + 1);
        }
        return fileName;
    }
    
    std::vector<CodeProfile> getHotspots(const ProfileSession& session) {
        // Get hotspots (functions with highest execution time)
        std::vector<CodeProfile> hotspots = session.profiles;
        std::sort(hotspots.begin(), hotspots.end(), 
                 [](const CodeProfile& a, const CodeProfile& b) {
                     return a.executionTime > b.executionTime;
                 });
        
        // Return top 10 hotspots
        size_t count = std::min(size_t(10), hotspots.size());
        return std::vector<CodeProfile>(hotspots.begin(), hotspots.begin() + count);
    }
    
    std::vector<CodeProfile> getBottlenecks(const ProfileSession& session) {
        // Get bottlenecks (functions with highest average time)
        std::vector<CodeProfile> bottlenecks = session.profiles;
        std::sort(bottlenecks.begin(), bottlenecks.end(), 
                 [](const CodeProfile& a, const CodeProfile& b) {
                     return a.averageTime > b.averageTime;
                 });
        
        // Return top 5 bottlenecks
        size_t count = std::min(size_t(5), bottlenecks.size());
        return std::vector<CodeProfile>(bottlenecks.begin(), bottlenecks.begin() + count);
    }
    
    std::string generateReport(const ProfileSession& session) {
        // Generate profiling report
        std::stringstream ss;
        ss << "Code Profile Report: " << session.name << "\n";
        ss << "Session Duration: " << 
           std::chrono::duration<double>(std::chrono::steady_clock::now() - 
                                        session.startTime).count() << " seconds\n\n";
        
        ss << "Top Functions by Execution Time:\n";
        auto hotspots = getHotspots(session);
        for (size_t i = 0; i < hotspots.size(); ++i) {
            const auto& profile = hotspots[i];
            ss << std::setw(3) << i + 1 << ". " << profile.functionName << "() "
               << "in " << profile.fileName << ":" << profile.lineNumber << "\n"
               << "   Time: " << profile.executionTime << "ms "
               << "(avg: " << profile.averageTime << "ms, "
               << "calls: " << profile.callCount << ")\n";
        }
        
        ss << "\nBottleneck Functions:\n";
        auto bottlenecks = getBottlenecks(session);
        for (size_t i = 0; i < bottlenecks.size(); ++i) {
            const auto& profile = bottlenecks[i];
            ss << std::setw(3) << i + 1 << ". " << profile.functionName << "()\n"
               << "   Avg Time: " << profile.averageTime << "ms\n"
               << "   Total Time: " << profile.executionTime << "ms\n"
               << "   Call Count: " << profile.callCount << "\n";
        }
        
        return ss.str();
    }
    
    std::string getOptimizationRecommendations(const ProfileSession& session) {
        // Generate optimization recommendations
        std::stringstream ss;
        ss << "Optimization Recommendations:\n\n";
        
        auto bottlenecks = getBottlenecks(session);
        
        if (!bottlenecks.empty()) {
            ss << "1. Optimize bottleneck functions:\n";
            for (size_t i = 0; i < bottlenecks.size() && i < 5; ++i) {
                const auto& profile = bottlenecks[i];
                ss << "   - " << profile.functionName << "() in " << profile.fileName
                   << ": Consider using more efficient algorithms\n";
            }
        }
        
        ss << "2. Profile memory usage of critical functions.\n";
        ss << "3. Consider using compiler optimizations (O2, O3).\n";
        ss << "4. Use parallelization where possible.\n";
        ss << "5. Reduce function call overhead in hot paths.\n";
        
        return ss.str();
    }
    
    std::string getPerformanceSummary(const ProfileSession& session) {
        // Generate performance summary
        std::stringstream ss;
        ss << "Performance Summary for " << session.name << ":\n\n";
        
        double totalTime = 0.0;
        size_t totalCalls = 0;
        
        for (const auto& profile : session.profiles) {
            totalTime += profile.executionTime;
            totalCalls += profile.callCount;
        }
        
        ss << "Total Execution Time: " << totalTime << "ms\n";
        ss << "Total Function Calls: " << totalCalls << "\n";
        ss << "Average Function Time: " << 
           (totalCalls > 0 ? totalTime / totalCalls : 0.0) << "ms\n";
        ss << "Number of Profiled Functions: " << session.profiles.size() << "\n";
        
        return ss.str();
    }
};

// Complete profiler with all subsystems
class NLMProfiler {
public:
    NLMProfiler() = default;
    
    void startProfiling(const std::string& name, const std::string& description = "") {
        // Start system profiling
        SystemProfiler profiler;
        currentProfile_ = profiler.startProfile(name, description);
        currentProfileStartTime_ = std::chrono::steady_clock::now();
    }
    
    void stopProfiling() {
        if (currentProfile_.name.empty()) return;
        
        SystemProfiler profiler;
        profiler.endProfile(currentProfile_);
        
        // Record system metrics
        recordSystemMetrics();
        
        // Store profile
        profiles_.push_back(currentProfile_);
        
        currentProfile_ = PerformanceProfile();
        currentProfileStartTime_ = std::chrono::steady_clock::time_point();
    }
    
    PerformanceProfile getCurrentProfile() const {
        return currentProfile_;
    }
    
    std::vector<PerformanceProfile> getAllProfiles() const {
        return profiles_;
    }
    
    void startMemoryTracking() {
        MemoryProfiler memoryProfiler;
        memoryProfiler.startTracking();
        currentMemoryProfiler_ = memoryProfiler;
    }
    
    void stopMemoryTracking() {
        if (currentMemoryProfiler_.has_value()) {
            currentMemoryProfiler_.reset();
        }
    }
    
    MemoryProfiler::MemorySnapshot takeMemorySnapshot() {
        if (currentMemoryProfiler_.has_value()) {
            return currentMemoryProfiler_->takeSnapshot();
        }
        return MemoryProfiler::MemorySnapshot();
    }
    
    void startCodeProfiling(const std::string& sessionName) {
        CodeProfiler profiler;
        currentCodeProfileSession_ = profiler.startSession(sessionName);
    }
    
    void stopCodeProfiling() {
        if (currentCodeProfileSession_.name.empty()) return;
        
        CodeProfiler profiler;
        profiler.endSession(currentCodeProfileSession_);
        currentCodeProfileSession_ = CodeProfiler::ProfileSession();
    }
    
    CodeProfiler::ProfileSession getCurrentCodeProfile() const {
        return currentCodeProfileSession_;
    }
    
    void profileFunction(const std::string& functionName, const std::string& fileName,
                        int lineNumber, double executionTime, double cpuTime,
                        double memoryUsage, size_t callCount) {
        if (!currentCodeProfileSession_.name.empty()) {
            CodeProfiler profiler;
            profiler.profileFunction(functionName, fileName, lineNumber,
                                    executionTime, cpuTime, memoryUsage, callCount);
        }
    }
    
    std::string generateCompleteReport() {
        std::stringstream ss;
        ss << "=== NLM Performance Analysis Report ===\n\n";
        
        // System profiling summary
        ss << "1. System Performance Profiles:\n";
        for (const auto& profile : profiles_) {
            ss << "   - " << profile.name << ": " << profile.getDuration() << " seconds\n";
            ss << "     Metrics: " << profile.getMetricCount() << "\n";
        }
        
        ss << "\n2. Memory Analysis:\n";
        if (currentMemoryProfiler_.has_value()) {
            ss << "   Current memory tracking is active.\n";
            ss << "   " << currentMemoryProfiler_->getMemoryUsageReport() << "\n";
        } else {
            ss << "   Memory tracking is not active.\n";
        }
        
        ss << "\n3. Code Profiling:\n";
        if (!currentCodeProfileSession_.name.empty()) {
            ss << "   Current code profiling session: " << currentCodeProfileSession_.name << "\n";
            ss << "   Profiled functions: " << currentCodeProfileSession_.profiles.size() << "\n";
            ss << "   " << CodeProfiler::generateReport(currentCodeProfileSession_) << "\n";
        } else {
            ss << "   No active code profiling session.\n";
        }
        
        ss << "\n4. Recommendations:\n";
        ss << "   - Profile memory usage of critical functions.\n";
        ss << "   - Optimize hotspot functions identified in code profiling.\n";
        ss << "   - Use parallelization where possible.\n";
        ss << "   - Consider memory pool optimizations.\n";
        ss << "   - Profile I/O operations for bottlenecks.\n";
        
        return ss.str();
    }
    
    void exportProfiles(const std::string& directory) {
        // Export profiles to directory
        for (size_t i = 0; i < profiles_.size(); ++i) {
            SystemProfiler profiler;
            std::string jsonFile = directory + "/profile_" + std::to_string(i) + ".json";
            std::string csvFile = directory + "/profile_" + std::to_string(i) + ".csv";
            std::string textFile = directory + "/profile_" + std::to_string(i) + ".txt";
            
            // Write to files (implementation would need file operations)
            std::cout << "Exported profile " << i << " to: " << jsonFile << "\n";
        }
    }
    
    void clearProfiles() {
        profiles_.clear();
        currentProfile_ = PerformanceProfile();
        currentMemoryProfiler_.reset();
        currentCodeProfileSession_ = CodeProfiler::ProfileSession();
    }

private:
    void recordSystemMetrics() {
        // Record system metrics
        CPUMonitor cpu;
        MemoryMonitor memory;
        
        PerformanceMetric cpuMetric("cpu_usage", MetricType::CPU, cpu.getCpuUsage(), "%");
        PerformanceMetric memoryMetric("memory_usage", MetricType::Memory, 
                                      memory.getMemoryUsage(), "%");
        
        SystemProfiler profiler;
        profiler.recordMetric(currentProfile_, cpuMetric);
        profiler.recordMetric(currentProfile_, memoryMetric);
    }
    
    PerformanceProfile currentProfile_;
    std::chrono::steady_clock::time_point currentProfileStartTime_;
    std::vector<PerformanceProfile> profiles_;
    std::optional<MemoryProfiler> currentMemoryProfiler_;
    CodeProfiler::ProfileSession currentCodeProfileSession_;
};

// Global profiler instance
class NLMProfilerManager {
public:
    static NLMProfiler& getProfiler() {
        static NLMProfiler profiler;
        return profiler;
    }
    
    static void startProfiling(const std::string& name, const std::string& description = "") {
        getProfiler().startProfiling(name, description);
    }
    
    static void stopProfiling() {
        getProfiler().stopProfiling();
    }
    
    static PerformanceProfile getCurrentProfile() {
        return getProfiler().getCurrentProfile();
    }
    
    static void startMemoryTracking() {
        getProfiler().startMemoryTracking();
    }
    
    static void stopMemoryTracking() {
        getProfiler().stopMemoryTracking();
    }
    
    static MemoryProfiler::MemorySnapshot takeMemorySnapshot() {
        return getProfiler().takeMemorySnapshot();
    }
    
    static void startCodeProfiling(const std::string& sessionName) {
        getProfiler().startCodeProfiling(sessionName);
    }
    
    static void stopCodeProfiling() {
        getProfiler().stopCodeProfiling();
    }
    
    static CodeProfiler::ProfileSession getCurrentCodeProfile() {
        return getProfiler().getCurrentCodeProfile();
    }
    
    static void profileFunction(const std::string& functionName, const std::string& fileName,
                               int lineNumber, double executionTime, double cpuTime,
                               double memoryUsage, size_t callCount) {
        getProfiler().profileFunction(functionName, fileName, lineNumber,
                                      executionTime, cpuTime, memoryUsage, callCount);
    }
    
    static std::string generateReport() {
        return getProfiler().generateCompleteReport();
    }
    
    static void exportProfiles(const std::string& directory) {
        getProfiler().exportProfiles(directory);
    }
    
    static void clearProfiles() {
        getProfiler().clearProfiles();
    }
};

// Convenience macros for profiling
#define NLM_PROFILER_START(name, description) nlm::NLMProfilerManager::startProfiling(name, description)
#define NLM_PROFILER_STOP nlm::NLMProfilerManager::stopProfiling()
#define NLM_PROFILER_START_MEMORY nlm::NLMProfilerManager::startMemoryTracking()
#define NLM_PROFILER_STOP_MEMORY nlm::NLMProfilerManager::stopMemoryTracking()
#define NLM_PROFILER_START_CODE(session) nlm::NLMProfilerManager::startCodeProfiling(session)
#define NLM_PROFILER_STOP_CODE nlm::NLMProfilerManager::stopCodeProfiling()
#define NLM_PROFILE_FUNCTION(funcName, file, line, time, cpu, mem, calls) \
    nlm::NLMProfilerManager::profileFunction(funcName, file, line, time, cpu, mem, calls)
#define NLM_PROFILE_SCOPE(profileName) \\
    static nlm::CodeProfiler::ProfileSession profile_##profileName = nlm::NLMProfilerManager::getCurrentCodeProfile(); \\
    nlm::CodeProfiler::ProfileSession profile_##profileName##_temp = profile_##profileName; \\
    auto start_time = std::chrono::high_resolution_clock::now(); \\
    struct profile_##profileName##_cleanup { \\
        ~profile_##profileName##_cleanup() { \\
            auto end_time = std::chrono::high_resolution_clock::now(); \\
            double duration = std::chrono::duration<double, std::milli>(end_time - start_time).count(); \\
            nlm::NLMProfilerManager::profileFunction(#profile_##profileName, __FILE__, __LINE__, duration, 0.0, 0.0, 1); \\
        } \\
    } profile_##profileName##_guard

} // namespace nlm

// Example usage in NLM code
void example_usage() {
    // Start system profiling
    NLM_PROFILER_START("brain_simulation", "Profile brain simulation performance");
    
    // Start memory tracking
    NLM_PROFILER_START_MEMORY();
    
    // Start code profiling for a specific session
    NLM_PROFILER_START_CODE("neuron_computation");
    
    // Simulate some work
    double totalTime = 0.0;
    for (int i = 0; i < 1000; ++i) {
        // Simulate computation
        double x = std::sin(i * 0.01) * std::cos(i * 0.02);
        totalTime += x;
        
        // Profile individual iterations (commented out to avoid overhead)
        // NLM_PROFILE_FUNCTION("compute_neuron", "src/brain/Neuron.cpp", __LINE__, 0.001, 0.0, 0.0, 1);
    }
    
    // Use scope-based profiling
    NLM_PROFILE_SCOPE(neuron_step)
    {
        // Simulate neuron step computation
        double computation = std::sqrt(2.0) * std::log(10.0);
    }
    
    // Stop profiling
    NLM_PROFILER_STOP;
    NLM_PROFILER_STOP_MEMORY;
    NLM_PROFILER_STOP_CODE;
    
    // Generate report
    std::cout << nlm::NLMProfilerManager::generateReport();
}