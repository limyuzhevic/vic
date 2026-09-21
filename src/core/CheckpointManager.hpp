#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace nlm {

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Create a checkpoint
    bool createCheckpoint(const std::string& filename, const Brain& brain);
    
    // Load a checkpoint
    bool loadCheckpoint(const std::string& filename, Brain& brain);
    
    // Update manager
    void update(SimulationStep step, Timestamp time);
    
    // Set brain reference
    bool setBrain(Brain& brain);
    
    // Get checkpoint interval
    size_t getCheckpointInterval() const { return checkpointInterval_; }
    void setCheckpointInterval(size_t interval) { checkpointInterval_ = interval; }
    
    // Get last update time
    Timestamp getLastUpdateTime() const { return lastUpdateTime_; }
    
    // Create a simple checkpoint in text format
    static bool createSimpleCheckpoint(const std::string& filename, const Brain& brain);
    
    // Load from simple text format
    static bool loadSimpleCheckpoint(const std::string& filename, Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    Timestamp lastUpdateTime_;
    size_t checkpointInterval_;
};

} // namespace nlm