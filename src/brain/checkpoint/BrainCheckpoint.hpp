#pragma once

#include "BrainCheckpoint.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {
    class Brain;
    
    // Checkpoint system for brain state persistence
    class Checkpoint {
    public:
        Checkpoint();
        ~Checkpoint();
        
        // Initialize checkpoint manager
        void initializeCheckpoint(Brain* brain);
        
        // Configure checkpoint settings
        void configure(const std::string& directory, size_t maxCheckpoints,
                       size_t maxHistory, bool autoSave);
        
        // Update checkpoint state
        void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
        
        // Save brain state to checkpoint
        bool save(Brain* brain, const std::string& filepath) const;
        
        // Load brain state from checkpoint
        bool load(Brain* brain, const std::string& filepath);
        
        // Create new checkpoint
        bool createCheckpoint(Brain* brain);
        
        // Delete old checkpoints
        void cleanupOldCheckpoints(size_t keepCount);
        
        // Get checkpoint count
        size_t getCheckpointCount() const;
        
        // Get latest checkpoint time
        Timestamp getLatestCheckpointTime() const;
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
