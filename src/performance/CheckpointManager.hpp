#pragma once

#include "../core/Types/Types.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <chrono>

namespace nlm {

// Checkpoint Manager
// Manages automatic and manual checkpointing for brain state preservation
// Handles checkpoint creation, loading, validation, and cleanup

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Configure checkpoint parameters
    void configure(const std::string& checkpointDir, 
                   uint64_t saveIntervalSteps = 10000,
                   size_t maxCheckpoints = 10,
                   bool compress = true);
    
    // Update checkpoint manager (call each simulation step)
    bool update(uint64_t currentStep, double currentTime);
    
    // Save checkpoint immediately
    bool saveImmediately(const std::string& name = "");
    
    // Load checkpoint by name
    bool load(const std::string& name);
    
    // List available checkpoints
    struct CheckpointInfo {
        std::string name;
        uint64_t step;
        double simulationTime;
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t fileSize;
        time_t timestamp;
    };
    
    std::vector<CheckpointInfo> listCheckpoints() const;
    
    // Delete old checkpoints (keep most recent N)
    void pruneOldCheckpoints(size_t keepCount);
    
    // Get checkpoint directory
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    // Get last checkpoint path
    std::string getLastCheckpointPath() const;
    
    // Check if checkpoint should be saved based on interval
    bool shouldSave(uint64_t currentStep) const;
    
    // Get checkpoint statistics
    uint64_t getCurrentStep() const { return currentStep_; }
    double getCurrentSimulationTime() const { return currentSimulationTime_; }
    uint64_t getTotalCheckpointsCreated() const { return totalCheckpointsCreated_; }
    uint64_t getTotalBytesWritten() const { return totalBytesWritten_; }
    
    // Reset checkpoint manager state
    void reset();
    
    // Set checkpoint data providers (for Brain to register)
    void setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
        neuronProvider_ = provider;
    }
    
    void setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
        synapseProvider_ = provider;
    }
    
    // Get current checkpoint status
    bool isCheckpointInProgress() const { return checkpointInProgress_; }
    double getCheckpointProgress() const { return checkpointProgress_; }
    
    // Force checkpoint on next update
    void requestImmediateCheckpoint(const std::string& reason = "manual");
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
