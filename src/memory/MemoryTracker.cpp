#include "MemoryTracker.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <execinfo.h> // For stack traces (Linux)

namespace nlm {

MemoryTracker::MemoryTracker()
    : enabled_(true)
    , initialized_(false)
    , maxHistorySize_(10000)
    , samplingRate_(1.0)
    , memoryPool_(nullptr)
    , brain_(nullptr)
    , eventQueue_(nullptr)
    , parallelProc_(nullptr)
    , totalMemoryAllocated_(0)
    , totalMemoryDeallocated_(0)
    , currentMemoryUsage_(0)
    , peakMemoryUsage_(0)
    , totalMemoryAccesses_(0)
    , cacheHits_(0)
    , memoryBandwidth_(0.0)
    , currentStep_(0)
    , currentTime_(0.0) {
    // Initialize tracking data structures
    activeAllocations_.reserve(maxHistorySize_);
    allocationHistory_.reserve(maxHistorySize_);
    deallocationHistory_.reserve(maxHistorySize_);
    memoryBlocks_.reserve(maxHistorySize_ / 10);
    
    initialized_ = true;
}

MemoryTracker::~MemoryTracker() {
    flush();
}

MemoryTracker& MemoryTracker::getInstance() {
    static MemoryTracker instance;
    return instance;
}

void MemoryTracker::enable(bool enable) {
    enabled_ = enable;
}

void MemoryTracker::trackAllocation(size_t address, size_t size, const std::string& site,
                                    AllocationType type, const std::string& stackTrace) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Record allocation
    AllocationTracker alloc(address, size, site, stackTrace, type, getCurrentThreadId());
    alloc.allocationTime = getCurrentTime();
    activeAllocations_.push_back(alloc);
    
    // Update statistics
    totalMemoryAllocated_ += size;
    currentMemoryUsage_ += size;
    peakMemoryUsage_ = std::max(peakMemoryUsage_.load(), currentMemoryUsage_.load());
    
    // Record in history
    AllocationRecord history(getCurrentTime(), address, size, site, stackTrace, type, getCurrentThreadId());
    allocationHistory_.push_back(history);
    
    // Keep history size bounded
    if (allocationHistory_.size() > maxHistorySize_) {
        allocationHistory_.erase(allocationHistory_.begin(), 
                               allocationHistory_.begin() + maxHistorySize_ / 2);
    }
}

void MemoryTracker::trackDeallocation(size_t address, const std::string& site) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Find and remove allocation
    auto it = std::find_if(activeAllocations_.begin(), activeAllocations_.end(),
                          [address](const AllocationTracker& alloc) { return alloc.address == address; });n    
    if (it != activeAllocations_.end()) {
        size_t size = it->size;
        
        // Update statistics
        currentMemoryUsage_ -= size;
        totalMemoryDeallocated_ += size;
        
        // Remove from active allocations
        activeAllocations_.erase(it);
        
        // Record deallocation
        DeallocationRecord dealloc(getCurrentTime(), address, site, getCurrentThreadId(), true);
        deallocationHistory_.push_back(dealloc);
        
        // Update memory blocks
        updateMemoryBlocks();
    } else {
        // Deallocation without corresponding allocation (leak or double free)
        DeallocationRecord dealloc(getCurrentTime(), address, site, getCurrentThreadId(), false);
        deallocationHistory_.push_back(dealloc);
    }
}

void MemoryTracker::trackMemoryPoolUsage(size_t allocated, size_t available, const std::string& poolName) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Update memory blocks information
    MemoryBlockInfo blockInfo;
    blockInfo.startAddress = 0; // Memory pool address would be tracked separately
    blockInfo.size = allocated + available;
    blockInfo.type = AllocationType::Pool;
    blockInfo.description = "Memory pool: " + poolName;
    blockInfo.threadId = getCurrentThreadId();
    blockInfo.lastAccessTime = getCurrentTime();
    
    memoryBlocks_.push_back(blockInfo);
}

void MemoryTracker::integrateWithMemoryPool(nlm::MemoryPool* memoryPool) {
    memoryPool_ = memoryPool;
}

void MemoryTracker::integrateWithBrain(nlm::Brain* brain) {
    brain_ = brain;
}

void MemoryTracker::integrateWithEventQueue(nlm::EventQueue* eventQueue) {
    eventQueue_ = eventQueue;
}

void MemoryTracker::integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc) {
    parallelProc_ = parallelProc;
}

