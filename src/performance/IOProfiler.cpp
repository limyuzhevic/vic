#include "IOProfiler.hpp"
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
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace nlm {

struct IOProfiler::Impl {
    // I/O data storage
    std::vector<FileIOOperation> fileOperations;
    mutable std::mutex fileOperationsMutex;
    
    std::vector<DiskPerformance> diskPerformances;
    mutable std::mutex diskPerformancesMutex;
    
    std::vector<NetworkIOPerformance> networkIOPerformances;
    mutable std::mutex networkIOPerformancesMutex;
    
    std::unordered_map<std::string, IOPattern> ioPatterns;
    mutable std::mutex patternsMutex;
    
    std::vector<IOBottleneck> bottlenecks;
    mutable std::mutex bottlenecksMutex;
    
    // Monitoring threads
    std::thread fileIOMonitorThread;
    bool fileIOMonitorActive;
    std::chrono::milliseconds fileIOMonitorInterval;
    
    std::thread diskMonitorThread;
    bool diskMonitorActive;
    std::chrono::milliseconds diskMonitorInterval;
    
    std::thread networkMonitorThread;
    bool networkMonitorActive;
    std::chrono::milliseconds networkMonitorInterval;
    
    std::thread patternAnalysisThread;
    bool patternAnalysisActive;
    std::chrono::milliseconds patternAnalysisInterval;
    
    // Configuration
    IOProfilerConfig config;
    
    // Statistics
    uint64_t totalFileOperations;
    uint64_t totalBytesTransferred;
    double totalDuration;
    
    Impl() : fileIOMonitorActive(false), diskMonitorActive(false), networkMonitorActive(false),
             patternAnalysisActive(false), fileIOMonitorInterval(100),
             diskMonitorInterval(1000), networkMonitorInterval(1000), patternAnalysisInterval(5000),
             totalFileOperations(0), totalBytesTransferred(0), totalDuration(0.0) {}
    
    ~Impl() {
        if (fileIOMonitorActive) {
            fileIOMonitorActive = false;
            if (fileIOMonitorThread.joinable()) {
                fileIOMonitorThread.join();
            }
        }
        
        if (diskMonitorActive) {
            diskMonitorActive = false;
            if (diskMonitorThread.joinable()) {
                diskMonitorThread.join();
            }
        }
        
        if (networkMonitorActive) {
            networkMonitorActive = false;
            if (networkMonitorThread.joinable()) {
                networkMonitorThread.join();
            }
        }
        
        if (patternAnalysisActive) {
            patternAnalysisActive = false;
            if (patternAnalysisThread.joinable()) {
                patternAnalysisThread.join();
            }
        }
    }
    
    void startFileIOMonitoring() {
        fileIOMonitorActive = true;
        fileIOMonitorThread = std::thread([this]() {
            while (fileIOMonitorActive) {
                monitorFileIO();
                std::this_thread::sleep_for(fileIOMonitorInterval);
            }
        });
    }
    
    void startDiskMonitoring() {
        diskMonitorActive = true;
        diskMonitorThread = std::thread([this]() {
            while (diskMonitorActive) {
                monitorDisk();
                std::this_thread::sleep_for(diskMonitorInterval);
            }
        });
    }
    
    void startNetworkMonitoring() {
        networkMonitorActive = true;
        networkMonitorThread = std::thread([this]() {
            while (networkMonitorActive) {
                monitorNetwork();
                std::this_thread::sleep_for(networkMonitorInterval);
            }
        });
    }
    
    void startPatternAnalysis() {
        patternAnalysisActive = true;
        patternAnalysisThread = std::thread([this]() {
            while (patternAnalysisActive) {
                analyzePatterns();
                std::this_thread::sleep_for(patternAnalysisInterval);
            }
        });
    }
    
    void monitorFileIO() {
        // Mock file I/O monitoring - in real implementation would use system APIs
        // or file monitoring libraries
        
        FileIOOperation operation;
        operation.operationId = totalFileOperations++;
        operation.timestamp = std::chrono::high_resolution_clock::now();
        operation.operation = FileOperation::READ;
        operation.filePath = "/workspace/app/data/neural_data.bin";
        operation.fileSize = 1024 * 1024 + (rand() % 5000000); // 1MB - 6MB
        operation.bytesTransferred = operation.fileSize;
        operation.latency = static_cast<double>(rand()) / RAND_MAX * 100.0; // 0-100ms
        operation.throughput = operation.bytesTransferred / (operation.latency / 1000.0); // bytes/s
        operation.blockSize = 4096;
        operation.accessPattern = "sequential";
        operation.ioPriority = "high";
        operation.success = (rand() % 10 != 0); // 90% success rate
        operation.userContext = "simulation";
        
        recordFileOperation(operation);
    }
    
    void monitorDisk() {
        // Mock disk monitoring - in real implementation would use system APIs
        // like iostat or custom disk monitoring
        
        DiskPerformance disk;
        disk.timestamp = std::chrono::high_resolution_clock::now();
        disk.deviceName = "/dev/sda";
        disk.deviceType = "SSD";
        disk.totalSpace = 500 * 1024 * 1024 * 1024; // 500 GB
        disk.freeSpace = disk.totalSpace * (0.2 + (rand() % 80) / 100.0); // 20-100% free
        disk.usedSpace = disk.totalSpace - disk.freeSpace;
        disk.readSpeed = (rand() % 500) * 1024.0 * 1024.0; // MB/s
        disk.writeSpeed = (rand() % 300) * 1024.0 * 1024.0; // MB/s
        disk.readIOPS = (rand() % 10000); // IOPS
        disk.writeIOPS = (rand() % 5000); // IOPS
        disk.averageReadLatency = static_cast<double>(rand()) / RAND_MAX * 10.0; // 0-10ms
        disk.averageWriteLatency = static_cast<double>(rand()) / RAND_MAX * 15.0; // 0-15ms
        disk.queueDepth = static_cast<double>(rand()) / RAND_MAX * 2.0; // 0-2
        disk.readOperations = rand() % 1000;
        disk.writeOperations = rand() % 500;
        disk.filesystem = "ext4";
        disk.mountPoint = "/workspace/app";
        disk.isRemovable = false;
        disk.isSSD = true;
        disk.isNVMe = false;
        disk.serialNumber = "SN123456789";
        disk.model = "Samsung 860 Evo";
        
        recordDiskPerformance(disk);
    }
    
    void monitorNetwork() {
        // Mock network monitoring - in real implementation would use system APIs
        // or network monitoring tools
        
        NetworkIOPerformance network;
        network.timestamp = std::chrono::high_resolution_clock::now();
        network.protocol = "TCP";
        network.endpoint = "10.0.0.1:8080";
        network.uploadSpeed = (rand() % 100) * 1024.0; // Mbps
        network.downloadSpeed = (rand() % 200) * 1024.0; // Mbps
        network.pingLatency = static_cast<double>(rand()) / RAND_MAX * 50.0; // 0-50ms
        network.jitter = static_cast<double>(rand()) / RAND_MAX * 5.0; // 0-5ms
        network.packetsSent = rand() % 1000;
        network.packetsReceived = rand() % 1000;
        network.packetLossRate = static_cast<double>(rand()) / RAND_MAX * 0.05; // 0-5%
        network.connectionTime = static_cast<double>(rand()) / RAND_MAX * 10.0; // 0-10s
        network.concurrentConnections = rand() % 10;
        network.bandwidthProfile = "high";
        
        recordNetworkIO(network);
    }
    
    void analyzePatterns() {
        std::lock_guard<std::mutex> lock(patternsMutex);
        
        auto now = std::chrono::high_resolution_clock::now();
        
        for (auto& pair : ioPatterns) {
            auto& pattern = pair.second;
            pattern.endTime = now;
            
            // Calculate pattern metrics
            if (pattern.totalOperations > 0) {
                pattern.peakOperationsPerSec = pattern.totalOperations / pattern.burstFrequency;
                pattern.averageLatency = pattern.latency / pattern.totalOperations;
                pattern.throughput = pattern.totalBytes / 
                                  std::chrono::duration<double>(pattern.endTime - pattern.startTime).count();
                
                // Calculate latency variation (simplified)
                pattern.latencyVariation = pattern.averageLatency * 0.1; // 10% variation
            }
            
            // Check for pattern type
            if (pattern.throughput > 1000 * 1024 * 1024) { // > 1 GB/s
                pattern.patternType = "burst";
            } else if (pattern.throughput > 100 * 1024 * 1024) { // > 100 MB/s
                pattern.patternType = "steady";
            } else {
                pattern.patternType = "periodic";
            }
        }
    }
    
    void cleanupOldData() {
        auto now = std::chrono::high_resolution_clock::now();
        auto threshold = now - std::chrono::hours(1);
        
        // Remove old file operations (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(fileOperationsMutex);
            fileOperations.erase(std::remove_if(fileOperations.begin(), fileOperations.end(),
                                               [threshold](const FileIOOperation& op) {
                                                   return op.timestamp < threshold;
                                               }),
                               fileOperations.end());
        }
        
        // Remove old disk samples (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(diskPerformancesMutex);
            diskPerformances.erase(std::remove_if(diskPerformances.begin(), diskPerformances.end(),
                                                [threshold](const DiskPerformance& d) {
                                                    return d.timestamp < threshold;
                                                }),
                                diskPerformances.end());
        }
        
        // Remove old network samples (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(networkIOPerformancesMutex);
            networkIOPerformances.erase(std::remove_if(networkIOPerformances.begin(), networkIOPerformances.end(),
                                                     [threshold](const NetworkIOPerformance& n) {
                                                         return n.timestamp < threshold;
                                                     }),
                              networkIOPerformances.end());
        }
        
        // Remove old patterns (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(patternsMutex);
            for (auto it = ioPatterns.begin(); it != ioPatterns.end();) {
                if (it->second.startTime < threshold) {
                    it = ioPatterns.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        // Remove old bottlenecks (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(bottlenecksMutex);
            bottlenecks.erase(std::remove_if(bottlenecks.begin(), bottlenecks.end(),
                                            [threshold](const IOBottleneck& b) {
                                                return b.timestamp < threshold;
                                            }),
                            bottlenecks.end());
        }
    }
    
    double calculateThroughput(const FileIOOperation& operation) const {
        if (operation.latency == 0.0) {
            return 0.0;
        }
        return operation.bytesTransferred / (operation.latency / 1000.0); // bytes/s
    }
    
    IOBottleneck detectBottleneck(const FileIOOperation& operation) const {
        IOBottleneck bottleneck;
        bottleneck.timestamp = operation.timestamp;
        bottleneck.component = operation.userContext;
        
        // Detect bottlenecks based on operation characteristics
        if (operation.latency > 100.0) { // > 100ms
            bottleneck.bottleneckType = "disk_latency";
            bottleneck.measuredValue = operation.latency;
            bottleneck.threshold = 100.0;
            bottleneck.severity = std::min(1.0, (operation.latency - 100.0) / 400.0);
            bottleneck.isActive = bottleneck.severity > 0.5;
            bottleneck.recommendation = "Optimize disk I/O patterns or upgrade storage";
        } else if (operation.throughput < 100 * 1024 * 1024) { // < 100 MB/s
            bottleneck.bottleneckType = "disk_throughput";
            bottleneck.measuredValue = operation.throughput;
            bottleneck.threshold = 100 * 1024 * 1024;
            bottleneck.severity = std::min(1.0, (100 * 1024 * 1024 - operation.throughput) / (100 * 1024 * 1024));
            bottleneck.isActive = bottleneck.severity > 0.5;
            bottleneck.recommendation = "Consider using faster storage or SSD";
        } else {
            return bottleneck; // No bottleneck detected
        }
        
        return bottleneck;
    }
};

