// BrainCheckpoint.hpp - Checkpoint system integration interface
#pragma once

#include <string>

namespace nlm {

class Brain;

class BrainCheckpoint {
public:
    BrainCheckpoint();
    ~BrainCheckpoint();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Save brain state to file
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Get checkpoint statistics
    std::string getCheckpointInfo() const;
    
private:
    Brain* brain_;
};

} // namespace nlm