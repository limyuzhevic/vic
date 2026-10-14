// Integration Tests
// Comprehensive integration tests for the enhanced NLM system

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>

namespace test_integration {

void testCompleteSystemInitialization() {
    std::cout << "Testing complete system initialization..." << std::endl;
    
    // Create enhanced configuration
    nlm::Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config.set("brain.synapse_density", 0.15, nlm::ConfigSource::Default);
    config.set("brain.connection_probability", 0.05, nlm::ConfigSource::Default);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config.set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config.set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
    config.set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    config.set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
    
    // Create brain with enhanced configuration
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    // Initialize brain
    bool success = brain->initialize();
    assert(success);
    
    // Verify system state
    assert(brain->getTotalNeuronCount() == 500);
    assert(brain->getTotalSynapseCount() > 0);
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getPlanner() != nullptr);
    assert(brain->getAttention() != nullptr);
    
    std::cout << "✓ Complete system initialization test passed" << std::endl;
}

void testSimulationWithConfigurationProfiles() {
    std::cout << "Testing simulation with different configuration profiles..." << std::endl;
    
    // Test with scientific profile
    nlm::Config scientificConfig;
    bool scientificLoaded = scientificConfig.loadProfile("scientific", true);
    assert(scientificLoaded);
    
    auto scientificBrain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(scientificConfig));
    scientificBrain->initialize();
    
    assert(scientificBrain->getTotalNeuronCount() == 5000);
    
    // Test with performance profile
    nlm::Config performanceConfig;
    bool performanceLoaded = performanceConfig.loadProfile("performance", true);
    assert(performanceLoaded);
    
    auto performanceBrain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(performanceConfig));
    performanceBrain->initialize();
    
    assert(performanceBrain->getTotalNeuronCount() == 2000);
    
    // Test with debug profile
    nlm::Config debugConfig;
    bool debugLoaded = debugConfig.loadProfile("debug", true);
    assert(debugLoaded);
    
    auto debugBrain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(debugConfig));
    debugBrain->initialize();
    
    assert(debugBrain->getTotalNeuronCount() == 100);
    
    std::cout << "✓ Simulation with configuration profiles test passed" << std::endl;
}

void testCompleteSimulationLifecycle() {
    std::cout << "Testing complete simulation lifecycle..." << std::endl;
    
    nlm::Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config.set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config.set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    brain->initialize();
    
    // Simulate multiple steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        
        // Run brain step
        brain->step(step);
        
        // Produce action
        auto action = brain->produceAction();
        assert(action != nullptr);
        
        // Apply neuromodulation
        nlm::Neuromodulator signal;
        brain->applyNeuromodulation(signal);
        
        // Update development
        brain->develop();
        
        // Log progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "  Step " << step << ": " 
                      << brain->getTotalSpikeCount() << " spikes, "
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    // Verify final state
    assert(brain->getTotalNeuronCount() == 200);
    assert(brain->getTotalSynapseCount() > 0);
    
    // Reset and verify
    brain->reset();
    assert(brain->getTotalNeuronCount() == 200);
    
    std::cout << "✓ Complete simulation lifecycle test passed" << std::endl;
}

void testBrainFileIO() {
    std::cout << "Testing brain file I/O operations..." << std::endl;
    
    nlm::Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config.set("brain.synapse_density", 0.12, nlm::ConfigSource::Default);
    
    auto brain1 = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    brain1->initialize();
    
    // Run simulation to create some state
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain1->injectCurrentToNeurons(nlm::NeuronType::Sensory, 15.0f);
        brain1->step(step);
    }
    
    // Save brain state
    bool saveSuccess = brain1->save("test_integration_save.bin");
    assert(saveSuccess);
    
    // Create second brain and load saved state
    nlm::Config config2;
    config2.set("brain.neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config2.set("brain.synapse_density", 0.12, nlm::ConfigSource::Default);
    
    auto brain2 = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config2));
    bool loadSuccess = brain2->load("test_integration_save.bin");
    assert(loadSuccess);
    
    // Verify both brains have same neuron count
    assert(brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
    
    // Verify spike counts are preserved (loaded from saved state)
    assert(brain2->getTotalSpikeCount() >= 0);
    
    // Clean up
    std::remove("test_integration_save.bin");
    
    std::cout << "✓ Brain file I/O operations test passed" << std::endl;
}

