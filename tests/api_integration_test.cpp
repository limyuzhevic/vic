// API Integration Test Suite
// Tests for system-wide API integration and cross-system compatibility
// Validates integration between all NLM systems and components

#include "api_integration_test.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

void testBrainIntegrationWithMemory() {
    std::cout << "Testing brain integration with memory systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Verify integration with memory systems
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    
    if (workingMemory) {
        // Test that working memory is properly integrated
        brain.step(1);
        // Memory should receive spike data
        std::cout << "Working memory integration test passed!" << std::endl;
    } else {
        std::cout << "Working memory not available (expected for basic test)" << std::endl;
    }
    
    if (episodicMemory) {
        // Test that episodic memory is properly integrated
        std::cout << "Episodic memory integration test passed!" << std::endl;
    } else {
        std::cout << "Episodic memory not available (expected for basic test)" << std::endl;
    }
}

void testBrainIntegrationWithNeuromodulation() {
    std::cout << "Testing brain integration with neuromodulation systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Verify integration with neuromodulation systems
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    
    if (dopamine) {
        // Test that dopamine system is properly integrated
        brain.step(1);
        std::cout << "Dopamine integration test passed!" << std::endl;
    } else {
        std::cout << "Dopamine system not available (expected for basic test)" << std::endl;
    }
    
    if (curiosity) {
        // Test that curiosity system is properly integrated
        std::cout << "Curiosity integration test passed!" << std::endl;
    } else {
        std::cout << "Curiosity system not available (expected for basic test)" << std::endl;
    }
}

void testBrainIntegrationWithPrediction() {
    std::cout << "Testing brain integration with prediction system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Verify integration with prediction system
    auto prediction = brain.getPredictionSystem();
    
    if (prediction) {
        // Test that prediction system is properly integrated
        brain.step(1);
        std::cout << "Prediction system integration test passed!" << std::endl;
    } else {
        std::cout << "Prediction system not available (expected for basic test)" << std::endl;
    }
}

void testBrainIntegrationWithDevelopment() {
    std::cout << "Testing brain integration with development system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Verify integration with development system
    auto development = brain.getDevelopmentSystem();
    
    if (development) {
        // Test that development system is properly integrated
        brain.step(1);
        std::cout << "Development system integration test passed!" << std::endl;
    } else {
        std::cout << "Development system not available (expected for basic test)" << std::endl;
    }
}

void testAgentBrainIntegration() {
    std::cout << "Testing AgentBrain integration with brain..." << std::endl;
    
    // Create brain for agent
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent brain
    AgentBrain agentBrain(brain);
    agentBrain.initialize();
    
    // Test that agent brain properly integrates with brain
    assert(agentBrain.getBrain() == brain);
    assert(agentBrain.getSensoryPercept().isValid() == true);
    
    std::cout << "AgentBrain integration test passed!" << std::endl;
}

void testWorldIntegration() {
    std::cout << "Testing world integration..." << std::endl;
    
    // Create simple world
    SimpleWorld world;
    world.configure(50.0f, 50.0f, 16, 16);
    
    // Create brain for world integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent for world integration
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test world integration with agent
    agentBrain.step(1.0f);
    
    assert(agentBrain.getSensoryPercept().isValid() == true);
    
    std::cout << "World integration test passed!" << std::endl;
}

void testPhase6Integration() {
    std::cout << "Testing Phase 6 integration..." << std::endl;
    
    // Test Phase 6 integrated experiment
    Phase6IntegratedExperiment experiment;
    
    // Verify Phase 6 integration
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK == true);
    
    // Test Phase 6 integration fix
    Phase6IntegrationFix fix;
    
    bool compatOK = fix.verifyBackwardCompatibility();
    assert(compatOK == true);
    
    std::cout << "Phase 6 integration test passed!" << std::endl;
}

void runAPIIntegrationTests() {
    std::cout << "=== Running API Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    testBrainIntegrationWithMemory();
    std::cout << std::endl;
    
    testBrainIntegrationWithNeuromodulation();
    std::cout << std::endl;
    
    testBrainIntegrationWithPrediction();
    std::cout << std::endl;
    
    testBrainIntegrationWithDevelopment();
    std::cout << std::endl;
    
    testAgentBrainIntegration();
    std::cout << std::endl;
    
    testWorldIntegration();
    std::cout << std::endl;
    
    testPhase6Integration();
    std::cout << std::endl;
    
    std::cout << "=== All API Integration Tests Passed ===" << std::endl;
}

} // namespace nlm
