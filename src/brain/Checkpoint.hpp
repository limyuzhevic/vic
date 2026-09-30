// Checkpoint.hpp
// Checkpoint system interface

#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class CheckpointSystem {
public:
    bool configure(const std::string& directory, size_t maxCheckpoints, 
                   size_t keepCount, bool compression);
    bool save(const std::string& filepath, const Brain* brain);
    bool load(const std::string& filepath, Brain* brain);
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    bool setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    
private:
    void manageCheckpoints();
    
    std::shared_ptr<Brain> brain_;
    std::string directory_;
    size_t maxCheckpoints_;
    size_t keepCount_;
    size_t checkpointInterval_ = 1000;
    bool compressionEnabled_;
};

} // namespace nlm