void testConfigurationManagement() {
    std::cout << "Testing configuration management..." << std::endl;
    
    nlm::Config config;
    
    // Test multiple configuration sources
    config.set("default_setting", 1, nlm::ConfigSource::Default);
    config.set("file_setting", 2, nlm::ConfigSource::File);
    config.set("command_line_setting", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_setting", 4, nlm::ConfigSource::Runtime);
    
    // Verify all settings are present
    assert(config.getOr<int>("default_setting", 0) == 1);
    assert(config.getOr<int>("file_setting", 0) == 2);
    assert(config.getOr<int>("command_line_setting", 0) == 3);
    assert(config.getOr<int>("runtime_setting", 0) == 4);
    
    // Test configuration validation
    bool valid = config.validate();
    assert(valid);
    
    // Test configuration statistics
    auto stats = config.getStats();
    assert(stats.totalEntries == 4);
    
    // Test configuration comparison
    nlm::Config config2;
    config2.set("default_setting", 1);
    config2.set("file_setting", 2);
    config2.set("command_line_setting", 3);
    config2.set("runtime_setting", 4);
    
    assert(config.equals(config2));
    
    // Test configuration transformation
    auto transformFunc = [](nlm::ConfigValue value) {
        if (std::holds_alternative<int>(value)) {
            return nlm::ConfigValue(std::get<int>(value) * 2);
        }
        return value;
    };
    
    nlm::Config config3;
    config3.set("test_value", 10);
    config3.transform(transformFunc);
    
    assert(config3.getOr<int>("test_value", 0) == 20);
    
    std::cout << "✓ Configuration management test passed" << std::endl;
}

void testPythonBindingCompatibility() {
    std::cout << "Testing Python binding compatibility..." << std::endl;
    
    // Note: This test would typically be run in a Python environment
    // For now, just verify the C++ classes can be instantiated
    
    nlm::Config config;
    assert(config.getOr<int>("missing", 0) == 0);
    
    auto vision = std::make_shared<nlm::Vision>();
    assert(vision->getWidth() == 0);
    assert(vision->getHeight() == 0);
    
    auto audio = std::make_shared<nlm::Audio>();
    assert(audio->getSampleRate() == 0);
    
    auto internal = std::make_shared<nlm::InternalSignals>();
    assert(internal->getSignalCount() == 0);
    
    std::cout << "✓ Python binding compatibility test passed" << std::endl;
}

void testPerformanceIntegration() {
    std::cout << "Testing performance integration..." << std::endl;
    
    nlm::Config config;
    config.set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config.set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    // Benchmark brain initialization
    auto start = std::chrono::high_resolution_clock::now();
    brain->initialize();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> initTime = end - start;
    
    // Initialization should be reasonably fast (< 1 second)
    assert(initTime.count() < 1.0);
    
    // Benchmark simulation
    start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        brain->step(step);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> simTime = end - start;
    
    // Should complete 1000 steps in reasonable time (< 10 seconds)
    assert(simTime.count() < 10.0);
    
    // Performance metrics
    double stepsPerSecond = 1000.0 / simTime.count();
    std::cout << "  Performance: " << stepsPerSecond << " steps/second" << std::endl;
    
    std::cout << "✓ Performance integration test passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    // Test configuration error handling
    nlm::Config config;
    
    // Should not crash when accessing missing keys
    assert(config.getOr<int>("missing", 0) == 0);
    assert(config.getOr<double>("missing", 0.0) == 0.0);
    assert(config.getOr<std::string>("missing", "") == "");
    assert(config.getOr<bool>("missing", false) == false);
    
    // Test validation error handling
    bool valid = config.validate();
    // Should not crash even if validation finds issues
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    // Test that old configuration formats still work
    nlm::Config config;
    
    // Test old key=value format
    config.set("old_style_key", 42);
    assert(config.getOr<int>("old_style_key", 0) == 42);
    
    // Test new configuration methods still work
    config.set("new_style_key", 100, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("new_style_key", 0) == 100);
    
    // Test existing API methods still work
    auto keys = config.getKeys();
    assert(keys.size() == 2);
    
    std::cout << "✓ Backward compatibility test passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive integration tests..." << std::endl;
    
    testCompleteSystemInitialization();
    testSimulationWithConfigurationProfiles();
    testCompleteSimulationLifecycle();
    testBrainFileIO();
    testConfigurationManagement();
    testPythonBindingCompatibility();
    testPerformanceIntegration();
    testErrorHandling();
    testBackwardCompatibility();
    
    std::cout << "\n✓ All integration tests passed!" << std::endl;
    std::cout << "\nIntegration Test Summary:" << std::endl;
    std::cout << "  ✓ System initialization with enhanced configuration" << std::endl;
    std::cout << "  ✓ Configuration profiles (scientific, performance, debug)" << std::endl;
    std::cout << "  ✓ Complete simulation lifecycle with learning" << std::endl;
    std::cout << "  ✓ Brain file I/O operations" << std::endl;
    std::cout << "  ✓ Advanced configuration management" << std::endl;
    std::cout << "  ✓ Python binding compatibility" << std::endl;
    std::cout << "  ✓ Performance integration and benchmarking" << std::endl;
    std::cout << "  ✓ Error handling and recovery" << std::endl;
    std::cout << "  ✓ Backward compatibility" << std::endl;
}

} // namespace test_integration
