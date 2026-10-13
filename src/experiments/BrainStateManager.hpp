#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "BrainState.hpp"

namespace nlm {

// Brain state comparison result
struct BrainStateDifference {
    double totalDifference;           // Overall difference metric (0-1)
    double neuronPotentialDifference; // Membrane potential difference
    double weightDifference;          // Synaptic weight difference
    double spikeDifference;           // Spike count difference
    std::vector<std::pair<size_t, double>> differingNeuronIds; // List of neurons with significant differences
    std::vector<std::pair<size_t, double>> differingSynapseIds; // List of synapses with significant differences
    std::string summary;
};

// Brain state backup metadata
struct BrainStateBackupInfo {
    std::string filename;
    Timestamp timestamp;               // When backup was created
    SimulationStep step;              // Simulation step at time of backup
    uint64_t checksum;                 // Data integrity checksum
    size_t memoryUsage;                // Memory usage in bytes
    bool compressed;                   // Whether backup was compressed
    std::string compressionAlgorithm;  // Compression algorithm used
};

// BrainStateManager class for advanced brain state operations
class BrainStateManager {
public:
    BrainStateManager();
    ~BrainStateManager();
    
    // Configure checkpointing
    void configureCheckpointing(bool enabled, size_t interval, const std::string& checkpointDir);
    
    // Checkpoint management
    bool createCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& label = "");
    std::shared_ptr<Brain> restoreCheckpoint(const std::string& label, const Config& config);
    std::shared_ptr<Brain> restoreLatestCheckpoint(const Config& config);
    
    // Incremental checkpointing
    bool createIncrementalCheckpoint(const std::shared_ptr<Brain>& brain, SimulationStep currentStep);
    bool restoreIncrementalCheckpoint(SimulationStep targetStep, const Config& config);
    
    // Brain state comparison and analysis
    BrainStateDifference compareBrainStates(const std::shared_ptr<Brain>& brain1, 
                                          const std::shared_ptr<Brain>& brain2) const;
    double calculateStateDifference(const std::shared_ptr<Brain>& brain1,
                                   const std::shared_ptr<Brain>& brain2) const;
    
    // Brain state backup and restore
    bool backupBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain);
    bool backupBrainStateCompressed(const std::string& filepath, const std::shared_ptr<Brain>& brain, 
                                   const std::string& algorithm = "gzip");
    std::shared_ptr<Brain> restoreBrainState(const std::string& filepath, const Config& config);
    
    // Brain state validation
    bool validateBrainState(const std::shared_ptr<Brain>& brain) const;
    std::vector<std::string> validateBrainStateDetailed(const std::shared_ptr<Brain>& brain) const;
    
    // Brain state export/import
    bool exportBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain);
    bool importBrainState(const std::string& filepath, std::shared_ptr<Brain>& brain, const Config& config);
    
    // Checkpoint listing and management
    std::vector<std::string> listCheckpoints(const std::string& directory = "") const;
    std::vector<BrainStateBackupInfo> getCheckpointInfo(const std::string& directory = "") const;
    bool deleteCheckpoint(const std::string& label);
    bool deleteAllCheckpoints(const std::string& directory = "");
    
    // State transformation
    BrainState brainToState(const std::shared_ptr<Brain>& brain) const;
    std::shared_ptr<Brain> stateToBrain(const BrainState& state, const Config& config) const;
    
    // State analysis
    double calculateStateEntropy(const std::shared_ptr<Brain>& brain) const;
    std::vector<size_t> getMostActiveNeurons(const std::shared_ptr<Brain>& brain, size_t count = 10) const;
    std::vector<size_t> getLeastActiveNeurons(const std::shared_ptr<Brain>& brain, size_t count = 10) const;
    
    // State compression
    std::vector<uint8_t> compressBrainState(const std::shared_ptr<Brain>& brain) const;
    std::shared_ptr<Brain> decompressBrainState(const std::vector<uint8_t>& data, const Config& config) const;
    
    // Get configuration
    bool isCheckpointingEnabled() const;
    size_t getCheckpointInterval() const;
    std::string getCheckpointDirectory() const;
    
    // Statistics and monitoring
    size_t getTotalCheckpointsCreated() const;
    size_t getTotalCheckpointsRestored() const;
    double getAverageCheckpointSize() const;
    Timestamp getLastCheckpointTime() const;
    
    // Cleanup
    void cleanupOldCheckpoints(size_t keepCount = 10);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string generateCheckpointFilename(const std::string& label, SimulationStep step) const;
    uint64_t calculateChecksum(const std::vector<uint8_t>& data) const;
    bool ensureDirectoryExists(const std::string& path) const;
    BrainStateBackupInfo createBackupInfo(const std::string& filepath, const std::shared_ptr<Brain>& brain, bool compressed = false) const;
};

} // namespace nlm
