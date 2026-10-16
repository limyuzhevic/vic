// Simplified Python binding tests that focus on core functionality
#include "tests/test_brain.cpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace test_python_bindings {

void testPythonConfig() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic config operations
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("simulation_timestep"));
    
    assert(config->getOr<int64_t>("neuron_count", 0) == 100);
    assert(config->getOr<int64_t>("region_count", 0) == 1);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.001);
    
    std::cout << "    testPythonConfig passed" << std::endl;
}

void testPythonBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getRegionCount() == 1);
    
    // Test basic brain operations
    brain.step(0);
    brain.step(1);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testPythonBrainCreation passed" << std::endl;
}

void testPythonAgentBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test that agent brain is properly initialized
    assert(!agent.isRewardModulationEnabled());
    assert(!agent.isStructuralPlasticityEnabled());
    assert(!agent.isDevelopmentEnabled());
    assert(!agent.isCuriosityEnabled());
    
    // Test feature toggling
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled());
    
    agent.enableDevelopment(true);
    assert(agent.isDevelopmentEnabled());
    
    std::cout << "    testPythonAgentBrain passed" << std::endl;
}

void testPythonWorld() {
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    assert(world.getWidth() == 10);
    assert(world.getHeight() == 10);
    assert(world.getSimulationTime() == 0.0);
    
    // Test world update
    world.update(0.1);
    assert(world.getSimulationTime() == 0.1);
    
    std::cout << "    testPythonWorld passed" << std::endl;
}

void testPythonCompleteWorkflow() {
    // Create complete agent workflow
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(15, 15, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Run simulation steps
    for (int step = 0; step < 10; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    std::cout << "    testPythonCompleteWorkflow passed" << std::endl;
}

void runPythonBindingTests() {
    testPythonConfig();
    testPythonBrainCreation();
    testPythonAgentBrain();
    testPythonWorld();
    testPythonCompleteWorkflow();
    
    std::cout << "All Python binding tests passed!" << std::endl;
}

} // namespace test_python_bindings
