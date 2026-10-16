#include "GPUProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>
#include <map>
#include <limits>

namespace nlm {

struct GPUProfiler::Impl {
    // GPU data storage
    std::vector<GPUTraceRecord> traces;
    mutable std::mutex tracesMutex;
    
    std::vector<GPUMemoryRecord> memories;
    mutable std::mutex memoriesMutex;
    
    std::vector<GPUUtilizationRecord> utilizations;
    mutable std::mutex utilizationsMutex;
    
    std::vector<GPUDeviceInfo> devices;
    mutable std::mutex devicesMutex;
    
    // Monitoring thread
    std::thread monitoringThread;
    bool monitoringActive;
    std::chrono::milliseconds monitoringInterval;
    
    // Configuration
    GPUProfilerConfig config;
    
    // Performance counter storage
    std::unordered_map<std::string, double> performanceCounters;
    mutable std::mutex countersMutex;
    
    Impl() : monitoringActive(false), monitoringInterval(1000) {
        // Initialize with mock device info
        GPUDeviceInfo device1;
        device1.deviceId = 0;
        device1.name = "NVIDIA RTX 4090"; 
        device1.deviceType = GPUDeviceType::NVIDIA_CUDA;
        device1.cudaCapability.major = 8;
        device1.cudaCapability.minor = 9;
        device1.cudaCapability.smxCount = 16;
        device1.cudaCapability.cudaCoreCount = 16384;
        device1.cudaCapability.maxThreadsPerBlock = 1024;
        device1.cudaCapability.maxThreadsPerSMX = 2048;
        device1.cudaCapability.sharedMemoryPerSMX = 64 * 1024;
        device1.cudaCapability.memoryClockRate = 1535000000; // 1.535 GHz
        device1.cudaCapability.coreClockRate = 2235000000;   // 2.235 GHz
        device1.globalMemory = 24 * 1024 * 1024 * 1024;  // 24 GB
        device1.sharedMemory = 48 * 1024;                 // 48 KB
        device1.multiProcessorCount = 16;
        device1.computeCapability = "8.9";
        device1.deviceArch = "Ampere";
        device1.pciBusId = "0000:01:00.0";
        
        devices.push_back(device1);
        
        // Initialize performance counters
        performanceCounters["sm__warps_active.avg.pct"] = 0.0;
        performanceCounters["sm__warps_issued.avg.pct"] = 0.0;
        performanceCounters["dram__bytes.read.max.pct"] = 0.0;
        performanceCounters["dram__bytes.write.max.pct"] = 0.0;
        performanceCounters["l1tex__throughput.avg.pct"] = 0.0;
        performanceCounters["lts__throughput.avg.pct"] = 0.0;
    }
    
    ~Impl() {
        if (monitoringActive) {
            monitoringActive = false;
            if (monitoringThread.joinable()) {
                monitoringThread.join();
            }
        }
    }
    
    void startMonitoring() {
        monitoringActive = true;
        monitoringThread = std::thread([this]() {
            while (monitoringActive) {
                monitorGPU();
                std::this_thread::sleep_for(monitoringInterval);
            }
        });
    }
    
    void monitorGPU() {
        // Mock GPU monitoring - in real implementation would use CUDA APIs
        GPUMemoryRecord memory;
        memory.timestamp = std::chrono::high_resolution_clock::now();
        memory.memoryType = GPUMemoryType::GLOBAL;
        memory.allocatedBytes = 1024 * 1024 * 1024 + (rand() % 2000000000);  // 1-3 GB
        memory.usedBytes = memory.allocatedBytes * (0.3 + (rand() % 70) / 100.0);  // 30-100% used
        memory.freeBytes = memory.totalBytes - memory.usedBytes;
        memory.totalBytes = 24 * 1024 * 1024 * 1024;  // 24 GB
        memory.usagePercentage = (static_cast<double>(memory.usedBytes) / memory.totalBytes) * 100.0;
        memory.allocationCount = 1000 + rand() % 5000;
        memory.deallocationCount = 950 + rand() % 4950;
        memory.memoryFragmentation = rand() % 1000;
        memory.allocationPattern = (rand() % 3 == 0) ? "random" : 
                                   (rand() % 2 == 0) ? "sequential" : "bursty";
        memory.deviceName = devices.empty() ? "Unknown" : devices[0].name;
        memory.deviceId = devices.empty() ? 0 : devices[0].deviceId;
        
        recordMemoryUsage(memory);
        
        // Update performance counters
        updatePerformanceCounters();
        
        cleanupOldRecords();
    }
    