IOProfiler::IOProfiler() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

IOProfiler::~IOProfiler() {
    shutdown();
}

bool IOProfiler::initialize() {
    try {
        // Configure based on settings
        pImpl->config = config_;
        
        // Start monitoring threads if enabled
        if (config_.enableFileIOProfiling) {
            pImpl->startFileIOMonitoring();
        }
        
        if (config_.enableDiskPerformanceMonitoring) {
            pImpl->startDiskMonitoring();
        }
        
        if (config_.enableNetworkIOMonitoring) {
            pImpl->startNetworkMonitoring();
        }
        
        if (config_.enablePatternAnalysis) {
            pImpl->startPatternAnalysis();
        }
        
        NLM_LOG_INFO("I/O profiler initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize I/O profiler: ") + e.what());
        return false;
    }
}

void IOProfiler::shutdown() {
    try {
        // Stop all monitoring threads
        pImpl->fileIOMonitorActive = false;
        pImpl->diskMonitorActive = false;
        pImpl->networkMonitorActive = false;
        pImpl->patternAnalysisActive = false;
        
        // Join threads
        if (pImpl->fileIOMonitorThread.joinable()) {
            pImpl->fileIOMonitorThread.join();
        }
        
        if (pImpl->diskMonitorThread.joinable()) {
            pImpl->diskMonitorThread.join();
        }
        
        if (pImpl->networkMonitorThread.joinable()) {
            pImpl->networkMonitorThread.join();
        }
        
        if (pImpl->patternAnalysisThread.joinable()) {
            pImpl->patternAnalysisThread.join();
        }
        
        NLM_LOG_INFO("I/O profiler shutdown complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during I/O profiler shutdown: ") + e.what());
    }
}

