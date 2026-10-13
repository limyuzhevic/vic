// Python Bindings Tests
// Phase 6: Comprehensive test of Python bindings functionality

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>

namespace test_python_bindings {

void test_config() {
    std::cout << "Testing Config..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic operations
    config->set("test_int", 42);
    config->set("test_double", 3.14);
    config->set("test_string", "hello");
    config->set("test_bool", true);
    
    // Test get operations
    assert(config->get<int>("test_int").value() == 42);
    assert(std::abs(config->get<double>("test_double").value() - 3.14) < 0.001);
    assert(config->get<std::string>("test_string").value() == "hello");
    assert(config->get<bool>("test_bool").value() == true);
    
    // Test getOr with defaults
    assert(config->getOr<int>("missing", 99) == 99);
    
    // Test has
    assert(config->has("test_int"));
    assert(!config->has("nonexistent"));
    
    // Test getKeys
    auto keys = config->getKeys();
    assert(keys.size() >= 4);
    
    std::cout << "  Config tests passed!" << std::endl;
}

void test_neuron_types() {
    std::cout << "Testing Neuron Types..." << std::endl;
    
    // Test NeuronType enum
    nlm::NeuronId id1;
    nlm::NeuronId id2(42);
    assert(id1.value == 0);
    assert(id2.value == 42);
    assert(id1.index() == 0);
    assert(id2.index() == 42);
    
    // Test SynapseId
    nlm::SynapseId synId1;
    nlm::SynapseId synId2(100);
    assert(synId2.value == 100);
    
    // Test RegionId
    nlm::RegionId regionId1;
    nlm::RegionId regionId2(5);
    assert(regionId2.value == 5);
    
    // Test PopulationId
    nlm::PopulationId popId1;
    nlm::PopulationId popId2(3);
    assert(popId2.value == 3);
    
    std::cout << "  Neuron type tests passed!" << std::endl;
}

void test_enums() {
    std::cout << "Testing Enums..." << std::endl;
    
    // Test NeuronType
    assert(static_cast<int>(nlm::NeuronType::Excitatory) == 0);
    assert(static_cast<int>(nlm::NeuronType::Inhibitory) == 1);
    assert(static_cast<int>(nlm::NeuronType::Modulatory) == 2);
    assert(static_cast<int>(nlm::NeuronType::Sensory) == 3);
    assert(static_cast<int>(nlm::NeuronType::Motor) == 4);
    assert(static_cast<int>(nlm::NeuronType::Internal) == 5);
    
    // Test SynapseType
    assert(static_cast<int>(nlm::SynapseType::Excitatory) == 0);
    assert(static_cast<int>(nlm::SynapseType::Inhibitory) == 1);
    assert(static_cast<int>(nlm::SynapseType::Modulatory) == 2);
    assert(static_cast<int>(nlm::SynapseType::Electrical) == 3);
    assert(static_cast<int>(nlm::SynapseType::GapJunction) == 4);
    
    // Test DevelopmentalStage
    assert(static_cast<int>(nlm::DevelopmentalStage::Initial) == 0);
    assert(static_cast<int>(nlm::DevelopmentalStage::CriticalPeriod) == 1);
    assert(static_cast<int>(nlm::DevelopmentalStage::Maturation) == 2);
    assert(static_cast<int>(nlm::DevelopmentalStage::Adult) == 3);
    assert(static_cast<int>(nlm::DevelopmentalStage::Aging) == 4);
    
    // Test FiringState
    assert(static_cast<int>(nlm::FiringState::Resting) == 0);
    assert(static_cast<int>(nlm::FiringState::Active) == 1);
    assert(static_cast<int>(nlm::FiringState::Refractory) == 2);
    assert(static_cast<int>(nlm::FiringState::Inhibited) == 3);
    
    // Test ActionType
    assert(static_cast<int>(nlm::ActionType::MoveForward) == 0);
    assert(static_cast<int>(nlm::ActionType::MoveBackward) == 1);
    assert(static_cast<int>(nlm::ActionType::MoveLeft) == 2);
    assert(static_cast<int>(nlm::ActionType::MoveRight) == 3);
    assert(static_cast<int>(nlm::ActionType::TurnLeft) == 4);
    assert(static_cast<int>(nlm::ActionType::TurnRight) == 5);
    
    // Test MotorCommand
    assert(static_cast<int>(nlm::MotorCommand::MoveForward) == 0);
    assert(static_cast<int>(nlm::MotorCommand::MoveBackward) == 1);
    assert(static_cast<int>(nlm::MotorCommand::TurnLeft) == 2);
    assert(static_cast<int>(nlm::MotorCommand::TurnRight) == 3);
    
    // Test WorldObjectType
    assert(static_cast<int>(nlm::WorldObjectType::Empty) == 0);
    assert(static_cast<int>(nlm::WorldObjectType::Resource) == 1);
    assert(static_cast<int>(nlm::WorldObjectType::Hazard) == 2);
    assert(static_cast<int>(nlm::WorldObjectType::Wall) == 3);
    assert(static_cast<int>(nlm::WorldObjectType::Marker) == 4);
    
    std::cout << "  Enum tests passed!" << std::endl;
}

void test_action() {
    std::cout << "Testing Action..." << std::endl;
    
    // Create action with type
    nlm::Action action1(nlm::ActionType::MoveForward);
    assert(action1.getType() == nlm::ActionType::MoveForward);
    
    // Create action with parameters
    std::vector<float> params = {0.1, 0.2, 0.3, 0.4};
    nlm::Action action2(nlm::ActionType::Custom, params);
    assert(action2.getType() == nlm::ActionType::Custom);
    assert(action2.getParameters() == params);
    
    // Test action name
    assert(action1.getName() == "MoveForward");
    assert(action2.getName() == "Custom");
    
    // Test cloning
    nlm::Action action3 = action1.clone();
    assert(action3.getType() == action1.getType());
    assert(action3.getName() == action1.getName());
    
    std::cout << "  Action tests passed!" << std::endl;
}

void test_world_object() {
    std::cout << "Testing WorldObject..." << std::endl;
    
    // Create world object
    nlm::WorldObject obj1(nlm::WorldObjectType::Resource);
    assert(obj1.type == nlm::WorldObjectType::Resource);
    assert(obj1.value == 0.0);
    assert(obj1.radius == 0.5);
    
    // Create object with parameters
    nlm::WorldObject obj2(5.0, 10.0, nlm::WorldObjectType::Wall, 2.0, 1.0);
    assert(obj2.x == 5.0);
    assert(obj2.y == 10.0);
    assert(obj2.type == nlm::WorldObjectType::Wall);
    assert(obj2.value == 2.0);
    assert(obj2.radius == 1.0);
    
    // Test properties
    obj2.active = false;
    assert(!obj2.active);
    
    std::cout << "  WorldObject tests passed!" << std::endl;
}

void test_agent_body() {
    std::cout << "Testing AgentBody..." << std::endl;
    
    nlm::AgentBody body;
    
    // Test default values
    assert(body.x == 0.0);
    assert(body.y == 0.0);
    assert(body.orientation == 0.0);
    assert(body.velocityX == 0.0);
    assert(body.velocityY == 0.0);
    assert(body.energy == 1.0);
    assert(body.health == 1.0);
    assert(body.age == 0.0);
    assert(body.isMoving == false);
    assert(body.isTurning == false);
    
    // Test setting values
    body.x = 10.0;
    body.y = 5.0;
    body.orientation = 90.0;
    body.velocityX = 0.5;
    body.velocityY = 0.2;
    body.angularVelocity = 0.1;
    body.energy = 0.8;
    body.health = 0.9;
    body.age = 10.0;
    body.isMoving = true;
    body.isTurning = true;
    
    assert(body.x == 10.0);
    assert(body.y == 5.0);
    assert(body.orientation == 90.0);
    assert(body.velocityX == 0.5);
    assert(body.velocityY == 0.2);
    assert(body.angularVelocity == 0.1);
    assert(body.energy == 0.8);
    assert(body.health == 0.9);
    assert(body.age == 10.0);
    assert(body.isMoving == true);
    assert(body.isTurning == true);
    
    // Test reset
    body.reset();
    assert(body.x == 0.0);
    assert(body.y == 0.0);
    assert(body.orientation == 0.0);
    assert(body.velocityX == 0.0);
    assert(body.velocityY == 0.0);
    assert(body.angularVelocity == 0.0);
    assert(body.energy == 1.0);
    assert(body.health == 1.0);
    assert(body.age == 0.0);
    
    std::cout << "  AgentBody tests passed!" << std::endl;
}

void test_sensory_percept() {
    std::cout << "Testing SensoryPercept..." << std::endl;
    
    nlm::SensoryPercept percept;
    
    // Test default values
    assert(percept.getVisionWidth() == 0);
    assert(percept.getVisionHeight() == 0);
    assert(percept.getTouch().empty());
    assert(percept.getInternal().empty());
    assert(percept.getProprioception().empty());
    assert(percept.getAudio().empty());
    
    // Test setting values
    std::vector<float> vision = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    percept.setVision(vision);
    assert(percept.getVision() == vision);
    assert(percept.getVisionWidth() == 9);
    assert(percept.getVisionHeight() == 1);
    
    std::vector<float> touch = {1.0, 0.0, 1.0};
    percept.setTouch(touch);
    assert(percept.getTouch() == touch);
    
    std::vector<float> internal = {0.5, -0.5};
    percept.setInternal(internal);
    assert(percept.getInternal() == internal);
    
    std::vector<float> proprioception = {1.0, -1.0, 0.0};
    percept.setProprioception(proprioception);
    assert(percept.getProprioception() == proprioception);
    
    std::vector<float> audio = {0.8, 0.9, 1.0};
    percept.setAudio(audio);
    assert(percept.getAudio() == audio);
    
    // Test timestamp
    percept.setTimestamp(12345.0);
    assert(percept.getTimestamp() == 12345.0);
    
    std::cout << "  SensoryPercept tests passed!" << std::endl;
}

void test_simple_world() {
    std::cout << "Testing SimpleWorld..." << std::endl;
    
    nlm::SimpleWorld world;
    
    // Test default values
    assert(world.getWidth() == 0);
    assert(world.getHeight() == 0);
    assert(world.getVisionWidth() == 0);
    assert(world.getVisionHeight() == 0);
    
    // Configure world
    world.configure(20, 20, 16, 16);
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    assert(world.getVisionWidth() == 16);
    assert(world.getVisionHeight() == 16);
    
    // Test reset
    world.reset();
    assert(world.getSimulationTime() == 0.0);
    
    // Set agent start
    world.setAgentStart(10.0, 5.0);
    
    // Test energy settings
    world.setMaxEnergy(2.0);
    assert(world.getMaxEnergy() == 2.0);
    
    world.setEnergyDecayRate(0.1);
    assert(world.getEnergyDecayRate() == 0.1);
    
    // Test random seed
    world.setRandomSeed(42);
    assert(world.getRandomSeed() == 42);
    
    // Test add/remove objects
    nlm::WorldObject obj(1.0, 2.0, nlm::WorldObjectType::Resource);
    world.addObject(obj);
    // Note: We don't have direct way to verify object was added without accessing internal state
    
    world.removeObject(1.0, 2.0);
    
    std::cout << "  SimpleWorld tests passed!" << std::endl;
}

void test_factory_functions() {
    std::cout << "Testing Factory Functions..." << std::endl;
    
    // Test createDefaultConfig
    auto config = nlm::createDefaultConfig();
    assert(config != nullptr);
    assert(config.use_count() > 0);
    
    // Test createBrain
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    assert(brain.use_count() > 0);
    
    // Test createSimpleWorld
    auto world = nlm::createSimpleWorld();
    assert(world != nullptr);
    assert(world.use_count() > 0);
    
    // Test createAgentBrain
    auto agent_brain = nlm::createAgentBrain(brain);
    assert(agent_brain != nullptr);
    assert(agent_brain.use_count() > 0);
    
    std::cout << "  Factory function tests passed!" << std::endl;
}

void runAll() {
    std::cout << "=== Python Bindings Tests ===" << std::endl;
    
    try {
        test_config();
        test_neuron_types();
        test_enums();
        test_action();
        test_world_object();
        test_agent_body();
        test_sensory_percept();
        test_simple_world();
        test_factory_functions();
        
        std::cout << std::endl;
        std::cout << "=== All Python Bindings Tests PASSED ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Python Bindings Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_python_bindings
