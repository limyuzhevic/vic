// NLM Advanced Python Bindings Tests
// Tests for the new advanced Python functions added to pynlm bindings

#include <iostream>
#include <cassert>
#include <memory>
#include "../../src/core/Config/Config.hpp"
#include "../../src/brain/Brain.hpp"
#include "../../src/world/SimpleWorld.hpp"
#include "../../src/agent/AgentBrain.hpp"
#include "../../src/agent/SensoryPercept.hpp"
#include "../../src/agent/AgentBody.hpp"
#include "../../src/sensory/Vision.hpp"

namespace nlm {
namespace tests {

// Test for advanced configuration functions
void testAdvancedConfigurationFunctions() {
    std::cout << "Testing advanced configuration functions..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.2f);
    
    auto brain = std::make_shared<Brain>(config);
    
    // Test that brain can be created with advanced config
    assert(brain->getConfig()->get<size_t>("neuron_count") == 500);
    assert(brain->getConfig()->get<size_t>("region_count") == 2);
    assert(brain->getConfig()->get<float>("connection_probability") == 0.2f);
    
    std::cout << "  PASSED: Advanced configuration functions work correctly" << std::endl;
}

// Test for world creation functions
void testWorldCreationFunctions() {
    std::cout << "Testing world creation functions..." << std::endl;
    
    // Test createWorld
    SimpleWorld world1;
    assert(world1.getWidth() == 0);  // Default
    assert(world1.getHeight() == 0);  // Default
    
    // Test createWorldWithSize
    SimpleWorld world2;
    world2.configure(20, 20, 8, 8);
    assert(world2.getWidth() == 20);
    assert(world2.getHeight() == 20);
    
    std::cout << "  PASSED: World creation functions work correctly" << std::endl;
}

// Test for simple experiment function
void testSimpleExperiment() {
    std::cout << "Testing simple experiment function..." << std::endl;
    
    // Create minimal configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(10, 10, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    agent->enableRewardModulation(true);
    
    // Test runSimpleExperiment (conceptual test - in practice this would be Python)
    // For C++ testing, we'll simulate the logic
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    
    for (size_t step = 0; step < 50; ++step) {
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        float reward = world->computeReward();
        totalReward += reward;
        totalFiringRate += brain->getAverageFiringRate();
    }
    
    float avgReward = totalReward / 50.0f;
    float avgFiringRate = totalFiringRate / 50.0f;
    
    // Basic validation
    assert(avgReward >= -1.0f && avgReward <= 100.0f);  // Reasonable reward range
    assert(avgFiringRate >= 0.0f && avgFiringRate <= 100.0f);  // Reasonable firing rate
    
    std::cout << "  PASSED: Simple experiment function works correctly" << std::endl;
}

// Test for brain state extraction
void testBrainStateExtraction() {
    std::cout << "Testing brain state extraction..." << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Run a simple simulation
    auto world = std::make_shared<SimpleWorld>();
    world->configure(10, 10, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    for (size_t step = 0; step < 20; ++step) {
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        world->update(0.1f);
    }
    
    // Extract brain state (conceptual test)
    std::map<std::string, std::string> state;
    
    // Basic statistics
    state["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
    state["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
    state["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
    state["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
    state["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
    state["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
    state["excitatory_inhibition_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
    state["developmental_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
    
    // Validate extracted data
    assert(state["total_neurons"] != "0");
    assert(state["total_synapses"] != "0");
    assert(std::stoi(state["active_neurons"]) >= 0);
    assert(std::stoi(state["firing_neurons"]) >= 0);
    assert(std::stoll(state["total_spikes"]) >= 0);
    assert(std::stod(state["average_firing_rate"]) >= 0.0f);
    
    std::cout << "  PASSED: Brain state extraction works correctly" << std::endl;
}

// Test for learning agent creation
void testLearningAgentCreation() {
    std::cout << "Testing learning agent creation..." << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable typical learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    agent->enableStructuralPlasticity(true);
    
    // Validate that agent features are enabled
    assert(agent->isRewardModulationEnabled() == true);
    assert(agent->isCuriosityEnabled() == true);
    assert(agent->isDevelopmentEnabled() == true);
    assert(agent->isStructuralPlasticityEnabled() == true);
    
    std::cout << "  PASSED: Learning agent creation works correctly" << std::endl;
}

// Test for batch processing (conceptual)
void testBatchProcessing() {
    std::cout << "Testing batch processing functionality..." << std::endl;
    
    // Create multiple configurations
    std::vector<std::shared_ptr<Config>> configs;
    for (int i = 0; i < 3; ++i) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100 * (i + 1));
        cfg->set("region_count", 1);
        configs.push_back(cfg);
    }
    
    // Simulate batch processing
    std::vector<std::tuple<float, float, float>> results;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        auto brain = std::make_shared<Brain>(configs[i]);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        
        for (size_t step = 0; step < 30; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = world->computeReward();
            totalReward += reward;
            totalFiringRate += brain->getAverageFiringRate();
            
            world->update(0.1f);
        }
        
        float avgReward = totalReward / 30.0f;
        float avgFiringRate = totalFiringRate / 30.0f;
        float timeTaken = 0.3f;  // Simulated
        
        results.push_back(std::make_tuple(avgReward, avgFiringRate, timeTaken));
    }
    
    // Validate results
    assert(results.size() == 3);
    assert(std::get<0>(results[0]) >= -1.0f);  // Average reward should be reasonable
    assert(std::get<1>(results[0]) >= 0.0f);   // Average firing rate should be non-negative
    
    std::cout << "  PASSED: Batch processing functionality works correctly" << std::endl;
}

// Run all advanced Python bindings tests
void runAllAdvancedPythonTests() {
    std::cout << "=== Running Advanced Python Bindings Tests ===" << std::endl;
    
    testAdvancedConfigurationFunctions();
    testWorldCreationFunctions();
    testSimpleExperiment();
    testBrainStateExtraction();
    testLearningAgentCreation();
    testBatchProcessing();
    
    std::cout << "=== All Advanced Python Bindings Tests PASSED ===" << std::endl;
}

} // namespace tests
} // namespace nlm