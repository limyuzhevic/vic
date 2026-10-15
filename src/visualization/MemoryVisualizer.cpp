#include "MemoryVisualizer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace nlm {

struct MemoryVisualizer::Impl {
    // Memory data
    std::vector<MemoryBlock> activeMemoryBlocks;
    std::deque<MemoryBlock> memoryHistory;
    
    // Statistics
    MemoryStats stats;
    
    // Visualization options
    bool showAllocationHistory = true;
    bool showUtilizationLevels = true;
    bool showFragmentation = true;
    bool highlightHighUsage = true;
    float highUsageThreshold = 0.8f;
    
    // Performance tracking
    double lastUpdateTime = 0.0;
    size_t allocationCount = 0;
    size_t deallocationCount = 0;
    
    Impl() {
        stats.peakUsage = 0;
    }
};

MemoryVisualizer::MemoryVisualizer() : pImpl(new Impl) {}

MemoryVisualizer::~MemoryVisualizer() = default;

bool MemoryVisualizer::initialize() {
    return true;
}

void MemoryVisualizer::update(double deltaTime) {
    pImpl->lastUpdateTime += deltaTime;
    
    // Update statistics
    pImpl->stats.totalActive = pImpl->activeMemoryBlocks.size();
    pImpl->stats.totalAllocated = pImpl->memoryHistory.size();
    
    // Calculate total memory usage
    size_t totalMemory = 0;
    for (const auto& block : pImpl->activeMemoryBlocks) {
        totalMemory += block.size;
    }
    
    // Calculate utilization
    if (totalMemory > 0) {
        pImpl->stats.memoryUsagePercentage = static_cast<float>(totalMemory) / 10000.0f * 100.0f; // Assuming 10KB total
    }
    
    // Calculate fragmentation
    if (pImpl->activeMemoryBlocks.size() > 1) {
        std::vector<size_t> sizes;
        for (const auto& block : pImpl->activeMemoryBlocks) {
            sizes.push_back(block.size);
        }
        
        float avgSize = std::accumulate(sizes.begin(), sizes.end(), 0.0f) / sizes.size();
        float variance = 0.0f;
        for (float size : sizes) {
            variance += (size - avgSize) * (size - avgSize);
        }
        variance /= sizes.size();
        pImpl->stats.fragmentation = std::sqrt(variance) / avgSize;
    }
    
    // Update peak usage
    if (pImpl->stats.memoryUsagePercentage > pImpl->stats.peakUsage) {
        pImpl->stats.peakUsage = pImpl->stats.memoryUsagePercentage;
    }
}

void MemoryVisualizer::render() {
    std::cout << "=== Memory Visualization ===" << std::endl;
    
    MemoryStats currentStats = getMemoryStats();
    
    std::cout << "Memory Statistics:" << std::endl;
    std::cout << "  Total Allocated: " << currentStats.totalAllocated << std::endl;
    std::cout << "  Currently Active: " << currentStats.totalActive << std::endl;
    std::cout << "  Peak Usage: " << std::fixed << std::setprecision(1) << currentStats.peakUsage << "%" << std::endl;
    std::cout << "  Current Usage: " << std::fixed << std::setprecision(1) << currentStats.memoryUsagePercentage << "%" << std::endl;
    std::cout << "  Fragmentation: " << std::fixed << std::setprecision(3) << currentStats.fragmentation << std::endl;
    std::cout << "  Allocations: " << pImpl->allocationCount << std::endl;
    std::cout << "  Deallocations: " << pImpl->deallocationCount << std::endl;
    
    // Generate memory map visualization
    std::string memoryMap = generateMemoryMap();
    std::cout << std::endl << "Memory Map:" << std::endl;
    std::cout << memoryMap;
    
    // Generate utilization heatmap
    std::string heatmap = generateUtilizationHeatmap();
    std::cout << std::endl << "Utilization Heatmap:" << std::endl;
    std::cout << heatmap;
    
    // Show performance metrics
    std::string metrics = getMemoryPerformanceMetrics();
    std::cout << std::endl << metrics << std::endl;
}

void MemoryVisualizer::close() {
    pImpl->activeMemoryBlocks.clear();
    pImpl->memoryHistory.clear();
}

void MemoryVisualizer::addMemoryBlock(size_t id, const std::string& type, size_t size) {
    MemoryBlock block(id, type, size);
    block.allocatedStep = static_cast<SimulationStep>(pImpl->memoryHistory.size());
    
    pImpl->activeMemoryBlocks.push_back(block);
    pImpl->memoryHistory.push_back(block);
    pImpl->allocationCount++;
    
    // Update statistics
    pImpl->stats.totalAllocated++;
    if (size > pImpl->stats.peakUsage) {
        pImpl->stats.peakUsage = static_cast<float>(size);
    }
}

void MemoryVisualizer::updateMemoryBlock(size_t id, float utilization) {
    for (auto& block : pImpl->activeMemoryBlocks) {
        if (block.id == id) {
            block.utilization = utilization;
            break;
        }
    }
}