void IOProfiler::recordFileOperation(const FileIOOperation& operation) {
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    pImpl->fileOperations.push_back(operation);
    
    // Update statistics
    pImpl->totalFileOperations++;
    pImpl->totalBytesTransferred += operation.bytesTransferred;
    
    // Detect and record bottlenecks
    IOBottleneck bottleneck = pImpl->detectBottleneck(operation);
    if (bottleneck.isActive) {
        recordBottleneck(bottleneck);
    }
}

void IOProfiler::recordDiskPerformance(const DiskPerformance& performance) {
    std::lock_guard<std::mutex> lock(pImpl->diskPerformancesMutex);
    pImpl->diskPerformances.push_back(performance);
}

void IOProfiler::recordNetworkIO(const NetworkIOPerformance& networkIO) {
    std::lock_guard<std::mutex> lock(pImpl->networkIOPerformancesMutex);
    pImpl->networkIOPerformances.push_back(networkIO);
}

void IOProfiler::startPatternTracking(const std::string& patternId, const std::string& patternType) {
    std::lock_guard<std::mutex> lock(pImpl->patternsMutex);
    
    IOPattern pattern;
    pattern.patternId = patternId;
    pattern.patternType = patternType;
    pattern.startTime = std::chrono::high_resolution_clock::now();
    pattern.dataPattern = "sequential"; // default
    
    pImpl->ioPatterns[patternId] = pattern;
}

void IOProfiler::updatePattern(const std::string& patternId, uint64_t bytesTransferred,
                               double latency, size_t operations) {
    std::lock_guard<std::mutex> lock(pImpl->patternsMutex);
    auto it = pImpl->ioPatterns.find(patternId);
    if (it != pImpl->ioPatterns.end()) {
        IOPattern& pattern = it->second;
        pattern.totalBytes += bytesTransferred;
        pattern.latency += latency;
        pattern.totalOperations += operations;
        
        if (operations > pattern.peakOperationsPerSec) {
            pattern.peakOperationsPerSec = operations;
        }
        
        // Calculate average latency
        if (pattern.totalOperations > 0) {
            pattern.averageLatency = pattern.latency / pattern.totalOperations;
        }
        
        // Calculate throughput
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - pattern.startTime).count();
        if (elapsed > 0) {
            pattern.throughput = pattern.totalBytes / elapsed;
            pattern.burstFrequency = pattern.totalOperations / elapsed;
        }
    }
}

