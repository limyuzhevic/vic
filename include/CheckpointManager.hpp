#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <memory>

namespace nlm {

// ==========================================
// CHECKPOINT MANAGER - Checkpoint Management
// ==========================================
// 
// Handles: save/load functionality
// 
// Manages: checkpoint creation and restoration

class CheckpointManager {
public:
    CheckpointManager(std::shared_ptr<Config> config);
    ~CheckpointManager();
    
    // Disable copying, enable moving
    CheckpointManager(const CheckpointManager&) = delete;
    CheckpointManager& operator=(const CheckpointManager&) = delete;
    CheckpointManager(CheckpointManager&&) noexcept;
    CheckpointManager& operator=(CheckpointManager&&) noexcept;
    
    // Configure checkpoint manager
    bool configure(const std::string& checkpointDir, size_t maxCheckpoints,
                   size_t maxCheckpointSize, bool enableCompression);
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update checkpoint manager
    void update(SimulationStep step, Timestamp time);
    
    // Checkpoint operations
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Checkpoint management
    bool createCheckpoint(const std::string& name);
    bool restoreCheckpoint(const std::string& name);
    
    // Get checkpoint statistics
    size_t getCheckpointCount() const;
    size_t getMaxCheckpointCount() const;
    uint64_t getTotalBytesSaved() const;
    
    // Clean up old checkpoints
    void cleanupOldCheckpoints();
    
    // Reset checkpoint manager
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
};

} // namespace nlm