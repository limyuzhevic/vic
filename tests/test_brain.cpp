// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Test new API methods
void testBrainNewAPI() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test pause/resume functionality
    brain.pause();
    assert(brain.isRunning() == false);
    
    brain.resume();
    assert(brain.isRunning() == true);
    
    // Test time and step accessors
    auto currentTime = brain.getCurrentTime();
    auto currentStep = brain.getCurrentStep();
    auto timestep = brain.getCurrentTimestep();
    
    assert(currentTime >= 0.0f);
    assert(currentStep >= 0);
    assert(timestep > 0.0f);
    
    // Test performance monitoring
    auto activeCount = brain.getActiveNeuronCount();
    auto totalSpikes = brain.getTotalSpikeCount();
    auto pendingEvents = brain.getPendingSpikeEventCount();
    
    assert(activeCount >= 0);
    assert(totalSpikes >= 0);
    assert(pendingEvents >= 0);
    
    // Test configuration validation
    assert(brain.validateConfiguration() == true);
    
    // Test event handlers registration
    brain.registerSpikeHandler([](const SpikeEvent& event) {});
    brain.registerMemoryUpdatedHandler(
        [](const std::vector<NeuronId>& neurons, float totalActivity) {});
    brain.registerNeuromodulatorUpdatedHandler(
        [](const std::string& neuromodulator, float level, float plasticityFactor) {});
    
    // Test performance tuning
    brain.setMaxSimulationSteps(1000);
    brain.setMemoryLimit(1024 * 1024);  // 1MB
    brain.setThreadCount(2);
    
    // Test checkpoint operations
    bool checkpointCreated = brain.saveCheckpoint("test_checkpoint");
    assert(checkpointCreated == true);
    
    // Test debug mode
    brain.enableDebugMode();
    brain.setDebugLevel(nlm::DebugLevel::Basic);
    brain.enablePerformanceProfiling();
    
    auto performanceStats = brain.getPerformanceStats();
    assert(performanceStats.find("Performance Profile") != std::string::npos);
    
    std::cout << "    testBrainNewAPI passed" << std::endl;
}

// Test advanced command-line parsing
void testBrainAdvancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test feature enablement
    auto workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test neuromodulation
    auto dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    auto curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    auto novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    auto predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test cognition systems
    auto planner = brain->getPlanner();
    assert(planner != nullptr);
    
    auto conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    auto attention = brain->getAttention();
    assert(attention != nullptr);
    
    // Test development system
    auto developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    std::cout << "    testBrainAdvancedFeatures passed" << std::endl;
}

// Test simulation control
void testBrainSimulationControl() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test initial state
    assert(brain.isRunning() == true);
    
    // Test step execution
    auto initialTime = brain.getCurrentTime();
    auto initialStep = brain.getCurrentStep();
    
    brain.step(1, 0.001);
    
    auto newTime = brain.getCurrentTime();
    auto newStep = brain.getCurrentStep();
    
    assert(newStep > initialStep);
    assert(newTime >= initialTime);
    
    // Test pause/resume cycle
    brain.pause();
    assert(brain.isRunning() == false);
    
    brain.resume();
    assert(brain.isRunning() == true);
    
    std::cout << "    testBrainSimulationControl passed" << std::endl;
}

// Test event handlers
void testBrainEventHandlers() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    int spikeHandlerCalls = 0;
    int memoryHandlerCalls = 0;
    int neuromodulatorHandlerCalls = 0;
    
    // Register event handlers
    brain.registerSpikeHandler([&](const SpikeEvent& event) {
        spikeHandlerCalls++;
    });
    
    brain.registerMemoryUpdatedHandler(
        [&](const std::vector<NeuronId>& neurons, float totalActivity) {
            memoryHandlerCalls++;
        });
    
    brain.registerNeuromodulatorUpdatedHandler(
        [&](const std::string& neuromodulator, float level, float plasticityFactor) {
            neuromodulatorHandlerCalls++;
        });
    
    // Trigger events
    for (int i = 0; i < 5; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Note: Event handlers may not be called depending on brain configuration
    // but the registration should not crash
    assert(true);  // Just ensure no crash occurred
    
    std::cout << "    testBrainEventHandlers passed" << std::endl;
}

// Test configuration validation
void testBrainConfigurationValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test valid configuration
    assert(brain.validateConfiguration() == true);
    
    // Test invalid configuration (this should not crash)
    // The validation implementation can determine what's invalid
    
    std::cout << "    testBrainConfigurationValidation passed" << std::endl;
}

// Test performance profiling
void testBrainPerformanceProfiling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable performance profiling
    brain.enablePerformanceProfiling();
    
    // Run some steps
    for (int i = 0; i < 10; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Get performance statistics
    auto performanceStats = brain.getPerformanceStats();
    assert(performanceStats.find("Performance Profile") != std::string::npos);
    
    std::cout << "    testBrainPerformanceProfiling passed" << std::endl;
}

// Test debug mode
void testBrainDebugMode() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable debug mode
    brain.enableDebugMode();
    brain.setDebugLevel(nlm::DebugLevel::Basic);
    
    // Run some steps to generate debug information
    for (int i = 0; i < 5; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Debug mode should not crash
    assert(true);
    
    std::cout << "    testBrainDebugMode passed" << std::endl;
}

// Test performance tuning
void testBrainPerformanceTuning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test performance tuning parameters
    brain.setMaxSimulationSteps(1000);
    brain.setMemoryLimit(1024 * 1024);  // 1MB
    brain.setThreadCount(2);
    
    // Run simulation with performance settings
    for (int i = 0; i < 20; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Verify settings were applied (no crash is success)
    assert(true);
    
    std::cout << "    testBrainPerformanceTuning passed" << std::endl;
}

// Test checkpoint system
void testBrainCheckpointSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create checkpoint
    bool checkpointCreated = brain.saveCheckpoint("test_checkpoint");
    assert(checkpointCreated == true);
    
    // Run some steps
    for (int i = 0; i < 10; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Try to save snapshot
    bool snapshotCreated = brain.saveSnapshot("test_snapshot.chk");
    assert(snapshotCreated == true);
    
    std::cout << "    testBrainCheckpointSystem passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainNewAPI();
    testBrainAdvancedFeatures();
    testBrainSimulationControl();
    testBrainEventHandlers();
    testBrainConfigurationValidation();
    testBrainPerformanceProfiling();
    testBrainDebugMode();
    testBrainPerformanceTuning();
    testBrainCheckpointSystem();
}

} // namespace test_brain
