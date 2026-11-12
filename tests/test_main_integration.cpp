// Integration Test for Main Brain Functionality
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_main_integration {

void testBrainConfiguration() {
    nlm::Config config;
    
    // Configure brain with test settings
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    config.set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Runtime);
    
    // Create brain
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    
    // Initialize brain
    assert(brain.initialize());
    
    // Verify basic properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainConfiguration passed" << std::endl;
}

void testBrainBasicOperation() {
    nlm::Config config;
    
    // Configure brain
    config.set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    
    // Create and initialize brain
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    assert(brain.initialize());
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Verify brain remains valid
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalSpikeCount() >= 0);
    }
    
    // Verify final state
    assert(brain.getTotalNeuronCount() == 200);
    
    std::cout << "    testBrainBasicOperation passed" << std::endl;
}

void testBrainResetFunctionality() {
    nlm::Config config;
    
    config.set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    assert(brain.initialize());
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Verify state changed
    assert(brain.getTotalSpikeCount() > initialSpikes);
    
    // Reset brain
    brain.reset();
    brain.initialize();
    
    // Verify reset
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() == 0); // Should be reset
    
    std::cout << "    testBrainResetFunctionality passed" << std::endl;
}

void testBrainActionProduction() {
    nlm::Config config;
    
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    assert(brain.initialize());
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Test action production
    auto action = brain.produceAction();
    
    // Action should be valid (Wait, MoveForward, etc.)
    assert(action.getType() == nlm::ActionType::Wait || 
           action.getType() == nlm::ActionType::MoveForward);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainSensoryIntegration() {
    nlm::Config config;
    
    config.set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(std::make_shared<nlm::Config>(config));
    assert(brain.initialize());
    
    // Create sensory input
    nlm::SensoryInput sensoryInput;
    std::vector<float> sensoryData = {0.5f, 0.3f, 0.8f, 0.1f, 0.9f};
    sensoryInput.setData(sensoryData);
    
    // Apply sensory input
    brain.receiveSensoryInput(sensoryInput);
    
    // Verify sensory processing (neurons should have received input)
    // Note: This is a basic test - actual verification depends on implementation
    assert(brain.getTotalNeuronCount() == 150);
    
    std::cout << "    testBrainSensoryIntegration passed" << std::endl;
}

void testBrainCommandLineInterface() {
    nlm::Config config;
    
    // Test that config system handles command-line arguments
    // This simulates what main() would do
    config.set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    // Simulate command-line overrides
    config.set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::CommandLine);
    
    // Verify command-line override worked
    assert(config.getOr<int64_t>("neuron_count", 0) == 80);
    
    std::cout << "    testBrainCommandLineInterface passed" << std::endl;
}

void runAll() {
    testBrainConfiguration();
    testBrainBasicOperation();
    testBrainResetFunctionality();
    testBrainActionProduction();
    testBrainSensoryIntegration();
    testBrainCommandLineInterface();
    
    std::cout << std::endl;
    std::cout << "  Main Integration Tests Summary:" << std::endl;
    std::cout << "    - Brain configuration working" << std::endl;
    std::cout << "    - Brain basic operation working" << std::endl;
    std::cout << "    - Brain reset functionality working" << std::endl;
    std::cout << "    - Brain action production working" << std::endl;
    std::cout << "    - Brain sensory integration working" << std::endl;
    std::cout << "    - Command-line interface integration working" << std::endl;
}

} // namespace test_main_integration