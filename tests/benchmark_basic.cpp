// Performance benchmark for basic neural operations
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/performance/OptimizedNeuronMath.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>

namespace benchmark_basic {

struct BenchmarkResults {
    double timeMs;
    uint64_t operations;
    double opsPerSecond;
    std::string description;
    
    BenchmarkResults(double time, uint64_t ops, const std::string& desc)
        : timeMs(time), operations(ops), opsPerSecond((ops / time) * 1e6), // ops per millisecond
          description(desc) {}
    
    void print() const {
        std::cout << std::setw(60) << std::left << description
                  << std::setw(15) << std::right << timeMs << " ms"
                  << std::setw(20) << std::right << operations << " operations"
                  << std::setw(20) << std::right << std::fixed << std::setprecision(2) << opsPerSecond << " ops/ms" << std::endl;
    }
};

BenchmarkResults benchmarkNeuronUpdates(size_t neuronCount, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create test neurons
    std::vector<nlm::NeuronState> neurons(neuronCount);
    
    // Initialize neurons
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons[i].potential = -70.0f;
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
        neurons[i].spikeCount = 0;
        neurons[i].updateCount = 0;
        neurons[i].lastSpikeTime = 0;
    }
    
    // Perform updates
    for (size_t iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < neuronCount; ++i) {
            // Basic neuron update
            float dt = 0.001f;
            
            // Decay membrane potential
            neurons[i].potential *= std::exp(-dt / 10.0f);
            
            // Check for spike
            if (neurons[i].potential > -50.0f && neurons[i].refractoryTime <= 0.0f) {
                neurons[i].potential = -70.0f; // Reset
                neurons[i].isSpiking = true;
                neurons[i].spikeCount++;
            } else {
                neurons[i].isSpiking = false;
            }
            
            // Update refractory time
            neurons[i].refractoryTime = std::max(0.0f, neurons[i].refractoryTime - dt);
            neurons[i].updateCount++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0; // Convert to milliseconds
    uint64_t operations = neuronCount * iterations;
    
    return BenchmarkResults(timeMs, operations, 
                           "Neuron State Updates: " + std::to_string(neuronCount) + " neurons, " + std::to_string(iterations) + " iterations");
}

BenchmarkResults benchmarkSynapticConnections(size_t neuronCount, size_t connectionsPerNeuron, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create synaptic weight matrix (sparse representation)
    const size_t maxConnections = neuronCount * connectionsPerNeuron;
    std::vector<float> weights(maxConnections, 0.5f);
    
    // Initialize weights with random values
    std::srand(42);
    for (size_t i = 0; i < maxConnections; ++i) {
        weights[i] = static_cast<float>(std::rand()) / RAND_MAX;
    }
    
    // Perform synaptic updates
    for (size_t iter = 0; iter < iterations; ++iter) {
        for (size_t i = 0; i < maxConnections; ++i) {
            // Simple weight update rule
            weights[i] += 0.001f * (1.0f - weights[i]); // LTP
            if (weights[i] > 1.0f) weights[i] = 1.0f;
            
            weights[i] -= 0.0005f * weights[i]; // LTD
            if (weights[i] < 0.0f) weights[i] = 0.0f;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    uint64_t operations = maxConnections * iterations * 4; // 4 operations per weight update
    
    return BenchmarkResults(timeMs, operations,
                           "Synaptic Weight Updates: " + std::to_string(maxConnections) + " weights, " + std::to_string(iterations) + " iterations");
}

BenchmarkResults benchmarkBrainStep(size_t neuronCount, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create a simple brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Perform brain steps
    for (size_t i = 0; i < iterations; ++i) {
        auto step = nlm::SimulationStep{i, static_cast<nlm::Timestamp>(i * 0.001f)};
        brain->step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    uint64_t operations = iterations * neuronCount * 10; // Estimate operations per step
    
    return BenchmarkResults(timeMs, operations,
                           "Brain Step Operations: " + std::to_string(neuronCount) + " neurons, " + std::to_string(iterations) + " steps");
}

BenchmarkResults benchmarkMemoryAllocation(size_t neuronCount, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Benchmark memory allocation and deallocation
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate neuron states
        std::vector<nlm::NeuronState> neurons(neuronCount);
        
        // Use the neurons
        for (size_t i = 0; i < neuronCount; ++i) {
            neurons[i].potential = -70.0f + static_cast<float>(i) * 0.01f;
            neurons[i].refractoryTime = 0.0f;
            neurons[i].isSpiking = false;
        }
        
        // Simulate computation
        float sum = 0.0f;
        for (size_t i = 0; i < neuronCount; ++i) {
            sum += neurons[i].potential;
        }
        
        // Deallocation happens automatically
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    uint64_t operations = neuronCount * iterations * 2; // Allocation + computation
    
    return BenchmarkResults(timeMs, operations,
                           "Memory Allocation: " + std::to_string(neuronCount) + " neurons, " + std::to_string(iterations) + " iterations");
}

void runBenchmarks() {
    std::cout << "========================================" << std::endl;
    std::cout << "Basic Performance Benchmarks" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Benchmark neuron updates with different neuron counts
    std::cout << "Neuron Update Benchmarks:" << std::endl;
    std::cout << std::setw(60) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(20) << std::right << "Operations"
              << std::setw(20) << std::right << "Ops/ms" << std::endl;
    std::cout << std::string(115, '-') << std::endl;
    
    auto results1 = benchmarkNeuronUpdates(1000, 100);
    results1.print();
    
    auto results2 = benchmarkNeuronUpdates(10000, 50);
    results2.print();
    
    auto results3 = benchmarkNeuronUpdates(100000, 10);
    results3.print();
    
    std::cout << std::endl;
    
    // Benchmark synaptic connections
    std::cout << "Synaptic Connection Benchmarks:" << std::endl;
    std::cout << std::setw(60) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(20) << std::right << "Operations"
              << std::setw(20) << std::right << "Ops/ms" << std::endl;
    std::cout << std::string(115, '-') << std::endl;
    
    auto results4 = benchmarkSynapticConnections(1000, 10, 100);
    results4.print();
    
    auto results5 = benchmarkSynapticConnections(5000, 5, 50);
    results5.print();
    
    std::cout << std::endl;
    
    // Benchmark brain operations
    std::cout << "Brain Operation Benchmarks:" << std::endl;
    std::cout << std::setw(60) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(20) << std::right << "Operations"
              << std::setw(20) << std::right << "Ops/ms" << std::endl;
    std::cout << std::string(115, '-') << std::endl;
    
    auto results6 = benchmarkBrainStep(1000, 50);
    results6.print();
    
    auto results7 = benchmarkBrainStep(5000, 20);
    results7.print();
    
    std::cout << std::endl;
    
    // Benchmark memory allocation
    std::cout << "Memory Allocation Benchmarks:" << std::endl;
    std::cout << std::setw(60) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(20) << std::right << "Operations"
              << std::setw(20) << std::right << "Ops/ms" << std::endl;
    std::cout << std::string(115, '-') << std::endl;
    
    auto results8 = benchmarkMemoryAllocation(1000, 100);
    results8.print();
    
    auto results9 = benchmarkMemoryAllocation(5000, 50);
    results9.print();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Benchmark completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace benchmark_basic

int main() {
    std::cout << "Running Basic Performance Benchmarks..." << std::endl;
    benchmark_basic::runBenchmarks();
    return 0;
}
