// MemoryProfiler.cpp implementation
#include "MemoryProfiler.cpp"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

MemoryProfiler::MemoryProfiler() : peakMemoryMB_(0) {}
MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::record() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemorySnapshot snapshot;
    snapshot.timestamp = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
    
    // Get current memory usage (simplified implementation)
    snapshot.totalMemory = getCurrentMemoryMB() * 1024 * 1024; // Convert to bytes
    snapshot.peakMemory = peakMemoryMB_ * 1024 * 1024;
    snapshot.currentMemory = snapshot.totalMemory;
    
    // Memory breakdown (simplified)
    snapshot.neuronMemory = snapshot.totalMemory * 0.6; // 60% neurons
    snapshot.synapseMemory = snapshot.totalMemory * 0.3; // 30% synapses
    snapshot.eventMemory = snapshot.totalMemory * 0.05; // 5% events
    snapshot.otherMemory = snapshot.totalMemory * 0.05; // 5% other
    
    // Count allocations/deallocations
    snapshot.allocationCount = allocationCounts_.size();
    snapshot.deallocationCount = deallocationCounts_.size();
    
    snapshots_.push_back(snapshot);
    
    // Update peak
    if (snapshot.currentMemory > peakMemoryMB_ * 1024 * 1024) {
        peakMemoryMB_ = snapshot.currentMemory / (1024 * 1024);
    }
}

void MemoryProfiler::recordAllocation(size_t size, const std::string& type) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    allocationCounts_[type]++;
    allocationSizes_[type] += size;
    
    // Update peak
    size_t currentTotal = 0;
    for (const auto& pair : allocationSizes_) {
        currentTotal += pair.second;
    }
    
    if (currentTotal > peakMemoryMB_ * 1024 * 1024) {
        peakMemoryMB_ = currentTotal / (1024 * 1024);
    }
}

void MemoryProfiler::recordDeallocation(size_t size, const std::string& type) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    deallocationCounts_[type]++;
    deallocationSizes_[type] += size;
}

bool MemoryProfiler::detectMemoryLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : allocationCounts_) {
        const auto& type = pair.first;
        
        if (allocationCounts_.find(type) != allocationCounts_.end() &&
            deallocationCounts_.find(type) != deallocationCounts_.end()) {
            
            size_t allocated = allocationSizes_.at(type);
            size_t deallocated = deallocationSizes_.at(type);
            
            if (deallocated < allocated) {
                return true; // Memory leak detected
            }
        }
    }
    
    return false;
}

std::vector<std::string> MemoryProfiler::getMemoryLeakLocations() const {
    std::vector<std::string> leakLocations;
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : allocationSizes_) {
        const auto& type = pair.first;
        
        if (allocationCounts_.find(type) != allocationCounts_.end() &&
            deallocationCounts_.find(type) != deallocationCounts_.end()) {
            
            size_t allocated = allocationSizes_.at(type);
            size_t deallocated = deallocationSizes_.at(type);
            
            if (deallocated < allocated) {
                leakLocations.push_back(type + ": " + std::to_string(allocated - deallocated) + " bytes leaked");
            }
        }
    }
    
    return leakLocations;
}

std::string MemoryProfiler::generateMemoryReport() const {
    std::stringstream ss;
    ss << "Memory Report\n";
    ss << "============\n\n";
    
    if (snapshots_.empty()) {
        ss << "No memory snapshots recorded.\n";
        return ss.str();
    }
    
    // Calculate statistics
    double totalTime = snapshots_.back().timestamp - snapshots_.front().timestamp;
    if (totalTime <= 0) totalTime = 1;
    
    size_t totalAllocations = 0;
    size_t totalDeallocations = 0;
    size_t totalAllocatedMemory = 0;
    size_t totalDeallocatedMemory = 0;
    
    for (const auto& snapshot : snapshots_) {
        totalAllocations += snapshot.allocationCount;
        totalDeallocations += snapshot.deallocationCount;
        totalAllocatedMemory += snapshot.neuronMemory + snapshot.synapseMemory + 
                              snapshot.eventMemory + snapshot.otherMemory;
        totalDeallocatedMemory += snapshot.deallocationCount; // Simplified
    }
    
    ss << "Time span: " << totalTime << " seconds\n";
    ss << "Number of snapshots: " << snapshots_.size() << "\n";
    ss << "Total allocations: " << totalAllocations << "\n";
    ss << "Total deallocations: " << totalDeallocations << "\n";
    ss << "Total allocated memory: " << totalAllocatedMemory / (1024 * 1024) << " MB\n";
    ss << "Peak memory usage: " << peakMemoryMB_ << " MB\n\n";
    
    // Memory growth rate
    if (snapshots_.size() > 1) {
        size_t firstMemory = snapshots_.front().currentMemory;
        size_t lastMemory = snapshots_.back().currentMemory;
        double growthRate = (lastMemory - firstMemory) / (totalTime * 1024 * 1024); // MB/s
        ss << "Average memory growth rate: " << growthRate << " MB/s\n\n";
    }
    
    // Show recent snapshots
    ss << "Recent memory snapshots:\n";
    size_t recentCount = std::min(size_t(5), snapshots_.size());
    for (size_t i = 0; i < recentCount; ++i) {
        size_t idx = snapshots_.size() - 1 - i;
        const auto& snapshot = snapshots_[idx];
        ss << "  " << snapshot.timestamp << "s: " 
           << snapshot.currentMemory / (1024 * 1024) << " MB (peak: " 
           << snapshot.peakMemory / (1024 * 1024) << " MB)\n";
    }
    
    return ss.str();
}

