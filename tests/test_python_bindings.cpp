#include "tests/test_python_bindings.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_python_bindings {

void testConfigPythonicMethods() {
    std::cout << "    Testing Config Pythonic methods..." << std::endl;
    
    nlm::Config config;
    
    // Test Pythonic get methods
    config.set("int_val", 42, nlm::ConfigSource::Default);
    config.set("double_val", 3.14, nlm::ConfigSource::Default);
    config.set("string_val", "hello", nlm::ConfigSource::Default);
    config.set("bool_val", true, nlm::ConfigSource::Default);
    
    int intVal = config.get_or<int>("int_val", 0);
    assert(intVal == 42);
    
    double doubleVal = config.get_or<double>("double_val", 0.0);
    assert(std::abs(doubleVal - 3.14) < 0.001);
    
    std::string stringVal = config.get_or<std::string>("string_val", "");
    assert(stringVal == "hello");
    
    bool boolVal = config.get_or<bool>("bool_val", false);
    assert(boolVal == true);
    
    // Test Pythonic get with defaults
    int missingInt = config.get_or<int>("missing_int", 100);
    assert(missingInt == 100);
    
    double missingDouble = config.get_or<double>("missing_double", 2.718);
    assert(std::abs(missingDouble - 2.718) < 0.001);
    
    std::string missingString = config.get_or<std::string>("missing_string", "default");
    assert(missingString == "default");
    
    bool missingBool = config.get_or<bool>("missing_bool", false);
    assert(missingBool == false);
    
    std::cout << "    Config Pythonic methods test passed" << std::endl;
}

void testConfigConvenienceMethods() {
    std::cout << "    Testing Config convenience methods..." << std::endl;
    
    nlm::Config config;
    
    // Test default brain config
    config.set_default_brain_config();
    
    // Verify some key settings
    auto neuronCount = config.get<size_t>("brain.neuron_count");
    assert(neuronCount && *neuronCount == 1000);
    
    auto regionCount = config.get<size_t>("brain.region_count");
    assert(regionCount && *regionCount == 1);
    
    auto stdpEnable = config.get<bool>("plasticity.stdp.enable");
    assert(stdpEnable && *stdpEnable == true);
    
    // Test performance config
    nlm::Config perfConfig;
    perfConfig.set_performance_config();
    
    auto perfNeuronCount = perfConfig.get<size_t>("brain.neuron_count");
    assert(perfNeuronCount && *perfNeuronCount == 500);
    
    // Test research config
    nlm::Config researchConfig;
    researchConfig.set_research_config();
    
    auto researchNeuronCount = researchConfig.get<size_t>("brain.neuron_count");
    assert(researchNeuronCount && *researchNeuronCount == 5000);
    
    // Test validation
    auto validationErrors = config.validate();
    assert(validationErrors.empty());  // Should be no errors for valid config
    
    // Test to_dict conversion
    auto configDict = config.to_dict();
    assert(configDict.size() > 0);
    
    // Test from_dict conversion
    nlm::Config newConfig;
    newConfig.set_from_dict(configDict);
    
    // Verify that keys were copied
    for (const auto& item : configDict) {
        std::string key = py::cast<std::string>(item.first);
        assert(newConfig.has(key));
    }
    
    std::cout << "    Config convenience methods test passed" << std::endl;
}

void testBrainPythonicInterface() {
    std::cout << "    Testing Brain Pythonic interface..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set_default_brain_config();
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test basic brain operations
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Run some simulation steps
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify statistics still make sense
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    Brain Pythonic interface test passed" << std::endl;
}

void testAgentBrainPythonicFeatures() {
    std::cout << "    Testing AgentBrain Pythonic features..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set_default_brain_config();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Test AgentBrain methods
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    // Enable various subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Test neuromodulation levels
    float curiosityLevel = agent.getCuriosityLevel();
    float noveltyLevel = agent.getNoveltyLevel();
    float predictionError = agent.getPredictionError();
    
    assert(curiosityLevel >= 0.0f);
    assert(noveltyLevel >= 0.0f);
    assert(predictionError >= -1.0f && predictionError <= 1.0f);
    
    std::cout << "    AgentBrain Pythonic features test passed" << std::endl;
}

