// Performance test for benchmarking
#include <iostream>
#include <chrono>
#include <cmath>
#include "../src/core/Random/Random.hpp"
#include "../src/brain/Neuron.hpp"

int test_performance() {
    std::cout << "Performance Test: Benchmarking neural operations..." << std::endl;
    
    // Performance test for random number generation
    nlm::Random rng;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate 1 million random numbers
    for (int i = 0; i < 1000000; ++i) {
        volatile float val = rng.randomFloat(0.0f, 1.0f);
        volatile float val2 = rng.randomNormal(0.0f, 1.0f);
        (void)val; (void)val2; // Prevent optimization
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Random generation: 1M calls took " << duration.count() << " ms" << std::endl;
    
    // Performance test for neuron operations
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        nlm::Neuron neuron;
        neuron.setMembranePotential(-70.0f);
        neuron.setRefractoryPeriod(2.0f);
        neuron.update(0.001f);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Neuron operations: 10K calls took " << duration.count() << " ms" << std::endl;
    
    return 0;
}

int main() {
    return test_performance();
}
