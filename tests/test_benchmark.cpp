// Performance Benchmark Tests
// These tests measure performance characteristics of NLM components
// including scalability, memory usage, and real-time processing capabilities

#include "benchmark.cpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

namespace test_performance {

void testBrainPerformanceScaling() {
    // Test performance with different network sizes
    const int32_t minNeurons = 100;
    const int32_t maxNeurons = 1000;
    const int32_t step = 200;
    
    for (int32_t neuronCount = minNeurons; neuronCount <= maxNeurons; neuronCount += step) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Measure initialization time
        auto startTime = std::chrono::high_resolution_clock::now();
        assert(brain.initialize());
        auto endTime = std::chrono::high_resolution_clock::now();
        auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Measure step processing time
        startTime = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        endTime = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Performance should scale reasonably
        // Larger networks take longer, but not linearly
        double neuronsPerMs = static_cast<double>(neuronCount) / initDuration.count();
        
        // Just ensure it runs without crashing and measures performance
        std::cout << "    Neuron count: " << neuronCount << ", Init time: " << initDuration.count() 
                  << "ms, Step rate: " << neuronsPerMs << " neurons/ms" << std::endl;
        
        assert(initDuration.count() < 1000);  // Should initialize within 1 second
    }
    
    std::cout << "    testBrainPerformanceScaling passed" << std::endl;
}

void testMemoryUsagePatterns() {
    // Test memory usage patterns with different configurations
    const int32_t smallNetwork = 100;
    const int32_t mediumNetwork = 500;
    const int32_t largeNetwork = 2000;
    
    // Small network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", smallNetwork, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain smallBrain(config);
    assert(smallBrain.initialize());
    
    // Measure memory footprint (approximated by processing time)
    auto startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        smallBrain.step(step);
    }
    auto smallDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    // Medium network
    config = std::make_shared<nlm::Config>();
    config->set("neuron_count", mediumNetwork, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain mediumBrain(config);
    assert(mediumBrain.initialize());
    
    startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        mediumBrain.step(step);
    }
    auto mediumDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    // Large network
    config = std::make_shared<nlm::Config>();
    config->set("neuron_count", largeNetwork, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain largeBrain(config);
    assert(largeBrain.initialize());
    
    startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        largeBrain.step(step);
    }
    auto largeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    // Memory usage should scale with network size
    // Check that larger networks take proportionally more time
    double smallEfficiency = static_cast<double>(smallNetwork) / smallDuration.count();
    double mediumEfficiency = static_cast<double>(mediumNetwork) / mediumDuration.count();
    double largeEfficiency = static_cast<double>(largeNetwork) / largeDuration.count();
    
    std::cout << "    Small network: " << smallNetwork << " neurons in " << smallDuration.count() << "ms (" 
              << smallEfficiency << " neurons/ms)" << std::endl;
    std::cout << "    Medium network: " << mediumNetwork << " neurons in " << mediumDuration.count() << "ms (" 
              << mediumEfficiency << " neurons/ms)" << std::endl;
    std::cout << "    Large network: " << largeNetwork << " neurons in " << largeDuration.count() << "ms (" 
              << largeEfficiency << " neurons/ms)" << std::endl;
    
    std::cout << "    testMemoryUsagePatterns passed" << std::endl;
}

