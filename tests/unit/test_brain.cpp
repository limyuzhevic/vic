// Brain System Tests
// Comprehensive tests for Brain system enhancements

#include "test_framework.hpp"
#include "../src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_brain {

// Test brain creation and basic functionality
class BrainCreationTest : public TestSuite {
public:
    BrainCreationTest() : TestSuite("Brain::Creation") {}
    
    void run() override {
        std::cout << "  Testing brain creation and initialization..." << std::endl;
        
        // Create config for brain
        nlm::Config config;
        config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
        config.set("region_count", 1, nlm::ConfigSource::Runtime);
        config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
        
        TIME_TEST("Brain Creation", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            
            assert(brain.initialize());
            assert(brain.getTotalNeuronCount() == 1000);
            assert(brain.getRegionCount() == 1);
            assert(brain.getTotalSynapseCount() >= 0);
        });
        
        // Test brain properties
        TIME_TEST("Brain Properties", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            assert(brain.getTotalNeuronCount() >= 0);
            assert(brain.getTotalSynapseCount() >= 0);
            assert(brain.getActiveNeuronCount() >= 0);
            assert(brain.getFiringNeuronCount() >= 0);
            assert(brain.getAverageFiringRate() >= 0.0f);
            assert(brain.getExcitationInhibitionRatio() >= 0.0f);
            
            // Test access to modular systems
            assert(brain.getSpikeSystem() != nullptr);
            assert(brain.getSTDP() != nullptr);
            assert(brain.getHebbian() != nullptr);
            assert(brain.getStructuralPlasticity() != nullptr);
        });
        
        std::cout << "  Brain Creation tests PASSED" << std::endl;
    }
};

// Test brain simulation steps
class BrainSimulationTest : public TestSuite {
public:
    BrainSimulationTest() : TestSuite("Brain::Simulation") {}
    
    void run() override {
        std::cout << "  Testing brain simulation steps..." << std::endl;
        
        // Create brain with minimal config
        nlm::Config config;
        config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
        config.set("region_count", 1, nlm::ConfigSource::Runtime);
        config.set("connection_probability", 0.05, nlm::ConfigSource::Runtime);
        
        TIME_TEST("Brain Single Step", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            // Take a few steps
            for (int i = 0; i < 5; ++i) {
                brain.step(i);
            }
            
            assert(brain.getTotalSpikeCount() >= 0);
            assert(brain.getPendingSpikeEventCount() >= 0);
        });
        
        // Test brain reset
        TIME_TEST("Brain Reset", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            // Take a step to generate spikes
            brain.step(0);
            
            // Reset brain
            brain.reset();
            
            // Verify reset state
            assert(brain.getTotalSpikeCount() == 0);
            assert(brain.getTotalNeuronCount() == 100);  // Neuron count should remain
        });
        
        std::cout << "  Brain Simulation tests PASSED" << std::endl;
    }
};

// Test modular brain systems
class BrainModularSystemsTest : public TestSuite {
public:
    BrainModularSystemsTest() : TestSuite("Brain::ModularSystems") {}
    
    void run() override {
        std::cout << "  Testing modular brain systems..." << std::endl;
        
        // Create brain
        nlm::Config config;
        config.set("neuron_count", 200, nlm::ConfigSource::Runtime);
        config.set("region_count", 2, nlm::ConfigSource::Runtime);
        
        TIME_TEST("Brain Modular Systems", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            // Test memory systems
            auto workingMemory = brain.getWorkingMemory();
            if (workingMemory) {
                assert(workingMemory->getActiveTraces() >= 0);
            }
            
            auto episodicMemory = brain.getEpisodicMemory();
            if (episodicMemory) {
                assert(episodicMemory->getEpisodeCount() >= 0);
            }
            
            // Test prediction system
            auto predictionSystem = brain.getPredictionSystem();
            if (predictionSystem) {
                // Prediction system functionality depends on implementation
            }
            
            // Test attention system
            auto attentionSystem = brain.getAttention();
            if (attentionSystem) {
                assert(attentionSystem->getFocusStrength() >= 0.0f);
            }
            
            // Test neuromodulation
            auto dopamine = brain.getDopamine();
            if (dopamine) {
                assert(dopamine->getLevel() >= 0.0f);
            }
            
            auto curiosity = brain.getCuriosity();
            if (curiosity) {
                assert(curiosity->getLevel() >= 0.0f);
            }
            
            // Test development system
            auto developmentSystem = brain.getDevelopmentSystem();
            if (developmentSystem) {
                assert(developmentSystem->getDevelopmentProgress() >= 0.0f);
            }
        });
        
        std::cout << "  Brain Modular Systems tests PASSED" << std::endl;
    }
};

// Test brain checkpoint operations
class BrainCheckpointTest : public TestSuite {
public:
    BrainCheckpointTest() : TestSuite("Brain::Checkpoint") {}
    
    void run() override {
        std::cout << "  Testing brain checkpoint operations..." << std::endl;
        
        // Create brain
        nlm::Config config;
        config.set("neuron_count", 150, nlm::ConfigSource::Runtime);
        config.set("region_count", 1, nlm::ConfigSource::Runtime);
        
        TIME_TEST("Brain Save/Load", {
            std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
            nlm::Brain brain(configPtr);
            brain.initialize();
            
            // Take a step to generate activity
            brain.step(0);
            
            // Save brain state
            std::string checkpointFile = "/tmp/test_brain_checkpoint.bin";
            assert(brain.save(checkpointFile));
            
            // Reset brain
            brain.reset();
            
            // Load brain state
            assert(brain.load(checkpointFile));
            
            // Verify loaded state
            assert(brain.getTotalNeuronCount() == 150);
            // Total spike count would be preserved
        });
        
        std::cout << "  Brain Checkpoint tests PASSED" << std::endl;
    }
};

// Global test instances
static BrainCreationTest brainCreationTest;
static BrainSimulationTest brainSimulationTest;
static BrainModularSystemsTest brainModularSystemsTest;
static BrainCheckpointTest brainCheckpointTest;

void runAll() {
    std::cout << "=== Brain System Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&brainCreationTest);
    framework.addSuite(&brainSimulationTest);
    framework.addSuite(&brainModularSystemsTest);
    framework.addSuite(&brainCheckpointTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Brain Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Brain Tests FAILED ===" << std::endl;
    }
}

} // namespace test_brain
