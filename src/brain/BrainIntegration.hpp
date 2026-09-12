// Brain integration component - handles the main coordination loop and system integration
#include "Brain.hpp"
#include "MemorySystem.hpp"
#include "PredictionSystem.hpp"
#include "CognitionSystem.hpp"
#include "NeuromodulationSystem.hpp"
#include "Integration.hpp"

namespace nlm {

class BrainIntegration {
public:
    BrainIntegration(Brain& brain);
    ~BrainIntegration();
    
    // Initialize all integrated systems
    bool initialize();
    
    // Main brain integration step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Coordinate system interactions
    void coordinateSystems();
    
    // Apply cross-system influences
    void applyCrossSystemInfluences();
    
private:
    Brain& brain;
    std::unique_ptr<Integration> integration;
};

} // namespace nlm