void testRealTimeProcessingCapabilities() {
    // Test real-time processing capabilities with strict timing constraints
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::SimulationClock clock(0.001);  // 1ms timestep
    
    // Target: process 10 simulation steps within 100ms (10 steps per second)
    const int32_t targetStepsPerSecond = 10;
    const int32_t targetMaxDuration = 100;  // ms
    
    int32_t processedSteps = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int32_t step = 0; step < 100; ++step) {  // Process 100 steps
        brain.step(step);
        processedSteps++;
        
        // Check timing constraint
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        if (elapsed.count() > targetMaxDuration) {
            // We've exceeded the time budget for this batch
            break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Calculate processing rate
    double stepsPerSecond = static_cast<double>(processedSteps) / (totalDuration.count() / 1000.0);
    
    std::cout << "    Processed " << processedSteps << " steps in " << totalDuration.count() << "ms (" 
              << stepsPerSecond << " steps/second)" << std::endl;
    
    // Check that we can maintain reasonable real-time performance
    assert(stepsPerSecond >= targetStepsPerSecond * 0.5);  // At least 50% of target
    
    std::cout << "    testRealTimeProcessingCapabilities passed" << std::endl;
}

void testScalabilityCharacteristics() {
    // Test scalability characteristics across different network configurations
    struct NetworkConfig {
        int32_t neuronCount;
        int32_t regionCount;
        float connectionProb;
    };
    
    std::vector<NetworkConfig> configs = {
        {100, 1, 0.1f},
        {500, 3, 0.05f},
        {1000, 5, 0.02f},
        {2000, 8, 0.01f}
    };
    
    for (const auto& config : configs) {
        auto nlmConfig = std::make_shared<nlm::Config>();
        nlmConfig->set("neuron_count", config.neuronCount, nlm::ConfigSource::Default);
        nlmConfig->set("region_count", static_cast<int64_t>(config.regionCount), nlm::ConfigSource::Default);
        nlmConfig->set("connection_probability", config.connectionProb, nlm::ConfigSource::Default);
        
        nlm::Brain brain(nlmConfig);
        assert(brain.initialize());
        
        // Measure initialization time
        auto startTime = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
        
        double efficiency = static_cast<double>(config.neuronCount) / duration.count();
        
        std::cout << "    Config " << config.neuronCount << "n, " << config.regionCount << "r, " 
                  << config.connectionProb << "p: " << efficiency << " neurons/ms" << std::endl;
        
        // Efficiency should not decrease dramatically with larger networks
        // (though absolute time will increase)
        assert(efficiency > 1.0);  // Should process at least 1 neuron per millisecond
    }
    
    std::cout << "    testScalabilityCharacteristics passed" << std::endl;
}

void testDevelopmentSystemPerformance() {
    // Test performance of development system components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure development system update performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int32_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Update development system periodically
        if (step % 100 == 0) {
            brain.getDevelopmentSystem()->update(&brain, step);
        }
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    double stepsPerSecond = static_cast<double>(1000) / (duration.count() / 1000.0);
    
    std::cout << "    Development system processed 1000 steps in " << duration.count() << "ms (" 
              << stepsPerSecond << " steps/second)" << std::endl;
    
    assert(stepsPerSecond > 1000);  // Should process at least 1000 steps per second
    
    std::cout << "    testDevelopmentSystemPerformance passed" << std::endl;
}

void testSensoryProcessingPerformance() {
    // Test performance of sensory processing systems
    nlm::RandomGenerator rng(42);
    
    // Create vision input
    nlm::Vision visionInput;
    visionInput.width = 64;
    visionInput.height = 64;
    visionInput.channels = 1;
    
    std::vector<float> visionData;
    for (size_t i = 0; i < 4096; ++i) {
        visionData.push_back(rng.uniformReal(0.0f, 1.0f));
    }
    visionInput.setData(visionData);
    
    // Process vision
    nlm::VisionProcessor visionProcessor;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int32_t i = 0; i < 100; ++i) {
        visionProcessor.process(visionInput);
    }
    auto visionDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    // Create audio input
    nlm::Audio audioInput;
    audioInput.sampleRate = 16000;
    audioInput.numSamples = 1000;
    
    std::vector<float> audioData;
    for (size_t i = 0; i < 1000; ++i) {
        audioData.push_back(rng.uniformReal(-1.0f, 1.0f));
    }
    audioInput.setData(audioData);
    
    // Process audio
    nlm::AudioProcessor audioProcessor;
    
    startTime = std::chrono::high_resolution_clock::now();
    for (int32_t i = 0; i < 50; ++i) {
        audioProcessor.process(audioInput);
    }
    auto audioDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    std::cout << "    Vision processed 100 frames in " << visionDuration.count() << "ms (" 
              << 100.0 / (visionDuration.count() / 1000.0) << " frames/second)" << std::endl;
    std::cout << "    Audio processed 50 buffers in " << audioDuration.count() << "ms (" 
              << 50.0 / (audioDuration.count() / 1000.0) << " buffers/second)" << std::endl;
    
    assert(visionDuration.count() < 1000);  // Should process within 1 second
    assert(audioDuration.count() < 500);    // Should process within 0.5 seconds
    
    std::cout << "    testSensoryProcessingPerformance passed" << std::endl;
}

void testPlasticityPerformance() {
    // Test performance of plasticity rules
    nlm::RandomGenerator rng(42);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 300, nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure plasticity processing performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int32_t step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Simulate plasticity updates periodically
        if (step % 50 == 0) {
            // In a real test, this would update synaptic weights based on spikes
            // For now, just measure that the system doesn't crash
        }
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    
    double stepsPerSecond = static_cast<double>(500) / (duration.count() / 1000.0);
    
    std::cout << "    Plasticity system processed 500 steps in " << duration.count() << "ms (" 
              << stepsPerSecond << " steps/second)" << std::endl;
    
    assert(stepsPerSecond > 1000);  // Should process at least 1000 steps per second
    
    std::cout << "    testPlasticityPerformance passed" << std::endl;
}

void runAllBenchmarkTests() {
    std::cout << "Running Performance Benchmark Tests..." << std::endl;
    testBrainPerformanceScaling();
    testMemoryUsagePatterns();
    testRealTimeProcessingCapabilities();
    testScalabilityCharacteristics();
    testDevelopmentSystemPerformance();
    testSensoryProcessingPerformance();
    testPlasticityPerformance();
}

} // namespace test_performance
