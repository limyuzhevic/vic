// Test program to verify NLM C++ API
// This is meant to be compiled and run to test that all the expected
// methods exist in the NLM classes before they are exposed to Python.

#include <iostream>
#include <cassert>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/motor/Action.hpp"

namespace test_api {

void testConfig() {
    std::cout << "Testing Config API..." << std::endl;
    
    nlm::Config config;
    
    // Test initialization
    assert(!config.has("test_key"));
    
    // Test setting different types
    config.set("int_val", 42);
    config.set("double_val", 3.14);
    config.set("string_val", std::string("hello"));
    config.set("bool_val", true);
    
    // Test getting values
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test has
    assert(config.has("string_val"));
    assert(!config.has("nonexistent"));
    
    // Test getKeys
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    // Test getOr
    int defaultVal = config.getOr<int>("nonexistent", 100);
    assert(defaultVal == 100);
    
    std::cout << "  ✓ Config API works correctly" << std::endl;
}

void testBrain() {
    std::cout << "Testing Brain API..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    // Test initialization
    assert(brain.initialize());
    
    // Test stepping with step only
    brain.step(0);
    brain.step(1);
    
    // Test stepping with step and time
    brain.step(2, 0.1);
    brain.step(3, 0.2);
    
    // Test statistics
    auto totalNeurons = brain.getTotalNeuronCount();
    assert(totalNeurons > 0);
    
    auto totalSynapses = brain.getTotalSynapseCount();
    assert(totalSynapses >= 0);
    
    auto firingNeurons = brain.getFiringNeuronCount();
    assert(firingNeurons >= 0);
    
    auto totalSpikes = brain.getTotalSpikeCount();
    assert(totalSpikes >= 0);
    
    auto avgFiringRate = brain.getAverageFiringRate();
    assert(avgFiringRate >= 0.0f);
    
    // Test developmental stage
    auto stage = brain.getDevelopmentalStage();
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    std::cout << "  ✓ Brain API works correctly" << std::endl;
}

void testAgentBrain() {
    std::cout << "Testing AgentBrain API..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agentBrain(brain);
    
    // Create a world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Test initialization
    agentBrain.initialize(world);
    
    // Test sensory processing
    auto percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    // Test motor decoding
    auto motorCmd = agentBrain.decodeMotorCommand();
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Test development
    agentBrain.updateDevelopment(0.1);
    
    // Test statistics
    auto curiosity = agentBrain.getCuriosityLevel();
    auto novelty = agentBrain.getNoveltyLevel();
    auto error = agentBrain.getPredictionError();
    
    // Test enable flags
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    
    std::cout << "  ✓ AgentBrain API works correctly" << std::endl;
}

void testSimpleWorld() {
    std::cout << "Testing SimpleWorld API..." << std::endl;
    
    nlm::SimpleWorld world;
    
    // Test configuration
    world.configure(20, 20, 16, 16);
    
    // Test reset
    world.reset();
    
    // Test agent start
    world.setAgentStart(10, 10);
    
    // Test update
    world.update(0.1);
    
    // Test sensory percept
    auto percept = world.getSensoryPercept();
    
    // Test agent body
    auto body = world.getAgentBody();
    
    // Test width/height
    auto width = world.getWidth();
    auto height = world.getHeight();
    assert(width == 20);
    assert(height == 20);
    
    // Test simulation time
    auto time = world.getSimulationTime();
    assert(time >= 0.0);
    
    std::cout << "  ✓ SimpleWorld API works correctly" << std::endl;
}

void runAllTests() {
    std::cout << "Testing NLM C++ API Before Python Bindings" << std::endl;
    std::cout << "=" * 60 << std::endl;
    
    testConfig();
    testBrain();
    testAgentBrain();
    testSimpleWorld();
    
    std::cout << "=" * 60 << std::endl;
    std::cout << "All C++ API tests passed! ✓" << std::endl;
}

} // namespace test_api

int main() {
    try {
        test_api::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
