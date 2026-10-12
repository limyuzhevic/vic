// Memory system integration tester
// 
// Comprehensive tests for memory system integration across the NLM architecture.
// Tests the integration of neural working memory, episodic memory, and associative memory
systems with other brain systems.

#pragma once

#include "IntegrationTest.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <gtest/gtest.h>

namespace nlm {
namespace test {

// Memory system integration tester
class MemoryIntegrationTester : public IntegrationTest {
protected:
    MemoryIntegrationTester() = default;
    virtual ~MemoryIntegrationTester() = default;
    
    // Test setup
    void SetUp() override;
    void TearDown() override;
    
    // Memory system access helpers
    NeuralWorkingMemory* getNeuralWorkingMemory(Brain* brain) const;
    NeuralEpisodicMemory* getNeuralEpisodicMemory(Brain* brain) const;
    NeuralAssociativeMemory* getNeuralAssociativeMemory(Brain* brain) const;
    
    // Integration test cases
    void testWorkingMemoryWithNeuralProcessing();
    void testEpisodicMemoryWithBrainRegions();
    void testAssociativeMemoryWithConcepts();
    void testMemoryConsolidationAcrossBrainRegions();
    void testMemoryWithNeuromodulation();
    void testMemoryWithPrediction();
    void testMemoryWithDevelopment();
    
    // Test utilities
    void verifyMemoryIntegrity(NeuralWorkingMemory* workingMemory);
    void verifyEpisodicMemoryIntegrity(NeuralEpisodicMemory* episodicMemory);
    void verifyAssociativeMemoryIntegrity(NeuralAssociativeMemory* associativeMemory);
    float calculateMemoryCorrelation(NeuralWorkingMemory* workingMemory, 
                                     NeuralEpisodicMemory* episodicMemory);
}; // class MemoryIntegrationTester

} // namespace test
} // namespace nlm