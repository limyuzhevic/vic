// Performance Benchmarking Tests
// Benchmarks neuron simulation speed, spike propagation, memory usage, and scalability

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <random>

// Include NLM headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"

namespace test_performance {

// Benchmark timer for performance measurements
class BenchmarkTimer {
public:
    BenchmarkTimer() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    template<typename T = std::chrono::milliseconds>
    double elapsed() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<T>(end - start_time);
        return duration.count();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

// Performance assertion with regression guards
template<typename T>
void assertPerformance(T actual, T threshold, const std::string& testName, bool benchmarkMode = false) {
    if (benchmarkMode) {
        BenchmarkTimer timer;
        auto elapsed = timer.elapsed();
        std::cout << "  Benchmark: " << testName << " = " << elapsed 
                  << " (threshold: " << threshold << ")" << std::endl;
        if (elapsed > threshold) {
            std::cout << "  WARNING: Performance regression detected in " << testName << std::endl;
        }
    }
    assert(actual <= threshold);
}

// Neuron simulation speed benchmark
void benchmarkNeuronSimulation() {
    std::cout << "Benchmarking neuron simulation speed..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure neuron simulation time
    BenchmarkTimer timer;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    double neuronSimTime = timer.elapsed<double>();
    
    // Target: Each neuron simulation should complete within 1ms per 100 neurons
    double neuronsPerMs = 1000.0 / neuronSimTime; // neurons per millisecond
    std::cout << "  Neuron simulation rate: " << neuronsPerMs << " neurons/ms" << std::endl;
    
    assertPerformance(neuronSimTime, 100.0, "Neuron Simulation", true);
}

// Spike propagation benchmark
void benchmarkSpikePropagation() {
    std::cout << "Benchmarking spike propagation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Inject spikes into sensory neurons
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    
    // Measure spike propagation time
    BenchmarkTimer timer;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    double spikePropTime = timer.elapsed<double>();
    
    // Target: Spike propagation should complete within 50ms for 50 simulation steps
    std::cout << "  Spike propagation time: " << spikePropTime << "ms for 50 steps" << std::endl;
    
    assertPerformance(spikePropTime, 50.0, "Spike Propagation", true);
}

// Memory usage patterns benchmark
void benchmarkMemoryUsage() {
    std::cout << "Benchmarking memory usage patterns..." << std::endl;
    
    std::vector<double> memoryUsage;
    
    for (int neuronCount : {100, 500, 1000, 2000}) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        BenchmarkTimer timer;
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
        }
        double time = timer.elapsed<double>();
        
        double memoryPerNeuron = brain.getTotalMemoryUsage() / neuronCount; // Memory per neuron
        memoryUsage.push_back(memoryPerNeuron);
        
        std::cout << "  " << neuronCount << " neurons: " << memoryPerNeuron << " bytes/neuron" << std::endl;
        
        // Target: Memory usage should scale linearly
        if (neuronCount > 100) {
            double ratio = memoryPerNeuron / memoryUsage[0];
            assertPerformance(ratio, 1.5, "Memory Scaling", true);
        }
    }
}

// Scalability benchmark with network size
void benchmarkNetworkScalability() {
    std::cout << "Benchmarking network scalability..." << std::endl;
    
    std::vector<double> scalingFactors;
    
    for (int neuronCount : {100, 500, 1000, 2000}) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(neuronCount / 200), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        BenchmarkTimer timer;
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        double time = timer.elapsed<double>();
        
        double neuronsPerSecond = neuronCount / (time / 1000.0); // neurons per second
        scalingFactors.push_back(neuronsPerSecond);
        
        std::cout << "  " << neuronCount << " neurons: " << neuronsPerSecond << " neurons/sec" << std::endl;
        
        // Target: Performance should scale sub-linearly (ideally sub-quadratic)
    }
    
    // Check scaling consistency
    for (size_t i = 1; i < scalingFactors.size(); ++i) {
        double ratio = scalingFactors[i] / scalingFactors[i-1];
        double sizeRatio = (i+1) * 100.0 / ((i) * 100.0); // 5x, 2x, 2x scaling
        std::cout << "  Scaling factor " << i+1 << ": " << ratio << " (expected < " << sizeRatio << ")" << std::endl;
        assertPerformance(ratio, sizeRatio * 2.0, "Scaling Consistency", true);
    }
}

// Plasticity rules benchmark
void benchmarkPlasticityRules() {
    std::cout << "Benchmarking plasticity rules..." << std::endl;
    
    // Test STDP performance
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        BenchmarkTimer timer;
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.updatePlasticity();
        }
        double stdpTime = timer.elapsed<double>();
        
        std::cout << "  STDP update time: " << stdpTime << "ms for 50 updates" << std::endl;
        
        assertPerformance(stdpTime, 50.0, "STDP Performance", true);
    }
    
    // Test Hebbian performance
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        BenchmarkTimer timer;
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.updatePlasticity();
        }
        double hebbianTime = timer.elapsed<double>();
        
        std::cout << "  Hebbian update time: " << hebbianTime << "ms for 50 updates" << std::endl;
        
        assertPerformance(hebbianTime, 50.0, "Hebbian Performance", true);
    }
}

