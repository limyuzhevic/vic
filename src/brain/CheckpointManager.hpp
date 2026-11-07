#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class CheckpointManager {
public:
    CheckpointManager(Brain* brain);
    ~CheckpointManager();
    
    bool initialize();
    void step(SimulationStep currentStep, Timestamp currentTime);
    void reset();
    
    // Checkpoint operations
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Checkpoint manager access
    CheckpointManager* getCheckpointManager() { return checkpointManager.get(); }
    
private:
    Brain* brain_;
    std::unique_ptr<CheckpointManager> checkpointManager;
};

} // namespace nlm
