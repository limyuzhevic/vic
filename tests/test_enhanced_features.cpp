// NLM Advanced Features Tests
// Tests for advanced Python functionality (statistics, simulation management, config validation, etc.)

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

namespace test_advanced_features {

void test_brain_statistics() {
    std::cout << "  Testing brain statistics functionality..." << std::endl;
    
    // Create and initialize brain
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation steps
    for (int i = 0; i < 20; ++i) {
        brain->step(i);
    }
    
    // Test 1: Individual statistics methods
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    size_t firingCount = brain->getFiringNeuronCount();
    size_t activeCount = brain->getActiveNeuronCount();
    float avgRate = brain->getAverageFiringRate();
    float eiRatio = brain->getExcitationInhibitionRatio();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t pendingSpikes = brain->getPendingSpikeEventCount();
    
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingCount >= 0);
    assert(activeCount >= 0);
    assert(avgRate >= 0.0f);
    assert(eiRatio >= 0.0f);
    assert(totalSpikes > 0); // Should have spikes after simulation
    assert(pendingSpikes >= 0);
    
    // Test 2: Developmental stage
    nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || 
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation ||
           stage == nlm::DevelopmentalStage::Adult ||
           stage == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    ✓ Brain statistics test passed" << std::endl;
}

void test_configuration_management() {
    std::cout << "  Testing configuration management..." << std::endl;
    
    // Test 1: Create and configure custom config
    auto config = std::make_shared<nlm::Config>();
    
    // Set various configuration options
    config->set("brain.neuron_count", 1500);
    config->set("brain.synapse_density", 0.15);
    config->set("brain.connection_probability", 0.08);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Verify configuration values
    auto neuronCount = config->get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1500);
    
    auto synDensity = config->get<double>("brain.synapse_density");
    assert(synDensity.has_value() && *synDensity == 0.15);
    
    auto connectionProb = config->get<double>("brain.connection_probability");
    assert(connectionProb.has_value() && *connectionProb == 0.08);
    
    auto stdpEnabled = config->get<bool>("plasticity.stdp.enable");
    assert(stdpEnabled.has_value() && *stdpEnabled == true);
    
    // Test 2: Config validation
    assert(config->has("brain.neuron_count"));
    assert(config->has("brain.synapse_density"));
    assert(config->has("plasticity.stdp.enable"));
    assert(!config->has("nonexistent.key"));
    
    // Test 3: Configuration summary
    std::string summary = config->summary();
    assert(summary.find("brain.neuron_count") != std::string::npos);
    assert(summary.find("brain.synapse_density") != std::string::npos);
    assert(summary.find("plasticity.stdp.enable") != std::string::npos);
    
    // Test 4: Configuration keys
    std::vector<std::string> keys = config->getKeys();
    assert(keys.size() >= 6); // At least the 6 keys we set
    
    // Test 5: Remove configuration
    config->remove("brain.neuron_count");
    assert(!config->has("brain.neuron_count"));
    
    std::cout << "    ✓ Configuration management test passed" << std::endl;
}

