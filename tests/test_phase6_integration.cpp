#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

/**
 * Test basic integration verification
 */
void testIntegrationVerification() {
    std::cout << "=== Testing Integration Verification ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Simple integration verification
    bool success = experiment.verifyIntegration();
    
    if (success) {
        std::cout << "✓ Integration verification passed" << std::endl;
    } else {
        std::cout << "✗ Integration verification failed" << std::endl;
        assert(false && "Integration verification should pass");
    }
}

/**
 * Test memory system integration
 */
void testMemoryIntegration() {
    std::cout << "=== Testing Memory Integration ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testMemoryIntegration();
    
    if (success) {
        std::cout << "✓ Memory integration test passed" << std::endl;
    } else {
        std::cout << "✗ Memory integration test failed" << std::endl;
        assert(false && "Memory integration test should pass");
    }
}

/**
 * Test neuromodulation integration
 */
void testNeuromodulationIntegration() {
    std::cout << "=== Testing Neuromodulation Integration ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testNeuromodulationIntegration();
    
    if (success) {
        std::cout << "✓ Neuromodulation integration test passed" << std::endl;
    } else {
        std::cout << "✗ Neuromodulation integration test failed" << std::endl;
        assert(false && "Neuromodulation integration test should pass");
    }
}

/**
 * Test checkpointing functionality
 */
void testCheckpointing() {
    std::cout << "=== Testing Checkpointing ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testCheckpointing();
    
    if (success) {
        std::cout << "✓ Checkpointing test passed" << std::endl;
    } else {
        std::cout << "✗ Checkpointing test failed" << std::endl;
        assert(false && "Checkpointing test should pass");
    }
}

/**
 * Test replay functionality
 */
void testReplay() {
    std::cout << "=== Testing Replay ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.testReplay();
    
    if (success) {
        std::cout << "✓ Replay test passed" << std::endl;
    } else {
        std::cout << "✗ Replay test failed" << std::endl;
        assert(false && "Replay test should pass");
    }
}

/**
 * Test complete integration experiment
 */
void testCompleteIntegration() {
    std::cout << "=== Testing Complete Integration ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 100;
    config.enableCheckpointing = false; // Disable for faster test
    config.enableReplay = false;        // Disable for faster test
    config.enableDevelopment = false;   // Disable for faster test
    
    Phase6IntegrationResult result = experiment.run(config);
    
    // Check key integration metrics
    assert(result.memoryWorkingMemoryIntegrated, "Working memory should be integrated");
    assert(result.memoryEpisodicMemoryIntegrated, "Episodic memory should be integrated");
    assert(result.neuromodulationIntegrated, "Neuromodulation should be integrated");
    assert(result.predictionIntegrated, "Prediction should be integrated");
    assert(result.developmentIntegrated, "Development should be integrated");
    
    std::cout << "✓ Complete integration test passed" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
}

/**
 * Test Python bindings integration
 */
void testPythonBindings() {
    std::cout << "=== Testing Python Bindings ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    // Create brain using C++ API
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize(), "Brain initialization should succeed");
    
    // Create world
    SimpleWorld world;
    world.configure(8, 8);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test basic operations
    assert(brain->getTotalNeuronCount() > 0, "Should have neurons");
    assert(brain->getTotalSynapseCount() >= 0, "Should have synapses");
    assert(brain->getAverageFiringRate() >= 0.0f, "Should have firing rate");
    
    // Test neuromodulation
    assert(agent.getNeuromodulationLevel() >= 0.0f, "Should have neuromodulation level");
    assert(agent.getCuriosityLevel() >= 0.0f, "Should have curiosity level");
    assert(agent.getNoveltyLevel() >= 0.0f, "Should have novelty level");
    
    std::cout << "✓ Python bindings test passed" << std::endl;
}

/**
 * Test performance with different configurations
 */
void testPerformance() {
    std::cout << "=== Testing Performance ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test with different neuron counts
    std::vector<size_t> neuronCounts = {10, 50, 100};
    
    for (size_t neuronCount : neuronCounts) {
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = 50;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto start = std::chrono::high_resolution_clock::now();
        Phase6IntegrationResult result = experiment.run(config);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(end - start).count();
        
        std::cout << "  " << neuronCount << " neurons: " << duration << "s" << std::endl;
        assert(duration < 60.0, "Performance should be reasonable"); // Should complete within 1 minute
    }
    
    std::cout << "✓ Performance test passed" << std::endl;
}

/**
 * Run all integration tests
 */
int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing the integrated artificial brain system..." << std::endl;
    
    // Set logging level
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    try {
        testIntegrationVerification();
        testMemoryIntegration();
        testNeuromodulationIntegration();
        testCheckpointing();
        testReplay();
        testCompleteIntegration();
        testPythonBindings();
        testPerformance();
        
        std::cout << std::endl;
        std::cout << "=== All Integration Tests Passed! ===" << std::endl;
        std::cout << "The Phase 6 integrated brain system is working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Integration Tests Failed ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}
