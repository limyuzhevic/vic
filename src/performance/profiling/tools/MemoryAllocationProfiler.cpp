#include "MemoryAllocationProfiler.hpp"
#include <unordered_map>
#include <algorithm>

namespace nlm {
namespace profiling {

// MemoryAllocationProfiler implementation

void MemoryAllocationProfiler::startProfiling() {
    if (profiling_) return;
    profiling_ = true;
    pImpl_->startTime = std::chrono::steady_clock::now();
}

void MemoryAllocationProfiler::stopProfiling() {
    if (!profiling_) return;
    profiling_ = false;
}

bool MemoryAllocationProfiler::isProfiling() const {
    return profiling_;
}

void MemoryAllocationProfiler::trackAllocation(void* address, size_t size, const std::string& site,
                                                const std::string& function, int line) {
    if (!profiling_) return;
    
    MemoryAllocation allocation;
    allocation.address = address;
    allocation.size = size;
    allocation.allocationSite = site;
    allocation.allocationFunction = function;
    allocation.allocationLine = line;
    allocation.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl_->startTime).count();
    allocation.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    allocation.allocationType = "heap";
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    allocations_[address] = allocation;
    currentMemoryUsage_ += size;
}

void MemoryAllocationProfiler::trackDeallocation(void* address, const std::string& site,
                                                 const std::string& function, int line) {
    if (!profiling_) return;
    
    MemoryDeallocation deallocation;
    deallocation.address = address;
    deallocation.deallocationSite = site;
    deallocation.deallocationFunction = function;
    deallocation.deallocationLine = line;
    deallocation.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl_->startTime).count();
    deallocation.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    deallocations_.push_back(deallocation);
    
    auto it = allocations_.find(address);
    if (it != allocations_.end()) {
        currentMemoryUsage_ -= it->second.size;
        allocations_.erase(it);
    }
}

size_t MemoryAllocationProfiler::getCurrentMemoryUsage() const {
    return currentMemoryUsage_;
}

size_t MemoryAllocationProfiler::getTotalMemoryAllocated() const {
    size_t total = 0;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    for (const auto& allocation : allocations_) {
        total += allocation.second.size;
    }
    return total;
}

size_t MemoryAllocationProfiler::getMemoryLeakSize() const {
    size_t leakSize = 0;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& allocation : allocations_) {
        // Check if allocation was deallocated (simplified - in reality would need better tracking)
        bool wasDeallocated = false;
        for (const auto& deallocation : deallocations_) {
            if (deallocation.address == allocation.first.address) {
                wasDeallocated = true;
                break;
            }
        }
        if (!wasDeallocated) {
            leakSize += allocation.second.size;
        }
    }
    
    return leakSize;
}

std::vector<MemoryAllocationProfiler::MemoryAllocation> MemoryAllocationProfiler::findLeaks() const {
    std::vector<MemoryAllocation> leaks;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& allocation : allocations_) {
        bool wasDeallocated = false;
        for (const auto& deallocation : deallocations_) {
            if (deallocation.address == allocation.first.address) {
                wasDeallocated = true;
                break;
            }
        }
        if (!wasDeallocated) {
            leaks.push_back(allocation.second);
        }
    }
    
    return leaks;
}

std::vector<std::string> MemoryAllocationProfiler::getLargestAllocations(size_t limit) const {
    std::vector<std::pair<MemoryAllocation, size_t>> allocationSizes;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& pair : allocations_) {
        allocationSizes.push_back({pair.second, pair.second.size});
    }
    
    std::sort(allocationSizes.begin(), allocationSizes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    result.reserve(std::min(limit, allocationSizes.size()));
    for (size_t i = 0; i < std::min(limit, allocationSizes.size()); ++i) {
        std::stringstream ss;
        ss << allocationSizes[i].first.allocationFunction << ":" 
           << allocationSizes[i].first.allocationLine << " (" 
           << allocationSizes[i].second << " bytes)";
        result.push_back(ss.str());
    }
    
    return result;
}

double MemoryAllocationProfiler::calculateAllocationFrequency(const std::string& function) const {
    size_t count = 0;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& allocation : allocations_) {
        if (allocation.second.allocationFunction.find(function) != std::string::npos) {
            count++;
        }
    }
    
    return static_cast<double>(count) / std::max(1.0, static_cast<double>(allocations_.size()));
}

bool MemoryAllocationProfiler::exportAllocationReport(const std::string& filepath) const {
    // TODO: Implement allocation report export
    return false;
}

bool MemoryAllocationProfiler::exportLeakReport(const std::string& filepath) const {
    // TODO: Implement leak report export
    return false;
}

void MemoryAllocationProfiler::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

} // namespace profiling
} // namespace nlm
