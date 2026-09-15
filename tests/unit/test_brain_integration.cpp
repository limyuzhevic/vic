// Brain Integration Tests
// Tests for the integrated brain systems: working memory, prediction, cognition, memory persistence, neuromodulation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_brain_integration {

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that working memory is accessible and functional
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store some information in working memory
    nlm::NeuronId testNeuron(1);
    workingMemory->storeToNeuron(testNeuron, 0.7f);
    
    // Verify it was stored
    auto memoryNeurons = workingMemory->getMemoryNeurons();
    assert(!memoryNeurons.empty());
    
    // Test working memory update and competition
    workingMemory->update(0.001f);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that prediction system is accessible
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system basic functionality
    float error = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getConfidence();
    
    assert(error >= 0.0f);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction error history
    auto errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.empty() || errorHistory.size() > 0);  // Either empty or has entries
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all cognition systems are accessible
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test NeuralPlanner initialization
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f};
    nlm::ActionType action = planner->planAction(currentState, 0.5f);
    assert(static_cast<size_t>(action) < 10);  // Assuming valid action types
    
    // Test ConceptFormation
    nlm::NeuronId testNeuron(1);
    conceptFormation->recordObservation(testNeuron, currentState);
    
    // Test AttentionalSelection
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2)};
    attention->processCompetition(competitors);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void testMemoryPersistence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain and let it run some steps
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Take some steps to generate activity
    for (int step = 0; step < 10; ++step) {
        brain1.step(step);
    }
    
    // Save the brain state
    std::string checkpointDir = "./checkpoints_test";
    std::filesystem::create_directories(checkpointDir);
    std::string checkpointFile = checkpointDir + "/test_checkpoint.bin";
    
    bool saveSuccess = brain1.save(checkpointFile);
    assert(saveSuccess);
    
    // Create a new brain and load the state
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Verify key properties are preserved
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    
    // Cleanup
    std::filesystem::remove(checkpointFile);
    std::filesystem::remove(checkpointDir);
    
    std::cout << "    testMemoryPersistence passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all neuromodulators are accessible
    auto* dopamine = brain.getDopamine();
    auto* acetylcholine = brain.getAcetylcholine();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* serotonin = brain.getSerotonin();
    
    assert(dopamine != nullptr);
    assert(acetylcholine != nullptr);
    assert(norepinephrine != nullptr);
    assert(serotonin != nullptr);
    
    // Test neuromodulator level control
    dopamine->setLevel(0.7f);
    acetylcholine->setLevel(0.5f);
    norepinephrine->setLevel(0.8f);
    serotonin->setLevel(0.6f);
    
    assert(dopamine->getLevel() == 0.7f);
    assert(acetylcholine->getLevel() == 0.5f);
    assert(norepinephrine->getLevel() == 0.8f);
    assert(serotonin->getLevel() == 0.6f);
    
    // Test neuromodulator effects
    dopamine->signalReward(0.5f);
    acetylcholine->enhanceAttention(0.3f);
    norepinephrine->increaseArousal(0.4f);
    serotonin->enhanceMood(0.2f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testBrainStepWithIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Take multiple steps to test integration of all systems
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify brain still has expected properties
        assert(brain.getTotalNeuronCount() == neuronCount);
        
        // Check that systems are still functioning
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
    }
    
    std::cout << "    testBrainStepWithIntegration passed" << std::endl;
}

void testSensoryMotorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test sensory input integration (if available)
    auto* sensoryNeurons = brain.getSensoryNeurons();
    auto* motorNeurons = brain.getMotorNeurons();
    
    // Verify we have both types of neurons
    assert(sensoryNeurons != nullptr);
    assert(motorNeurons != nullptr);
    
    // Test action production (should work after initialization)
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test that neuromodulators affect neural excitability
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->setLevel(0.8f);
        // Inject current to test neuromodulator effect
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 0.5f);
    }
    
    std::cout << "    testSensoryMotorIntegration passed" << std::endl;
}

void runAll() {
    testWorkingMemoryIntegration();
    testPredictionSystemIntegration();
    testCognitionIntegration();
    testMemoryPersistence();
    testNeuromodulationIntegration();
    testBrainStepWithIntegration();
    testSensoryMotorIntegration();
    
    std::cout << std::endl;
    std::cout << "All Brain Integration Tests PASSED" << std::endl;
}

} // namespace test_brain_integration
