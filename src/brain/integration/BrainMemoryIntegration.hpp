#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainMemoryIntegration {
public:
    BrainMemoryIntegration();
    ~BrainMemoryIntegration();
    
    // Initialize all memory systems
    void initializeMemorySystems(Brain* brain, size_t neuronCount);
    
    // Integrate working memory operations
    void integrateWorkingMemory(Brain* brain);
    
    // Integrate episodic memory operations
    void integrateEpisodicMemory(Brain* brain);
    
    // Integrate associative memory operations
    void integrateAssociativeMemory(Brain* brain);
    
    // Integrate memory with cognition systems
    void integrateMemoryWithCognition(Brain* brain);
    
    // Store experience in episodic memory
    void storeExperience(Brain* brain, const std::vector<float>& sensoryPattern,
                        float reward, size_t action);
    
    // Retrieve relevant memories
    std::vector<float> retrieveMemories(Brain* brain, const std::vector<float>& query);
    
    // Update memory systems
    void updateMemorySystems(Brain* brain, TimestepDuration dt);
    
    // Check for memory consolidation
    void checkMemoryConsolidation(Brain* brain, SimulationStep currentStep);
    
    // Apply memory to decision making
    float applyMemoryToDecision(Brain* brain, const std::vector<float>& sensoryInput);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm