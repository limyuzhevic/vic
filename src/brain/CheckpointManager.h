// CheckpointManager.h - Manages brain checkpointing and saving
#pragma once

#include <string>
#include <memory>
#include <vector>

namespace nlm {

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Configure checkpoint manager
    bool configure(const std::string& directory, size_t maxCheckpoints,
                   size_t maxFileSize, bool compress);
    
    // Update checkpoint manager
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Save current brain state
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // List available checkpoints
    std::vector<std::string> listCheckpoints() const;
    
    // Delete old checkpoints to make room
    void cleanupOldCheckpoints();
    
    // Get checkpoint statistics
    size_t getCheckpointCount() const;
    size_t getMaxCheckpoints() const;
    
    // Get checkpoint directory
    const std::string& getDirectory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