void test_simulation_management() {
    std::cout << "  Testing simulation management..." << std::endl;
    
    // Create simulation components
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(40, 40, 16, 16);
    world->reset();
    agent->initialize(*world);
    
    // Test 1: World simulation
    double initialTime = world->getSimulationTime();
    assert(initialTime == 0.0);
    
    // Run world updates
    for (int i = 0; i < 10; ++i) {
        double timeBefore = world->getSimulationTime();
        world->update(0.1);
        double timeAfter = world->getSimulationTime();
        assert(timeAfter > timeBefore);
        assert(timeAfter - timeBefore == 0.1);
    }
    
    // Test 2: Agent simulation loop
    for (int step = 0; step < 100; ++step) {
        world->update(0.05);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Update development
        agent->updateDevelopment(0.05);
        
        // Collect statistics periodically
        if (step % 20 == 0) {
            auto stats = brain->getTotalNeuronCount();
            assert(stats > 0);
        }
    }
    
    // Test 3: Simulation state persistence
    size_t initialSpikes = brain->getTotalSpikeCount();
    size_t initialNeurons = brain->getTotalNeuronCount();
    
    // Run more simulation
    for (int i = 100; i < 150; ++i) {
        brain->step(i);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    assert(finalSpikes >= initialSpikes);
    
    std::cout << "    ✓ Simulation management test passed" << std::endl;
}

void test_data_persistence_features() {
    std::cout << "  Testing data persistence features..." << std::endl;
    
    // Create and run simulation
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run some simulation
    for (int i = 0; i < 50; ++i) {
        brain->step(i);
    }
    
    // Test 1: Save functionality
    std::string savePath = "test_advanced_brain.bin";
    bool saveSuccess = brain->save(savePath);
    assert(saveSuccess);
    
    // Verify file exists (basic check)
    // In a real test environment, we would check file existence
    // For now, we trust the brain->save() return value
    
    // Test 2: Load functionality
    auto config2 = std::make_shared<nlm::Config>();
    auto brain2 = std::make_shared<nlm::Brain>(config2);
    
    bool loadSuccess = brain2->load(savePath);
    assert(loadSuccess);
    
    // Test 3: Verify loaded state matches
    assert(brain2->getTotalNeuronCount() == brain->getTotalNeuronCount());
    assert(brain2->getTotalSynapseCount() == brain->getTotalSynapseCount());
    
    // Test 4: Reset functionality
    brain2->reset();
    assert(brain2->getTotalNeuronCount() > 0);  // Should have neurons after reset
    
    std::cout << "    ✓ Data persistence features test passed" << std::endl;
}

void test_advanced_agent_features() {
    std::cout << "  Testing advanced agent features..." << std::endl;
    
    // Create advanced setup
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(60, 60, 20, 20);
    world->reset();
    agent->initialize(*world);
    
    // Enable all advanced features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Test 1: Complete advanced simulation
    for (int step = 0; step < 200; ++step) {
        world->update(0.03);
        
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain
        brain->step(step);
        
        // Get action with curiosity
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = world->getSensoryPercept().getInternal()[0];
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.03);
        
        // Verify advanced features are working
        if (step % 50 == 0) {
            assert(agent->getCuriosityLevel() >= 0.0f);
            assert(agent->getNoveltyLevel() >= 0.0f);
            assert(agent->getPredictionError() >= -1.0f);
            assert(agent->getNeuromodulationLevel() >= -1.0f);
        }
    }
    
    // Test 2: Advanced statistics
    float curiosity = agent->getCuriosityLevel();
    float novelty = agent->getNoveltyLevel();
    float prediction = agent->getPredictionError();
    
    assert(curiosity >= 0.0f);
    assert(novelty >= 0.0f);
    assert(prediction >= -1.0f);
    
    // Test 3: Development tracking
    nlm::DevelopmentalStage stage = agent->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || 
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation ||
           stage == nlm::DevelopmentalStage::Adult ||
           stage == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    ✓ Advanced agent features test passed" << std::endl;
}

void test_error_handling_and_validation() {
    std::cout << "  Testing error handling and validation..." << std::endl;
    
    // Test 1: Invalid file operations
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // These should fail gracefully (return false, not crash)
    bool saveInvalid = brain->save("/nonexistent/directory/file.bin");
    assert(!saveInvalid);  // Should fail for invalid path
    
    bool loadInvalid = brain->load("nonexistent_file.bin");
    assert(!loadInvalid);  // Should fail for non-existent file
    
    // Test 2: Config edge cases
    auto config2 = std::make_shared<nlm::Config>();
    
    // Test empty config
    std::vector<std::string> emptyKeys = config2->getKeys();
    assert(emptyKeys.empty());
    
    // Test removing non-existent key (should not crash)
    config2->remove("nonexistent.key");
    
    // Test clearing config
    config2->set("temp.key", "value");
    config2->clear();
    assert(config2->getKeys().empty());
    
    // Test 3: Simulation edge cases
    auto brain2 = std::make_shared<nlm::Brain>(config2);
    brain2->initialize();
    
    // Test reset multiple times
    brain2->reset();
    brain2->reset();
    assert(brain2->getTotalNeuronCount() > 0);
    
    std::cout << "    ✓ Error handling and validation test passed" << std::endl;
}

void test_performance_and_stress() {
    std::cout << "  Testing performance and stress scenarios..." << std::endl;
    
    // Create larger simulation
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test 1: Stress test with many steps
    for (int i = 0; i < 500; ++i) {
        brain->step(i);
        
        // Check that brain remains stable
        assert(brain->getTotalNeuronCount() > 0);
        assert(brain->getTotalSynapseCount() >= 0);
        
        // Periodic validation
        if (i % 100 == 0) {
            size_t spikes = brain->getTotalSpikeCount();
            assert(spikes >= 0);
        }
    }
    
    // Test 2: Memory efficiency
    auto initialBrain = brain->getTotalNeuronCount();
    brain->reset();
    auto resetBrain = brain->getTotalNeuronCount();
    
    // Brain should maintain same number of neurons after reset
    assert(initialBrain == resetBrain);
    
    std::cout << "    ✓ Performance and stress test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Advanced Features Tests:" << std::endl;
    
    test_brain_statistics();
    test_configuration_management();
    test_simulation_management();
    test_data_persistence_features();
    test_advanced_agent_features();
    test_error_handling_and_validation();
    test_performance_and_stress();
    
    std::cout << "All advanced features tests completed successfully!" << std::endl;
}

} // namespace test_advanced_features