    void updatePerformanceCounters() {
        std::lock_guard<std::mutex> lock(countersMutex);
        
        // Update performance counters with mock values
        performanceCounters["sm__warps_active.avg.pct"] = 75.0 + (rand() % 25);  // 75-100%
        performanceCounters["sm__warps_issued.avg.pct"] = 85.0 + (rand() % 15); // 85-100%
        performanceCounters["dram__bytes.read.max.pct"] = 60.0 + (rand() % 40); // 60-100%
        performanceCounters["dram__bytes.write.max.pct"] = 30.0 + (rand() % 30); // 30-60%
        performanceCounters["l1tex__throughput.avg.pct"] = 90.0 + (rand() % 10); // 90-100%
        performanceCounters["lts__throughput.avg.pct"] = 70.0 + (rand() % 30); // 70-100%
    }
    
    double calculateOccupancy(const GPUTraceRecord& trace, const CUDACapability& capability) const {
        if (trace.blockSizeX == 0 || trace.blockSizeY == 0 || trace.blockSizeZ == 0) {
            return 0.0;
        }
        
        uint64_t threadsPerBlock = trace.blockSizeX * trace.blockSizeY * trace.blockSizeZ;
        uint64_t maxThreadsPerSMX = capability.maxThreadsPerSMX;
        
        double occupancy = (static_cast<double>(threadsPerBlock) / maxThreadsPerSMX) * 100.0;
        return std::min(occupancy, 100.0);
    }
    
    double calculateComputationalIntensity(const GPUTraceRecord& trace) const {
        if (trace.memoryInstructions == 0) {
            return 0.0;
        }
        
        return static_cast<double>(trace.arithmeticInstructions) / 
               static_cast<double>(trace.memoryInstructions);
    }
    
    bool detectMemoryBandwidthBottleneck(const GPUTraceRecord& trace) const {
        if (trace.memoryThroughput == 0) {
            return false;
        }
        
        // Simple heuristic: if computational intensity is low and memory throughput is high,
        // it might be memory bandwidth bound
        double computationalIntensity = calculateComputationalIntensity(trace);
        
        // Heuristic: if computational intensity < 0.1 and memory throughput > 0
        return (computationalIntensity < 0.1) && (trace.memoryThroughput > 0);
    }
    
    void gatherDeviceInfo() {
        // In real implementation, would query actual GPU devices
        // For now, use the mock data
    }
    
    double calculateUtilizationEfficiency(const GPUUtilizationRecord& utilization) const {
        if (utilization.totalTime == std::chrono::duration<double>::zero()) {
            return 0.0;
        }
        
        double efficiency = utilization.activeTime / utilization.totalTime;
        
        // Weighted average of different utilization metrics
        double weightedEfficiency = 
            (utilization.gpuUtilization * 0.4 + utilization.memoryUtilization * 0.3 + 
             utilization.smUtilization * 0.2 + utilization.memoryBandwidthUtilization * 0.1) / 100.0;
        
        return std::min(efficiency, 1.0) * weightedEfficiency;
    }
    
    void analyzePerformanceCounterTrends() {
        // Analyze trends in performance counters
        // In real implementation, would use statistical analysis
    }
    
    void cleanupOldRecords() {
        auto now = std::chrono::high_resolution_clock::now();
        auto threshold = now - std::chrono::hours(1);
        
        // Remove old traces (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(tracesMutex);
            traces.erase(std::remove_if(traces.begin(), traces.end(),
                                       [threshold](const GPUTraceRecord& t) {
                                           return (t.timestamp + t.executionTime) < threshold;
                                       }),
                        traces.end());
        }
        
        // Remove old memory records (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(memoriesMutex);
            memories.erase(std::remove_if(memories.begin(), memories.end(),
                                        [threshold](const GPUMemoryRecord& m) {
                                            return m.timestamp < threshold;
                                        }),
                         memories.end());
        }
        
        // Remove old utilization records (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(utilizationsMutex);
            utilizations.erase(std::remove_if(utilizations.begin(), utilizations.end(),
                                            [threshold](const GPUUtilizationRecord& u) {
                                                return u.timestamp < threshold;
                                            }),
                            utilizations.end());
        }
    }
};

