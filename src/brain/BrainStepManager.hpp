#pragma once

#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class BrainStepManager {
public:
    BrainStepManager(Brain* brain);
    ~BrainStepManager();
    
    // Execute the complete 15-step brain loop
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Execute individual step phases (for testing and debugging)
    void executeStepPhase1(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase2(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase3(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase4(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase5(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase6(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase7(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase8(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase9(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase10(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase11(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase12(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase13(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase14(SimulationStep currentStep, Timestamp currentTime);
    void executeStepPhase15(SimulationStep currentStep, Timestamp currentTime);
    
    // Get execution statistics
    size_t getSpikesThisStep() const { return totalSpikesThisStep_; }
    size_t getTotalSpikes() const { return totalSpikesTotal_; }
    
    // Configuration
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t totalSpikesThisStep_;
    size_t totalSpikesTotal_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
    Brain* brain_;
};

} // namespace nlm