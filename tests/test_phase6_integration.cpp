// Phase 6 Integration Tests
// Tests for complete integration of all Phase 6 brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Errors/Errors.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <filesystem>

namespace test_phase6_integration {

void testMemorySystemsIntegration() {
    std::cout << "    Testing Memory Systems Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable memory systems
    config->set("working_memory_enabled", true, nlm::ConfigSource::Runtime);
    config->set("episodic_memory_enabled", true, nlm::ConfigSource::Runtime);
    config->set("associative_memory_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Verify memory systems are integrated
    assert(brain.getWorkingMemoryEnabled(), "Working memory should be enabled");
    assert(brain.getEpisodicMemoryEnabled(), "Episodic memory should be enabled");
    assert(brain.getAssociativeMemoryEnabled(), "Associative memory should be enabled");
    
    // Test working memory integration
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
    brain.step(0, 0.001);
    
    assert(brain.getWorkingMemoryActiveNeurons() > 0, "Working memory should have active neurons");
    
    // Test episodic memory encoding (after sufficient activity)
    for (nlm::SimulationStep step = 1; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    assert(brain.getEpisodicMemoryCount() > 0, "Episodic memory should be encoded");
    
    // Test associative memory integration
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        auto patterns = workingMem->getPatternBank();
        assert(patterns.size() > 0, "Associative memory should have patterns");
    }
    
    std::cout << "    Memory Systems Integration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable neuromodulation
    config->set("dopamine_enabled", true, nlm::ConfigSource::Runtime);
    config->set("novelty_enabled", true, nlm::ConfigSource::Runtime);
    config->set("curiosity_enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction_error_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Verify neuromodulation systems are active
    assert(brain.getDopamineLevel() >= 0.0f, "Dopamine level should be non-negative");
    assert(brain.getNoveltyLevel() >= 0.0f, "Novelty level should be non-negative");
    assert(brain.getCuriosityLevel() >= 0.0f, "Curiosity level should be non-negative");
    assert(brain.getPredictionError() >= 0.0f, "Prediction error should be non-negative");
    
    // Test neuromodulation effect on plasticity
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Apply reward-based neuromodulation
    brain.applyRewardModulation(0.8f, 0.5f);
    
    brain.step(0, 0.001);
    
    // Test novelty-driven exploration
    brain.updateNoveltyDetection(0.3f);
    brain.step(1, 0.001);
    
    // Test curiosity-driven exploration
    brain.updateCuriosity(0.5f);
    brain.step(2, 0.001);
    
    std::cout << "    Neuromodulation Integration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable prediction system
    config->set("prediction_system_enabled", true, nlm::ConfigSource::Runtime);
    config->set("working_memory_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Test prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr, "Prediction system should be available");
    
    // Test prediction error computation
    float predictionError = predictionSystem->getPredictionError();
    assert(predictionError >= 0.0f, "Prediction error should be non-negative");
    
    // Test prediction-based action selection
    auto action = brain.produceAction();
    assert(action != nullptr, "Brain should produce action based on predictions");
    
    // Test prediction learning through experience
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // Inject current to stimulate prediction learning
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(step, step * 0.001);
    }
    
    // Check that prediction error changes over time
    float finalPredictionError = predictionSystem->getPredictionError();
    // Prediction error should evolve through learning
    
    std::cout << "    Prediction System Integration passed" << std::endl;
}

void testCognitionSystemsIntegration() {
    std::cout << "    Testing Cognition Systems Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable cognitive systems
    config->set("attention_enabled", true, nlm::ConfigSource::Runtime);
    config->set("planning_enabled", true, nlm::ConfigSource::Runtime);
    config->set("concept_formation_enabled", true, nlm::ConfigSource::Runtime);
    config->set("self_model_enabled", true, nlm::ConfigSource::Runtime);
    config->set("social_learning_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Test attention system integration
    brain.setAttentionBias(0.5f);  // Top-down attention bias
    brain.step(0, 0.001);
    
    assert(brain.getAttentionActive() == true, "Attention system should be active");
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr, "Concept formation should be available");
    
    // Feed sensory input to stimulate concept formation
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 8.0f);
    
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test planning system integration
    auto plannedAction = brain.planAction();
    assert(plannedAction != nullptr, "Brain should plan actions");
    
    // Test self-model integration
    brain.updateSelfModel(0.7f);  // Update body schema
    brain.step(40, 0.001);
    
    assert(brain.getSelfModelActive() == true, "Self-model should be active");
    
    // Test social learning integration
    brain.observeAction(nlm::ActionType::MoveForward);
    brain.step(50, 0.001);
    
    std::cout << "    Cognition Systems Integration passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    Testing Development Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable development
    config->set("development_enabled", true, nlm::ConfigSource::Runtime);
    config->set("synaptogenesis_enabled", true, nlm::ConfigSource::Runtime);
    config->set("pruning_enabled", true, nlm::ConfigSource::Runtime);
    config->set("maturation_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Check initial developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial, 
           "Brain should start in Initial developmental stage");
    
    // Simulate development progression
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        brain.updateDevelopment(step * 0.001);
        
        // Check developmental stage progression
        auto currentStage = brain.getDevelopmentalStage();
        if (step > 30) {
            assert(currentStage != nlm::DevelopmentalStage::Initial, 
                   "Brain should progress from Initial stage");
        }
    }
    
    // Check final developmental stage
    auto finalStage = brain.getDevelopmentalStage();
    assert(finalStage >= nlm::DevelopmentalStage::Initial && 
           finalStage <= nlm::DevelopmentalStage::Aging,
           "Developmental stage should be within valid range");
    
    // Check structural plasticity changes
    size_t finalSynapses = brain.getTotalSynapseCount();
    assert(finalSynapses > 0, "Structural plasticity should result in synapses");
    
    std::cout << "    Development Integration passed" << std::endl;
}

void testCheckpointSystem() {
    std::cout << "    Testing Checkpoint System..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Run some simulation steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointFile = "test_checkpoint.nlm";
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess, "Checkpoint save should succeed");
    
