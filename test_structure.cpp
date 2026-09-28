// Simple test to verify Brain refactoring structure
#include "src/brain/Brain.hpp"
#include "src/brain/BrainSystemManager.hpp"
#include "src/memory/MemorySystemManager.hpp"
#include "src/neuromodulation/NeuromodulationManager.hpp"
#include "src/development/DevelopmentManager.hpp"
#include "src/performance/CheckpointManager.hpp"
#include "src/motor/ActionSystem.hpp"
#include "src/plasticity/PlasticitySystem.hpp"

// Test that all headers can be included
namespace test {
    void testIncludes() {
        // Verify all system manager classes can be instantiated
        BrainSystemManager brainManager;
        MemorySystemManager memoryManager;
        NeuromodulationManager neuromodulationManager;
        DevelopmentManager developmentManager;
        CheckpointManager checkpointManager;
        ActionSystem actionManager;
        PlasticitySystem plasticityManager;
        
        // Verify Brain class has BrainSystemManager access
        std::shared_ptr<Config> config = std::make_shared<Config>();
        Brain brain(config);
        
        // Check BrainSystemManager access (if available)
        if (brain.getBrainSystemManager()) {
            // System is properly initialized
        }
    }
}

int main() {
    return 0;
}
