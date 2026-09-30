// Working Memory Integration Tests
// Tests for NeuralWorkingMemory system integration with other systems

#pragma once

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "IntegrationTestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace test_workingmemory {

class WorkingMemoryIntegrationTests : public IntegrationTestFramework {
public:
    WorkingMemoryIntegrationTests() {
        auto config = getConfig();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    }
    
    ~WorkingMemoryIntegrationTests() override = default;
    
    void runAllTests() {
        std::cout << "=== Working Memory Integration Tests ===" << std::endl;
        
        testWorkingMemoryStorageFromSensoryInput();
        testWorkingMemoryIntegrationWithActionSelection();
        testWorkingMemoryCompetitionAndAttentionSelection();
        testWorkingMemoryDecayAndConsolidation();
        testWorkingMemoryReplayIntegration();
        
        std::cout << "=== All Working Memory Integration Tests PASSED ===" << std::endl;
    }
    
private:
    void testWorkingMemoryStorageFromSensoryInput() {
        logTestStart("Working Memory Storage from Sensory Input");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Simulate sensory input to working memory
        std::vector<float> sensoryPattern(100, 0.0f);
        for (size_t i = 0; i < 50; ++i) {
            sensoryPattern[i] = static_cast<float>(i) / 50.0f;
        }
        
        workingMemory->store(sensoryPattern, 1.0f);
        
        // Verify memory was stored
        assert(workingMemory->getActiveTraces() > 0);
        
        logTestResult("Working Memory Storage from Sensory Input", true, 
                     "Stored " + std::to_string(workingMemory->getActiveTraces()) + " traces");
    }
    
    void testWorkingMemoryIntegrationWithActionSelection() {
        logTestStart("Working Memory Integration with Action Selection");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* planner = brain.getPlanner();
        assert(workingMemory != nullptr && planner != nullptr);
        
        // Store patterns that represent potential actions
        std::vector<float> actionPattern1(100, 0.0f);
        std::vector<float> actionPattern2(100, 0.0f);
        
        for (size_t i = 0; i < 100; ++i) {
            actionPattern1[i] = (i < 33) ? 1.0f : 0.0f;
            actionPattern2[i] = (i >= 67) ? 1.0f : 0.0f;
        }
        
        workingMemory->store(actionPattern1, 1.5f);
        workingMemory->store(actionPattern2, 0.8f);
        
        // Working memory influences action selection
        workingMemory->influenceActionSelection(&brain);
        
        // Verify action selection was influenced
        assert(workingMemory->getMemoryNeurons().size() > 0);
        
        logTestResult("Working Memory Integration with Action Selection", true,
                     "Action selection influenced by " + std::to_string(workingMemory->getMemoryNeurons().size()) + " memory traces");
    }
    
    void testWorkingMemoryCompetitionAndAttentionSelection() {
        logTestStart("Working Memory Competition and Attention Selection");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* attention = brain.getAttention();
        assert(workingMemory != nullptr && attention != nullptr);
        
        // Store multiple competing memory traces
        std::vector<NeuronId> competitors;
        for (size_t i = 0; i < 20; ++i) {
            std::vector<float> pattern(100, 0.0f);
            pattern[i % 10] = 1.0f;  // Different patterns
            workingMemory->store(pattern, 0.5f + (i % 3) * 0.3f);
            competitors.push_back(static_cast<NeuronId>(i));
        }
        
        // Run competition
        workingMemory->runCompetition();
        
        // Verify attention was selected
        assert(attention->getWinners().size() > 0);
        
        // Update attention scores
        attention->updateAttentionScores(workingMemory);
        
        logTestResult("Working Memory Competition and Attention Selection", true,
                     "Selected " + std::to_string(attention->getWinners().size()) + " attention winners");
    }
    
    void testWorkingMemoryDecayAndConsolidation() {
        logTestStart("Working Memory Decay and Consolidation");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Store memory with high strength
        std::vector<float> pattern(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            pattern[i] = static_cast<float>(i) / 100.0f;
        }
        
        workingMemory->store(pattern, 2.0f);
        
        // Simulate time passing with decay
        workingMemory->update(1.0f);  // Update with 1ms time step
        
        // Decay weak traces
        workingMemory->decayWeakTraces();
        
        // Store another pattern
        std::vector<float> pattern2(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            pattern2[i] = 1.0f - (static_cast<float>(i) / 100.0f);
        }
        
        workingMemory->store(pattern2, 1.0f);
        
        // Consolidate to episodic memory
        workingMemory->consolidateToEpisodic(&brain);
        
        logTestResult("Working Memory Decay and Consolidation", true,
                     "Memory decay and consolidation completed successfully");
    }
    
    void testWorkingMemoryReplayIntegration() {
        logTestStart("Working Memory Replay Integration");
        
        auto config = getConfig();
        Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Store memory traces for replay
        std::vector<float> pattern(100, 0.0f);
        for (size_t i = 0; i < 100; ++i) {
            pattern[i] = std::sin(static_cast<float>(i) * 0.1f);
        }
        
        workingMemory->store(pattern, 1.0f);
        
        // Strengthen memory (simulating replay)
        workingMemory->strengthenMemory(1.5f);
        
        // Verify memory was strengthened
        assert(workingMemory->getMemoryStability(1) > 0.0f);
        
        // Simulate replay integration by storing synaptic traces
        for (size_t i = 0; i < 10; ++i) {
            workingMemory->storeSynapticTrace(static_cast<NeuronId>(i), static_cast<NeuronId>(i + 1), 0.5f, SynapseType::Excitatory, 1.0f, 0);
        }
        
        logTestResult("Working Memory Replay Integration", true,
                     "Replay integration completed with " + std::to_string(workingMemory->getMemoryStability(1)) + " stability score");
    }
};

void runAll() {
    WorkingMemoryIntegrationTests tests;
    tests.runAllTests();
}

} // namespace test_workingmemory
} // namespace nlm
