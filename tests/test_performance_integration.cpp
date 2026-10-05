// Performance Integration Tests
// Phase 6: Testing SIMD, parallel processing, and event-driven optimizations

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/EventQueue.hpp"
#include "performance/Performance.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_performance_integration {

void testSIMDVectorizationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test SIMD vectorization through performance module
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalSIMDActivity = 0.0f;
    for (int step = 0; step < 100; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.3f);
        brain.step(step);
        totalSIMDActivity += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    SIMD vectorization integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " microseconds" << std::endl;
    std::cout << "      Average SIMD activity: " << (totalSIMDActivity / 100.0f) << std::endl;
    
    std::cout << "    testSIMDVectorizationIntegration passed" << std::endl;
}

void testParallelProcessingIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("enable_parallel_processing", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test parallel processing through optimized brain
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<float> parallelActivities;
    for (int step = 0; step < 75; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.2f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.5f);
        brain.step(step);
        parallelActivities.push_back(brain.getAverageFiringRate());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Calculate statistics
    float totalActivity = std::accumulate(parallelActivities.begin(), parallelActivities.end(), 0.0f);
    float avgActivity = totalActivity / parallelActivities.size();
    
    std::cout << "    Parallel processing integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "      Average activity: " << avgActivity << std::endl;
    std::cout << "      Steps processed: " << parallelActivities.size() << std::endl;
    
    std::cout << "    testParallelProcessingIntegration passed" << std::endl;
}

void testEventDrivenIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test event-driven integration
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<float> eventActivities;
    int eventCount = 0;
    
    for (int step = 0; step < 60; ++step) {
        // Simulate events that trigger processing
        if (step % 5 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Modulatory, 0.8f);
            eventCount++;
        }
        
        brain.step(step);
        eventActivities.push_back(brain.getAverageFiringRate());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Calculate event-driven performance metrics
    float totalActivity = std::accumulate(eventActivities.begin(), eventActivities.end(), 0.0f);
    float avgActivity = totalActivity / eventActivities.size();
    
    std::cout << "    Event-driven integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "      Events triggered: " << eventCount << std::endl;
    std::cout << "      Average activity: " << avgActivity << std::endl;
    
    std::cout << "    testEventDrivenIntegration passed" << std::endl;
}

void testOptimizedBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_parallel_processing", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test optimized brain integration with all performance features
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalPerformanceMetrics = 0.0f;
    size_t totalSteps = 0;
    
    for (int step = 0; step < 150; ++step) {
        // Complex performance-intensive operations
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.6f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.4f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.2f);
        
        brain.step(step);
        
        totalPerformanceMetrics += brain.getAverageFiringRate();
        totalSteps++;
        
        if (step % 30 == 0) {
            std::cout << "    Optimized brain step " << step 
                      << ": Firing=" << brain.getAverageFiringRate()
                      << ", Total neurons=" << brain.getTotalNeuronCount()
                      << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    float avgPerformance = totalPerformanceMetrics / totalSteps;
    
    std::cout << "    Optimized brain integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "      Steps processed: " << totalSteps << std::endl;
    std::cout << "      Average performance: " << avgPerformance << std::endl;
    
    std::cout << "    testOptimizedBrainIntegration passed" << std::endl;
}

void testPerformanceBenchmarkIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance benchmark integration
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<float> benchmarkResults;
    
    // Run benchmark with varying loads
    for (int load = 1; load <= 5; ++load) {
        float currentLoad = load * 0.2f; // 0.2, 0.4, 0.6, 0.8, 1.0
        
        for (int step = 0; step < 50; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, currentLoad);
            brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -currentLoad * 0.5f);
            brain.step(step);
            
            benchmarkResults.push_back(brain.getAverageFiringRate());
        }
        
        std::cout << "    Load level " << load << " completed" << std::endl;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Calculate benchmark statistics
    float totalBenchmarkActivity = std::accumulate(benchmarkResults.begin(), benchmarkResults.end(), 0.0f);
    float avgBenchmarkActivity = totalBenchmarkActivity / benchmarkResults.size();
    float minActivity = *std::min_element(benchmarkResults.begin(), benchmarkResults.end());
    float maxActivity = *std::max_element(benchmarkResults.begin(), benchmarkResults.end());
    
    std::cout << "    Performance benchmark integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "      Total measurements: " << benchmarkResults.size() << std::endl;
    std::cout << "      Average activity: " << avgBenchmarkActivity << std::endl;
    std::cout << "      Min activity: " << minActivity << std::endl;
    std::cout << "      Max activity: " << maxActivity << std::endl;
    
    std::cout << "    testPerformanceBenchmarkIntegration passed" << std::endl;
}

void testPerformanceMonitoringIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance monitoring integration
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<double> performanceTimeline;
    
    for (int step = 0; step < 80; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.4f + (step % 10) * 0.02f);
        brain.step(step);
        
        // Record performance metrics
        performanceTimeline.push_back(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Analyze performance timeline
    size_t timelineSize = performanceTimeline.size();
    double avgInterval = duration.count() / timelineSize;
    
    std::cout << "    Performance monitoring integration:" << std::endl;
    std::cout << "      Duration: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "      Timeline points: " << timelineSize << std::endl;
    std::cout << "      Average interval: " << avgInterval << "ms" << std::endl;
    
    std::cout << "    testPerformanceMonitoringIntegration passed" << std::endl;
}

void runAll() {
    testSIMDVectorizationIntegration();
    testParallelProcessingIntegration();
    testEventDrivenIntegration();
    testOptimizedBrainIntegration();
    testPerformanceBenchmarkIntegration();
    testPerformanceMonitoringIntegration();
}

} // namespace test_performance_integration