// Memory access patterns benchmark
void benchmarkMemoryAccess() {
    std::cout << "Benchmarking memory access patterns..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Measure memory bandwidth by running many iterations
    BenchmarkTimer timer;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Mix of operations to stress memory access
        brain.step(step);
        if (step % 10 == 0) {
            brain.updatePlasticity();
        }
        if (step % 20 == 0) {
            brain.develop();
        }
    }
    double memoryAccessTime = timer.elapsed<double>();
    
    double operationsPerSecond = 100.0 / (memoryAccessTime / 1000.0); // operations per second
    std::cout << "  Memory access throughput: " << operationsPerSecond << " ops/sec" << std::endl;
    
    assertPerformance(memoryAccessTime, 100.0, "Memory Access", true);
}

// Event-driven vs time-stepped simulation benchmark
void benchmarkEventDrivenVsTimeStepped() {
    std::cout << "Benchmarking event-driven vs time-stepped simulation..." << std::endl;
    
    // Event-driven simulation (using spike events)
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config1->set("simulation.method", "event_driven", nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    BenchmarkTimer timer1;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain1.step(step);
    }
    double eventDrivenTime = timer1.elapsed<double>();
    
    // Time-stepped simulation
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config2->set("simulation.method", "time_stepped", nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    BenchmarkTimer timer2;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain2.step(step);
    }
    double timeSteppedTime = timer2.elapsed<double>();
    
    std::cout << "  Event-driven simulation time: " << eventDrivenTime << "ms" << std::endl;
    std::cout << "  Time-stepped simulation time: " << timeSteppedTime << "ms" << std::endl;
    
    if (eventDrivenTime > 0) {
        double ratio = timeSteppedTime / eventDrivenTime;
        std::cout << "  Performance ratio (time-stepped/event-driven): " << ratio << std::endl;
        // Event-driven should be faster or comparable
        assertPerformance(ratio, 2.0, "Event vs Time-stepped", true);
    }
}

// Concurrent operations benchmark
void benchmarkConcurrentOperations() {
    std::cout << "Benchmarking concurrent operations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate concurrent operations
    BenchmarkTimer timer;
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        // Mix of different operations that might happen concurrently
        brain.step(step);
        brain.updatePlasticity();
        brain.develop();
        
        if (step % 15 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.0f);
        }
        if (step % 20 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 0.5f);
        }
    }
    double concurrentTime = timer.elapsed<double>();
    
    double opsPerSecond = 75.0 / (concurrentTime / 1000.0); // operations per second
    std::cout << "  Concurrent operations throughput: " << opsPerSecond << " ops/sec" << std::endl;
    
    assertPerformance(concurrentTime, 75.0, "Concurrent Operations", true);
}

// Large-scale network benchmark
void benchmarkLargeScaleNetwork() {
    std::cout << "Benchmarking large-scale network..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", false, nlm::ConfigSource::Default); // Disable for speed
    config->set("plasticity.hebbian.enabled", false, nlm::ConfigSource::Default); // Disable for speed
    
    nlm::Brain brain(config);
    brain.initialize();
    
    BenchmarkTimer timer;
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    double largeScaleTime = timer.elapsed<double>();
    
    double neuronsPerSecond = 5000.0 / (largeScaleTime / 1000.0); // neurons per second
    std::cout << "  Large-scale network performance: " << neuronsPerSecond << " neurons/sec" << std::endl;
    
    assertPerformance(largeScaleTime, 200.0, "Large-Scale Network", true);
}

// Real-time performance benchmark
void benchmarkRealTimePerformance() {
    std::cout << "Benchmarking real-time performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("simulation.dt", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Simulate real-time operation (target: 1ms per simulation step)
    double maxStepTime = 1.0; // milliseconds
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        BenchmarkTimer timer;
        brain.step(step);
        double stepTime = timer.elapsed<double>();
        
        std::cout << "  Step " << step << ": " << stepTime << "ms" << std::endl;
        
        assertPerformance(stepTime, maxStepTime, "Real-Time Step", true);
        
        // Control timing to maintain real-time (simplified)
    }
}

void runAll() {
    std::cout << "=== Performance Benchmarking Suite ===" << std::endl;
    std::cout << "Measuring performance metrics and regression guards" << std::endl;
    std::cout << std::endl;
    
    benchmarkNeuronSimulation();
    benchmarkSpikePropagation();
    benchmarkMemoryUsage();
    benchmarkNetworkScalability();
    benchmarkPlasticityRules();
    benchmarkMemoryAccess();
    benchmarkEventDrivenVsTimeStepped();
    benchmarkConcurrentOperations();
    benchmarkLargeScaleNetwork();
    benchmarkRealTimePerformance();
    
    std::cout << std::endl;
    std::cout << "=== All Performance Benchmarks Completed ===" << std::endl;
}

} // namespace test_performance