MemoryEfficiencyMetrics MemoryTracker::getEfficiencyMetrics() const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    MemoryEfficiencyMetrics metrics;
    
    // Calculate basic metrics
    metrics.totalAllocated = totalMemoryAllocated_.load();
    metrics.totalDeallocated = totalMemoryDeallocated_.load();
    metrics.currentMemoryUsage = currentMemoryUsage_.load();
    metrics.peakMemoryUsage = peakMemoryUsage_.load();
    metrics.activeAllocations = activeAllocations_.size();
    
    // Calculate efficiency
    metrics.allocationEfficiency = metrics.totalAllocated > 0 ? 
        static_cast<double>(metrics.totalDeallocated) / metrics.totalAllocated : 0.0;
    
    // Calculate fragmentation (simplified)
    if (metrics.totalAllocated > 0) {
        metrics.fragmentationRatio = static_cast<double>(metrics.activeAllocations) / 
                                     (metrics.totalAllocated / 64); // Assuming 64-byte minimum allocation
    } else {
        metrics.fragmentationRatio = 0.0;
    }
    
    // Calculate average allocation size
    if (metrics.activeAllocations > 0) {
        size_t totalSize = 0;
        for (const auto& alloc : activeAllocations_) {
            totalSize += alloc.size;
        }
        metrics.averageAllocationSize = totalSize / metrics.activeAllocations;
    } else {
        metrics.averageAllocationSize = 0;
    }
    
    // Calculate bandwidth efficiency
    metrics.bandwidthUtilization = memoryBandwidth_.load();
    
    // Calculate cache efficiency
    if (totalMemoryAccesses_.load() > 0) {
        metrics.cacheHits = cacheHits_.load();
        size_t totalAccesses = totalMemoryAccesses_.load();
        metrics.cacheHits = cacheHits_.load();
    }
    
    return metrics;
}

std::vector<MemoryLeakDetection> MemoryTracker::detectMemoryLeaks() const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    analyzeMemoryLeaks();
    return detectedLeaks_;
}

std::vector<MemoryBlockInfo> MemoryTracker::getMemoryBlocks() const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    return memoryBlocks_;
}

std::vector<AllocationRecord> MemoryTracker::getAllocationHistory(size_t address) const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    std::vector<AllocationRecord> history;
    for (const auto& record : allocationHistory_) {
        if (record.address == address) {
            history.push_back(record);
        }
    }
    return history;
}

std::vector<DeallocationRecord> MemoryTracker::getDeallocationHistory(size_t address) const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    std::vector<DeallocationRecord> history;
    for (const auto& record : deallocationHistory_) {
        if (record.address == address) {
            history.push_back(record);
        }
    }
    return history;
}

void MemoryTracker::recordAccess(size_t address, uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Update memory block access time
    for (auto& block : memoryBlocks_) {
        if (block.startAddress == address) {
            block.lastAccessTime = getCurrentTime();
            break;
        }
    }
}

void MemoryTracker::recordMemoryAccess(size_t bytes, bool cacheHit) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    totalMemoryAccesses_++;
    if (cacheHit) {
        cacheHits_++;
    }
    
    // Update bandwidth (simplified - bytes per time unit)
    memoryBandwidth_ = static_cast<double>(totalMemoryAccesses_.load()) / 1000.0; // Placeholder
}

double MemoryTracker::getMemoryBandwidth() const {
    return memoryBandwidth_.load();
}

double MemoryTracker::getCacheEfficiency() const {
    if (totalMemoryAccesses_.load() > 0) {
        return static_cast<double>(cacheHits_.load()) / totalMemoryAccesses_.load();
    }
    return 0.0;
}

double MemoryTracker::getCurrentMemoryUsage() const {
    return static_cast<double>(currentMemoryUsage_.load());
}

size_t MemoryTracker::getTotalMemoryAllocated() const {
    return totalMemoryAllocated_.load();
}

size_t MemoryTracker::getTotalMemoryDeallocated() const {
    return totalMemoryDeallocated_.load();
}

void MemoryTracker::startStep(size_t step, double time) {
    currentStep_ = step;
    currentTime_ = time;
}

void MemoryTracker::endStep(size_t step, double time) {
    // Analyze memory usage for this step
    if (enabled_) {
        analyzeMemoryLeaks();
    }
}

