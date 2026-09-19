#pragma once

#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainCheckpoint {
public:
    // Constructor
    BrainCheckpoint(std::shared_ptr<Config> config);
    ~BrainCheckpoint();
    
    // Configure checkpoint system
    void configure(const std::string& directory, size_t maxCheckpoints, 
                  size_t maxFileSize, bool compressionEnabled);
    
    // Save brain state
    bool save(const std::string& filepath) const;
    
    // Load brain state
    bool load(const std::string& filepath);
    
    // Checkpoint management
    void update(SimulationStep currentStep, Timestamp currentTime);
    void cleanup();
    
    // Get checkpoint manager accessor
    CheckpointManager* getCheckpointManager() { return checkpointManager.get(); }
    const CheckpointManager* getCheckpointManager() const { return checkpointManager.get(); }
    
    // Get checkpoint statistics
    size_t getMaxCheckpoints() const { return maxCheckpoints; }
    size_t getMaxFileSize() const { return maxFileSize; }
    bool isCompressionEnabled() const { return compressionEnabled; }
    
    // Reset checkpoint state
    void reset();
    
    // Log checkpoint system status
    void logStatus() const;
    
private:
    // Checkpoint manager component
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Configuration
    std::shared_ptr<Config> config;
    
    // Checkpoint configuration
    std::string checkpointDirectory;
    size_t maxCheckpoints;
    size_t maxFileSize;
    bool compressionEnabled;
    
    // Checkpoint metadata
    size_t checkpointCount;
    SimulationStep lastCheckpointStep;
    
    // Helper methods
    void initializeFromConfig();
    bool saveNeurons(const std::string& filepath) const;
    bool saveSynapses(const std::string& filepath) const;
    bool loadNeurons(const std::string& filepath);
    bool loadSynapses(const std::string& filepath);
};

} // namespace nlm