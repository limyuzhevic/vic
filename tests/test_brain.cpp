// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_brain {

/**
 * Configuration structure for brain tests
 * Centralizes test parameters for better maintainability and flexibility
 */
struct BrainTestConfig {
    size_t neuronCount = 100;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    size_t stepCount = 10;
    uint64_t randomSeed = 42;
    float simulationTimestep = 0.001f;
    bool enableVerboseLogging = false;
};

/**
 * Create a configured test brain with proper error handling
 * 
 * @param config Test configuration
 * @return Initialized brain instance
 */
nlm::Brain createTestBrain(const BrainTestConfig& config) {
    auto brainConfig = std::make_shared<nlm::Config>();
    
    // Set configuration with validation
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), nlm::ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount), nlm::ConfigSource::Default);
    brainConfig->set("connection_probability", config.connectionProbability, nlm::ConfigSource::Default);
    brainConfig->set("random_seed", static_cast<int64_t>(config.randomSeed), nlm::ConfigSource::Default);
    brainConfig->set("simulation_timestep", config.simulationTimestep, nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    
    // Initialize with error checking
    if (!brain.initialize()) {
        std::cerr << "ERROR: Failed to initialize test brain!" << std::endl;
        // Return a default-initialized brain for graceful failure
        return nlm::Brain(std::make_shared<nlm::Config>());
    }
    
    if (config.enableVerboseLogging) {
        brain.logStatus();
    }
    
    return brain;
}

void testBrainCreation() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 1;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Verify brain was created
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 2;
    config.connectionProbability = 0.15f;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 1;
    config.stepCount = 25;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Store initial spike count
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run steps with validation
    for (nlm::SimulationStep step = 0; step < config.stepCount; ++step) {
        brain.step(step, step * config.simulationTimestep);
    }
    
    // Verify brain is still in valid state
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSpikeCount() >= initialSpikes);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    BrainTestConfig config;
    config.neuronCount = 50;
    config.regionCount = 1;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Get initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    
    // Take some steps
    brain.step(0);
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 3;
    
    nlm::Brain brain = createTestBrain(config);
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 1;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Verify brain can produce action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Verify action has valid properties
    assert(action->getType() != nlm::ActionType::Custom || !action->getParameters().empty());
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    BrainTestConfig config;
    config.neuronCount = 100;
    config.regionCount = 1;
    
    nlm::Brain brain = createTestBrain(config);
    
    // Verify statistics are accessible and valid
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    
    // Verify counts are consistent
    assert(brain.getFiringNeuronCount() <= brain.getActiveNeuronCount());
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

/**
 * Run all brain tests with configurable parameters
 * 
 * @param config Global test configuration
 */
void runAll(const BrainTestConfig& config = BrainTestConfig()) {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    if (config.enableVerboseLogging) {
        std::cout << std::endl << "=== All brain tests completed successfully ===" << std::endl;
    }
}

} // namespace test_brain


} // namespace test_brain
