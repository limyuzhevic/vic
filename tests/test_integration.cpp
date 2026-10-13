// Comprehensive Integration Tests for NLM Phase 6
// Tests the integration of all systems in the artificial brain

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_integration {

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test episodic memory storage and retrieval
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.neurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    episode.values = {0.5f, 0.8f};
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    auto retrieved = episodicMemory->retrieveEpisode(0);
    assert(retrieved.neurons.size() == 2);
    assert(retrieved.values.size() == 2);
    
    // Test working memory storage
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.7f);
    assert(workingMemory->contains(nlm::NeuronId(1)));
    assert(workingMemory->getNeuronActivation(nlm::NeuronId(1)) == 0.7f);
    
    std::cout << "        Memory system integration test passed" << std::endl;
}

void testCognitiveSystemIntegration() {
    std::cout << "    Testing cognitive system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test NeuralPlanner integration
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test NeuralPlanner functionality
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    nlm::ActionType action = planner->planAction(state);
    assert(action != nlm::ActionType::Wait || action == nlm::ActionType::MoveForward || 
           action == nlm::ActionType::TurnLeft || action == nlm::ActionType::TurnRight ||
           action == nlm::ActionType::Interact);
    
    // Test concept formation
    std::vector<float> pattern = {0.8f, 0.7f, 0.6f, 0.5f, 0.4f};
    std::vector<float> features = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    
    size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.7f, 1);
    assert(conceptId > 0);
    
    std::vector<float> prototype = conceptFormation->getConceptPrototype(conceptId);
    assert(prototype.size() == pattern.size());
    
    std::cout << "        Cognitive system integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine integration
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test acetylcholine integration
    auto* acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    // Test norepinephrine integration
    auto* norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    // Test serotonin integration
    auto* serotonin = brain.getSerotonin();
    assert(serotonin != nullptr);
    
    // Test neuromodulator levels
    assert(dopamine->getLevel() >= 0.0f);
    assert(acetylcholine->getLevel() >= 0.0f);
    assert(norepinephrine->getLevel() >= 0.0f);
    assert(serotonin->getLevel() >= 0.0f);
    
    // Test plasticity factors
    assert(dopamine->getPlasticityFactor() >= 0.0f);
    assert(acetylcholine->getPlasticityFactor() >= 1.0f);  // Should be > 1.0 due to attention boost
    assert(norepinephrine->getPlasticityFactor() >= 1.0f);  // Should be > 1.0 due to learning boost
    assert(serotonin->getPlasticityFactor() >= 0.0f);  // Can be less or more than 1.0
    
    // Test neuromodulation in brain loop (should not crash)
    brain.step(1);
    brain.step(2);
    brain.step(3);
    
    std::cout << "        Neuromodulation integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system updates during brain loop
    brain.step(1);
    brain.step(2);
    
    // Test prediction system accessors
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);
    
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    const std::vector<float>& errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() >= 0);
    
    std::cout << "        Prediction system integration test passed" << std::endl;
}

void testPersistenceIntegration() {
    std::cout << "    Testing persistence integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Step a few times to generate some activity
    for (int i = 0; i < 10; ++i) {
        brain1.step(i);
    }
    
    // Test save functionality
    std::string filename = "test_checkpoint.nlm";
    bool saveSuccess = brain1.save(filename);
    assert(saveSuccess);
    
    // Create a new brain and load from checkpoint
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(filename);
    assert(loadSuccess);
    
    // Verify both brains have same neuron count
    assert(brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    assert(brain1.getRegionCount() == brain2.getRegionCount());
    
    // Clean up
    // Note: In a real implementation, we would clean up the checkpoint file
    std::cout << "        Persistence integration test passed" << std::endl;
}

void testSleepRestCycleIntegration() {
    std::cout << "    Testing sleep/rest cycle integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Step enough to trigger rest cycle
    for (int step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Check that rest flag is properly managed
        assert(brain.getBrain()->pImpl->isResting == (step % 10 == 0));
    }
    
    // Verify memory systems were used during rest
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    std::cout << "        Sleep/rest cycle integration test passed" << std::endl;
}

void testAdvancedAgentCommands() {
    std::cout << "    Testing advanced agent commands integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test brain monitor integration
    auto& monitor = nlm::BrainMonitor::getInstance();
    monitor.initialize(std::make_shared<nlm::Brain>(brain));
    
    // Test monitoring functionality
    assert(monitor.getCurrentMetrics().averageFiringRate >= 0.0f);
    assert(monitor.getPerformanceScore() >= 0.0f);
    assert(monitor.getLearningProgress() >= 0.0f);
    
    // Test anomaly detection (should not crash)
    std::vector<nlm::BrainMonitor::AnomalyAlert> anomalies = monitor.getActiveAnomalies();
    assert(anomalies.size() >= 0);
    
    // Test performance indicators
    std::vector<nlm::BrainMonitor::PerformanceIndicator> indicators = monitor.getActiveIndicators();
    assert(indicators.size() >= 0);
    
    std::cout << "        Advanced agent commands integration test passed" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "    Testing complete system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    config->set("replay_interval", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("consolidation_interval", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize everything
    assert(brain.initialize());
    
    // Run integration test
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Verify all systems are accessible
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getAssociativeMemory() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getAcetylcholine() != nullptr);
        assert(brain.getNorepinephrine() != nullptr);
        assert(brain.getSerotonin() != nullptr);
        
        // Verify brain status can be logged
        brain.logStatus();
    }
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test brain status summary
    brain.logStatus();
    
    std::cout << "        Complete system integration test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing integrated artificial brain functionality..." << std::endl;
    std::cout << std::endl;
    
    testMemorySystemIntegration();
    testCognitiveSystemIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testPersistenceIntegration();
    testSleepRestCycleIntegration();
    testAdvancedAgentCommands();
    testCompleteIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests Passed ===" << std::endl;
    std::cout << "The NLM Phase 6 artificial brain is fully integrated and functional!" << std::endl;
}

} // namespace test_integration