std::string MemoryTracker::generateReport() const {
    std::stringstream report;
    
    report << "=== NLM Memory Tracker Report ===" << std::endl;
    report << "Generated at: " << getCurrentTimeString() << std::endl;
    report << "Current Step: " << currentStep_ << std::endl;
    report << "Current Time: " << currentTime_ << "s" << std::endl;
    report << std::endl;
    
    // Basic statistics
    report << "=== Basic Statistics ===" << std::endl;
    report << "Total Memory Allocated: " << totalMemoryAllocated_.load() << " bytes" << std::endl;
    report << "Total Memory Deallocated: " << totalMemoryDeallocated_.load() << " bytes" << std::endl;
    report << "Current Memory Usage: " << currentMemoryUsage_.load() << " bytes" << std::endl;
    report << "Peak Memory Usage: " << peakMemoryUsage_.load() << " bytes" << std::endl;
    report << "Active Allocations: " << activeAllocations_.size() << std::endl;
    report << std::endl;
    
    // Efficiency metrics
    report << "=== Efficiency Metrics ===" << std::endl;
    MemoryEfficiencyMetrics metrics = getEfficiencyMetrics();
    report << "Allocation Efficiency: " << std::fixed << std::setprecision(2) 
           << (metrics.allocationEfficiency * 100.0) << "%" << std::endl;
    report << "Fragmentation Ratio: " << std::fixed << std::setprecision(2)
           << metrics.fragmentationRatio << std::endl;
    report << "Average Allocation Size: " << metrics.averageAllocationSize << " bytes" << std::endl;
    report << "Memory Bandwidth: " << std::fixed << std::setprecision(2)
           << metrics.bandwidthUtilization << " GB/s" << std::endl;
    report << "Cache Efficiency: " << std::fixed << std::setprecision(2)
           << (metrics.cacheHits > 0 ? (static_cast<double>(metrics.cacheHits) / totalMemoryAccesses_.load() * 100.0) : 0.0)
           << "%" << std::endl;
    report << std::endl;
    
    // Memory leak detection
    report << "=== Memory Leak Detection ===" << std::endl;
    analyzeMemoryLeaks();
    if (detectedLeaks_.empty()) {
        report << "No memory leaks detected." << std::endl;
    } else {
        report << "Detected " << detectedLeaks_.size() << " potential memory leaks:" << std::endl;
        for (size_t i = 0; i < detectedLeaks_.size(); ++i) {
            const auto& leak = detectedLeaks_[i];
            report << "  Leak " << i + 1 << ": " << memoryLeakTypeToString(leak.type)
                   << " (Address: 0x" << std::hex << leak.address << std::dec << ")"
                   << " Size: " << leak.size << " bytes"
                   << " Confidence: " << std::fixed << std::setprecision(2) << leak.confidenceScore * 100.0 << "%"
                   << std::endl;
        }
    }
    report << std::endl;
    
    // Memory blocks
    report << "=== Active Memory Blocks ===" << std::endl;
    if (memoryBlocks_.empty()) {
        report << "No memory blocks tracked." << std::endl;
    } else {
        report << "Tracked " << memoryBlocks_.size() << " memory blocks:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), memoryBlocks_.size()); ++i) {
            const auto& block = memoryBlocks_[i];
            report << "  Block " << i + 1 << ": " << memoryBlockTypeToString(block.type)
                   << " Size: " << block.size << " bytes"
                   << " Thread: " << block.threadId
                   << " Last Access: " << block.lastAccessTime << std::endl;
        }
        if (memoryBlocks_.size() > 10) {
            report << "  ... and " << memoryBlocks_.size() - 10 << " more blocks." << std::endl;
        }
    }
    report << std::endl;
    
    return report.str();
}

void MemoryTracker::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateReport();
        file.close();
    }
}

void MemoryTracker::updateMemoryBlocks() {
    // Simplified implementation - would need more sophisticated tracking in production
    memoryBlocks_.clear();
    
    // Track active allocations as memory blocks
    for (const auto& alloc : activeAllocations_) {
        MemoryBlockInfo block;
        block.startAddress = alloc.address;
        block.size = alloc.size;
        block.type = alloc.type;
        block.description = alloc.allocationSite;
        block.threadId = alloc.threadId;
        block.lastAccessTime = getCurrentTime();
        memoryBlocks_.push_back(block);
    }
}

void MemoryTracker::analyzeMemoryLeaks() {
    std::lock_guard<std::mutex> lock(leakDetectionMutex_);
    
    detectedLeaks_.clear();
    detectUnfreedAllocations();
    detectPoolDepletion();
    detectFragmentation();
}

void MemoryTracker::detectUnfreedAllocations() {
    // Simple detection of allocations that have no deallocation record
    for (const auto& alloc : activeAllocations_) {
        // Check if there's a deallocation record for this address
        bool deallocated = false;
        for (const auto& dealloc : deallocationHistory_) {
            if (dealloc.address == alloc.address) {
                deallocated = true;
                break;
            }
        }
        
        if (!deallocated) {
            MemoryLeakDetection leak;
            leak.type = MemoryLeakType::UnfreedAllocation;
            leak.address = alloc.address;
            leak.size = alloc.size;
            leak.allocationSite = alloc.allocationSite;
            leak.stackTrace = alloc.stackTrace;
            leak.threadId = alloc.threadId;
            leak.timeSinceAllocation = getCurrentTime() - alloc.allocationTime;
            leak.confidenceScore = std::min(1.0, leak.timeSinceAllocation / 1000.0); // Increases over time
            
            detectedLeaks_.push_back(leak);
            leakedAddresses_.insert(alloc.address);
        }
    }
}

void MemoryTracker::detectPoolDepletion() {
    // Check if memory pool is exhausted (simplified)
    if (memoryPool_ && currentMemoryUsage_.load() > peakMemoryUsage_.load() * 0.95) {
        MemoryLeakDetection leak;
        leak.type = MemoryLeakType::PoolDepletion;
        leak.address = 0; // Memory pool address would be tracked separately
        leak.size = peakMemoryUsage_.load() - currentMemoryUsage_.load();
        leak.allocationSite = "MemoryPool::allocate()";
        leak.stackTrace = getCurrentStackTrace();
        leak.threadId = getCurrentThreadId();
        leak.timeSinceAllocation = 0.0;
        leak.confidenceScore = 1.0;
        
        detectedLeaks_.push_back(leak);
    }
}

void MemoryTracker::detectFragmentation() {
    // Simple fragmentation detection based on allocation size distribution
    if (activeAllocations_.size() < 2) return;
    
    std::vector<size_t> allocationSizes;
    for (const auto& alloc : activeAllocations_) {
        allocationSizes.push_back(alloc.size);
    }
    
    std::sort(allocationSizes.begin(), allocationSizes.end());
    
    // Check for unusual size distribution (potential fragmentation)
    size_t minSize = allocationSizes.front();
    size_t maxSize = allocationSizes.back();
    size_t avgSize = std::accumulate(allocationSizes.begin(), allocationSizes.end(), 0) / allocationSizes.size();
    
    if (maxSize > avgSize * 10 && minSize < avgSize / 10) {
        MemoryLeakDetection leak;
        leak.type = MemoryLeakType::Fragmentation;
        leak.address = 0;
        leak.size = currentMemoryUsage_.load();
        leak.allocationSite = "Memory allocation distribution analysis";
        leak.stackTrace = "Multiple allocations with highly varying sizes";
        leak.threadId = getCurrentThreadId();
        leak.timeSinceAllocation = 0.0;
        leak.confidenceScore = 0.8;
        
        detectedLeaks_.push_back(leak);
    }
}

std::string MemoryTracker::getCurrentStackTrace() const {
    // Simplified stack trace collection
    std::stringstream trace;
    trace << "Stack trace at 0x" << std::hex << getCurrentThreadId();
    
    // In a real implementation, this would capture actual stack trace
    return trace.str();
}

std::string MemoryTracker::getAllocationSite() const {
    // Get current file and line (simplified)
    return "Unknown allocation site";
}

size_t MemoryTracker::getCurrentTime() const {
    return static_cast<size_t>(currentTime_ * 1000); // Convert to milliseconds
}

std::string MemoryTracker::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string MemoryTracker::memoryLeakTypeToString(MemoryLeakType type) const {
    switch (type) {
        case MemoryLeakType::UnfreedAllocation: return "Unfreed Allocation";
        case MemoryLeakType::PoolDepletion: return "Pool Depletion";
        case MemoryLeakType::Fragmentation: return "Memory Fragmentation";
        case MemoryLeakType::CrossThreadReference: return "Cross-thread Reference";
        case MemoryLeakType::SimulationError: return "Simulation Error";
        default: return "Unknown";
    }
}

std::string MemoryTracker::memoryBlockTypeToString(AllocationType type) const {
    switch (type) {
        case AllocationType::Standard: return "Standard";
        case AllocationType::Pool: return "Memory Pool";
        case AllocationType::ThreadLocal: return "Thread Local";
        case AllocationType::SIMD: return "SIMD";
        case AllocationType::Event: return "Event Queue";
        case AllocationType::Simulation: return "Simulation";
        default: return "Unknown";
    }
}