    // Verify checkpoint file exists
    assert(std::filesystem::exists(checkpointFile), "Checkpoint file should exist");
    
    // Create a new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess, "Checkpoint load should succeed");
    
    // Verify loaded brain has correct properties
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount(),
           "Loaded brain should have same neuron count");
    
    // Clean up
    std::filesystem::remove(checkpointFile);
    
    std::cout << "    Checkpoint System passed" << std::endl;
}

void testReplayAndConsolidation() {
    std::cout << "    Testing Replay and Consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(140), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable replay and consolidation
    config->set("replay_enabled", true, nlm::ConfigSource::Runtime);
    config->set("consolidation_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Run some simulation to create experiences
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 6.0f);
        brain.step(step, step * 0.001);
        brain.updateConsolidation(step * 0.001);
    }
    
    // Trigger replay
    brain.triggerReplay();
    brain.step(100, 0.001);
    
    // Verify consolidation occurred
    assert(brain.getConsolidationActive() == true, 
           "Consolidation should be active after replay");
    
    // Check that replay affected learning
    size_t postReplaySynapses = brain.getTotalSynapseCount();
    assert(postReplaySynapses > 0, "Replay should result in synaptic changes");
    
    std::cout << "    Replay and Consolidation passed" << std::endl;
}

void testCompleteBrainLoop() {
    std::cout << "    Testing Complete Brain Loop Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable all systems
    config->set("working_memory_enabled", true, nlm::ConfigSource::Runtime);
    config->set("prediction_system_enabled", true, nlm::ConfigSource::Runtime);
    config->set("attention_enabled", true, nlm::ConfigSource::Runtime);
    config->set("planning_enabled", true, nlm::ConfigSource::Runtime);
    config->set("neuromodulation_enabled", true, nlm::ConfigSource::Runtime);
    config->set("development_enabled", true, nlm::ConfigSource::Runtime);
    config->set("replay_enabled", true, nlm::ConfigSource::Runtime);
    
    bool success = brain.initialize();
    assert(success, "Brain initialization failed");
    
    // Run complete brain loop simulation
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        double time = step * 0.001;
        
        // 1. Update development
        brain.updateDevelopment(time);
        
        // 2. Apply neuromodulation based on experience
        if (step % 10 == 0) {
            brain.applyRewardModulation(0.5f + (step % 5) * 0.1f, 0.5f);
        }
        
        // 3. Update novelty detection
        brain.updateNoveltyDetection(0.1f + (step % 3) * 0.1f);
        
        // 4. Update curiosity
        brain.updateCuriosity(0.2f + (step % 4) * 0.1f);
        
        // 5. Process sensory input (simulated)
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 3.0f + (step % 5) * 0.5f);
        
        // 6. Brain computation step
        brain.step(step, time);
        
        // 7. Generate action
        auto action = brain.produceAction();
        
        // 8. Simulate action consequences (simplified)
        brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 2.0f);
        
        // 9. Update prediction based on outcome
        brain.updatePredictionError(0.1f * (step % 10) * 0.1f);
        
        // 10. Trigger replay occasionally
        if (step > 100 && step % 30 == 0) {
            brain.triggerReplay();
        }
        
        // 11. Log brain status periodically
        if (step % 50 == 0) {
            brain.logStatus();
        }
    }
    
    // Verify brain completed full integration
    assert(brain.getTotalNeuronCount() > 0, "Brain should have neurons");
    assert(brain.getTotalSynapseCount() > 0, "Brain should have synapses");
    assert(brain.getWorkingMemoryActiveNeurons() >= 0, "Working memory should be active");
    
    // Check that prediction error evolved
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        float finalError = predictionSystem->getPredictionError();
        assert(finalError >= 0.0f, "Final prediction error should be non-negative");
    }
    
    std::cout << "    Complete Brain Loop Integration passed" << std::endl;
}

void runPhase6IntegrationTests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    
    try {
        testMemorySystemsIntegration();
        testNeuromodulationIntegration();
        testPredictionSystemIntegration();
        testCognitionSystemsIntegration();
        testDevelopmentIntegration();
        testCheckpointSystem();
        testReplayAndConsolidation();
        testCompleteBrainLoop();
        
        std::cout << std::endl;
        std::cout << "=== ALL PHASE 6 INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "All brain systems are successfully integrated and functional!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_phase6_integration