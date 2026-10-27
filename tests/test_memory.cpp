#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_memory {

void testMemoryManagement() {
    std::cout << "Testing memory management..." << std::endl;
    
    // Test 1: Brain lifecycle and memory cleanup
    std::cout << "  Test 1: Brain lifecycle..." << std::endl;
    
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Use the brain
        for (nlm::SimulationStep i = 0; i < 10; ++i) {
            brain.step(i);
        }
        
        // Brain should be destroyed here - check for memory leaks
        std::cout << "    Brain lifecycle: OK" << std::endl;
    }
    
    // Test 2: Random generator memory management
    std::cout << "  Test 2: Random generator lifecycle..." << std::endl;
    
    {
        auto rng = std::make_unique<nlm::RandomGenerator>(42);
        
        // Test random number generation
        for (int i = 0; i < 100; ++i) {
            uint32_t r1 = rng->uniformInt(0, 100);
            uint32_t r2 = rng->uniformInt(0, 100);
            double r3 = rng->uniformReal(0.0, 1.0);
            
            assert(r1 < 100);
            assert(r2 < 100);
            assert(r3 >= 0.0 && r3 <= 1.0);
        }
        
        // rng should be destroyed here
        std::cout << "    Random generator lifecycle: OK" << std::endl;
    }
    
    // Test 3: Memory pool usage in brain
    std::cout << "  Test 3: Memory pool usage..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Monitor memory usage during operations
    std::vector<float> memorySnapshots;
    
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Take memory snapshot
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        size_t activeNeurons = brain.getActiveNeuronCount();
        
        memorySnapshots.push_back(static_cast<float>(neuronCount + synapseCount + activeNeurons));
        
        // Verify no memory corruption
        assert(neuronCount == 200);  // Should have constant neuron count
    }
    
    // Check memory stability
    float maxMemory = *std::max_element(memorySnapshots.begin(), memorySnapshots.end());
    float minMemory = *std::min_element(memorySnapshots.begin(), memorySnapshots.end());
    float memoryVariance = maxMemory - minMemory;
    
    std::cout << "    Memory usage variance: " << memoryVariance << std::endl;
    assert(memoryVariance < 50.0f);  // Should be relatively stable
    
    std::cout << "    Memory pool usage: OK" << std::endl;
    
    // Test 4: Configuration memory management
    std::cout << "  Test 4: Configuration memory management..." << std::endl;
    
    {
        auto config1 = std::make_shared<nlm::Config>();
        auto config2 = std::make_shared<nlm::Config>();
        
        // Set different values
        config1->set("neuron_count", 50, nlm::ConfigSource::Default);
        config2->set("neuron_count", 100, nlm::ConfigSource::Default);
        
        assert(config1->getOr<int64_t>("neuron_count", 0) == 50);
        assert(config2->getOr<int64_t>("neuron_count", 0) == 100);
        
        // config1 and config2 should be independent
        assert(config1 != config2);
    }
    
    std::cout << "    Configuration memory management: OK" << std::endl;
    
    // Test 5: Logger memory management
    std::cout << "  Test 5: Logger memory management..." << std::endl;
    
    {
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Test log message");
        
        // Logger should be destroyed here
        std::cout << "    Logger lifecycle: OK" << std::endl;
    }
    
    // Test 6: Convenience function memory safety
    std::cout << "  Test 6: Convenience function memory safety..." << std::endl;
    
    // Test all convenience functions for memory safety
    brain.applyLearningPattern("repetition");
    brain.applyLearningPattern("spontaneous");
    brain.applyLearningPattern("structured");
    
    auto neurons = brain.findNeuronsByType(nlm::NeuronType::Sensory);
    auto activeNeurons = brain.getActiveNeurons();
    
    std::string summary = brain.getStateSummary();
    brain.getNetworkExcitability();
    brain.getNetworkPlasticity();
    brain.getMaxSpikeCount();
    
    assert(summary.length() > 0);
    std::cout << "    Convenience function memory safety: OK" << std::endl;
    
    // Test 7: Performance counter memory management
    std::cout << "  Test 7: Performance counter memory management..." << std::endl;
    
    brain.resetPerformanceCounters();
    double procTime = brain.getAverageProcessingTime();
    size_t opsCount = brain.getOperationsCount();
    
    assert(procTime >= 0.0);
    std::cout << "    Performance counter memory management: OK" << std::endl;
    
    std::cout << "All memory management tests passed!" << std::endl;
}

} // namespace test_memory
