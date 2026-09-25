#pragma once

#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "AgentBrain.cpp"
#include "BrainStepRefactor.cpp"

// Include the constants header
#include "Constants.hpp"

// This file includes all the refactored components
// and provides backward compatibility with the original Brain class

namespace nlm {

// Forward declarations for the helper classes used in the refactored implementation
class BrainStepProcessor;
class MemorySystemManager;
class NeuromodulationSystem;
class DevelopmentSystem;
class PredictionSystem;
class CognitionSystem;
class PlasticitySystem;
class CheckpointSystem;

// Legacy wrapper to maintain backward compatibility
// The Brain class now uses the refactored implementation internally
class LegacyBrain {
public:
    LegacyBrain(std::shared_ptr<Config> config) : brain_(config) {}
    
    // Delegated methods for backward compatibility
    void step(SimulationStep currentStep) {
        brain_.step(currentStep);
    }
    
    void step(SimulationStep currentStep, Timestamp currentTime) {
        brain_.step(currentStep, currentTime);
    }
    
    // All other methods are delegated similarly...
    
    // Main implementation
    Brain brain_;
};

// Factory function to create a legacy-compatible brain
inline std::unique_ptr<LegacyBrain> CreateLegacyBrain(std::shared_ptr<Config> config) {
    return std::make_unique<LegacyBrain>(config);
}

} // namespace nlm