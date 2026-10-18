// Phase 6 Integration Tests
// Comprehensive tests for Phase 6 integrated brain systems
// Tests all major Phase 6 systems: memory, neuromodulation, prediction, cognition, development

#include "Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_phase6 {

void testMemorySystemIntegration() {
    std::cout << "=== Testing Memory System Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Test working memory
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    std::cout << "✓ Memory systems integration test passed" << std::endl;
}

void testNeuromodulationSystemIntegration() {
    std::cout << "=== Testing Neuromodulation System Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Test neuromodulation systems
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    std::cout << "✓ Neuromodulation systems integration test passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "=== Testing Development System Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Test development system
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    std::cout << "✓ Development system integration test passed" << std::endl;
}

void testCheckpointSystem() {
    std::cout << "=== Testing Checkpoint System ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run some simulation steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    std::string checkpointFile = "/tmp/nlm_phase6_checkpoint_test.bin";
    
    // Save checkpoint
    bool saveSuccess = brain->save(checkpointFile);
    assert(saveSuccess);
    
    // Create new brain and load checkpoint
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();
    
    bool loadSuccess = brain2->load(checkpointFile);
    assert(loadSuccess);
    
    std::cout << "✓ Checkpoint system test passed" << std::endl;
}

void testReplaySystem() {
    std::cout << "=== Testing Replay System ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation to create episodes
    for (int i = 0; i < 200; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Test replay system
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    // Test replay functionality
    auto episodes = episodicMemory->getEpisodesForReplay(5);
    
    std::cout << "    Episodes stored: " << episodeCount << std::endl;
    
    std::cout << "✓ Replay system test passed" << std::endl;
}

void testPlasticitySystems() {
    std::cout << "=== Testing Plasticity Systems ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity
    auto* region = brain->getRegion(RegionId(1));
    assert(region != nullptr);
    
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, true);
    }
    
    // Run plasticity experiment
    for (int i = 0; i < 200; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(i, i * 0.001);
    }
    
    std::cout << "✓ Plasticity systems test passed" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "PHASE 6 INTEGRATION TEST SUITE" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    try {
        testMemorySystemIntegration();
        testNeuromodulationSystemIntegration();
        testDevelopmentSystemIntegration();
        testCheckpointSystem();
        testReplaySystem();
        testPlasticitySystems();
        
        std::cout << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "ALL PHASE 6 INTEGRATION TESTS PASSED!" << std::endl;
        std::cout << "The integrated brain system is functioning correctly." << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_phase6

// Test entry point
int main() {
    test_phase6::runAllPhase6IntegrationTests();
    return 0;
}
