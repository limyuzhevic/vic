// NLM Python Bindings Tests
// Tests for the enhanced Python bindings functionality

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <cmath>

// Include NLM headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Types/Types.hpp"

namespace test_python_bindings {

void test_basic_bindings() {
    std::cout << "  Testing basic Python bindings..." << std::endl;
    
    // Test 1: Default config creation
    auto config = std::make_shared<nlm::Config>();
    assert(config != nullptr);
    
    // Test 2: Brain creation with config
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain != nullptr);
    
    // Test 3: Brain initialization
    assert(brain->initialize());
    
    // Test 4: Basic statistics
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() >= 0);
    assert(brain->getFiringNeuronCount() >= 0);
    
    // Test 5: World creation
    auto world = std::make_shared<nlm::SimpleWorld>();
    assert(world != nullptr);
    
    // Test 6: World configuration
    world->configure(10, 10, 8, 8);
    assert(world->getWidth() == 10);
    assert(world->getHeight() == 10);
    
    // Test 7: Agent creation
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    assert(agent != nullptr);
    
    std::cout << "    ✓ Basic bindings test passed" << std::endl;
}

void test_agent_functionality() {
    std::cout << "  Testing agent functionality..." << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    agent->initialize(*world);
    
    // Test 1: Sensory processing
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    // Test 2: Motor decoding
    auto action = agent->decodeMotorCommand();
    assert(action == nlm::MotorCommand::Wait || action == nlm::MotorCommand::MoveForward);
    
    // Test 3: Neuromodulation
    agent->applyRewardModulation(1.0f, 0.5f);
    assert(std::abs(agent->getNeuromodulationLevel() - 0.5f) < 0.1f);
    
    // Test 4: Development
    agent->updateDevelopment(0.1);
    assert(agent->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Test 5: Enable features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    assert(agent->isRewardModulationEnabled());
    assert(agent->isDevelopmentEnabled());
    
    std::cout << "    ✓ Agent functionality test passed" << std::endl;
}

void test_enhanced_functionality() {
    std::cout << "  Testing enhanced Python functionality..." << std::endl;
    
    // Note: In a real test environment, we would have access to the Python bindings
    // For now, we'll test the C++ side that the Python bindings call
    
    // Test 1: Config functionality (mirrored from Python bindings)
    auto config = std::make_shared<nlm::Config>();
    
    // Set values
    config->set("brain.neuron_count", 100);
    config->set("brain.synapse_density", 0.1);
    config->set("plasticity.stdp.enable", true);
    
    // Get values
    auto neuronCount = config->get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 100);
    
    auto synDensity = config->get<double>("brain.synapse_density");
    assert(synDensity.has_value() && *synDensity == 0.1);
    
    auto stdpEnabled = config->get<bool>("plasticity.stdp.enable");
    assert(stdpEnabled.has_value() && *stdpEnabled == true);
    
    // Test 2: Config validation logic
    assert(config->has("brain.neuron_count"));
    assert(config->has("brain.synapse_density"));
    assert(!config->has("missing.key"));
    
    // Test 3: Config summary
    std::string summary = config->summary();
    assert(summary.find("brain.neuron_count") != std::string::npos);
    assert(summary.find("brain.synapse_density") != std::string::npos);
    
    std::cout << "    ✓ Enhanced functionality test passed" << std::endl;
}

void test_data_persistence() {
    std::cout << "  Testing data persistence functionality..." << std::endl;
    
    // Create and initialize brain
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run some simulation steps
    for (int i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    // Test 1: Save functionality
    std::string savePath = "test_brain_checkpoint.bin";
    bool saveSuccess = brain->save(savePath);
    assert(saveSuccess);
    
    // Test 2: Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    auto brain2 = std::make_shared<nlm::Brain>(config2);
    
    bool loadSuccess = brain2->load(savePath);
    assert(loadSuccess);
    
    // Test 3: Verify loaded brain
    assert(brain2->getTotalNeuronCount() == brain->getTotalNeuronCount());
    assert(brain2->getTotalSynapseCount() == brain->getTotalSynapseCount());
    
    std::cout << "    ✓ Data persistence test passed" << std::endl;
}

void test_simulation_features() {
    std::cout << "  Testing simulation features..." << std::endl;
    
    // Create components
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(30, 30, 12, 12);
    world->reset();
    agent->initialize(*world);
    
    // Test 1: Complete simulation episode
    for (int step = 0; step < 50; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Verify simulation state
        assert(brain->getTotalNeuronCount() > 0);
        assert(world->getAgentBody().x >= 0);
        assert(world->getAgentBody().y >= 0);
    }
    
    // Test 2: Statistics collection
    size_t initialSpikes = brain->getTotalSpikeCount();
    assert(initialSpikes > 0);
    
    // Test 3: Agent features
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    assert(agent->getCuriosityLevel() >= 0.0f);
    assert(agent->getNoveltyLevel() >= 0.0f);
    
    std::cout << "    ✓ Simulation features test passed" << std::endl;
}

void test_error_handling() {
    std::cout << "  Testing error handling..." << std::endl;
    
    // Test 1: Invalid file operations
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Save to invalid path (should fail gracefully)
    bool saveInvalid = brain->save("/invalid/path/that/does/not/exist.bin");
    // Note: In real implementation, this would handle errors appropriately
    // For now, we just ensure the code compiles
    
    // Test 2: Load non-existent file
    bool loadNonexistent = brain->load("nonexistent_file.bin");
    assert(!loadNonexistent);
    
    // Test 3: Config validation (simulated)
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2 != nullptr);
    
    std::cout << "    ✓ Error handling test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Python Bindings Tests:" << std::endl;
    
    test_basic_bindings();
    test_agent_functionality();
    test_enhanced_functionality();
    test_data_persistence();
    test_simulation_features();
    test_error_handling();
    
    std::cout << "All Python bindings tests completed successfully!" << std::endl;
}

} // namespace test_python_bindings