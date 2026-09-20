// Memory system experiment integration
// Moved from Phase4Experiment.cpp - NeuralWorkingMemory, NeuralEpisodicMemory systems

#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Memory Integration Test
// Tests working memory and episodic memory systems integration
class MemoryExperimentIntegration {
public:
    MemoryExperimentIntegration();
    ~MemoryExperimentIntegration();
    
    // Run complete memory experiment
    void runMemoryTest(Brain* brain, size_t numTrials = 100);
    
    // Test working memory system
    void testWorkingMemorySystem(Brain* brain);
    
    // Test episodic memory system
    void testEpisodicMemorySystem(Brain* brain);
    
    // Test memory interaction with concept formation
    void testMemoryConceptIntegration(Brain* brain);
    
    // Get memory metrics
    float getWorkingMemoryRetention() const { return workingMemoryRetention_; }
    float getEpisodicRecallAccuracy() const { return episodicRecallAccuracy_; }
    size_t getEpisodicMemoryCount() const { return episodicMemoryCount_; }
    
private:
    void setupMemorySystems(Brain* brain);
    void runMemoryTasks(Brain* brain, size_t numTrials);
    void measureMemoryPerformance(Brain* brain);
    void verifyMemoryIntegration(Brain* brain);
    
    // Memory system pointers
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<ConceptFormation> concepts_;
    
    // Metrics
    float workingMemoryRetention_ = 0.0f;
    float episodicRecallAccuracy_ = 0.0f;
    size_t episodicMemoryCount_ = 0;
    std::vector<float> memoryRetentionHistory_;
    std::vector<float> episodicRecallHistory_;
};

} // namespace nlm