uint32_t MemoryTracker::getCurrentThreadId() const {
    // Simplified thread ID - in production would use std::this_thread::get_id()
    return static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

void MemoryTracker::flush() {
    // Flush all tracking data
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    activeAllocations_.clear();
    allocationHistory_.clear();
    deallocationHistory_.clear();
    memoryBlocks_.clear();
    
    detectedLeaks_.clear();
    leakedAddresses_.clear();
}

void MemoryTracker::reset() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    totalMemoryAllocated_ = 0;
    totalMemoryDeallocated_ = 0;
    currentMemoryUsage_ = 0;
    peakMemoryUsage_ = 0;
    totalMemoryAccesses_ = 0;
    cacheHits_ = 0;
    memoryBandwidth_ = 0.0;
    
    activeAllocations_.clear();
    allocationHistory_.clear();
    deallocationHistory_.clear();
    memoryBlocks_.clear();
    
    detectedLeaks_.clear();
    leakedAddresses_.clear();
}

} // namespace nlm

// Python bindings for MemoryTracker
void initMemoryTrackerBindings(py::module& m) {
    py::class_<nlm::MemoryTracker>(m, "MemoryTracker")
        .def("enable", &nlm::MemoryTracker::enable, py::arg("enable") = true,
             "Enable or disable memory tracking")
        .def("isEnabled", &nlm::MemoryTracker::isEnabled,
             "Check if memory tracking is enabled")
        .def("trackAllocation", &nlm::MemoryTracker::trackAllocation,
             py::arg("address"), py::arg("size"), py::arg("allocationSite"),
             py::arg("type"), py::arg("stackTrace"),
             "Track a memory allocation")
        .def("trackDeallocation", &nlm::MemoryTracker::trackDeallocation,
             py::arg("address"), py::arg("deallocationSite"),
             "Track a memory deallocation")
        .def("getCurrentMemoryUsage", &nlm::MemoryTracker::getCurrentMemoryUsage,
             "Get current memory usage in bytes")
        .def("getTotalMemoryAllocated", &nlm::MemoryTracker::getTotalMemoryAllocated,
             "Get total memory allocated in bytes")
        .def("getTotalMemoryDeallocated", &nlm::MemoryTracker::getTotalMemoryDeallocated,
             "Get total memory deallocated in bytes")
        .def("getEfficiencyMetrics", &nlm::MemoryTracker::getEfficiencyMetrics,
             "Get memory efficiency metrics")
        .def("detectMemoryLeaks", &nlm::MemoryTracker::detectMemoryLeaks,
             "Detect memory leaks")
        .def("startStep", &nlm::MemoryTracker::startStep,
             py::arg("step"), py::arg("time"),
             "Start tracking a simulation step")
        .def("endStep", &nlm::MemoryTracker::endStep,
             py::arg("step"), py::arg("time"),
             "End tracking a simulation step")
        .def("generateReport", &nlm::MemoryTracker::generateReport,
             "Generate a memory tracking report")
        .def("saveReport", &nlm::MemoryTracker::saveReport,
             py::arg("filename"), "Save report to file");
}

