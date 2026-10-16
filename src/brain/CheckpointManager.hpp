#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <chrono>
#include <mutex>
#include "DistributedBrain.hpp"
#include "Communicator.hpp"

namespace nlm {

// Checkpoint management modes
enum class CheckpointMode {
    Local,            // Save checkpoints locally on each node
    Distributed,      // Distribute checkpoints across nodes
    MasterSlave,      // Master node manages all checkpoints
    Mirrored          // Mirror checkpoints across all nodes
};

// Checkpoint state information
struct CheckpointInfo {
    int nodeId;
    std::string checkpointPath;
    uint64_t stepNumber;
    uint64_t timestamp;
    uint64_t memoryUsage;         // bytes
    float checkpointTime;         // seconds
    bool valid;                   // Whether checkpoint is valid
    std::string errorMessage;     // Error message if invalid
    
    CheckpointInfo() : nodeId(0), stepNumber(0), timestamp(0), memoryUsage(0),
                      checkpointTime(0.0f), valid(false) {}
};

// Checkpoint request/response
struct CheckpointRequest {
    int requestingNodeId;
    int targetNodeId;
    uint64_t stepNumber;
    std::string checkpointPath;
    uint64_t timestamp;
};

struct CheckpointResponse {
    int requestingNodeId;
    int respondingNodeId;
    bool accepted;
    CheckpointInfo checkpointInfo;
    std::string errorMessage;
    uint64_t timestamp;
};

// Checkpoint compression levels
enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    Best
};

// Checkpoint format versions
struct CheckpointMetadata {
    int version;                    // Checkpoint format version
    std::string softwareVersion;    // Software version that created checkpoint
    uint64_t creationTime;          // Unix timestamp when checkpoint was created
    uint64_t stepNumber;            // Simulation step at checkpoint
    uint64_t neuronCount;           // Number of neurons in checkpoint
    uint64_t synapseCount;          // Number of synapses in checkpoint
    float checkpointSize;           // Size in bytes
    std::string format;             // File format
    
    CheckpointMetadata() : version(1), creationTime(0), stepNumber(0),
                          neuronCount(0), synapseCount(0), checkpointSize(0.0f) {}
};

// Distributed checkpoint manager for multi-node neural simulations
class CheckpointManager {
public:
    // Create checkpoint manager with configuration
    explicit CheckpointManager(const DistributedConfig& config);
    
    ~CheckpointManager();
    
    // Disable copying, enable moving
    CheckpointManager(const CheckpointManager&) = delete;
    CheckpointManager& operator=(const CheckpointManager&) = delete;
    CheckpointManager(CheckpointManager&&) noexcept;
    CheckpointManager& operator=(CheckpointManager&&) noexcept;
    
    // Initialize checkpoint manager
    bool initialize();
    
    // Create checkpoint on local node
    bool createCheckpoint(int stepNumber, const std::string& customPath = "");
    
    // Load checkpoint to local node
    bool loadCheckpoint(int stepNumber, const std::string& customPath = "");
    
    // Distribute checkpoint to other nodes
    bool distributeCheckpoint(const CheckpointInfo& checkpoint);
    
    // Retrieve checkpoint from other nodes
    bool retrieveCheckpoint(int sourceNode, int stepNumber, std::string& checkpointPath);
    
    // Delete checkpoint from all nodes
    bool deleteCheckpoint(int stepNumber);
    
    // List available checkpoints
    std::vector<CheckpointInfo> listCheckpoints(int nodeId = -1) const;
    
    // Get latest checkpoint
    CheckpointInfo getLatestCheckpoint(int nodeId = -1) const;
    
    // Configure checkpoint manager
    void configure(int nodeId, const std::string& checkpointDir, 
                  CheckpointMode mode = CheckpointMode::Distributed,
                  CompressionLevel compression = CompressionLevel::Balanced,
                  uint64_t maxCheckpoints = 100,
                  uint64_t checkpointInterval = 100);
    
    // Set checkpoint compression level
    void setCompressionLevel(CompressionLevel level);
    
    // Set maximum number of checkpoints to keep
    void setMaxCheckpoints(uint64_t maxCheckpoints);
    
    // Set checkpoint interval
    void setCheckpointInterval(uint64_t interval);
    
    // Get checkpoint statistics
    float getAverageCheckpointTime() const;
    uint64_t getTotalCheckpointsCreated() const;
    uint64_t getTotalCheckpointsLoaded() const;
    uint64_t getTotalBytesTransferred() const;
    
    // Get checkpoint directory
    std::string getCheckpointDirectory() const;
    
    // Cleanup old checkpoints
    bool cleanupOldCheckpoints();
    
    // Validate checkpoint integrity
    bool validateCheckpoint(const std::string& checkpointPath) const;
    
    // Checkpoint synchronization across nodes
    bool synchronizeCheckpoints(const std::vector<int>& activeNodes);
    
    // Recovery from failed node
    bool recoverFromFailedNode(int failedNodeId, const std::vector<int>& backupNodes);
    
    // Pre-emptive checkpointing for rollback protection
    bool preemptiveCheckpoint(const std::string& reason);
    
    // Query checkpoint availability
    bool isCheckpointAvailable(int nodeId, int stepNumber) const;
    
    // Get checkpoint metadata
    CheckpointMetadata getCheckpointMetadata(const std::string& checkpointPath) const;
    
    // Save checkpoint metadata
    bool saveCheckpointMetadata(const CheckpointMetadata& metadata);
    
    // Load checkpoint metadata
    CheckpointMetadata loadCheckpointMetadata(const std::string& checkpointPath) const;
    
    // Monitor checkpoint system health
    bool monitorHealth(std::vector<std::string>& healthIssues) const;
    
    // Set monitoring callback
    void setCheckpointCallback(std::function<void(const CheckpointInfo&)> callback);
    
    // Save checkpoint manager state
    bool saveState(const std::string& filepath) const;
    
    // Load checkpoint manager state
    bool loadState(const std::string& filepath);

private:
    struct Impl;
    Impl* pImpl;
    
    // Internal helper methods
    bool createLocalCheckpoint(int stepNumber, const std::string& customPath = "");
    bool loadLocalCheckpoint(int stepNumber, const std::string& customPath = "");
    std::string getNodeCheckpointPath(int nodeId, int stepNumber) const;
    bool isPathAccessible(const std::string& path) const;
    uint64_t calculateCheckpointSize(const std::string& checkpointPath) const;
    bool compressCheckpoint(const std::string& sourcePath, const std::string& destPath,
                           CompressionLevel level) const;
    bool decompressCheckpoint(const std::string& sourcePath, const std::string& destPath,
                             CompressionLevel level) const;
    std::string generateChecksum(const std::string& filepath) const;
    bool verifyChecksum(const std::string& filepath, const std::string& checksum) const;
    bool waitForAllNodes(const std::vector<int>& nodes, std::function<bool()> check) const;
    bool broadcastCheckpoint(int rootNode, const CheckpointRequest& request);
    bool exchangeCheckpoint(int sourceNode, const CheckpointRequest& request, CheckpointResponse& response);
};

} // namespace nlm