GPUProfiler::GPUProfiler() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

GPUProfiler::~GPUProfiler() {
    shutdown();
}

bool GPUProfiler::initialize() {
    try {
        // Configure based on settings
        pImpl->config = config_;
        
        // Start monitoring if enabled
        if (config_.enableKernelProfiling || config_.enableMemoryProfiling || 
            config_.enableUtilizationMonitoring) {
            pImpl->startMonitoring();
        }
        
        NLM_LOG_INFO("GPU profiler initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize GPU profiler: ") + e.what());
        return false;
    }
}

void GPUProfiler::shutdown() {
    try {
        // Stop monitoring
        pImpl->monitoringActive = false;
        
        if (pImpl->monitoringThread.joinable()) {
            pImpl->monitoringThread.join();
        }
        
        NLM_LOG_INFO("GPU profiler shutdown complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during GPU profiler shutdown: ") + e.what());
    }
}

void GPUProfiler::recordKernelTrace(const GPUTraceRecord& trace) {
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    pImpl->traces.push_back(trace);
}

void GPUProfiler::recordMemoryUsage(const GPUMemoryRecord& memory) {
    std::lock_guard<std::mutex> lock(pImpl->memoriesMutex);
    pImpl->memories.push_back(memory);
}

void GPUProfiler::recordUtilization(const GPUUtilizationRecord& utilization) {
    std::lock_guard<std::mutex> lock(pImpl->utilizationsMutex);
    pImpl->utilizations.push_back(utilization);
}

std::vector<GPUTraceRecord> GPUProfiler::getKernelTraces() const {
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    return pImpl->traces;
}

std::vector<GPUMemoryRecord> GPUProfiler::getMemoryRecords() const {
    std::lock_guard<std::mutex> lock(pImpl->memoriesMutex);
    return pImpl->memories;
}

std::vector<GPUUtilizationRecord> GPUProfiler::getUtilizationRecords() const {
    std::lock_guard<std::mutex> lock(pImpl->utilizationsMutex);
    return pImpl->utilizations;
}

std::vector<GPUDeviceInfo> GPUProfiler::getDeviceInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->devicesMutex);
    return pImpl->devices;
}

std::unordered_map<std::string, double> GPUProfiler::analyzeKernelPerformance() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    
    if (pImpl->traces.empty()) {
        return metrics;
    }
    
    double totalExecutionTime = 0.0;
    double totalOccupancy = 0.0;
    size_t count = 0;
    
    for (const auto& trace : pImpl->traces) {
        totalExecutionTime += std::chrono::duration<double>(trace.executionTime).count();
        totalOccupancy += trace.occupancy;
        count++;
    }
    
    if (count > 0) {
        metrics["average_execution_time_ms"] = (totalExecutionTime / count) * 1000.0;
        metrics["average_occupancy_percent"] = totalOccupancy / count;
        metrics["total_kernels_executed"] = static_cast<double>(count);
        metrics["total_execution_time_seconds"] = totalExecutionTime;
    }
    
    return metrics;
}

std::unordered_map<std::string, double> GPUProfiler::analyzeMemoryUsage() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->memoriesMutex);
    
    if (pImpl->memories.empty()) {
        return metrics;
    }
    
    double totalAllocated = 0.0;
    double totalUsed = 0.0;
    double avgUsagePercentage = 0.0;
    
    for (const auto& memory : pImpl->memories) {
        totalAllocated += memory.allocatedBytes;
        totalUsed += memory.usedBytes;
        avgUsagePercentage += memory.usagePercentage;
    }
    
    size_t count = pImpl->memories.size();
    if (count > 0) {
        metrics["total_allocated_bytes"] = totalAllocated;
        metrics["total_used_bytes"] = totalUsed;
        metrics["average_usage_percent"] = avgUsagePercentage / count;
        metrics["estimated_memory_utilization"] = (totalUsed / totalAllocated) * 100.0;
    }
    
    return metrics;
}

