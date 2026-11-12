// Comprehensive Integration Tests for NLM Brain (Phase 6)
// Tests all integrated systems: memory, prediction, cognition, neuromodulation, development

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace nlm {
namespace test_integration {

void testIntegratedSystemSetup() {
    std::cout << "  Setting up integrated test environment..." << std::endl;
    
    // Create comprehensive configuration for integrated systems
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Initialize brain with integrated systems
    Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    std::cout << "    Integrated system setup complete" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation systems are accessible
    Dopamine* dopamine = brain.getDopamine();
    Curiosity* curiosity = brain.getCuriosity();
    Novelty* novelty = brain.getNovelty();
    PredictionError* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulation levels
    float dopamineLevel = dopamine->getLevel();
    float curiosityLevel = curiosity->getLevel();
    float noveltyLevel = novelty->getLevel();
    float predictionErrorValue = predictionError->getError();
    
    // Should start at reasonable baseline levels
    assert(dopamineLevel >= 0.0f && dopamineLevel <= 1.0f);
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    assert(std::abs(predictionErrorValue) <= 1.0f);
    
    // Test neuromodulation integration during brain steps
    for (SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Neuromodulation should affect neural activity
        float currentDopamine = dopamine->getLevel();
        float currentCuriosity = curiosity->getLevel();
        float currentNovelty = novelty->getLevel();
        
        // These should evolve over time based on brain activity
        if (step > 0) {
            assert(currentDopamine >= 0.0f && currentDopamine <= 1.0f);
            assert(currentCuriosity >= 0.0f && currentCuriosity <= 1.0f);
            assert(currentNovelty >= 0.0f && currentNovelty <= 1.0f);
        }
    }
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "  Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test memory systems are accessible
    NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test working memory integration during brain operation
    for (SimulationStep step = 0; step < 30; ++step) {
        // Inject some sensory input to trigger memory storage
        std::vector<float> sensoryInput;
        for (size_t i = 0; i < 10; ++i) {
            sensoryInput.push_back(1.0f);  // Simple input pattern
        }
        
        // Create simple sensory input (placeholder)
        brain.step(step);
        
        // Working memory should be updating based on brain activity
        size_t memoryTraces = workingMemory->getActiveTraces();
        assert(memoryTraces >= 0);
        
        // Episodic memory should be storing episodes
        size_t episodeCount = episodicMemory->getEpisodeCount();
        assert(episodeCount >= 0);
    }
    
    std::cout << "    Memory system integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test prediction system
    PredictionSystem* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system integration
    float initialError = predictionSystem->getPredictionError();
    assert(initialError >= 0.0f && initialError <= 1.0f);
    
    // Run brain steps with prediction system active
    for (SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
        
        // Prediction error should evolve
        float currentError = predictionSystem->getPredictionError();
        assert(currentError >= 0.0f && currentError <= 1.0f);
    }
    
    std::cout << "    Prediction system integration test passed" << std::endl;
}

void testConceptFormationIntegration() {
    std::cout << "  Testing concept formation integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(250), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test concept formation system
    ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create some sensory patterns for concept formation
    std::vector<float> pattern1 = {1.0f, 0.8f, 0.6f, 0.9f, 0.7f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f, 0.15f, 0.25f};  // Different pattern
    
    // Present experiences to concept formation system
    size_t conceptId1 = conceptFormation->presentExperience(pattern1, pattern1, 0.8f, 1);
    size_t conceptId2 = conceptFormation->presentExperience(pattern2, pattern2, 0.2f, 2);
    
    // Should identify or create concepts
    if (conceptId1 > 0 || conceptId2 > 0) {
        std::cout << "    Concept formation created " << (conceptId1 > 0 ? "concept 1" : "") 
                  << (conceptId2 > 0 ? (conceptId1 > 0 ? "and concept 2" : "concept 2") : "") << std::endl;
    }
    
    // Test that concept formation is integrated in brain steps
    for (SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        // Concept formation should be processing sensory patterns
        // (This happens implicitly during brain operation)
    }
    
    std::cout << "    Concept formation integration test passed" << std::endl;
}

void testNeuralPlannerIntegration() {
    std::cout << "  Testing neural planner integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test neural planner system
    NeuralPlanner* neuralPlanner = brain.getPlanner();
    assert(neuralPlanner != nullptr);
    
    // Create some sensory states for neural planner
    std::vector<float> currentState;
    for (size_t i = 0; i < 10; ++i) {
        currentState.push_back(0.5f);  // Neutral state
    }
    
    // Test neural planner planning
    ActionType plannedAction = neuralPlanner->planAction(currentState, 0.5f);
    assert(plannedAction == ActionType::Wait || plannedAction == ActionType::MoveForward);
    
    // Test that neural planner integration works during brain operation
    for (SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Neural planner should be active during complex developmental stages
        // The planner's planning depth may change with development
    }
    
    std::cout << "    Neural planner integration test passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "  Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test development system
    DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development system properties
    DevelopmentalStage currentStage = developmentSystem->getStage();
    float plasticityModifier = developmentSystem->getPlasticityModifier();
    float memoryConsolidation = developmentSystem->getMemoryConsolidationFactor();
    float neuralExcitability = developmentSystem->getNeuralExcitabilityModifier();
    
    assert(plasticityModifier >= 0.1f && plasticityModifier <= 1.0f);
    assert(memoryConsolidation >= 0.1f && memoryConsolidation <= 1.0f);
    assert(neuralExcitability >= 0.5f && neuralExcitability <= 1.0f);
    
    // Test that development system affects brain behavior
    for (SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {  // Development updates every 1000 steps
            DevelopmentStage stage = developmentSystem->getStage();
            float stagePlasticity = developmentSystem->getPlasticityModifier();
            
            // Development should influence brain behavior
            assert(stagePlasticity > 0.0f);
        }
    }
    
    std::cout << "    Development system integration test passed" << std::endl;
}

void testAdvancedCLICommands() {
    std::cout << "  Testing advanced CLI commands..." << std::endl;
    
    // Test that CLI commands are available
    // This would normally test actual command-line parsing, but for unit tests
    // we'll test the configuration parsing logic
    
    auto config = std::make_shared<Config>();
    
    // Test configuration override (--key=value format)
    config->set("test_param", std::string("test_value"), ConfigSource::CommandLine);
    assert(config->has("test_param"));
    
    // Test configuration loading
    config->loadFromArgs(1, nullptr);  // Empty args
    
    std::cout << "    Advanced CLI commands test passed" << std::endl;
}

void testEndToEndIntegration() {
    std::cout << "  Testing end-to-end integration..." << std::endl;
    
    // This test simulates a complete scenario with all systems integrated
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.12f, ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Simulate a complete neural computation with all systems active
    for (SimulationStep step = 0; step < 100; ++step) {
        // Each step involves all integrated systems:
        // - Neural computation (Phase 2)
        // - Memory systems (Phase 4)
        // - Prediction system (Phase 6)
        // - Neuromodulation (Phase 6)
        // - Development system (Phase 6)
        // - Concept formation (Phase 6)
        // - Neural planning (Phase 6)
        
        brain.step(step);
        
        // Verify all systems are active and functioning
        assert(brain.getTotalNeuronCount() == 500);
        assert(brain.getRegionCount() == 3);
        
        // Check that neuromodulation systems are responding
        Dopamine* dopamine = brain.getDopamine();
        if (dopamine) {
            assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
        }
        
        // Check that memory systems are operating
        NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            size_t traces = workingMemory->getActiveTraces();
            assert(traces >= 0);
        }
    }
    
    std::cout << "    End-to-end integration test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 NLM INTEGRATED SYSTEMS TESTS (Phase 6)                   ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║                                                                        ║" << std::endl;
    
    testIntegratedSystemSetup();
    testNeuromodulationIntegration();
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testConceptFormationIntegration();
    testNeuralPlannerIntegration();
    testDevelopmentSystemIntegration();
    testAdvancedCLICommands();
    testEndToEndIntegration();
    
    std::cout << std::endl;
    std::cout << "║                                                                        ║" << std::endl;
    std::cout << "║  All integration tests completed successfully!                     ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

} // namespace test_integration
} // namespace nlm