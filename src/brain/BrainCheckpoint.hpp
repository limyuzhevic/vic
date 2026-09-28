#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Checkpoint System Integration
// Handles brain state save/load functionality

class BrainCheckpoint {
public:
    // Save brain state to file
    bool saveBrainState(const Brain::Impl& pImpl, const std::string& filepath) const;
    
    // Load brain state from file
    bool loadBrainState(Brain::Impl& pImpl, const std::string& filepath);
    
    // Get checkpoint system statistics
    void logCheckpointStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for checkpoint integration
    void prepareBrainForSave(Brain::Impl& pImpl) const;
    void restoreBrainFromSave(Brain::Impl& pImpl);
    void cleanupAfterCheckpoint(Brain::Impl& pImpl);
};

} // namespace nlm
