#pragma once

#include "../brain/Brain.hpp"
#include "../performance/CheckpointManager.hpp"

namespace nlm {

// BrainCheckpointManager manages checkpoint system
class BrainCheckpointManager {
public:
    BrainCheckpointManager() = default;
    ~BrainCheckpointManager() = default;
    
    // Initialize checkpoint manager
    void initialize(Brain* brain);
    
    // Update checkpoint system
    void update(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Save brain state
    bool save(Brain* brain, const std::string& filepath) const;
    
    // Load brain state
    bool load(Brain* brain, const std::string& filepath);
    
    // Get checkpoint statistics
    void collectCheckpointStats(Brain* brain) const;
};

} // namespace nlm
