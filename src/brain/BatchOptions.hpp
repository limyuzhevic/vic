#pragma once

#include <string>
#include <vector>

namespace nlm {

// Batch statistics
struct BatchStatistics {
    size_t totalBatches = 0;                // Total number of batches processed
    size_t successfulBatches = 0;           // Number of successful batches
    size_t failedBatches = 0;               // Number of failed batches
    float averageReward = 0.0f;             // Average reward across all batches
    float bestReward = -std::numeric_limits<float>::max();  // Best reward achieved
    float worstReward = std::numeric_limits<float>::max();  // Worst reward achieved
    size_t totalSteps = 0;                  // Total steps processed
    double totalTime = 0.0;                 // Total processing time in seconds
    
    BatchStatistics() : bestReward(-std::numeric_limits<float>::max()),
                       worstReward(std::numeric_limits<float>::max()),
                       totalSteps(0), totalTime(0.0) {}
};

// Checkpoint information
struct CheckpointInfo {
    std::string name;                       // Checkpoint name/identifier
    std::string path;                       // File path
    size_t step = 0;                        // Simulation step at checkpoint
    double simulationTime = 0.0;            // Simulation time at checkpoint
    size_t neuronCount = 0;                 // Number of neurons saved
    size_t synapseCount = 0;                // Number of synapses saved
    size_t regionCount = 0;                 // Number of regions saved
    uint64_t fileSize = 0;                  // File size in bytes
    time_t timestamp = 0;                   // Creation timestamp
    bool compressed = false;                // Whether checkpoint is compressed
    std::string checksum;                   // Checksum for integrity verification
    
    CheckpointInfo() : step(0), simulationTime(0.0), neuronCount(0),
                      synapseCount(0), regionCount(0), fileSize(0), timestamp(0),
                      compressed(false) {}
    
    CheckpointInfo(const std::string& n, const std::string& p, size_t s, double st,
                   size_t nc, size_t sc, size_t rc, uint64_t fs)
        : name(n), path(p), step(s), simulationTime(st), neuronCount(nc),
          synapseCount(sc), regionCount(rc), fileSize(fs), timestamp(std::time(nullptr)),
          compressed(false) {}
};

// Memory statistics
struct MemoryStats {
    size_t totalMemoryMB = 0;               // Total memory usage in MB
    size_t availableMemoryMB = 0;           // Available memory in MB
    size_t usedMemoryMB = 0;                // Used memory in MB
    float memoryUtilization = 0.0f;        // Memory utilization (0.0 - 1.0)
    size_t neuronMemoryMB = 0;              // Memory used by neurons in MB
    size_t synapseMemoryMB = 0;             // Memory used by synapses in MB
    size_t memorySystemMemoryMB = 0;       // Memory used by memory systems in MB
    
    MemoryStats() : totalMemoryMB(0), availableMemoryMB(0), usedMemoryMB(0),
                   memoryUtilization(0.0f), neuronMemoryMB(0), synapseMemoryMB(0),
                   memorySystemMemoryMB(0) {}
};

} // namespace nlm