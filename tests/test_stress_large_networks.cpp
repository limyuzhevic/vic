// Stress Testing for Large Networks
// Tests the NLM system with 10,000+ neurons

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_large_networks {

void testStressTest10kNeurons() {
    std::cout << "    Testing stress test with 10,000 neurons..." << std::endl;
    
    // Create configuration for large network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("max_simulation_steps", 50000, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize
    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = brain.initialize();
    auto initTime = std::chrono::high_resolution_clock::now();
    
    assert(success && "Failed to initialize large network");
    assert(brain.getRegionCount() == 10);
    assert(brain.getTotalNeuronCount() >= 10000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Run simulation steps
    size_t stepsRun = 0;
    size_t maxSteps = 50000;
    
    auto stepStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < maxSteps; ++i) {
        brain.step();
        stepsRun++;
        
        // Monitor memory usage
        if (i % 10000 == 0) {
            std::cout << "    Step " << i << ", " << brain.getTotalNeuronCount() << " neurons, " 
                      << brain.getTotalSynapseCount() << " synapses" << std::endl;
        }
    }
    auto stepEnd = std::chrono::high_resolution_clock::now();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    // Calculate performance metrics
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(initTime - startTime);
    auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart);
    
    std::cout << "    Initialization time: " << initDuration.count() << "ms" << std::endl;
    std::cout << "    Step execution time: " << stepDuration.count() << "ms for " << stepsRun << " steps" << std::endl;
    std::cout << "    Average step time: " << (stepDuration.count() / (double)stepsRun) << "ms per step" << std::endl;
    std::cout << "    Total simulation time: " << totalDuration.count() << "ms" << std::endl;
    
    // Verify performance is reasonable
    double avgStepTimeMs = stepDuration.count() / (double)stepsRun;
    assert(avgStepTimeMs < 10.0 && "Average step time too high for 10k neurons");
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Stress test with 10,000 neurons passed!" << std::endl;
}

void testStressTest50kNeurons() {
    std::cout << "    Testing stress test with 50,000 neurons..." << std::endl;
    
    // Create configuration for very large network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.02f, nlm::ConfigSource::Default);
    config->set("max_simulation_steps", 20000, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize
    bool success = brain.initialize();
    assert(success && "Failed to initialize 50k neuron network");
    assert(brain.getRegionCount() == 20);
    assert(brain.getTotalNeuronCount() >= 50000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Run simulation steps
    size_t stepsRun = 0;
    size_t maxSteps = 20000;
    
    auto stepStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < maxSteps; ++i) {
        brain.step();
        stepsRun++;
        
        // Monitor every 5000 steps
        if (i % 5000 == 0) {
            std::cout << "    Step " << i << ", " << brain.getTotalNeuronCount() << " neurons, " 
                      << brain.getTotalSynapseCount() << " synapses" << std::endl;
        }
    }
    auto stepEnd = std::chrono::high_resolution_clock::now();
    
    // Calculate performance metrics
    auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart);
    double avgStepTimeMs = stepDuration.count() / (double)stepsRun;
    
    std::cout << "    Step execution time: " << stepDuration.count() << "ms for " << stepsRun << " steps" << std::endl;
    std::cout << "    Average step time: " << avgStepTimeMs << "ms per step" << std::endl;
    
    // Verify performance is reasonable
    assert(avgStepTimeMs < 20.0 && "Average step time too high for 50k neurons");
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Stress test with 50,000 neurons passed!" << std::endl;
}

void testMemoryUsageMonitoring() {
    std::cout << "    Testing memory usage monitoring..." << std::endl;
    
    // Test memory efficiency with medium-sized network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial memory
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run steps and monitor
    for (size_t i = 0; i < 1000; ++i) {
        brain.step();
        
        // Verify neuron count doesn't grow unboundedly (should be stable)
        assert(brain.getTotalNeuronCount() <= initialNeurons + 100 && 
               "Neuron count growing too rapidly");
        
        // Synapses should be relatively stable
        assert(brain.getTotalSynapseCount() <= initialSynapses + 1000 &&
               "Synapse count growing too rapidly");
    }
    
    size_t finalNeurons = brain.getTotalNeuronCount();
    size_t finalSynapses = brain.getTotalSynapseCount();
    
    std::cout << "    Initial: " << initialNeurons << " neurons, " << initialSynapses << " synapses" << std::endl;
    std::cout << "    Final: " << finalNeurons << " neurons, " << finalSynapses << " synapses" << std::endl;
    
    // Neuron count should be stable (within 10%)
    assert((finalNeurons - initialNeurons) < (initialNeurons * 0.1) &&
           "Neuron count too unstable");
    
    std::cout << "    Memory usage monitoring passed!" << std::endl;
}

void testNetworkDensityScaling() {
    std::cout << "    Testing network density scaling..." << std::endl;
    
    std::vector<size_t> neuronCounts = {100, 1000, 5000, 10000};
    std::vector<float> connectionProbabilities = {0.1f, 0.05f, 0.02f, 0.01f};
    
    for (size_t i = 0; i < neuronCounts.size(); ++i) {
        std::cout << "    Testing " << neuronCounts[i] << " neurons with " 
                  << connectionProbabilities[i] * 100 << "% connectivity..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCounts[i]), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(std::min<size_t>(10, neuronCounts[i] / 1000)), nlm::ConfigSource::Default);
        config->set("connection_probability", connectionProbabilities[i], nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        
        std::cout << "      Created " << neuronCount << " neurons, " << synapseCount << " synapses" << std::endl;
        
        // Calculate actual connection density
        float expectedMaxSynapses = static_cast<float>(neuronCount * neuronCount * connectionProbabilities[i]);
        float actualDensity = (static_cast<float>(synapseCount) / expectedMaxSynapses) * 100.0f;
        
        std::cout << "      Connection density: " << actualDensity << "%" << std::endl;
        
        // Density should be reasonably close to target
        assert(actualDensity > connectionProbabilities[i] * 90.0f && 
               "Connection density too low");
        assert(actualDensity < connectionProbabilities[i] * 110.0f && 
               "Connection density too high");
        
        // Run a few steps to ensure stability
        for (size_t j = 0; j < 100; ++j) {
            brain.step();
        }
        
        std::cout << "    " << neuronCounts[i] << " neuron test passed!" << std::endl;
    }
    
    std::cout << "    Network density scaling passed!" << std::endl;
}

} // namespace test_large_networks