void IOProfiler::stopPatternTracking(const std::string& patternId) {
    std::lock_guard<std::mutex> lock(pImpl->patternsMutex);
    auto it = pImpl->ioPatterns.find(patternId);
    if (it != pImpl->ioPatterns.end()) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
    }
}

void IOProfiler::recordBottleneck(const IOBottleneck& bottleneck) {
    std::lock_guard<std::mutex> lock(pImpl->bottlenecksMutex);
    pImpl->bottlenecks.push_back(bottleneck);
}

std::vector<FileIOOperation> IOProfiler::getFileOperations() const {
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    return pImpl->fileOperations;
}

std::vector<DiskPerformance> IOProfiler::getDiskPerformance() const {
    std::lock_guard<std::mutex> lock(pImpl->diskPerformancesMutex);
    return pImpl->diskPerformances;
}

std::vector<NetworkIOPerformance> IOProfiler::getNetworkIOPerformance() const {
    std::lock_guard<std::mutex> lock(pImpl->networkIOPerformancesMutex);
    return pImpl->networkIOPerformances;
}

std::unordered_map<std::string, IOPattern> IOProfiler::getIOPatterns() const {
    std::lock_guard<std::mutex> lock(pImpl->patternsMutex);
    return pImpl->ioPatterns;
}

std::vector<IOBottleneck> IOProfiler::getIOBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->bottlenecksMutex);
    return pImpl->bottlenecks;
}

std::unordered_map<std::string, double> IOProfiler::analyzeIOPerformance() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    
    if (pImpl->fileOperations.empty()) {
        return metrics;
    }
    
    double totalOperations = 0.0;
    double totalBytes = 0.0;
    double totalLatency = 0.0;
    
    for (const auto& operation : pImpl->fileOperations) {
        totalOperations++;
        totalBytes += operation.bytesTransferred;
        totalLatency += operation.latency;
    }
    
    double averageLatency = totalOperations > 0 ? totalLatency / totalOperations : 0.0;
    
    metrics["total_operations"] = totalOperations;
    metrics["total_bytes_transferred"] = totalBytes;
    metrics["average_latency_ms"] = averageLatency;
    
    // Calculate throughput
    if (pImpl->fileOperations.size() > 1) {
        auto firstTime = pImpl->fileOperations.front().timestamp;
        auto lastTime = pImpl->fileOperations.back().timestamp;
        double timeSpan = std::chrono::duration<double>(lastTime - firstTime).count();
        if (timeSpan > 0) {
            metrics["overall_throughput_bps"] = totalBytes / timeSpan;
            metrics["overall_throughput_mbps"] = (totalBytes / timeSpan) / (1024.0 * 1024.0);
        }
    }
    
    return metrics;
}

std::unordered_map<std::string, double> IOProfiler::analyzeDiskPerformance() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->diskPerformancesMutex);
    
    if (pImpl->diskPerformances.empty()) {
        return metrics;
    }
    
    double totalReadSpeed = 0.0;
    double totalWriteSpeed = 0.0;
    double totalReadIOPS = 0.0;
    double totalWriteIOPS = 0.0;
    double totalReadLatency = 0.0;
    double totalWriteLatency = 0.0;
    
    for (const auto& disk : pImpl->diskPerformances) {
        totalReadSpeed += disk.readSpeed;
        totalWriteSpeed += disk.writeSpeed;
        totalReadIOPS += disk.readIOPS;
        totalWriteIOPS += disk.writeIOPS;
        totalReadLatency += disk.averageReadLatency;
        totalWriteLatency += disk.averageWriteLatency;
    }
    
    size_t count = pImpl->diskPerformances.size();
    if (count > 0) {
        metrics["average_read_speed_mbps"] = totalReadSpeed / count / (1024.0 * 1024.0);
        metrics["average_write_speed_mbps"] = totalWriteSpeed / count / (1024.0 * 1024.0);
        metrics["average_read_iops"] = totalReadIOPS / count;
        metrics["average_write_iops"] = totalWriteIOPS / count;
        metrics["average_read_latency_ms"] = totalReadLatency / count;
        metrics["average_write_latency_ms"] = totalWriteLatency / count;
        metrics["io_ratio"] = totalReadIOPS / (totalReadIOPS + totalWriteIOPS);
    }
    
    return metrics;
}

std::unordered_map<std::string, double> IOProfiler::analyzeNetworkPerformance() const {
    std::unordered_map<std::string, double> metrics;
    
    std::lock_guard<std::mutex> lock(pImpl->networkIOPerformancesMutex);
    
    if (pImpl->networkIOPerformances.empty()) {
        return metrics;
    }
    
    double totalUpload = 0.0;
    double totalDownload = 0.0;
    double totalPing = 0.0;
    double totalJitter = 0.0;
    double totalPacketLoss = 0.0;
    
    for (const auto& network : pImpl->networkIOPerformances) {
        totalUpload += network.uploadSpeed;
        totalDownload += network.downloadSpeed;
        totalPing += network.pingLatency;
        totalJitter += network.jitter;
        totalPacketLoss += network.packetLossRate;
    }
    
    size_t count = pImpl->networkIOPerformances.size();
    if (count > 0) {
        metrics["average_upload_speed_mbps"] = totalUpload / count;
        metrics["average_download_speed_mbps"] = totalDownload / count;
        metrics["average_ping_latency_ms"] = totalPing / count;
        metrics["average_jitter_ms"] = totalJitter / count;
        metrics["average_packet_loss_rate"] = totalPacketLoss / count;
    }
    
    return metrics;
}

