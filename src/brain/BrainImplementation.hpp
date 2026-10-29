#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainMemorySystem;
class BrainCognitionSystem;
class BrainNeuromodulationSystem;
class BrainDevelopmentSystem;
class BrainPersistence;

class BrainImplementation {
public:
    // Initialize brain with configuration
    static bool initialize(Brain* brain);
    
    // Main simulation step implementation
    static void step(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Initialize step systems
    static void initializeStepSystems(Brain* brain);
    
    // Reset brain state
    static void reset(Brain* brain);
};

} // namespace nlm