void MemoryVisualizer::deallocateMemoryBlock(size_t id) {
    auto it = std::remove_if(pImpl->activeMemoryBlocks.begin(), pImpl->activeMemoryBlocks.end(),
                             [id](const MemoryBlock& block) {
                                 if (block.id == id) {
                                     return true;
                                 }
                                 return false;
                             });
    
    pImpl->activeMemoryBlocks.erase(it, pImpl->activeMemoryBlocks.end());
    pImpl->deallocationCount++;
}

MemoryStats MemoryVisualizer::getMemoryStats() const {
    MemoryStats stats = pImpl->stats;
    
    // Calculate current active blocks
    stats.totalActive = pImpl->activeMemoryBlocks.size();
    
    // Calculate current utilization
    size_t totalMemory = 0;
    for (const auto& block : pImpl->activeMemoryBlocks) {
        totalMemory += block.size;
    }
    
    stats.memoryUsagePercentage = (totalMemory > 0) ? static_cast<float>(totalMemory) / 10000.0f * 100.0f : 0.0f;
    
    return stats;
}

const std::vector<MemoryBlock>& MemoryVisualizer::getActiveMemoryBlocks() const {
    return pImpl->activeMemoryBlocks;
}

const std::deque<MemoryBlock>& MemoryVisualizer::getMemoryHistory() const {
    return pImpl->memoryHistory;
}

std::string MemoryVisualizer::generateMemoryMap() const {
    std::stringstream ss;
    
    if (pImpl->activeMemoryBlocks.empty()) {
        return "  No active memory blocks";
    }
    
    // Sort by utilization (high to low)
    std::vector<MemoryBlock> sortedBlocks = pImpl->activeMemoryBlocks;
    std::sort(sortedBlocks.begin(), sortedBlocks.end(),
              [](const MemoryBlock& a, const MemoryBlock& b) {
                  return a.utilization > b.utilization;
              });
    
    ss << "  Memory Blocks (sorted by utilization):" << std::endl;
    
    for (const auto& block : sortedBlocks) {
        std::string status = block.isActive ? "ACTIVE" : "INACTIVE";
        std::string highUsageMarker = (pImpl->highlightUsage && block.utilization > pImpl->highUsageThreshold) ? " [HIGH]" : "";
        
        ss << "    Block " << block.id << ": " << block.type 
           << " size=" << block.size << "B "
           << "util=" << std::fixed << std::setprecision(2) << block.utilization * 100.0f << "% "
           << status << highUsageMarker << std::endl;
    }
    
    return ss.str();
}

std::string MemoryVisualizer::generateUtilizationHeatmap() const {
    std::stringstream ss;
    
    if (pImpl->activeMemoryBlocks.empty()) {
        return "  No memory blocks to display";
    }
    
    ss << "  Utilization Levels:" << std::endl;
    
    for (const auto& block : pImpl->activeMemoryBlocks) {
        std::string level;
        if (block.utilization < 0.2f) {
            level = "LOW (0-20%)";
        } else if (block.utilization < 0.5f) {
            level = "MEDIUM (20-50%)";
        } else if (block.utilization < 0.8f) {
            level = "HIGH (50-80%)";
        } else {
            level = "CRITICAL (80%+)";
        }
        
        std::string bar = "";
        for (int i = 0; i < 10; ++i) {
            float threshold = static_cast<float>(i) / 10.0f;
            if (block.utilization > threshold) {
                bar += "#";
            } else {
                bar += "-";
            }
        }
        
        ss << "    " << level << ": [" << bar << " " 
           << std::fixed << std::setprecision(1) << block.utilization * 100.0f << "%]" << std::endl;
    }
    
    return ss.str();
}

std::string MemoryVisualizer::getMemoryPerformanceMetrics() const {
    std::stringstream ss;
    
    ss << "Memory Performance Metrics:" << std::endl;
    ss << "  Allocation Rate: " << (pImpl->allocationCount / std::max(pImpl->lastUpdateTime, 1.0)) << " alloc/s" << std::endl;
    ss << "  Deallocation Rate: " << (pImpl->deallocationCount / std::max(pImpl->lastUpdateTime, 1.0)) << " dealloc/s" << std::endl;
    ss << "  Average Block Size: ";
    
    if (!pImpl->activeMemoryBlocks.empty()) {
        size_t totalSize = 0;
        for (const auto& block : pImpl->activeMemoryBlocks) {
            totalSize += block.size;
        }
        float avgSize = static_cast<float>(totalSize) / pImpl->activeMemoryBlocks.size();
        ss << std::fixed << std::setprecision(1) << avgSize << "B" << std::endl;
    } else {
        ss << "0 B" << std::endl;
    }
    
    ss << "  Memory Efficiency: ";
    if (pImpl->stats.peakUsage > 0) {
        float efficiency = (pImpl->activeMemoryBlocks.size() * 100.0f) / pImpl->stats.peakUsage;
        ss << std::fixed << std::setprecision(1) << efficiency << "%" << std::endl;
    } else {
        ss << "N/A" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
