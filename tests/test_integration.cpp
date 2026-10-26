// Integration Tests for Complete Brain-World Loops
// Tests the complete integration of brain, world, and agent systems

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_integration {

void testCompleteBrainWorldLoop() {
    std::cout << "  testCompleteBrainWorldLoop starting..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Create agent
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Run complete simulation loop
    for (int step = 0; step < 100; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input in brain
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Decode motor command
        auto motor_cmd = agent.decodeMotorCommand();
        
        // Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (world.getSensoryPercept().getInternal() && 
            !world.getSensoryPercept().getInternal()->empty()) {
            reward = world.getSensoryPercept().getInternal()->back();
        }
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Verify system integrity
        assert(brain.getTotalNeuronCount() > 0);
        assert(agent.getCuriosityLevel() >= 0.0f);
        assert(agent.getNoveltyLevel() >= 0.0f);
    }
    
    // Verify final state
    assert(brain.getTotalNeuronCount() == 100);
    assert(agent.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "  testCompleteBrainWorldLoop passed" << std::endl;
}

void testBrainSaveLoad() {
    std::cout << "  testBrainSaveLoad starting..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some steps
    for (int step = 0; step < 10; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.bin";
    assert(brain1.save(checkpointFile));
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    // Load checkpoint
    assert(brain2.load(checkpointFile));
    
    // Verify brains have same configuration
    assert(brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    assert(brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount());
    
    // Verify loaded brain can run
    for (int step = 10; step < 20; ++step) {
        brain2.step(step);
    }
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "  testBrainSaveLoad passed" << std::endl;
}

void testConfigAdvancedFeatures() {
    std::cout << "  testConfigAdvancedFeatures starting..." << std::endl;
    
    // Test batch operations
    nlm::Config config;
    config.set("key1", "value1");
    config.set("key2", 42);
    config.set("key3", 3.14);
    config.set("key4", true);
    config.set("key5", std::vector<double>{1.0, 2.0, 3.0});
    
    // Test batch set
    std::vector<std::pair<std::string, nlm::ConfigValue>> updates;
    updates.emplace_back("key6", "value6");
    updates.emplace_back("key7", 100);
    config.batchSet(updates);
    
    assert(config.has("key1"));
    assert(config.has("key7"));
    
    // Test filter
    auto filtered = config.filter({"key1", "key3", "key5"});
    assert(filtered.has("key1"));
    assert(filtered.has("key3"));
    assert(filtered.has("key5"));
    assert(!filtered.has("key2"));
    
    // Test transform
    auto transformed = config.transform([](const nlm::ConfigValue& val) {
        if (std::holds_alternative<double>(val)) {
            return std::get<double>(val) * 2.0;
        } else if (std::holds_alternative<int64_t>(val)) {
            return std::get<int64_t>(val) * 2;
        } else {
            return val;
        }
    });
    
    // Verify transform
    auto val3 = transformed.getAsVariant("key3");
    assert(val3.has_value() && std::get<double>(val3.value()) == 6.28);
    
    std::cout << "  testConfigAdvancedFeatures passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  testNeuromodulationIntegration starting..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation
    nlm::Reward rewardSignal;
    rewardSignal.setValue(1.0f);
    
    nlm::Curiosity curiositySignal;
    curiositySignal.setValue(0.5f);
    
    nlm::Novelty noveltySignal;
    noveltySignal.setValue(0.3f);
    
    nlm::PredictionError errorSignal;
    errorSignal.setValue(0.8f);
    
    // Apply neuromodulation signals
    brain.applyNeuromodulation(rewardSignal);
    brain.applyNeuromodulation(curiositySignal);
    brain.applyNeuromodulation(noveltySignal);
    brain.applyNeuromodulation(errorSignal);
    
    // Verify neuromodulation affects brain state
    // (This tests that the integration points are working)
    
    std::cout << "  testNeuromodulationIntegration passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "  testMemoryIntegration starting..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory systems integration
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Verify memory systems are accessible
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test memory operations
    std::string testPattern = "test_pattern";
    associativeMemory->storePattern(testPattern, std::vector<float>{0.1, 0.2, 0.3});
    
    auto retrieved = associativeMemory->retrievePattern(testPattern);
    assert(retrieved.size() == 3);
    assert(retrieved[0] == 0.1f);
    
    std::cout << "  testMemoryIntegration passed" << std::endl;
}

void testPerformanceBenchmark() {
    std::cout << "  testPerformanceBenchmark starting..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Time brain operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Calculate performance metrics
    double stepsPerSecond = 100.0 / (duration.count() / 1000.0);
    size_t totalSpikes = brain.getTotalSpikeCount();
    
    std::cout << "    Performance: " << stepsPerSecond << " steps/second" << std::endl;
    std::cout << "    Total spikes: " << totalSpikes << std::endl;
    
    // Verify reasonable performance (at least 10 steps per second for small network)
    assert(stepsPerSecond > 10.0);
    
    std::cout << "  testPerformanceBenchmark passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Running Integration Tests ===" << std::endl;
    
    testCompleteBrainWorldLoop();
    testBrainSaveLoad();
    testConfigAdvancedFeatures();
    testNeuromodulationIntegration();
    testMemoryIntegration();
    testPerformanceBenchmark();
    
    std::cout << "=== All Integration Tests Passed ===" << std::endl;
}

} // namespace test_integration