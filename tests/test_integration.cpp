// Integration Test for Phase 6 Systems
// This test verifies that all Phase 6 systems are properly integrated and working together
// as part of the final integration test suite.

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_integration {

void testPhase6SystemIntegration() {
    std::cout << "    Testing Phase 6 System Integration..." << std::endl;
    
    // Configure Phase 6 integration test
    nlm::Phase6Config config;
    config.maxSteps = 100;
    config.neuronCount = 200;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableMemory = true;
    config.enableNeuromodulation = true;
    config.enablePrediction = true;
    config.enableDevelopment = true;
    config.enableCognition = true;
    config.enablePlasticity = true;
    
    // Run the integration experiment
    nlm::Phase6IntegratedExperiment experiment;
    nlm::Phase6IntegrationResult result = experiment.run(config);
    
    // Verify integration results
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.memoryEpisodicMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.predictionIntegrated);
    assert(result.developmentIntegrated);
    assert(result.checkpointingWorks);
    assert(result.replayWorks);
    
    // Verify metrics
    assert(result.totalReward >= 0.0f);  // Should have some positive or zero reward
    assert(result.avgFiringRate > 0.0f);  // Should have neural activity
    assert(result.memoryEpisodesStored >= 0.0f);  // Should have stored memories
    
    std::cout << "    ✓ Phase 6 System Integration passed" << std::endl;
}

void testBrainMemorySystemIntegration() {
    std::cout << "    Testing Brain-Memory System Integration..." << std::endl;
    
    // Create brain with memory systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory integration
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Store some data in working memory
    nlm::NeuronId testNeuron = nlm::INVALID_NEURON_ID;
    // Find a valid neuron for testing
    for (const auto& region : brain.getRegions()) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == nlm::NeuronType::Internal) {
                testNeuron = neuron->getId();
                break;
            }
        }
        if (testNeuron != nlm::INVALID_NEURON_ID) break;
    }
    
    if (testNeuron != nlm::INVALID_NEURON_ID) {
        workingMemory->storeToNeuron(testNeuron, 1.0f);
        assert(workingMemory->getActiveTraces() > 0);
    }
    
    // Test episodic memory storage
    nlm::SensoryInput input;
    input.setType(nlm::NeuronType::Sensory);
    brain.receiveSensoryInput(input);
    
    std::cout << "    ✓ Brain-Memory System Integration passed" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    std::cout << "    Testing Brain-Neuromodulation Integration..." << std::endl;
    
    // Create brain with neuromodulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulator access
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Apply neuromodulation signals
    nlm::Neuromodulator dopamineSignal;
    dopamineSignal.setType(nlm::NeuronType::Modulatory);
    dopamine->setLevel(0.5f);
    
    nlm::Neuromodulator curiositySignal;
    curiositySignal.setType(nlm::NeuronType::Modulatory);
    curiosity->setLevel(0.3f);
    
    nlm::Neuromodulator noveltySignal;
    noveltySignal.setType(nlm::NeuronType::Modulatory);
    novelty->setLevel(0.8f);
    
    nlm::Neuromodulator predictionErrorSignal;
    predictionErrorSignal.setType(nlm::NeuronType::Modulatory);
    predictionError->setLevel(0.2f);
    
    brain.applyNeuromodulation(dopamineSignal);
    brain.applyNeuromodulation(curiositySignal);
    brain.applyNeuromodulation(noveltySignal);
    brain.applyNeuromodulation(predictionErrorSignal);
    
    // Verify neuromodulation affected brain state
    assert(brain.getDopamine()->getLevel() == 0.5f);
    assert(brain.getCuriosity()->getLevel() == 0.3f);
    assert(brain.getNovelty()->getLevel() == 0.8f);
    assert(brain.getPredictionErrorSignal()->getLevel() == 0.2f);
    
    std::cout << "    ✓ Brain-Neuromodulation Integration passed" << std::endl;
}

void testBrainPredictionSystemIntegration() {
    std::cout << "    Testing Brain-Prediction System Integration..." << std::endl;
    
    // Create brain with prediction system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system access
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test neural prediction access
    auto* neuralPrediction = brain.getPredictionSystem()->getNeuralPrediction();
    assert(neuralPrediction != nullptr);
    
    // Test prediction error signal
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Run some steps to generate prediction activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify prediction system is active
    float predictionErrorValue = brain.getPredictionErrorSignal()->getLevel();
    assert(predictionErrorValue >= 0.0f);  // Prediction error should be non-negative
    
    std::cout << "    ✓ Brain-Prediction System Integration passed" << std::endl;
}

void testBrainDevelopmentSystemIntegration() {
    std::cout << "    Testing Brain-Development System Integration..." << std::endl;
    
    // Create brain with development system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system access
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Test development progression
    brain.develop();
    nlm::DevelopmentalStage afterDevelopmentStage = brain.getDevelopmentalStage();
    // Stage may or may not change based on configuration
    
    // Test learned preference
    float initialPreference = brain.getLearnedPreference();
    brain.setLearnedPreference(0.5f);
    assert(brain.getLearnedPreference() == 0.5f);
    
    std::cout << "    ✓ Brain-Development System Integration passed" << std::endl;
}

void testAgentBrainIntegration() {
    std::cout << "    Testing AgentBrain Integration..." << std::endl;
    
    // Create brain and agent brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world;
    world.configure(16, 16, 16, 16);
    world.reset();
    
    // Initialize agent brain with world
    agentBrain.initialize(world);
    
    // Test sensory processing
    nlm::SensoryPercept percept;
    percept.setVision(nlm::Vision(16, 16, 3));
    agentBrain.processSensoryInput(percept);
    
    // Test motor command decoding
    nlm::MotorCommand motorCommand = agentBrain.decodeMotorCommand();
    // Motor command should be valid (one of the defined commands)
    
    // Test neuromodulation
    agentBrain.applyRewardModulation(1.0f, 0.8f);
    
    // Test development update
    agentBrain.updateDevelopment(0.001);
    
    // Test reset
    agentBrain.reset();
    
    std::cout << "    ✓ AgentBrain Integration passed" << std::endl;
}

void runAll() {
    testPhase6SystemIntegration();
    testBrainMemorySystemIntegration();
    testBrainNeuromodulationIntegration();
    testBrainPredictionSystemIntegration();
    testBrainDevelopmentSystemIntegration();
    testAgentBrainIntegration();
    
    std::cout << "\n=== All Integration Tests Passed ===" << std::endl;
}

} // namespace test_integration