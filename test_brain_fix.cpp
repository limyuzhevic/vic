#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>

int main() {
    // Create a minimal config
    auto config = std::make_shared<Config>();
    
    // Create brain instance
    Brain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test all memory accessors - they should all be accessible
    brain.getWorkingMemory();
    brain.getEpisodicMemory();
    brain.getSemanticMemory();
    brain.getProceduralMemory();
    brain.getAssociativeMemory();
    
    return 0;
}