void testWorldPythonicInterface() {
    std::cout << "    Testing World Pythonic interface..." << std::endl;
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Test world properties
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    assert(world.getVisionWidth() == 8);
    assert(world.getVisionHeight() == 8);
    
    // Set agent position
    world.setAgentStart(10.0f, 10.0f);
    auto agentBody = world.getAgentBody();
    assert(agentBody.x == 10.0f);
    assert(agentBody.y == 10.0f);
    
    // Update world
    world.update(0.1f);
    
    // Get sensory percept
    auto percept = world.getSensoryPercept();
    assert(percept.getVisionWidth() == 8);
    assert(percept.getVisionHeight() == 8);
    
    std::cout << "    World Pythonic interface test passed" << std::endl;
}

void testCompleteSimulationPipeline() {
    std::cout << "    Testing complete simulation pipeline..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set_default_brain_config();
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent interface
    nlm::AgentBrain agent(brain);
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(15, 15, 8, 8);
    world.reset();
    world.setAgentStart(7.5f, 7.5f);
    
    // Initialize agent
    agent.initialize(world);
    
    // Enable all learning subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run simulation steps
    for (int step = 0; step < 100; ++step) {
        // Update world
        world.update(0.1f);
        
        // Get percept and process
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        // Run brain
        brain.step(step);
        
        // Get action and apply
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Apply reward modulation if enabled
        if (agent.isRewardModulationEnabled()) {
            float reward = 0.0f;
            if (!percept.getInternal().empty()) {
                reward = percept.getInternal()[0];
            }
            agent.applyRewardModulation(reward, 0.0f);
        }
        
        // Update development if enabled
        if (agent.isDevelopmentEnabled()) {
            agent.updateDevelopment(0.1f);
        }
        
        // Print progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "      Step " << step << ": "
                     << brain.getFiringNeuronCount() << " firing, "
                     << "Curiosity: " << std::fixed << std::setprecision(2) 
                     << agent.getCuriosityLevel() << ", "
                     << "Dev Stage: " << static_cast<int>(agent.getDevelopmentalStage())
                     << std::endl;
        }
    }
    
    // Final status check
    auto finalPercept = world.getSensoryPercept();
    auto finalAction = agent.decodeMotorCommand();
    
    std::cout << "    Complete simulation pipeline test passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "    Testing error handling..." << std::endl;
    
    // Test config validation with bad values
    nlm::Config config;
    config.set("brain.neuron_count", -100, nlm::ConfigSource::Default);
    config.set("brain.region_count", 1000, nlm::ConfigSource::Default);
    
    auto errors = config.validate();
    assert(!errors.empty()); // Should have validation errors
    
    // Test that validation catches multiple issues
    assert(errors.size() >= 2); // Should catch both neuron count and region count
    
    std::cout << "    Error handling test passed" << std::endl;
}

void testMemoryManagement() {
    std::cout << "    Testing memory management..." << std::endl;
    
    // Test config reset
    nlm::Config config;
    config.set("key1", "value1", nlm::ConfigSource::Default);
    config.set("key2", "value2", nlm::ConfigSource::Default);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    
    // Test brain reset
    auto config2 = std::make_shared<nlm::Config>();
    config2->set_default_brain_config();
    
    nlm::Brain brain(config2);
    brain.initialize();
    
    // Take a snapshot
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run some steps
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSpikeCount() == 0);
    
    std::cout << "    Memory management test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Python Bindings Tests..." << std::endl;
    std::cout << "  Testing enhanced Python API and user experience..." << std::endl;
    std::cout << std::endl;
    
    testConfigPythonicMethods();
    testConfigConvenienceMethods();
    testBrainPythonicInterface();
    testAgentBrainPythonicFeatures();
    testWorldPythonicInterface();
    testCompleteSimulationPipeline();
    testErrorHandling();
    testMemoryManagement();
    
    std::cout << std::endl;
    std::cout << "=== All Python Bindings Tests PASSED ===" << std::endl;
}

} // namespace test_python_bindings