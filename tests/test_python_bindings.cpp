// C++ tests for Python bindings integration
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>

namespace test_python_bindings {

void test_config_save_load() {
    nlm::Config config;
    
    config.set("test_string", "hello");
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_bool", true);
    
    // Test saving to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Test loading from file
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.cfg"));
    
    assert(config2.has("test_string"));
    assert(config2.has("test_int"));
    assert(config2.has("test_double"));
    assert(config2.has("test_bool"));
    
    std::cout << "    test_config_save_load passed" << std::endl;
}

void test_brain_functionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    test_brain_functionality passed" << std::endl;
}

void test_agent_brain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test neuromodulation features
    assert(agent.getNeuromodulationLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getPredictionError() >= 0.0f);
    
    // Test enabling curiosity
    agent.enableCuriosity(true);
    assert(agent.isCuriosityEnabled());
    
    std::cout << "    test_agent_brain passed" << std::endl;
}

void runAll() {
    test_config_save_load();
    test_brain_functionality();
    test_agent_brain();
}

} // namespace test_python_bindings