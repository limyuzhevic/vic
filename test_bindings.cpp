#include <pybind11/pybind11.h>
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Touch.hpp"
#include "../src/sensory/Proprioception.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include <iostream>
#include <vector>
#include <memory>

namespace py = pybind11;

void test_basic_functionality() {
    std::cout << "Testing basic Python bindings functionality..." << std::endl;
    
    // Test NeuronId operations
    nlm::NeuronId id1(123);
    nlm::NeuronId id2(456);
    assert(id1 != id2);
    assert(id1.index() == 123);
    
    // Test enum values
    assert(static_cast<int>(nlm::NeuronType::Excitatory) == 0);
    assert(static_cast<int>(nlm::ActionType::MoveForward) == 0);
    assert(static_cast<int>(nlm::DevelopmentalStage::Adult) == 3);
    
    // Test config operations
    auto config = nlm::createDefaultConfig();
    config->setConfigValue("test_key", "test_value");
    assert(config->has("test_key"));
    assert(config->getConfigValue("test_key") == "test_value");
    
    // Test world creation
    auto world = nlm::createSimpleWorld();
    assert(world->getWidth() == 100);
    assert(world->getHeight() == 100);
    
    // Test agent brain creation
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    assert(brain->initialize());
    
    auto agentBrain = nlm::createAgentBrain(brain);
    assert(agentBrain != nullptr);
    
    // Test world configuration
    world->configure(200, 200, 80, 80);
    assert(world->getWidth() == 200);
    assert(world->getHeight() == 200);
    
    // Test agent body
    auto body = world->getAgentBody();
    assert(body != nullptr);
    assert(body->x == 0.0f);
    assert(body->y == 0.0f);
    
    // Test world object management
    nlm::WorldObject obj;
    obj.x = 50.0f;
    obj.y = 50.0f;
    obj.type = nlm::WorldObjectType::Resource;
    world->addObject(obj);
    
    bool valid = world->isValidPosition(60.0f, 60.0f);
    assert(valid);
    
    // Test sensory inputs
    auto touch = std::make_unique<nlm::Touch>(5);
    assert(touch->getType() == std::string("Touch"));
    assert(touch->getNumSensors() == 5);
    
    auto proprioception = std::make_unique<nlm::Proprioception>();
    assert(proprioception->getType() == std::string("Proprioception"));
    assert(proprioception->getNumSensors() == 6);
    
    // Test setting proprioception data
    proprioception->setBodyState(10.0f, 20.0f, 0.5f, 0.1f, 0.2f, 0.05f);
    assert(proprioception->getX() == 10.0f);
    assert(proprioception->getY() == 20.0f);
    assert(proprioception->getOrientation() == 0.5f);
    assert(proprioception->getVelocityX() == 0.1f);
    assert(proprioception->getVelocityY() == 0.2f);
    assert(proprioception->getAngularVelocity() == 0.05f);
    
    // Test visualization interface
    auto vis = nlm::createVisualizationInterface();
    assert(vis != nullptr);
    assert(!vis->isActive());
    
    std::cout << "All basic tests passed!" << std::endl;
}

void test_advanced_features() {
    std::cout << "Testing advanced features..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Test memory systems access
    auto workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test neuromodulation systems
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    auto predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test development system
    auto devSystem = brain->getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test cognition systems
    auto planner = brain->getPlanner();
    assert(planner != nullptr);
    
    auto conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    auto attention = brain->getAttention();
    assert(attention != nullptr);
    
    // Test prediction system
    auto predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test experiment runner
    auto experimentRunner = nlm::createExperimentRunner();
    assert(experimentRunner != nullptr);
    
    auto experiment = experimentRunner->createExperiment("test_exp");
    assert(experiment != nullptr);
    assert(experiment->getName() == "test_exp");
    
    // Test developmental stages
    assert(devSystem->getStageName() == std::string("Initial"));
    devSystem->setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem->getStageName() == std::string("Adult"));
    devSystem->advanceStage();
    assert(devSystem->getStageName() == std::string("Aging"));
    
    // Test neuromodulation levels
    assert(curiosity->getLevel() == 0.0f);
    assert(novelty->getLevel() == 0.0f);
    assert(predictionError->getError() == 0.0f);
    
    std::cout << "All advanced tests passed!" << std::endl;
}

void test_pybind11_features() {
    std::cout << "Testing pybind11 integration features..." << std::endl;
    
    // Test factory functions
    auto config1 = nlm::createDefaultConfig();
    auto config2 = nlm::createDefaultConfig();
    assert(config1 != config2);
    
    auto brain1 = nlm::createBrain(config1);
    auto brain2 = nlm::createBrain(config2);
    assert(brain1 != brain2);
    
    auto world1 = nlm::createSimpleWorld();
    auto world2 = nlm::createSimpleWorld();
    assert(world1 != world2);
    
    auto agentBrain1 = nlm::createAgentBrain(brain1);
    auto agentBrain2 = nlm::createAgentBrain(brain2);
    assert(agentBrain1 != agentBrain2);
    
    // Test constants
    assert(nlm::INVALID_NEURON_ID.value == 0);
    assert(nlm::INVALID_SYNAPSE_ID.value == 0);
    assert(nlm::INVALID_REGION_ID.value == 0);
    assert(nlm::INVALID_POPULATION_ID.value == 0);
    
    // Test cloning
    auto touchClone = nlm::make_unique<nlm::Touch>(3);
    auto original = nlm::make_unique<nlm::Touch>(5);
    assert(original->getNumSensors() == 5);
    
    std::cout << "All pybind11 integration tests passed!" << std::endl;
}

PYBIND11_MODULE(test_bindings, m) {
    m.def("test_basic_functionality", &test_basic_functionality);
    m.def("test_advanced_features", &test_advanced_features);
    m.def("test_pybind11_features", &test_pybind11_features);
}
