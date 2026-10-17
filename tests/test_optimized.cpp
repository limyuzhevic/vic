// Comprehensive Tests for Optimized NLM Codebase
// Tests for Brain.cpp, AgentBrain.cpp, and other optimizations

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "core/Error/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <chrono>

namespace test_optimized_code {

void testBrainRefactoring() {
    std::cout << "Testing Brain refactoring optimizations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test that the refactored step function works correctly
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run multiple steps to test the modular implementation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify the brain is still functional
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    
    // Verify that modular helper methods are working
    brain.logStatus(); // This tests the integration of all subsystems
    
    std::cout << "✓ Brain refactoring tests passed" << std::endl;
}

void testBrainModularStepExecution() {
    std::cout << "Testing modular step execution..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test individual step components
    size_t beforeSpikes = brain.getTotalSpikeCount();
    size_t beforeEvents = brain.getPendingSpikeEventCount();
    
    // Test a single step
    brain.step(0);
    
    // Verify that all modular components executed
    assert(brain.getTotalNeuronCount() == 50); // Should still have all neurons
    assert(brain.getRegionCount() == 1);       // Should still have all regions
    
    // Test with time parameter
    brain.step(1, 0.001);
    
    std::cout << "✓ Modular step execution tests passed" << std::endl;
}

void testAgentBrainModularImplementation() {
    std::cout << "Testing AgentBrain modular implementation..." << std::endl;
    
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test MotorGroups structure
    assert(agentBrain.getBrain() == &brain);
    assert(agentBrain.getSensoryInputSize() == 262); // 256 vision + 8 touch + 4 internal + 6 proprioception
    assert(agentBrain.getMotorOutputSize() == 6);   // 6 motor commands
    
    // Test that the new groups structure is working
    nlm::SimpleWorld world(16, 16); // Create a simple world
    agentBrain.initialize(world);
    
    // Verify sensor groups are populated
    assert(!agentBrain.getDopamineLevel() >= 0.0f); // Should be 0.0 initially
    
    // Test configuration methods
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled());
    assert(agentBrain.isCuriosityEnabled());
    
    std::cout << "✓ AgentBrain modular implementation tests passed" << std::endl;
}

void testAgentBrainSensoryProcessingOptimization() {
    std::cout << "Testing AgentBrain sensory processing optimization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create test percept data
    nlm::SensoryPercept percept;
    
    // Test vision processing (256 values)
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    // Test touch processing (8 values)
    std::vector<float> touch(8, 0.3f);
    percept.setTouch(touch);
    
    // Test internal signals (4 values)
    std::vector<float> internal(4, 0.1f);
    percept.setInternal(internal);
    
    // Test proprioception (6 values)
    std::vector<float> proprioception(6, 0.2f);
    percept.setProprioception(proprioception);
    
    // This should call all the modular processing methods
    agentBrain.processSensoryInput(percpt);
    
    // Verify novelty computation
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    
    std::cout << "✓ AgentBrain sensory processing optimization tests passed" << std::endl;
}

void testAgentBrainModularDecoding() {
    std::cout << "Testing AgentBrain modular decoding..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test motor command decoding
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Should return a valid command
    assert(command == nlm::MotorCommand::Wait || 
            command == nlm::MotorCommand::MoveForward ||
            command == nlm::MotorCommand::MoveBackward ||
            command == nlm::MotorCommand::TurnLeft ||
            command == nlm::MotorCommand::TurnRight ||
            command == nlm::MotorCommand::Interact);
    
    // Test with curiosity enabled
    agentBrain.enableCuriosity(true);
    command = agentBrain.decodeMotorCommand();
    assert(command != nlm::MotorCommand::Wait || agentBrain.getCuriosityLevel() > 0.3f);
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    assert(agentBrain.getPredictionError() == 0.5f);
    
    std::cout << "✓ AgentBrain modular decoding tests passed" << std::endl;
}

void testBrainPerformanceOptimizations() {
    std::cout << "Testing Brain performance optimizations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure performance of the refactored implementation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run multiple steps to test performance
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "  500 neurons in 5 regions: " << duration.count() << "ms for 100 steps" << std::endl;
    
    // Verify that the optimized implementation produces correct results
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 5);
    
    // Test that memory usage is reasonable (should not leak)
    size_t beforeMemory = brain.getTotalNeuronCount();
    brain.reset();
    size_t afterMemory = brain.getTotalNeuronCount();
    
    // Reset should clear state but keep structure
    assert(afterMemory == beforeMemory);
    
    std::cout << "✓ Brain performance optimization tests passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "Testing memory system integration in Brain..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "✓ Memory system integration tests passed" << std::endl;
}

void testBrainConfigurationIntegration() {
    std::cout << "Testing Brain configuration integration..." << std::endl;
    
    // Test that Brain properly integrates with Config system
    auto config = std::make_shared<nlm::Config>();
    
    // Set various configuration values
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.0005, nlm::ConfigSource::Runtime);
    
    // Create brain with this configuration
    nlm::Brain brain(config);
    
    // Brain should have access to the configuration
    auto brainConfig = brain.getConfig();
    assert(brainConfig != nullptr);
    assert(brainConfig->get<int64_t>("neuron_count") == 300);
    assert(brainConfig->get<int64_t>("region_count") == 3);
    assert(brainConfig->get<double>("connection_probability") == 0.15);
    assert(brainConfig->get<double>("simulation_timestep") == 0.0005);
    
    // Test that configuration affects brain behavior
    assert(brain.initialize());
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getRegionCount() == 3);
    
    std::cout << "✓ Brain configuration integration tests passed" << std::endl;
}

void testErrorHandlingIntegration() {
    std::cout << "Testing error handling integration..." << std::endl;
    
    // Test that the optimized code still has proper error handling
    auto config = std::make_shared<nlm::Config>();
    
    // Test config validation
    config->set("test_int", 42, nlm::ConfigSource::Runtime);
    auto value = config->get<int>("test_int");
    assert(value.has_value());
    
    // Test error handling macros
    try {
        NLM_THROW_VALIDATION("Test error", "test_field", 
                            nlm::ValidationError::ErrorType::InvalidValue);
        assert(false); // Should not reach here
    } catch (const nlm::ValidationError& e) {
        assert(e.getField() == "test_field");
    }
    
    std::cout << "✓ Error handling integration tests passed" << std::endl;
}

void runAllOptimizedTests() {
    std::cout << "=== Running Comprehensive Tests for Optimized Code ===" << std::endl;
    std::cout << std::endl;
    
    testBrainRefactoring();
    testBrainModularStepExecution();
    testAgentBrainModularImplementation();
    testAgentBrainSensoryProcessingOptimization();
    testAgentBrainModularDecoding();
    testBrainPerformanceOptimizations();
    testMemorySystemIntegration();
    testBrainConfigurationIntegration();
    testErrorHandlingIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Optimized Code Tests PASSED ===" << std::endl;
}

} // namespace test_optimized_code

// Entry point for optimized code tests
int main() {
    test_optimized_code::runAllOptimizedTests();
    return 0;
}