// BrainUpdaterOrchestrator.hpp - Manages and executes brain system updaters
#pragma once

#include "BrainSystemUpdater.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class BrainUpdaterOrchestrator {
public:
    BrainUpdaterOrchestrator();
    ~BrainUpdaterOrchestrator();
    
    // Add a system updater
    void addUpdater(std::unique_ptr<BrainSystemUpdater> updater);
    
    // Execute all registered updaters
    void update(Brain& brain, SimulationStep currentStep,
                Timestamp currentTime, TimestepDuration timestep);
    
    // Get all updater names for logging/debugging
    std::vector<std::string> getUpdaterNames() const;
    
    // Clear all updaters
    void clearUpdaters();
    
    // Get number of updaters
    size_t getUpdaterCount() const { return updaters_.size(); }
    
    // Check if orchestrator is empty
    bool isEmpty() const { return updaters_.empty(); }
    
private:
    std::vector<std::unique_ptr<BrainSystemUpdater>> updaters_;
};

} // namespace nlm
