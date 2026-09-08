#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Memory system initialization and configuration
class MemoryInitializer {
public:
    // Configure working memory based on brain state
    static void configureWorkingMemory(NeuralWorkingMemory* wmem, Brain* brain);
    
    // Configure episodic memory based on brain state  
    static void configureEpisodicMemory(NeuralEpisodicMemory* emem, Brain* brain);
    
    // Consolidate between memory systems
    static void consolidateMemory(Brain* brain);
    
    // Update all memory systems with new sensory input
    static void updateWithSensoryInput(Brain* brain, const std::vector<float>& sensoryPattern);
};

} // namespace nlm