std::string IOProfiler::generateFileIOReport() const {
    std::ostringstream oss;
    oss << "=== NLM File I/O Report ===\n\n";
    oss << "File I/O Performance Statistics:\n";
    oss << "  Total File Operations: " << pImpl->totalFileOperations << "\n";
    oss << "  Total Bytes Transferred: " << pImpl->totalBytesTransferred << "\n";
    oss << "  Average Throughput: " << (pImpl->totalBytesTransferred / pImpl->totalFileOperations) << " bytes/op\n\n";
    
    auto topFiles = getTopFilesByIO(5);
    oss << "Top Files by I/O Operations:\n";
    for (size_t i = 0; i < topFiles.size(); ++i) {
        oss << "  " << (i + 1) << ". " << topFiles[i].first << ": " << topFiles[i].second << " operations\n";
    }
    
    return oss.str();
}

std::string IOProfiler::generateDiskReport() const {
    std::ostringstream oss;
    oss << "=== NLM Disk I/O Report ===\n\n";
    
    auto diskStats = getDiskStatistics();
    oss << "Disk Performance Statistics:\n";
    oss << "  Average Read Speed: " << diskStats["average_read_speed_mbps"] << " MB/s\n";
    oss << "  Average Write Speed: " << diskStats["average_write_speed_mbps"] << " MB/s\n";
    oss << "  Read/Write Ratio: " << diskStats["io_ratio"] << "\n";
    oss << "  Average Read Latency: " << diskStats["average_read_latency_ms"] << " ms\n";
    oss << "  Average Write Latency: " << diskStats["average_write_latency_ms"] << " ms\n\n";
    
    std::lock_guard<std::mutex> lock(pImpl->diskPerformancesMutex);
    if (!pImpl->diskPerformances.empty()) {
        oss << "Current Disk Status:\n";
        const auto& disk = pImpl->diskPerformances.back();
        oss << "  Device: " << disk.deviceName << " (" << disk.deviceType << ")\n"
            << "  Used Space: " << (disk.usedSpace / (1024 * 1024 * 1024)) << " GB\n"
            << "  Free Space: " << (disk.freeSpace / (1024 * 1024 * 1024)) << " GB\n"
            << "  Usage: " << disk.usagePercentage << "%\n"
            << "  IOPS: Read " << disk.readIOPS << ", Write " << disk.writeIOPS << "\n";
    }
    
    return oss.str();
}

std::string IOProfiler::generateNetworkIOReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network I/O Report ===\n\n";
    
    auto networkStats = getNetworkStatistics();
    oss << "Network I/O Statistics:\n";
    oss << "  Average Upload Speed: " << networkStats["average_upload_speed_mbps"] << " Mbps\n";
    oss << "  Average Download Speed: " << networkStats["average_download_speed_mbps"] << " Mbps\n";
    oss << "  Average Ping Latency: " << networkStats["average_ping_latency_ms"] << " ms\n";
    oss << "  Average Jitter: " << networkStats["average_jitter_ms"] << " ms\n";
    oss << "  Average Packet Loss Rate: " << networkStats["average_packet_loss_rate"] << "\n\n";
    
    return oss.str();
}

std::string IOProfiler::generateIOPatternReport() const {
    std::ostringstream oss;
    oss << "=== NLM I/O Pattern Analysis Report ===\n\n";
    
    auto patterns = getIOPatterns();
    oss << "Active I/O Patterns:\n";
    for (const auto& pair : patterns) {
        const auto& pattern = pair.second;
        oss << "  Pattern ID: " << pair.first << "\n"
            << "    Type: " << pattern.patternType << "\n"
            << "    Data Pattern: " << pattern.dataPattern << "\n"
            << "    Total Operations: " << pattern.totalOperations << "\n"
            << "    Total Bytes: " << pattern.totalBytes << "\n"
            << "    Average Latency: " << pattern.averageLatency << " ms\n"
            << "    Throughput: " << pattern.throughput << " bytes/sec\n"
            << "    Peak IOPS: " << pattern.peakOperationsPerSec << "\n\n";
    }
    
    return oss.str();
}

std::string IOProfiler::generateBottleneckReport() const {
    std::ostringstream oss;
    oss << "=== NLM I/O Bottleneck Report ===\n\n";
    
    auto bottlenecks = detectBottlenecks();
    if (bottlenecks.empty()) {
        oss << "No I/O bottlenecks detected.\n";
        return oss.str();
    }
    
    oss << "Detected I/O Bottlenecks:\n";
    for (size_t i = 0; i < bottlenecks.size(); ++i) {
        oss << "  " << (i + 1) << ". " << bottlenecks[i] << "\n";
    }
    
    return oss.str();
}

