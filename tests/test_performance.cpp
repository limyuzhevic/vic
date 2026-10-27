#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_performance {

void testPerformanceBenchmark() {
    std::cout << "Testing performance benchmark..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance metrics
    std::vector<double> stepTimes;
    std::vector<double> memoryUsage;
    
    // Warm-up
    for (nlm::SimulationStep i = 0; i < 10; ++i) {
        brain.step(i);
    }
    
    // Benchmark test steps
    const int numSteps = 100;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numSteps; ++i) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        brain.step(i);
        auto stepEnd = std::chrono::high_resolution_clock::now();
        
        double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
        stepTimes.push_back(stepTime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate statistics
    double avgStepTime = 0.0;
    double minStepTime = 1e9;
    double maxStepTime = 0.0;
    
    for (double time : stepTimes) {
        avgStepTime += time;
        minStepTime = std::min(minStepTime, time);
        maxStepTime = std::max(maxStepTime, time);
    }
    avgStepTime /= stepTimes.size();
    
    // Performance assertions
    std::cout << "  Total time for " << numSteps << " steps: " << totalTime << "s" << std::endl;
    std::cout << "  Average step time: " << avgStepTime * 1000 << "ms" << std::endl;
    std::cout << "  Min step time: " << minStepTime * 1000 << "ms" << std::endl;
    std::cout << "  Max step time: " << maxStepTime * 1000 << "ms" << std::endl;
    
    // Performance requirements
    assert(avgStepTime < 0.01);  // Average step should be < 10ms for 1000 neurons
    assert(stepTimes.size() == numSteps);
    
    // Test network metrics
    float excit = brain.getNetworkExcitability();
    float plastic = brain.getNetworkPlasticity();
    size_t maxSpikes = brain.getMaxSpikeCount();
    
    std::cout << "  Network excitability: " << excit << std::endl;
    std::cout << "  Network plasticity: " << plastic << std::endl;
    std::cout << "  Max spike count: " << maxSpikes << std::endl;
    
    assert(excit >= 0.0f && excit <= 10.0f);
    assert(plastic >= 0.0f && plastic <= 1.0f);
    
    std::cout << "    Performance benchmark passed" << std::endl;
}

} // namespace test_performance