std::unordered_map<std::string, double> GPUProfiler::analyzeUtilization() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->utilizationsMutex);
    
    if (pImpl->utilizations.empty()) {
        return metrics;
    }
    
    double totalGpuUtil = 0.0;
    double totalMemUtil = 0.0;
    double totalSmUtil = 0.0;
    double totalBwUtil = 0.0;
    double totalEfficiency = 0.0;
    
    for (const auto& utilization : pImpl->utilizations) {
        totalGpuUtil += utilization.gpuUtilization;
        totalMemUtil += utilization.memoryUtilization;
        totalSmUtil += utilization.smUtilization;
        totalBwUtil += utilization.memoryBandwidthUtilization;
        totalEfficiency += utilization.utilizationEfficiency;
    }
    
    size_t count = pImpl->utilizations.size();
    if (count > 0) {
        metrics["average_gpu_utilization_percent"] = totalGpuUtil / count;
        metrics["average_memory_utilization_percent"] = totalMemUtil / count;
        metrics["average_sm_utilization_percent"] = totalSmUtil / count;
        metrics["average_bandwidth_utilization_percent"] = totalBwUtil / count;
        metrics["average_utilization_efficiency"] = totalEfficiency / count;
        metrics["current_utilization_efficiency"] = 
            pImpl->calculateUtilizationEfficiency(pImpl->utilizations.back());
    }
    
    return metrics;
}

std::string GPUProfiler::generateKernelReport() const {
    std::ostringstream oss;
    oss << "=== NLM GPU Kernel Report ===\n\n";
    oss << "Kernel Performance Statistics:\n";
    oss << "  Total Kernel Traces: " << getKernelTraces().size() << "\n";
    oss << "  Total Execution Time: " << getTotalExecutionTime() << " seconds\n";
    oss << "  Average Occupancy: " << getAverageOccupancy() << "%\n\n";
    
    auto topKernels = getTopKernelsByTime(5);
    oss << "Top Kernels by Execution Time:\n";
    for (size_t i = 0; i < topKernels.size(); ++i) {
        oss << "  " << (i + 1) << ". " << topKernels[i].first << "\n"
            << "     Execution Time: " << topKernels[i].second << " seconds\n";
    }
    
    return oss.str();
}

std::string GPUProfiler::generateMemoryReport() const {
    std::ostringstream oss;
    oss << "=== NLM GPU Memory Report ===\n\n";
    
    auto memoryStats = getMemoryStatistics();
    oss << "Memory Usage Statistics:\n";
    oss << "  Total Allocated Memory: " << memoryStats["total_allocated_bytes"] << " bytes\n";
    oss << "  Total Used Memory: " << memoryStats["total_used_bytes"] << " bytes\n";
    oss << "  Average Memory Utilization: " << memoryStats["average_usage_percent"] << "%\n";
    oss << "  Estimated Memory Utilization: " << memoryStats["estimated_memory_utilization"] << "%\n\n";
    
    std::lock_guard<std::mutex> lock(pImpl->memoriesMutex);
    if (!pImpl->memories.empty()) {
        oss << "Recent Memory Usage Samples:\n";
        for (size_t i = 0; i < std::min(size_t(5), pImpl->memories.size()); ++i) {
            const auto& memory = pImpl->memories[pImpl->memories.size() - 1 - i];
            oss << "  Sample " << (i + 1) << ": "
                << (memory.usedBytes / (1024 * 1024)) << " MB used out of "
                << (memory.totalBytes / (1024 * 1024)) << " MB ("
                << memory.usagePercentage << "% used)\n";
        }
    }
    
    return oss.str();
}

std::string GPUProfiler::generateUtilizationReport() const {
    std::ostringstream oss;
    oss << "=== NLM GPU Utilization Report ===\n\n";
    
    auto utilStats = getUtilizationStatistics();
    oss << "GPU Utilization Statistics:\n";
    oss << "  Average GPU Utilization: " << utilStats["average_gpu_utilization_percent"] << "%\n";
    oss << "  Average Memory Utilization: " << utilStats["average_memory_utilization_percent"] << "%\n";
    oss << "  Average SM Utilization: " << utilStats["average_sm_utilization_percent"] << "%\n";
    oss << "  Average Bandwidth Utilization: " << utilStats["average_bandwidth_utilization_percent"] << "%\n";
    oss << "  Average Utilization Efficiency: " << utilStats["average_utilization_efficiency"] << "\n\n";
    
    oss << "Utilization Efficiency Analysis:\n";
    double efficiency = getUtilizationStatistics()["current_utilization_efficiency"];
    oss << "  Current Efficiency: " << efficiency << "\n";
    if (efficiency > 0.8) {
        oss << "  Status: Excellent - GPU is well-utilized\n";
    } else if (efficiency > 0.6) {
        oss << "  Status: Good - GPU utilization is acceptable\n";
    } else if (efficiency > 0.4) {
        oss << "  Status: Fair - GPU utilization could be improved\n";
    } else {
        oss << "  Status: Poor - GPU is underutilized\n";
    }
    
    return oss.str();
}

