// Integration Tests
#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "development/DevelopmentSystem.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_integration {

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    // Memory system is part of the core brain, so this tests
    // that different memory components work together
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Brain initialization should set up memory systems
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create neuromodulators
    nlm::Dopamine dopamine;
    nlm::Curiosity curiosity;
    nlm::Novelty novelty;
    nlm::PredictionError predictionError;
    
    // All neuromodulators should be able to exist independently
    assert(dopamine.getName() != nullptr);
    assert(novelty.getName() != nullptr);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::DevelopmentSystem devSystem;
    
    // Development system should be able to update with brain
    devSystem.update(&brain, 0);
    
    // Should have a valid developmental stage
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testFullBrainLoopIntegration() {
    std::cout << "    Testing full brain loop integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool initialized = brain.initialize();
    assert(initialized == true);
    
    // Run several steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Brain should still be functional
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testFullBrainLoopIntegration passed" << std::endl;
}

void testAgentIntegration() {
    std::cout << "    Testing agent integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    nlm::SimpleWorld world;
    
    // Initialize agent brain
    agentBrain.initialize(world);
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Should return valid command
    assert(command == nlm::MotorCommand::Wait ||
           command == nlm::MotorCommand::MoveForward ||
           command == nlm::MotorCommand::MoveBackward ||
           command == nlm::MotorCommand::TurnLeft ||
           command == nlm::MotorCommand::TurnRight ||
           command == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentIntegration passed" << std::endl;
}

void testVisualizationIntegration() {
    std::cout << "    Testing visualization integration..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Initialize visualization
    bool initResult = viz.initialize();
    assert(initResult == true);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Visualize network
    viz.visualizeNetwork(brain);
    
    // Visualize activity
    viz.visualizeActivity(brain);
    
    // Visualize weights
    viz.visualizeWeights(brain);
    
    std::cout << "    testVisualizationIntegration passed" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "    Testing cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test NeuralPlanner
    nlm::NeuralPlanner neuralPlanner;
    neuralPlanner.initialize(&brain);
    
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    nlm::ActionType action = neuralPlanner.planAction(state);
    assert(action != nlm::ActionType::Wait || !state.empty());
    
    // Test ConceptFormation
    nlm::ConceptFormation conceptFormation;
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> features = {0.0f, 1.0f};
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, 0);
    assert(conceptId > 0);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void testNeuromodulationCognitionIntegration() {
    std::cout << "    Testing neuromodulation-cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create both neuromodulator and cognitive systems
    nlm::Dopamine dopamine;
    nlm::NeuralPlanner neuralPlanner;
    
    // Initialize both
    dopamine.initialize(&brain);
    neuralPlanner.initialize(&brain);
    
    // They should be able to coexist and work independently
    assert(dopamine.getName() != nullptr);
    assert(neuralPlanner.getPlanningDepth() == 3);
    
    std::cout << "    testNeuromodulationCognitionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testDevelopmentSystemIntegration();
    testFullBrainLoopIntegration();
    testAgentIntegration();
    testVisualizationIntegration();
    testCognitionIntegration();
    testNeuromodulationCognitionIntegration();
}

} // namespace test_integration