std::string MemoryProfiler::generateMemoryHeatmap() const {
    std::stringstream ss;
    ss << "Memory Heatmap\n";
    ss << "==============\n\n";
    
    if (snapshots_.empty()) {
        ss << "No memory data available.\n";
        return ss.str();
    }
    
    // Create a simple heatmap representation
    ss << "Memory allocation over time:\n";
    
    size_t maxMemory = 0;
    for (const auto& snapshot : snapshots_) {
        maxMemory = std::max(maxMemory, snapshot.currentMemory);
    }
    
    if (maxMemory > 0) {
        for (const auto& snapshot : snapshots_) {
            double usageRatio = double(snapshot.currentMemory) / double(maxMemory);
            int barLength = int(usageRatio * 50);
            ss << "  " << snapshot.timestamp << "s: [";
            for (int i = 0; i < barLength; ++i) ss << "#";
            ss << " " << usageRatio * 100 << "%\n";
        }
    }
    
    return ss.str();
}

bool MemoryProfiler::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    j["snapshots"] = json::array();
    
    for (const auto& snapshot : snapshots_) {
        json jSnapshot;
        jSnapshot["timestamp"] = snapshot.timestamp;
        jSnapshot["totalMemory"] = snapshot.totalMemory;
        jSnapshot["peakMemory"] = snapshot.peakMemory;
        jSnapshot["currentMemory"] = snapshot.currentMemory;
        jSnapshot["neuronMemory"] = snapshot.neuronMemory;
        jSnapshot["synapseMemory"] = snapshot.synapseMemory;
        jSnapshot["eventMemory"] = snapshot.eventMemory;
        jSnapshot["otherMemory"] = snapshot.otherMemory;
        jSnapshot["allocationCount"] = snapshot.allocationCount;
        jSnapshot["deallocationCount"] = snapshot.deallocationCount;
        j["snapshots"].push_back(jSnapshot);
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool MemoryProfiler::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        file.close();
        return false;
    }
    
    clearSnapshots();
    resetCounters();
    
    for (const auto& jSnapshot : j["snapshots"]) {
        MemorySnapshot snapshot;
        snapshot.timestamp = jSnapshot["timestamp"].get<double>();
        snapshot.totalMemory = jSnapshot["totalMemory"].get<size_t>();
        snapshot.peakMemory = jSnapshot["peakMemory"].get<size_t>();
        snapshot.currentMemory = jSnapshot["currentMemory"].get<size_t>();
        snapshot.neuronMemory = jSnapshot["neuronMemory"].get<size_t>();
        snapshot.synapseMemory = jSnapshot["synapseMemory"].get<size_t>();
        snapshot.eventMemory = jSnapshot["eventMemory"].get<size_t>();
        snapshot.otherMemory = jSnapshot["otherMemory"].get<size_t>();
        snapshot.allocationCount = jSnapshot["allocationCount"].get<size_t>();
        snapshot.deallocationCount = jSnapshot["deallocationCount"].get<size_t>();
        
        snapshots_.push_back(snapshot);
        
        // Update peak
        if (snapshot.currentMemory > peakMemoryMB_ * 1024 * 1024) {
            peakMemoryMB_ = snapshot.currentMemory / (1024 * 1024);
        }
    }
    
    file.close();
    return true;
}

MemoryProfiler::MemoryBreakdown MemoryProfiler::getBreakdown() const {
    MemoryBreakdown breakdown;
    
    for (const auto& snapshot : snapshots_) {
        breakdown.neuronsMB += snapshot.neuronMemory / (1024 * 1024);
        breakdown.synapsesMB += snapshot.synapseMemory / (1024 * 1024);
        breakdown.eventsMB += snapshot.eventMemory / (1024 * 1024);
        breakdown.otherMB += snapshot.otherMemory / (1024 * 1024);
    }
    
    return breakdown;
}

} // namespace nlm
