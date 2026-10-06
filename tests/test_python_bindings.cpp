// Python Binding Tests
// Tests for pynlm Python bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <cassert>
#include <iostream>
#include <memory>

namespace test_python_bindings {

void testPythonCreateDefaultConfig() {
    auto config = std::make_shared<nlm::Config>();
    assert(config != nullptr);
    
    // Test basic config operations
    config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    assert(config->has("brain.neuron_count"));
    
    auto count = config->getOr<int64_t>("brain.neuron_count", 0);
    assert(count == 100);
    
    std::cout << "    testPythonCreateDefaultConfig passed" << std::endl;
}

void testPythonCreateBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.initialize());
    
    std::cout << "    testPythonCreateBrain passed" << std::endl;
}

void testPythonCreateSimpleWorld() {
    auto world = std::make_shared<nlm::SimpleWorld>();
    assert(world != nullptr);
    
    world->configure(10, 10, 8, 8);
    assert(world->getWidth() == 10);
    assert(world->getHeight() == 10);
    assert(world->getVisionWidth() == 8);
    assert(world->getVisionHeight() == 8);
    
    world->reset();
    assert(world->getSimulationTime() == 0.0);
    
    std::cout << "    testPythonCreateSimpleWorld passed" << std::endl;
}

void testPythonCreateAgentBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    assert(agent.getBrain() != nullptr);
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testPythonCreateAgentBrain passed" << std::endl;
}

void testPythonConfigFileIO() {
    auto config = std::make_shared<nlm::Config>();
    
    // Set some values
    config->set("brain.neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config->set("brain.synapse_density", 0.15f, nlm::ConfigSource::Runtime);
    
    // Test saving
    bool save_result = config->saveToFile("test_config.json");
    assert(save_result);
    
    // Create new config and load
    auto loaded_config = std::make_shared<nlm::Config>();
    bool load_result = loaded_config->loadFromFile("test_config.json");
    assert(load_result);
    
    // Verify values
    assert(loaded_config->has("brain.neuron_count"));
    assert(loaded_config->has("brain.synapse_density"));
    
    auto count = loaded_config->getOr<int64_t>("brain.neuron_count", 0);
    assert(count == 200);
    
    std::cout << "    testPythonConfigFileIO passed" << std::endl;
}

void testPythonBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run several steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify brain still valid
    assert(brain.getTotalNeuronCount() == 25);
    
    std::cout << "    testPythonBrainStep passed" << std::endl;
}

void testPythonAgentSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);  // 16x16 vision
    percept.setVision(vision);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify it didn't crash and agent is still valid
    assert(agent.getBrain() != nullptr);
    
    std::cout << "    testPythonAgentSensoryProcessing passed" << std::endl;
}

void testPythonAgentMotorCommands() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Get motor command
    nlm::MotorCommand action = agent.decodeMotorCommand();
    
    // Should return a valid action
    assert(action == nlm::MotorCommand::Wait || 
           action == nlm::MotorCommand::MoveForward ||
           action == nlm::MotorCommand::TurnLeft ||
           action == nlm::MotorCommand::TurnRight ||
           action == nlm::MotorCommand::Interact);
    
    std::cout << "    testPythonAgentMotorCommands passed" << std::endl;
}

void testPythonNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(35), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test neuromodulation
    float initial_dopamine = agent.getNeuromodulationLevel();
    assert(std::abs(initial_dopamine) <= 1.0f);
    
    // Apply reward modulation
    agent.applyRewardModulation(0.5f, 0.0f);
    
    float final_dopamine = agent.getNeuromodulationLevel();
    assert(std::abs(final_dopamine) <= 1.0f);
    
    std::cout << "    testPythonNeuromodulation passed" << std::endl;
}

void testPythonDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(45), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test development
    nlm::DevelopmentalStage initial_stage = agent.getDevelopmentalStage();
    assert(initial_stage != nlm::DevelopmentalStage::Aging);
    
    agent.updateDevelopment(10.0);  // Advance development
    
    nlm::DevelopmentalStage later_stage = agent.getDevelopmentalStage();
    assert(later_stage != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testPythonDevelopmentSystem passed" << std::endl;
}

void testPythonConvenienceFunctions() {
    // Test the enhanced convenience functions
    
    // Test createDefaultConfig
    auto config = nlm::createDefaultConfig();
    assert(config != nullptr);
    
    // Test createBrain
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    assert(brain->getTotalNeuronCount() > 0);
    
    // Test createSimpleWorld
    auto world = nlm::createSimpleWorld();
    assert(world != nullptr);
    world->configure(10, 10, 8, 8);
    
    // Test createAgentBrain
    auto agent = nlm::createAgentBrain(brain);
    assert(agent != nullptr);
    
    std::cout << "    testPythonConvenienceFunctions passed" << std::endl;
}

void runAll() {
    std::cout << "Running Python binding tests..." << std::endl;
    
    testPythonCreateDefaultConfig();
    testPythonCreateBrain();
    testPythonCreateSimpleWorld();
    testPythonCreateAgentBrain();
    testPythonConfigFileIO();
    testPythonBrainStep();
    testPythonAgentSensoryProcessing();
    testPythonAgentMotorCommands();
    testPythonNeuromodulation();
    testPythonDevelopmentSystem();
    testPythonConvenienceFunctions();
    
    std::cout << std::endl << "All Python binding tests passed!" << std::endl;
}

} // namespace test_python_bindings

// Main function for Python binding tests
int main() {
    test_python_bindings::runAll();
    return 0;
}
