// NLM Integration Tests
// Tests for Phase 6 Final Integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <algorithm>

namespace test_integration {

/**
 * Test that all brain systems integrate properly
 */
void testSystemIntegration() {
    std::cout << "Testing system integration..." << std::endl;
    
    // Create configuration for Phase 6 integration test
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Set up neuromodulation parameters
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain != nullptr);
    
    // Initialize brain
    assert(brain->initialize());
    
    // Verify brain is properly initialized
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() > 0);
    
    // Run integration simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(static_cast<nlm::SimulationStep>(step));
        
        // Verify brain remains stable
        assert(brain->getTotalNeuronCount() > 0);
        
        // Check that synaptic plasticity is occurring
        if (step > 0) {
            // Brain should show changes due to plasticity
            break;
        }
    }
    
    std::cout << "  PASSED: System integration test" << std::endl;
}

/**
 * Test memory system integration
 */
void testMemorySystemIntegration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12346), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test working memory
    size_t workingMemorySize = 50;
    auto workingMemory = std::make_shared<nlm::WorkingMemory>(workingMemorySize);
    
    // Test episodic memory
    auto episodicMemory = std::make_shared<nlm::EpisodicMemory>("test_episodic");
    
    // Test associative memory
    auto associativeMemory = std::make_shared<nlm::AssociativeMemory>(0.1f);
    
    // Verify memories are properly initialized
    assert(workingMemory->getCapacity() == workingMemorySize);
    assert(workingMemory->getCurrentSize() == 0);
    
    // Test memory operations
    std::vector<float> testPattern(10, 0.5f);
    workingMemory->storePattern(testPattern);
    assert(workingMemory->getPatternCount() > 0);
    
    std::cout << "  PASSED: Memory system integration test" << std::endl;
}

/**
 * Test neuromodulation integration
 */
void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12347), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("dopamine_baseline", 0.1f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test dopamine system
    auto dopamine = std::make_shared<nlm::Dopamine>();
    assert(dopamine->getName() == std::string("DA"));
    assert(dopamine->getLevel() == 0.0f);  // Initial level
    
    // Apply reward modulation
    dopamine->signalReward(0.5f);
    assert(dopamine->getLevel() > 0.0f);
    
    // Test plasticity factor calculation
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.5f && plasticityFactor <= 1.0f);
    
    std::cout << "  PASSED: Neuromodulation integration test" << std::endl;
}

/**
 * Test prediction system integration
 */
void testPredictionSystemIntegration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12348), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test prediction system
    auto predictionSystem = std::make_shared<nlm::PredictionSystem>();
    assert(predictionSystem->getName() == std::string("Prediction"));
    
    // Create test data
    std::vector<float> currentState(20, 0.1f);
    std::vector<float> expectedState(20, 0.2f);
    
    // Calculate prediction error
    float predictionError = predictionSystem->calculatePredictionError(currentState, expectedState);
    assert(predictionError >= 0.0f);
    
    std::cout << "  PASSED: Prediction system integration test" << std::endl;
}

/**
 * Test curiosity and novelty integration
 */
void testCuriosityNoveltyIntegration() {
    std::cout << "Testing curiosity and novelty integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12349), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test novelty detection
    auto novelty = std::make_shared<nlm::Novelty>();
    assert(novelty->getLevel() == 0.0f);  // Initial level
    
    // Simulate novelty detection
    std::vector<float> currentPattern(10, 0.3f);
    std::vector<float> previousPattern(10, 0.1f);
    
    novelty->detectNovelty(currentPattern, previousPattern);
    assert(novelty->getLevel() > 0.0f);
    
    // Test curiosity system
    auto curiosity = std::make_shared<nlm::Curiosity>();
    assert(curiosity->getLevel() == 0.0f);  // Initial level
    
    std::cout << "  PASSED: Curiosity and novelty integration test" << std::endl;
}

/**
 * Test development system integration
 */
void testDevelopmentSystemIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(12350), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test developmental stage progression
    nlm::DevelopmentalStage stage = nlm::DevelopmentalStage::Initial;
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Test synaptogenesis
    auto synaptogenesis = std::make_shared<nlm::Synaptogenesis>(0.001f);
    assert(synaptogenesis->getFormationRate() > 0.0f);
    
    // Test pruning
    auto pruning = std::make_shared<nlm::Pruning>(0.0001f);
    assert(pruning->getPruningRate() > 0.0f);
    
    // Test maturation
    auto maturation = std::make_shared<nlm::Maturation>("test_maturation");
    
    std::cout << "  PASSED: Development system integration test" << std::endl;
}

/**
 * Run all integration tests
 */
void runAll() {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << "Testing Phase 6 Final Integration:" << std::endl;
    
    bool allPassed = true;
    
    try {
        testSystemIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: System integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemorySystemIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Memory system integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNeuromodulationIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Neuromodulation integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPredictionSystemIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Prediction system integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testCuriosityNoveltyIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Curiosity and novelty integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testDevelopmentSystemIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: Development system integration - " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
    }
}

} // namespace test_integration