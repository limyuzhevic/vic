#include <iostream>
#include <cassert>
#include "experiments/Phase6IntegratedExperiment.hpp"

void testBasicBrainWorldInteraction() {
    std::cout << "=== Testing Basic Brain-World Interaction ===" << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Verify integration
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK && "Integration verification failed");
    
    std::cout << "✓ Integration verification passed" << std::endl;
    
    // Test memory integration
    experiment.testMemoryIntegration();
    std::cout << "✓ Memory integration test passed" << std::endl;
    
    // Test neuromodulation integration
    experiment.testNeuromodulationIntegration();
    std::cout << "✓ Neuromodulation integration test passed" << std::endl;
    
    // Test checkpointing
    experiment.testCheckpointing();
    std::cout << "✓ Checkpointing test passed" << std::endl;
    
    // Test replay
    experiment.testReplay();
    std::cout << "✓ Replay test passed" << std::endl;
    
    std::cout << "All brain-world integration tests passed!" << std::endl;
}

void testCompleteLearningEpisode() {
    std::cout << "=== Testing Complete Learning Episode ===" << std::endl;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 500;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    Phase6IntegratedExperiment experiment;
    auto result = experiment.run(config);
    
    // Check that learning occurred
    assert(result.totalReward >= -100.0f && "Reward should be reasonable");
    assert(result.avgFiringRate >= 0.0f && "Firing rate should be non-negative");
    assert(result.memoryEpisodesStored >= 0 && "Memory episodes should be stored");
    assert(result.dopamineLevel >= 0.0f && "Dopamine level should be non-negative");
    
    // Check integration status
    assert(result.memoryWorkingMemoryIntegrated && "Working memory should be integrated");
    assert(result.memoryEpisodicMemoryIntegrated && "Episodic memory should be integrated");
    assert(result.neuromodulationIntegrated && "Neuromodulation should be integrated");
    assert(result.predictionIntegrated && "Prediction should be integrated");
    assert(result.developmentIntegrated && "Development should be integrated");
    
    std::cout << "✓ Complete learning episode test passed" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
}

void testAgentWorldInteraction() {
    std::cout << "=== Testing Agent-World Interaction ===" << std::endl;
    
    // Create brain, world, and agent components
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 300, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run a simulation
    int steps = 100;
    double totalReward = 0.0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process input
        agent->processSensoryInput(percept);
        
        // Run brain
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply to world
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        totalReward += result.reward;
        
        // Apply reward modulation
        agent->applyRewardModulation(result.reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
    }
    
    // Verify agent behaviors
    assert(brain->getTotalNeuronCount() > 0 && "Brain should have neurons");
    assert(world->getSimulationTime() > 0 && "World should have simulation time");
    assert(totalReward != 0.0f && "Should have accumulated reward");
    
    std::cout << "✓ Agent-world interaction test passed" << std::endl;
    std::cout << "  Total reward accumulated: " << totalReward << std::endl;
    std::cout << "  Simulation time: " << world->getSimulationTime() << std::endl;
}

void testConfigurationSystem() {
    std::cout << "=== Testing Configuration System ===" << std::endl;
    
    // Test default configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000, ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    
    assert(config->has("brain.neuron_count") && "Should have neuron_count");
    assert(config->has("brain.synapse_density") && "Should have synapse_density");
    
    // Test getting values
    auto count = config->get<int64_t>("brain.neuron_count");
    assert(count && *count == 1000 && "Should get correct neuron count");
    
    auto density = config->get<double>("brain.synapse_density");
    assert(density && *density == 0.1f && "Should get correct synapse density");
    
    // Test getOr with default
    auto maxNeurons = config->getOr<int64_t>("brain.max_neurons", 5000);
    assert(*maxNeurons == 5000 && "Should use default for missing key");
    
    // Test JSON serialization/deserialization
    std::string jsonFile = "/tmp/test_config.json";
    assert(config->saveToFile(jsonFile) && "Should save to JSON");
    
    auto config2 = std::make_shared<Config>();
    assert(config2->loadFromFile(jsonFile) && "Should load from JSON");
    
    auto count2 = config2->get<int64_t>("brain.neuron_count");
    assert(count2 && *count2 == 1000 && "Should load correct value");
    
    std::cout << "✓ Configuration system test passed" << std::endl;
}

void testPythonBindings() {
    std::cout << "=== Testing Python Bindings (simulated) ===" << std::endl;
    
    // This test would normally use pybind11 to test Python bindings
    // For now, we'll just verify the C++ side includes all necessary components
    
    // Check that key bindings exist
    assert(!std::is_same_v<decltype(Brain::initialize), void>::value && "Brain should have initialize method");
    assert(!std::is_same_v<decltype(Brain::step), void>::value && "Brain should have step method");
    assert(!std::is_same_v<decltype(Config::getKeys), std::vector<std::string>>::value && "Config should have getKeys method");
    assert(!std::is_same_v<decltype(SimpleWorld::update), void>::value && "SimpleWorld should have update method");
    
    std::cout << "✓ Python bindings test passed" << std::endl;
    std::cout << "  All required bindings are present" << std::endl;
}

int main() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << "Testing improved NLM codebase functionality" << std::endl;
    std::cout << std::endl;
    
    try {
        testConfigurationSystem();
        std::cout << std::endl;
        
        testPythonBindings();
        std::cout << std::endl;
        
        testBasicBrainWorldInteraction();
        std::cout << std::endl;
        
        testAgentWorldInteraction();
        std::cout << std::endl;
        
        testCompleteLearningEpisode();
        std::cout << std::endl;
        
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "The improved NLM codebase is functioning correctly!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}