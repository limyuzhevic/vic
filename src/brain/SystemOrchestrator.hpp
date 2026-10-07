//=============================================================
// SYSTEM ORCHESTRATOR - Brain.cpp Refactoring (Phase 7)
// Single Responsibility: System Coordination
//=============================================================

#pragma once

#include "Brain.hpp"
#include "BrainManager.hpp"

namespace nlm {

class SystemOrchestrator {
public:
    SystemOrchestrator();
    ~SystemOrchestrator();
    
    // Initialize all systems
    void initialize(BrainCore* coreBrain, BrainManager* brainManager, std::shared_ptr<Config> config);
    
    // Execute complete integration step
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Process sensory input
    void processSensoryInput(const class SensoryInput& input);
    
    // Update all systems
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Reset all systems
    void reset();
    
    // Logging and status
    void logStatus() const;
    
private:
    // Core components
    BrainCore* m_coreBrain;
    BrainManager* m_brainManager;
    std::shared_ptr<Config> m_config;
    
    // State tracking
    SimulationStep currentStep;
    Timestamp currentTime;
};

} // namespace nlm
