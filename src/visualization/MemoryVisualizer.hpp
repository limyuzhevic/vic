#pragma once

#include "VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

// Memory block visualization
struct MemoryBlock {
    size_t id;
    std::string type;
    size_t size;
    float utilization;
    SimulationStep allocatedStep;
    SimulationStep deallocatedStep;
    bool isActive;
    
    MemoryBlock() : id(0), size(0), utilization(0.0f), allocatedStep(0), deallocatedStep(-1), isActive(true) {}
    MemoryBlock(size_t i, const std::string& t, size_t s) 
        : id(i), type(t), size(s), utilization(0.0f), allocatedStep(0), deallocatedStep(-1), isActive(true) {}
};

// Memory system statistics
struct MemoryStats {
    size_t totalAllocated;
    size_t totalActive;
    size_t totalDeallocated;
    float memoryUsagePercentage;
    size_t peakUsage;
    float fragmentation;
    
    MemoryStats() : totalAllocated(0), totalActive(0), totalDeallocated(0), 
                   memoryUsagePercentage(0.0f), peakUsage(0), fragmentation(0.0f) {}
};

// Memory visualization
class MemoryVisualizer : public VisualizationInterface {
public:
    MemoryVisualizer();
    ~MemoryVisualizer() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Add memory block
    void addMemoryBlock(size_t id, const std::string& type, size_t size);
    
    // Update memory block
    void updateMemoryBlock(size_t id, float utilization);
    
    // Deallocate memory block
    void deallocateMemoryBlock(size_t id);
    
    // Get memory statistics
    MemoryStats getMemoryStats() const;
    
    // Get active memory blocks
    const std::vector<MemoryBlock>& getActiveMemoryBlocks() const;
    
    // Get memory allocation history
    const std::deque<MemoryBlock>& getMemoryHistory() const;
    
    // Generate memory allocation visualization
    std::string generateMemoryMap() const;
    
    // Generate memory utilization heatmap
    std::string generateUtilizationHeatmap() const;
    
    // Generate memory performance metrics
    std::string getMemoryPerformanceMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