std::string GPUProfiler::generateDeviceReport() const {
    std::ostringstream oss;
    oss << "=== NLM GPU Device Report ===\n\n";
    
    auto devices = getDeviceInfo();
    oss << "Available GPU Devices:\n";
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& device = devices[i];
        oss << "  Device " << (i + 1) << ": " << device.name << "\n"
            << "    ID: " << device.deviceId << "\n"
            << "    Type: " << static_cast<int>(device.deviceType) << "\n"
            << "    Compute Capability: " << device.computeCapability << "\n"
            << "    Architecture: " << device.deviceArch << "\n"
            << "    Global Memory: " << (device.globalMemory / (1024 * 1024 * 1024)) << " GB\n"
            << "    Shared Memory per Block: " << device.sharedMemory << " bytes\n"
            << "    Max Threads per Block: " << device.maxThreadsPerBlock << "\n"
            << "    Multi-Processor Count: " << device.multiProcessorCount << "\n"
            << "    ECC Enabled: " << (device.eccEnabled ? "Yes" : "No") << "\n"
            << "    PCI Bus ID: " << device.pciBusId << "\n\n";
    }
    
    return oss.str();
}

std::string GPUProfiler::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    oss << "  \"config\": {\n";
    oss << "    \"enableKernelProfiling\": " << (config_.enableKernelProfiling ? "true" : "false") << ",\n";
    oss << "    \"enableMemoryProfiling\": " << (config_.enableMemoryProfiling ? "true" : "false") << ",\n";
    oss << "    \"enableUtilizationMonitoring\": " << (config_.enableUtilizationMonitoring ? "true" : "false") << ",\n";
    oss << "    \"enablePerformanceCounters\": " << (config_.enablePerformanceCounters ? "true" : "false") << ",\n";
    oss << "    \"enableOccupancyAnalysis\": " << (config_.enableOccupancyAnalysis ? "true" : "false") << ",\n";
    oss << "    \"enableMemoryBandwidthAnalysis\": " << (config_.enableMemoryBandwidthAnalysis ? "true" : "false") << "\n";
    oss << "  },\n";
    
    // Export kernel traces
    oss << "  \"kernelTraces\": [\n";
    bool firstTrace = true;
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    for (const auto& trace : pImpl->traces) {
        if (!firstTrace) oss << ",\n";
        firstTrace = false;
        
        oss << "    {\n";
        oss << "      \"traceId\": " << trace.traceId << ",\n";
        oss << "      \"timestamp\": \"" << trace.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"kernelName\": \"" << trace.kernelName << "\",\n";
        oss << "      \"gridSize\": [" << trace.gridSizeX << ", " << trace.gridSizeY << ", " << trace.gridSizeZ << "],\n";
        oss << "      \"blockSize\": [" << trace.blockSizeX << ", " << trace.blockSizeY << ", " << trace.blockSizeZ << "],\n";
        oss << "      \"sharedMemoryBytes\": " << trace.sharedMemoryBytes << ",\n";
        oss << "      \"executionTime\": " << std::chrono::duration<double>(trace.executionTime).count() << ",\n";
        oss << "      \"occupancy\": " << trace.occupancy << ",\n";
        oss << "      \"registersPerThread\": " << trace.registersPerThread << ",\n";
        oss << "      \"memoryThroughput\": " << trace.memoryThroughput << ",\n";
        oss << "      \"arithmeticInstructions\": " << trace.arithmeticInstructions << ",\n";
        oss << "      \"memoryInstructions\": " << trace.memoryInstructions << ",\n";
        oss << "      \"computationalIntensity\": " << trace.computationalIntensity << ",\n";
        oss << "      \"memoryType\": " << static_cast<int>(trace.memoryType) << ",\n";
        oss << "      \"successful\": " << (trace.successful ? "true" : "false") << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export memory records
    oss << "  \"memoryRecords\": [\n";
    bool firstMemory = true;
    std::lock_guard<std::mutex> lock2(pImpl->memoriesMutex);
    for (const auto& memory : pImpl->memories) {
        if (!firstMemory) oss << ",\n";
        firstMemory = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << memory.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"memoryType\": " << static_cast<int>(memory.memoryType) << ",\n";
        oss << "      \"allocatedBytes\": " << memory.allocatedBytes << ",\n";
        oss << "      \"usedBytes\": " << memory.usedBytes << ",\n";
        oss << "      \"freeBytes\": " << memory.freeBytes << ",\n";
        oss << "      \"totalBytes\": " << memory.totalBytes << ",\n";
        oss << "      \"usagePercentage\": " << memory.usagePercentage << ",\n";
        oss << "      \"allocationCount\": " << memory.allocationCount << ",\n";
        oss << "      \"deallocationCount\": " << memory.deallocationCount << ",\n";
        oss << "      \"memoryFragmentation\": " << memory.memoryFragmentation << ",\n";
        oss << "      \"deviceName\": \"" << memory.deviceName << "\",\n";
        oss << "      \"deviceId\": " << memory.deviceId << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export utilization records
    oss << "  \"utilizationRecords\": [\n";
    bool firstUtilization = true;
    std::lock_guard<std::mutex> lock3(pImpl->utilizationsMutex);
    for (const auto& utilization : pImpl->utilizations) {
        if (!firstUtilization) oss << ",\n";
        firstUtilization = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << utilization.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"cpuUtilization\": " << utilization.cpuUtilization << ",\n";
        oss << "      \"gpuUtilization\": " << utilization.gpuUtilization << ",\n";
        oss << "      \"memoryUtilization\": " << utilization.memoryUtilization << ",\n";
        oss << "      \"smUtilization\": " << utilization.smUtilization << ",\n";
        oss << "      \"memoryBandwidthUtilization\": " << utilization.memoryBandwidthUtilization << ",\n";
        oss << "      \"computeCapability\": " << utilization.computeCapability << ",\n";
        oss << "      \"activeTime\": " << std::chrono::duration<double>(utilization.activeTime).count() << ",\n";
        oss << "      \"totalTime\": " << std::chrono::duration<double>(utilization.totalTime).count() << ",\n";
        oss << "      \"utilizationEfficiency\": " << utilization.utilizationEfficiency << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export device info
    oss << "  \"deviceInfo\": [\n";
    bool firstDevice = true;
    std::lock_guard<std::mutex> lock4(pImpl->devicesMutex);
    for (const auto& device : pImpl->devices) {
        if (!firstDevice) oss << ",\n";
        firstDevice = false;
        
        oss << "    {\n";
        oss << "      \"deviceId\": " << device.deviceId << ",\n";
        oss << "      \"name\": \"" << device.name << "\",\n";
        oss << "      \"deviceType\": " << static_cast<int>(device.deviceType) << ",\n";
        oss << "      \"computeCapability\": \"" << device.computeCapability << "\",\n";
        oss << "      \"globalMemory\": " << device.globalMemory << ",\n";
        oss << "      \"sharedMemory\": " << device.sharedMemory << ",\n";
        oss << "      \"maxThreadsPerBlock\": " << device.maxThreadsPerBlock << "\n";
        oss << "    }";
    }
    oss << "\n  ]\n";
    
    oss << "}\n";
    
    return oss.str();
}

std::string GPUProfiler::exportToCSV() const {
    std::ostringstream oss;
    oss << "TraceID,Timestamp,KernelName,GridSizeX,BlockSizeX,ExecutionTimeMs,Occupancy,Success\n";
    
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    for (const auto& trace : pImpl->traces) {
        oss << trace.traceId << ","
            << trace.timestamp.time_since_epoch().count() << ","
            << trace.kernelName << ","
            << trace.gridSizeX << ","
            << trace.blockSizeX << ","
            << std::chrono::duration<double>(trace.executionTime).count() * 1000.0 << ","
            << trace.occupancy << ","
            << (trace.successful ? "true" : "false") << "\n";
    }
    
    return oss.str();
}

std::string GPUProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM GPU Profiler Report ===\n\n";
    oss << "GPU Profiling Statistics:\n";
    oss << "  Total Kernel Traces: " << getKernelTraces().size() << "\n";
    oss << "  Total Memory Records: " << getMemoryRecords().size() << "\n";
    oss << "  Total Utilization Records: " << getUtilizationRecords().size() << "\n";
    oss << "  Available GPU Devices: " << getDeviceInfo().size() << "\n\n";
    
    // Generate performance metrics
    auto kernelMetrics = analyzeKernelPerformance();
    auto memoryMetrics = analyzeMemoryUsage();
    auto utilMetrics = analyzeUtilization();
    
    oss << "Kernel Performance:\n";
    for (const auto& pair : kernelMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "Memory Usage:\n";
    for (const auto& pair : memoryMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "GPU Utilization:\n";
    for (const auto& pair : utilMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    // Generate individual reports
    oss << generateKernelReport();
    oss << "\n";
    oss << generateMemoryReport();
    oss << "\n";
    oss << generateUtilizationReport();
    oss << "\n";
    oss << generateDeviceReport();
    
    // Add optimization recommendations
    auto optimizations = getOptimizationRecommendations();
    if (!optimizations.empty()) {
        oss << "\nOptimization Recommendations:\n";
        for (size_t i = 0; i < optimizations.size(); ++i) {
            const auto& opt = optimizations[i];
            oss << "  " << (i + 1) << ". " << opt.recommendation << "\n"
                << "     Type: " << opt.optimizationType << "\n"
                << "     Potential Speedup: " << opt.potentialSpeedup << "x\n"
                << "     Priority: " << opt.priority << "\n";
        }
    }
    
    return oss.str();
}

void GPUProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    pImpl->traces.clear();
    
    std::lock_guard<std::mutex> lock2(pImpl->memoriesMutex);
    pImpl->memories.clear();
    
    std::lock_guard<std::mutex> lock3(pImpl->utilizationsMutex);
    pImpl->utilizations.clear();
    
    std::lock_guard<std::mutex> lock4(pImpl->countersMutex);
    pImpl->performanceCounters.clear();
}

GPUProfiler& GPUProfiler::getInstance() {
    static GPUProfiler instance;
    return instance;
}

std::vector<std::pair<std::string, double>> GPUProfiler::getTopKernelsByTime(size_t limit) const {
    std::vector<std::pair<std::string, double>> kernelTimes;
    
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    for (const auto& trace : pImpl->traces) {
        kernelTimes.push_back(std::make_pair(trace.kernelName, 
                                            std::chrono::duration<double>(trace.executionTime).count()));
    }
    
    // Sort by execution time (descending)
    std::sort(kernelTimes.begin(), kernelTimes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (kernelTimes.size() > limit) {
        kernelTimes.resize(limit);
    }
    
    return kernelTimes;
}

std::unordered_map<std::string, double> GPUProfiler::getUtilizationStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    std::lock_guard<std::mutex> lock(pImpl->utilizationsMutex);
    
    if (pImpl->utilizations.empty()) {
        return stats;
    }
    
    double totalGpuUtil = 0.0;
    double totalMemUtil = 0.0;
    double totalSmUtil = 0.0;
    double totalBwUtil = 0.0;
    double totalEfficiency = 0.0;
    
    for (const auto& utilization : pImpl->utilizations) {
        totalGpuUtil += utilization.gpuUtilization;
        totalMemUtil += utilization.memoryUtilization;
        totalSmUtil += utilization.smUtilization;
        totalBwUtil += utilization.memoryBandwidthUtilization;
        totalEfficiency += utilization.utilizationEfficiency;
    }
    
    size_t count = pImpl->utilizations.size();
    if (count > 0) {
        stats["average_gpu_utilization"] = totalGpuUtil / count;
        stats["average_memory_utilization"] = totalMemUtil / count;
        stats["average_sm_utilization"] = totalSmUtil / count;
        stats["average_bandwidth_utilization"] = totalBwUtil / count;
        stats["average_efficiency"] = totalEfficiency / count;
        stats["current_efficiency"] = pImpl->calculateUtilizationEfficiency(pImpl->utilizations.back());
    }
    
    return stats;
}

std::unordered_map<std::string, uint64_t> GPUProfiler::getMemoryStatistics() const {
    std::unordered_map<std::string, uint64_t> stats;
    
    std::lock_guard<std::mutex> lock(pImpl->memoriesMutex);
    
    if (pImpl->memories.empty()) {
        return stats;
    }
    
    uint64_t totalAllocated = 0;
    uint64_t totalUsed = 0;
    
    for (const auto& memory : pImpl->memories) {
        totalAllocated += memory.allocatedBytes;
        totalUsed += memory.usedBytes;
    }
    
    stats["total_allocated_bytes"] = totalAllocated;
    stats["total_used_bytes"] = totalUsed;
    stats["average_allocation_count"] = static_cast<uint64_t>(std::accumulate(pImpl->memories.begin(), pImpl->memories.end(), 0,
                                                                              [](size_t sum, const GPUMemoryRecord& m) {
                                                                                  return sum + m.allocationCount;
                                                                              }) / pImpl->memories.size());
}

std::unordered_map<std::string, double> GPUProfiler::getPerformanceCounters() const {
    std::lock_guard<std::mutex> lock(pImpl->countersMutex);
    return pImpl->performanceCounters;
}

double GPUProfiler::getAverageOccupancy() const {
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    
    if (pImpl->traces.empty()) {
        return 0.0;
    }
    
    double totalOccupancy = 0.0;
    for (const auto& trace : pImpl->traces) {
        totalOccupancy += trace.occupancy;
    }
    
    return totalOccupancy / pImpl->traces.size();
}

double GPUProfiler::getTotalExecutionTime() const {
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    
    double totalTime = 0.0;
    for (const auto& trace : pImpl->traces) {
        totalTime += std::chrono::duration<double>(trace.executionTime).count();
    }
    
    return totalTime;
}

std::vector<std::string> GPUProfiler::detectBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    
    for (const auto& trace : pImpl->traces) {
        if (pImpl->detectMemoryBandwidthBottleneck(trace)) {
            bottlenecks.push_back("memory_bandwidth: " + trace.kernelName);
        }
        
        if (trace.occupancy < 50.0) {  // Low occupancy threshold
            bottlenecks.push_back("low_occupancy: " + trace.kernelName);
        }
        
        double computationalIntensity = pImpl->calculateComputationalIntensity(trace);
        if (computationalIntensity < 0.1) {
            bottlenecks.push_back("low_computational_intensity: " + trace.kernelName);
        }
    }
    
    return bottlenecks;
}

std::vector<GPUOptimization> GPUProfiler::getOptimizationRecommendations() const {
    std::vector<GPUOptimization> optimizations;
    
    std::lock_guard<std::mutex> lock(pImpl->tracesMutex);
    
    // Analyze for optimization opportunities
    for (const auto& trace : pImpl->traces) {
        GPUOptimization opt;
        opt.targetKernel = trace.kernelName;
        
        // Check for optimization opportunities
        if (trace.occupancy < 70.0) {
            opt.recommendation = "Increase thread block size to improve occupancy";
            opt.optimizationType = "THREAD_BLOCK_SIZE";
            opt.potentialSpeedup = (70.0 - trace.occupancy) / 100.0 + 1.0; // Estimate
            opt.implementationCost = 3.0; // Low cost
            opt.priority = (70.0 - trace.occupancy) * 10.0; // Higher priority for lower occupancy
            optimizations.push_back(opt);
        }
        
        if (trace.registersPerThread > 64) { // High register usage threshold
            opt.recommendation = "Reduce register usage or increase register spilling";
            opt.optimizationType = "REGISTER_OPTIMIZATION";
            opt.potentialSpeedup = 1.2; // Estimated speedup
            opt.implementationCost = 5.0; // Medium cost
            opt.priority = 50.0; // Medium priority
            optimizations.push_back(opt);
        }
        
        if (pImpl->detectMemoryBandwidthBottleneck(trace)) {
            opt.recommendation = "Optimize memory access patterns for better bandwidth utilization";
            opt.optimizationType = "MEMORY_ACCESS_OPTIMIZATION";
            opt.potentialSpeedup = 1.5; // High potential speedup
            opt.implementationCost = 7.0; // High cost
            opt.priority = 80.0; // High priority
            optimizations.push_back(opt);
        }
    }
    
    // Sort by priority
    std::sort(optimizations.begin(), optimizations.end(),
              [](const GPUOptimization& a, const GPUOptimization& b) {
                  return a.priority > b.priority;
              });
    
    return optimizations;
}

} // namespace nlm