PYBIND11_MODULE(memory_tracker, m) {
    m.doc() = R"pbdoc(
        NLM Memory Tracker
        -----------------
        Comprehensive memory tracking and monitoring for the NLM neural simulation framework.
        
        Features:
        - Real-time memory usage monitoring
        - Detailed allocation/deallocation tracking
        - Memory leak detection and reporting
        - Memory efficiency analysis
        - Integration with existing NLM memory systems
    )pbdoc";
    
    // Memory type enums
    py::enum_<nlm::AllocationType>(m, "AllocationType")
        .value("Standard", nlm::AllocationType::Standard)
        .value("Pool", nlm::AllocationType::Pool)
        .value("ThreadLocal", nlm::AllocationType::ThreadLocal)
        .value("SIMD", nlm::AllocationType::SIMD)
        .value("Event", nlm::AllocationType::Event)
        .value("Simulation", nlm::AllocationType::Simulation)
        .export_values();
    
    py::enum_<nlm::MemoryLeakType>(m, "MemoryLeakType")
        .value("UnfreedAllocation", nlm::MemoryLeakType::UnfreedAllocation)
        .value("PoolDepletion", nlm::MemoryLeakType::PoolDepletion)
        .value("Fragmentation", nlm::MemoryLeakType::Fragmentation)
        .value("CrossThreadReference", nlm::MemoryLeakType::CrossThreadReference)
        .value("SimulationError", nlm::MemoryLeakType::SimulationError)
        .export_values();
    
    // Memory structures
    py::struct_<nlm::AllocationRecord>(m, "AllocationRecord")
        .def_readwrite("timestamp", &nlm::AllocationRecord::timestamp)
        .def_readwrite("address", &nlm::AllocationRecord::address)
        .def_readwrite("size", &nlm::AllocationRecord::size)
        .def_readwrite("allocationSite", &nlm::AllocationRecord::allocationSite)
        .def_readwrite("stackTrace", &nlm::AllocationRecord::stackTrace)
        .def_readwrite("type", &nlm::AllocationRecord::type)
        .def_readwrite("threadId", &nlm::AllocationRecord::threadId)
        .def_readwrite("tracked", &nlm::AllocationRecord::tracked);
    
    py::struct_<nlm::DeallocationRecord>(m, "DeallocationRecord")
        .def_readwrite("timestamp", &nlm::DeallocationRecord::timestamp)
        .def_readwrite("address", &nlm::DeallocationRecord::address)
        .def_readwrite("deallocationSite", &nlm::DeallocationRecord::deallocationSite)
        .def_readwrite("threadId", &nlm::DeallocationRecord::threadId)
        .def_readwrite("wasTracked", &nlm::DeallocationRecord::wasTracked);
    
    py::struct_<nlm::MemoryBlockInfo>(m, "MemoryBlockInfo")
        .def_readwrite("startAddress", &nlm::MemoryBlockInfo::startAddress)
        .def_readwrite("size", &nlm::MemoryBlockInfo::size)
        .def_readwrite("type", &nlm::MemoryBlockInfo::type)
        .def_readwrite("description", &nlm::MemoryBlockInfo::description)
        .def_readwrite("threadId", &nlm::MemoryBlockInfo::threadId)
        .def_readwrite("lastAccessTime", &nlm::MemoryBlockInfo::lastAccessTime);
    
    py::struct_<nlm::MemoryEfficiencyMetrics>(m, "MemoryEfficiencyMetrics")
        .def_readwrite("allocationEfficiency", &nlm::MemoryEfficiencyMetrics::allocationEfficiency)
        .def_readwrite("fragmentationRatio", &nlm::MemoryEfficiencyMetrics::fragmentationRatio)
        .def_readwrite("totalAllocated", &nlm::MemoryEfficiencyMetrics::totalAllocated)
        .def_readwrite("totalDeallocated", &nlm::MemoryEfficiencyMetrics::totalDeallocated)
        .def_readwrite("peakMemoryUsage", &nlm::MemoryEfficiencyMetrics::peakMemoryUsage)
        .def_readwrite("currentMemoryUsage", &nlm::MemoryEfficiencyMetrics::currentMemoryUsage)
        .def_readwrite("activeAllocations", &nlm::MemoryEfficiencyMetrics::activeAllocations)
        .def_readwrite("averageAllocationSize", &nlm::MemoryEfficiencyMetrics::averageAllocationSize)
        .def_readwrite("bandwidthUtilization", &nlm::MemoryEfficiencyMetrics::bandwidthUtilization)
        .def_readwrite("cacheHits", &nlm::MemoryEfficiencyMetrics::cacheHits);
    
    py::struct_<nlm::MemoryLeakDetection>(m, "MemoryLeakDetection")
        .def_readwrite("type", &nlm::MemoryLeakDetection::type)
        .def_readwrite("address", &nlm::MemoryLeakDetection::address)
        .def_readwrite("size", &nlm::MemoryLeakDetection::size)
        .def_readwrite("allocationSite", &nlm::MemoryLeakDetection::allocationSite)
        .def_readwrite("stackTrace", &nlm::MemoryLeakDetection::stackTrace)
        .def_readwrite("threadId", &nlm::MemoryLeakDetection::threadId)
        .def_readwrite("timeSinceAllocation", &nlm::MemoryLeakDetection::timeSinceAllocation)
        .def_readwrite("confidenceScore", &nlm::MemoryLeakDetection::confidenceScore);
    
    // Main MemoryTracker class
    initMemoryTrackerBindings(m);
}

} // namespace nlm

// Export for inclusion in bindings.cpp
void initMemoryTrackerBindings(py::module& m);