std::string IOProfiler::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    oss << "  \"config\": {\n";
    oss << "    \"enableFileIOProfiling\": " << (config_.enableFileIOProfiling ? "true" : "false") << ",\n";
    oss << "    \"enableDiskPerformanceMonitoring\": " << (config_.enableDiskPerformanceMonitoring ? "true" : "false") << ",\n";
    oss << "    \"enableNetworkIOMonitoring\": " << (config_.enableNetworkIOMonitoring ? "true" : "false") << ",\n";
    oss << "    \"enablePatternAnalysis\": " << (config_.enablePatternAnalysis ? "true" : "false") << ",\n";
    oss << "    \"enableBottleneckDetection\": " << (config_.enableBottleneckDetection ? "true" : "false") << "\n";
    oss << "  },\n";
    
    // Export file operations
    oss << "  \"fileOperations\": [\n";
    bool firstOperation = true;
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    for (const auto& operation : pImpl->fileOperations) {
        if (!firstOperation) oss << ",\n";
        firstOperation = false;
        
        oss << "    {\n";
        oss << "      \"operationId\": " << operation.operationId << ",\n";
        oss << "      \"timestamp\": \"" << operation.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"operation\": " << static_cast<int>(operation.operation) << ",\n";
        oss << "      \"filePath\": \"" << operation.filePath << "\",\n";
        oss << "      \"fileSize\": " << operation.fileSize << ",\n";
        oss << "      \"bytesTransferred\": " << operation.bytesTransferred << ",\n";
        oss << "      \"latency\": " << operation.latency << ",\n";
        oss << "      \"throughput\": " << operation.throughput << ",\n";
        oss << "      \"blockSize\": " << operation.blockSize << ",\n";
        oss << "      \"accessPattern\": \"" << operation.accessPattern << "\",\n";
        oss << "      \"ioPriority\": \"" << operation.ioPriority << "\",\n";
        oss << "      \"successful\": " << (operation.successful ? "true" : "false") << ",\n";
        oss << "      \"userContext\": \"" << operation.userContext << "\"\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export disk performance
    oss << "  \"diskPerformance\": [\n";
    bool firstDisk = true;
    std::lock_guard<std::mutex> lock2(pImpl->diskPerformancesMutex);
    for (const auto& disk : pImpl->diskPerformances) {
        if (!firstDisk) oss << ",\n";
        firstDisk = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << disk.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"deviceName\": \"" << disk.deviceName << "\",\n";
        oss << "      \"deviceType\": \"" << disk.deviceType << "\",\n";
        oss << "      \"totalSpace\": " << disk.totalSpace << ",\n";
        oss << "      \"freeSpace\": " << disk.freeSpace << ",\n";
        oss << "      \"usedSpace\": " << disk.usedSpace << ",\n";
        oss << "      \"readSpeed\": " << disk.readSpeed << ",\n";
        oss << "      \"writeSpeed\": " << disk.writeSpeed << ",\n";
        oss << "      \"readIOPS\": " << disk.readIOPS << ",\n";
        oss << "      \"writeIOPS\": " << disk.writeIOPS << ",\n";
        oss << "      \"averageReadLatency\": " << disk.averageReadLatency << ",\n";
        oss << "      \"averageWriteLatency\": " << disk.averageWriteLatency << ",\n";
        oss << "      \"queueDepth\": " << disk.queueDepth << ",\n";
        oss << "      \"readOperations\": " << disk.readOperations << ",\n";
        oss << "      \"writeOperations\": " << disk.writeOperations << ",\n";
        oss << "      \"filesystem\": \"" << disk.filesystem << "\",\n";
        oss << "      \"mountPoint\": \"" << disk.mountPoint << "\",\n";
        oss << "      \"isSSD\": " << (disk.isSSD ? "true" : "false") << ",\n";
        oss << "      \"isNVMe\": " << (disk.isNVMe ? "true" : "false") << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export network performance
    oss << "  \"networkPerformance\": [\n";
    bool firstNetwork = true;
    std::lock_guard<std::mutex> lock3(pImpl->networkIOPerformancesMutex);
    for (const auto& network : pImpl->networkIOPerformances) {
        if (!firstNetwork) oss << ",\n";
        firstNetwork = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << network.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"protocol\": \"" << network.protocol << "\",\n";
        oss << "      \"endpoint\": \"" << network.endpoint << "\",\n";
        oss << "      \"uploadSpeed\": " << network.uploadSpeed << ",\n";
        oss << "      \"downloadSpeed\": " << network.downloadSpeed << ",\n";
        oss << "      \"pingLatency\": " << network.pingLatency << ",\n";
        oss << "      \"jitter\": " << network.jitter << ",\n";
        oss << "      \"packetsSent\": " << network.packetsSent << ",\n";
        oss << "      \"packetsReceived\": " << network.packetsReceived << ",\n";
        oss << "      \"packetLossRate\": " << network.packetLossRate << ",\n";
        oss << "      \"connectionTime\": " << network.connectionTime << ",\n";
        oss << "      \"concurrentConnections\": " << network.concurrentConnections << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export I/O patterns
    oss << "  \"ioPatterns\": {\n";
    bool firstPattern = true;
    std::lock_guard<std::mutex> lock4(pImpl->patternsMutex);
    for (const auto& pair : pImpl->ioPatterns) {
        if (!firstPattern) oss << ",\n";
        firstPattern = false;
        
        const auto& pattern = pair.second;
        oss << "    \"" << pair.first << "\": {\n";
        oss << "      \"patternType\": \"" << pattern.patternType << "\",\n";
        oss << "      \"startTime\": \"" << pattern.startTime.time_since_epoch().count() << "\",\n";
        oss << "      \"endTime\": \"" << pattern.endTime.time_since_epoch().count() << "\",\n";
        oss << "      \"totalBytes\": " << pattern.totalBytes << ",\n";
        oss << "      \"totalOperations\": " << pattern.totalOperations << ",\n";
        oss << "      \"averageLatency\": " << pattern.averageLatency << ",\n";
        oss << "      \"latencyVariation\": " << pattern.latencyVariation << ",\n";
        oss << "      \"throughput\": " << pattern.throughput << ",\n";
        oss << "      \"peakOperationsPerSec\": " << pattern.peakOperationsPerSec << ",\n";
        oss << "      \"burstFrequency\": " << pattern.burstFrequency << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export bottlenecks
    oss << "  \"bottlenecks\": [\n";
    bool firstBottleneck = true;
    std::lock_guard<std::mutex> lock5(pImpl->bottlenecksMutex);
    for (const auto& bottleneck : pImpl->bottlenecks) {
        if (!firstBottleneck) oss << ",\n";
        firstBottleneck = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << bottleneck.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"bottleneckType\": \"" << bottleneck.bottleneckType << "\",\n";
        oss << "      \"component\": \"" << bottleneck.component << "\",\n";
        oss << "      \"measuredValue\": " << bottleneck.measuredValue << ",\n";
        oss << "      \"threshold\": " << bottleneck.threshold << ",\n";
        oss << "      \"severity\": " << bottleneck.severity << ",\n";
        oss << "      \"isActive\": " << (bottleneck.isActive ? "true" : "false") << ",\n";
        oss << "      \"recommendation\": \"" << bottleneck.recommendation << "\",\n";
        oss << "      \"duration\": " << std::chrono::duration<double>(bottleneck.duration).count() << "\n";
        oss << "    }";
    }
    oss << "\n  ]\n";
    
    oss << "}\n";
    
    return oss.str();
}

std::string IOProfiler::exportToCSV() const {
    std::ostringstream oss;
    oss << "OperationID,Timestamp,FilePath,Operation,BytesTransferred,LatencyMs,Throughput,AccessPattern,UserContext\n";
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    for (const auto& operation : pImpl->fileOperations) {
        oss << operation.operationId << ","
            << operation.timestamp.time_since_epoch().count() << ","
            << operation.filePath << ","
            << static_cast<int>(operation.operation) << ","
            << operation.bytesTransferred << ","
            << operation.latency << ","
            << operation.throughput << ","
            << operation.accessPattern << ","
            << operation.userContext << "\n";
    }
    
    return oss.str();
}

std::string IOProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM I/O Profiler Report ===\n\n";
    oss << "I/O Performance Statistics:\n";
    oss << "  Total File Operations: " << pImpl->totalFileOperations << "\n";
    oss << "  Total Bytes Transferred: " << pImpl->totalBytesTransferred << "\n\n";
    
    // Generate performance metrics
    auto ioMetrics = analyzeIOPerformance();
    auto diskMetrics = analyzeDiskPerformance();
    auto networkMetrics = analyzeNetworkPerformance();
    
    oss << "File I/O Performance:\n";
    for (const auto& pair : ioMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "Disk Performance:\n";
    for (const auto& pair : diskMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "Network Performance:\n";
    for (const auto& pair : networkMetrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    // Generate individual reports
    oss << generateFileIOReport();
    oss << "\n";
    oss << generateDiskReport();
    oss << "\n";
    oss << generateNetworkIOReport();
    oss << "\n";
    oss << generateIOPatternReport();
    oss << "\n";
    oss << generateBottleneckReport();
    
    return oss.str();
}

void IOProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    pImpl->fileOperations.clear();
    
    std::lock_guard<std::mutex> lock2(pImpl->diskPerformancesMutex);
    pImpl->diskPerformances.clear();
    
    std::lock_guard<std::mutex> lock3(pImpl->networkIOPerformancesMutex);
    pImpl->networkIOPerformances.clear();
    
    std::lock_guard<std::mutex> lock4(pImpl->patternsMutex);
    pImpl->ioPatterns.clear();
    
    std::lock_guard<std::mutex> lock5(pImpl->bottlenecksMutex);
    pImpl->bottlenecks.clear();
}

IOProfiler& IOProfiler::getInstance() {
    static IOProfiler instance;
    return instance;
}

std::vector<std::pair<std::string, uint64_t>> IOProfiler::getTopFilesByIO(size_t limit) const {
    std::unordered_map<std::string, uint64_t> fileIOCounts;
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    for (const auto& operation : pImpl->fileOperations) {
        fileIOCounts[operation.filePath]++;
    }
    
    // Convert to vector and sort
    std::vector<std::pair<std::string, uint64_t>> topFiles;
    for (const auto& pair : fileIOCounts) {
        topFiles.push_back(pair);
    }
    
    std::sort(topFiles.begin(), topFiles.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (topFiles.size() > limit) {
        topFiles.resize(limit);
    }
    
    return topFiles;
}

std::unordered_map<std::string, double> IOProfiler::getPerformanceByOperation() const {
    std::unordered_map<std::string, double> operationPerformance;
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    for (const auto& operation : pImpl->fileOperations) {
        std::string operationStr;
        switch (operation.operation) {
            case FileOperation::READ: operationStr = "READ"; break;
            case FileOperation::WRITE: operationStr = "WRITE"; break;
            case FileOperation::READ_WRITE: operationStr = "READ_WRITE"; break;
            case FileOperation::DELETE: operationStr = "DELETE"; break;
            case FileOperation::CREATE: operationStr = "CREATE"; break;
            case FileOperation::OPEN: operationStr = "OPEN"; break;
            case FileOperation::CLOSE: operationStr = "CLOSE"; break;
            case FileOperation::SYNC: operationStr = "SYNC"; break;
            case FileOperation::FTRUNCATE: operationStr = "FTRUNCATE"; break;
            case FileOperation::FSTAT: operationStr = "FSTAT"; break;
            case FileOperation::LSEEK: operationStr = "LSEEK"; break;
            case FileOperation::FSYNC: operationStr = "FSYNC"; break;
            default: operationStr = "UNKNOWN";
        }
        
        operationPerformance[operationStr] += operation.throughput;
    }
    
    return operationPerformance;
}

std::unordered_map<std::string, double> IOProfiler::getPerformanceByAccessPattern() const {
    std::unordered_map<std::string, double> patternPerformance;
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    for (const auto& operation : pImpl->fileOperations) {
        patternPerformance[operation.accessPattern] += operation.throughput;
    }
    
    return patternPerformance;
}

std::unordered_map<std::string, double> IOProfiler::getDiskStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    std::lock_guard<std::mutex> lock(pImpl->diskPerformancesMutex);
    
    if (pImpl->diskPerformances.empty()) {
        return stats;
    }
    
    double totalReadSpeed = 0.0;
    double totalWriteSpeed = 0.0;
    double totalReadLatency = 0.0;
    double totalWriteLatency = 0.0;
    
    for (const auto& disk : pImpl->diskPerformances) {
        totalReadSpeed += disk.readSpeed;
        totalWriteSpeed += disk.writeSpeed;
        totalReadLatency += disk.averageReadLatency;
        totalWriteLatency += disk.averageWriteLatency;
    }
    
    size_t count = pImpl->diskPerformances.size();
    if (count > 0) {
        stats["average_read_speed"] = totalReadSpeed / count;
        stats["average_write_speed"] = totalWriteSpeed / count;
        stats["average_read_latency"] = totalReadLatency / count;
        stats["average_write_latency"] = totalWriteLatency / count;
    }
    
    return stats;
}

std::unordered_map<std::string, double> IOProfiler::getNetworkStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    std::lock_guard<std::mutex> lock(pImpl->networkIOPerformancesMutex);
    
    if (pImpl->networkIOPerformances.empty()) {
        return stats;
    }
    
    double totalUpload = 0.0;
    double totalDownload = 0.0;
    double totalPing = 0.0;
    double totalJitter = 0.0;
    double totalPacketLoss = 0.0;
    
    for (const auto& network : pImpl->networkIOPerformances) {
        totalUpload += network.uploadSpeed;
        totalDownload += network.downloadSpeed;
        totalPing += network.pingLatency;
        totalJitter += network.jitter;
        totalPacketLoss += network.packetLossRate;
    }
    
    size_t count = pImpl->networkIOPerformances.size();
    if (count > 0) {
        stats["average_upload_speed"] = totalUpload / count;
        stats["average_download_speed"] = totalDownload / count;
        stats["average_ping_latency"] = totalPing / count;
        stats["average_jitter"] = totalJitter / count;
        stats["average_packet_loss_rate"] = totalPacketLoss / count;
    }
    
    return stats;
}

std::vector<std::string> IOProfiler::detectBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    
    for (const auto& operation : pImpl->fileOperations) {
        if (operation.latency > 100.0) {
            bottlenecks.push_back("high_latency: " + operation.filePath + " (" + std::to_string(operation.latency) + " ms)");
        }
        
        if (operation.throughput < 100 * 1024 * 1024) { // < 100 MB/s
            bottlenecks.push_back("low_throughput: " + operation.filePath + " (" + std::to_string(operation.throughput / (1024 * 1024)) + " MB/s)");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> IOProfiler::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Analyze for optimization opportunities
    std::lock_guard<std::mutex> lock(pImpl->fileOperationsMutex);
    
    // Check for latency issues
    double highLatencyCount = 0.0;
    for (const auto& operation : pImpl->fileOperations) {
        if (operation.latency > 100.0) {
            highLatencyCount++;
        }
    }
    
    if (highLatencyCount > pImpl->fileOperations.size() * 0.3) {
        recommendations.push_back("Investigate high latency issues - consider SSD storage or optimized file access patterns");
    }
    
    // Check for throughput issues
    double lowThroughputCount = 0.0;
    for (const auto& operation : pImpl->fileOperations) {
        if (operation.throughput < 100 * 1024 * 1024) {
            lowThroughputCount++;
        }
    }
    
    if (lowThroughputCount > pImpl->fileOperations.size() * 0.3) {
        recommendations.push_back("Consider parallel I/O operations or distributed file systems to improve throughput");
    }
    
    // Check for access pattern issues
    std::unordered_map<std::string, size_t> accessPatternCounts;
    for (const auto& operation : pImpl->fileOperations) {
        accessPatternCounts[operation.accessPattern]++;
    }
    
    size_t totalOperations = pImpl->fileOperations.size();
    for (const auto& pair : accessPatternCounts) {
        if (static_cast<double>(pair.second) / totalOperations > 0.7) {
            recommendations.push_back("Optimize access patterns - consider " + pair.first + " patterns for better performance");
            break;
        }
    }
    
    return recommendations;
}

} // namespace nlm