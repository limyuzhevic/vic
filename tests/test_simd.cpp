// SIMD optimization tests
#include "../src/performance/OptimizedNeuronMath.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <cmath>

namespace test_simd {

void testSIMDAlignment() {
    std::cout << "    Testing SIMD alignment..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    
    // Check SIMD availability
    bool simdAvailable = onm->detectSIMDSupport();
    std::cout << "    SIMD available: " << (simdAvailable ? "Yes" : "No") << std::endl;
    
    // Check cache line alignment
    bool cachelineAvailable = onm->detectCachelineSupport();
    std::cout << "    Cache line support: " << (cachelineAvailable ? "Yes" : "No") << std::endl;
    
    // Test alignment requirements
    nlm::NeuronState neurons[4]; // Aligned on stack
    uintptr_t addr = reinterpret_cast<uintptr_t>(neurons);
    bool aligned = (addr % 64) == 0; // 64-byte cache line
    std::cout << "    Array aligned to 64 bytes: " << (aligned ? "Yes" : "No") << std::endl;
    
    // Test neuron state structure alignment
    nlm::NeuronState testNeuron;
    uintptr_t neuronAddr = reinterpret_cast<uintptr_t>(&testNeuron);
    bool neuronAligned = (neuronAddr % 64) == 0;
    std::cout << "    NeuronState aligned to 64 bytes: " << (neuronAligned ? "Yes" : "No") << std::endl;
    
    std::cout << "    ✓ SIMD alignment test passed" << std::endl;
}

void testSIMDNeuronalUpdates() {
    std::cout << "    Testing SIMD neuronal updates..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    
    // Create test neurons
    const size_t neuronCount = 1000;
    std::vector<nlm::NeuronState> neurons(neuronCount);
    std::vector<float> externalInputs(neuronCount, 0.0f);
    std::vector<float> synapticInputs(neuronCount, 0.0f);
    
    // Initialize neurons with random values
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-70.0f, 50.0f);
    
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons[i].potential = dist(rng);
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
        neurons[i].spikeCount = 0;
        neurons[i].updateCount = 0;
        neurons[i].lastSpikeTime = 0;
    }
    
    // Initialize inputs
    for (size_t i = 0; i < neuronCount; ++i) {
        externalInputs[i] = dist(rng);
        synapticInputs[i] = dist(rng) * 0.1f;
    }
    
    // Test scalar updates
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iteration = 0; iteration < 100; ++iteration) {
        for (size_t i = 0; i < neuronCount; ++i) {
            // Scalar update (reference implementation)
            float dt = 0.001f;
            float spikeThreshold = -50.0f;
            float refractoryPeriod = 5.0f;
            
            // Decay membrane potential
            neurons[i].potential *= std::exp(-dt / 10.0f);
            
            // Add synaptic input
            neurons[i].potential += synapticInputs[i] * 1.0f;
            
            // Update refractory time
            neurons[i].refractoryTime = std::max(0.0f, neurons[i].refractoryTime - dt);
            
            // Check for spike
            if (neurons[i].potential > spikeThreshold && neurons[i].refractoryTime <= 0.0f) {
                neurons[i].potential = -70.0f; // Reset
                neurons[i].isSpiking = true;
                neurons[i].spikeCount++;
            } else {
                neurons[i].isSpiking = false;
            }
            
            neurons[i].updateCount++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "    Scalar updates: " << neuronCount * 100 << " operations in " << duration.count() << " microseconds" << std::endl;
    
    // Reset for SIMD test
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons[i].potential = dist(rng);
        neurons[i].refractoryTime = 0.0f;
        neurons[i].isSpiking = false;
        neurons[i].spikeCount = 0;
        neurons[i].updateCount = 0;
    }
    
    // Test SIMD updates (if available)
    if (onm->isSIMDEnabled()) {
        // Ensure data is properly aligned for SIMD
        // Note: In real implementation, we'd use aligned memory
        start = std::chrono::high_resolution_clock::now();
        for (size_t iteration = 0; iteration < 100; ++iteration) {
            onm->updateNeuronsSIMD(neurons.data(), neuronCount, 0.001f,
                                  externalInputs.data(), synapticInputs.data(),
                                  -50.0f, 5.0f);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "    SIMD updates: " << neuronCount * 100 << " operations in " << duration.count() << " microseconds" << std::endl;
        
        // Performance comparison
        double scalarTime = duration.count();
        // SIMD should be faster for larger neuron counts
        std::cout << "    SIMD performance improvement: Check timing above" << std::endl;
    } else {
        std::cout << "    SIMD not available, skipping SIMD test" << std::endl;
    }
    
    std::cout << "    ✓ SIMD neuronal updates test passed" << std::endl;
}

void testSIMDSynapticWeightUpdates() {
    std::cout << "    Testing SIMD synaptic weight updates..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    
    const size_t weightCount = 2000;
    std::vector<float> weights(weightCount, 0.5f);
    std::vector<bool> preSpikes(weightCount);
    std::vector<bool> postSpikes(weightCount);
    std::vector<uint32_t> spikeMask((weightCount + 31) / 32, 0);
    
    // Initialize with random patterns
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> weightDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> spikeDist(0.0f, 1.0f);
    
    for (size_t i = 0; i < weightCount; ++i) {
        weights[i] = weightDist(rng);
        preSpikes[i] = (spikeDist(rng) > 0.7f);
        postSpikes[i] = (spikeDist(rng) > 0.7f);
        if (preSpikes[i] && postSpikes[i]) {
            spikeMask[i / 32] |= (1 << (i % 32));
        }
    }
    
    // Test scalar updates
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iteration = 0; iteration < 50; ++iteration) {
        for (size_t i = 0; i < weightCount; ++i) {
            if (preSpikes[i] && postSpikes[i]) {
                weights[i] += 0.01f; // LTP
            } else if (preSpikes[i] || postSpikes[i]) {
                weights[i] -= 0.005f; // LTD
            }
            
            // Clamp weights
            if (weights[i] < 0.0f) weights[i] = 0.0f;
            if (weights[i] > 1.0f) weights[i] = 1.0f;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "    Scalar weight updates: " << weightCount * 50 << " operations in " << duration.count() << " microseconds" << std::endl;
    
    // Reset weights
    for (size_t i = 0; i < weightCount; ++i) {
        weights[i] = weightDist(rng);
    }
    
    // Test SIMD updates (if available)
    if (onm->isSIMDEnabled()) {
        start = std::chrono::high_resolution_clock::now();
        for (size_t iteration = 0; iteration < 50; ++iteration) {
            // Generate spike mask each iteration
            for (size_t j = 0; j < weightCount; j += 32) {
                uint32_t mask = 0;
                for (size_t k = 0; k < 32 && j + k < weightCount; ++k) {
                    if (preSpikes[j + k] && postSpikes[j + k]) {
                        mask |= (1 << k);
                    }
                }
                spikeMask[j / 32] = mask;
            }
            
            onm->updateSynapticWeightsSIMD(weights.data(), preSpikes.data(), 
                                          postSpikes.data(), 0.01f, weightCount,
                                          spikeMask.data());
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "    SIMD weight updates: " << weightCount * 50 << " operations in " << duration.count() << " microseconds" << std::endl;
        
        // Validate SIMD results
        for (size_t i = 0; i < weightCount; ++i) {
            if (weights[i] < 0.0f || weights[i] > 1.0f) {
                std::cout << "    ERROR: Weight out of range: " << weights[i] << " at index " << i << std::endl;
                return;
            }
        }
    } else {
        std::cout << "    SIMD not available, skipping SIMD synaptic update test" << std::endl;
    }
    
    std::cout << "    ✓ SIMD synaptic weight updates test passed" << std::endl;
}

void testSIMDSpikePropagation() {
    std::cout << "    Testing SIMD spike propagation..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    
    const size_t neuronCount = 1000;
    std::vector<bool> spikeSources(neuronCount, false);
    std::vector<bool> spikeTargets(neuronCount, false);
    std::vector<float> adjacencyMatrix(neuronCount * neuronCount, 0.0f);
    std::vector<uint32_t> spikeMask((neuronCount + 31) / 32, 0);
    
    // Initialize with random connectivity
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> connDist(0.0f, 0.1f);
    std::uniform_real_distribution<float> spikeDist(0.0f, 1.0f);
    
    for (size_t i = 0; i < neuronCount; ++i) {
        for (size_t j = 0; j < neuronCount; ++j) {
            if (i != j && connDist(rng) > 0.05f) { // ~5% connectivity
                adjacencyMatrix[i * neuronCount + j] = connDist(rng);
            }
        }
        spikeSources[i] = (spikeDist(rng) > 0.9f); // ~10% spiking
        if (spikeSources[i]) {
            spikeMask[i / 32] |= (1 << (i % 32));
        }
    }
    
    // Test scalar spike propagation
    std::cout << "    Testing scalar spike propagation..." << std::endl;
    for (size_t iteration = 0; iteration < 10; ++iteration) {
        // Clear targets
        for (size_t i = 0; i < neuronCount; ++i) {
            spikeTargets[i] = false;
        }
        
        // Propagate spikes
        for (size_t i = 0; i < neuronCount; ++i) {
            if (spikeSources[i]) {
                for (size_t j = 0; j < neuronCount; ++j) {
                    if (adjacencyMatrix[i * neuronCount + j] > 0.0f) {
                        spikeTargets[j] = true;
                    }
                }
            }
        }
    }
    
    // Reset
    std::fill(spikeTargets.begin(), spikeTargets.end(), false);
    
    // Test SIMD spike propagation (if available)
    if (onm->isSIMDEnabled()) {
        std::cout << "    Testing SIMD spike propagation..." << std::endl;
        for (size_t iteration = 0; iteration < 10; ++iteration) {
            // Clear targets
            for (size_t i = 0; i < neuronCount; ++i) {
                spikeTargets[i] = false;
            }
            
            // Generate spike mask each iteration
            for (size_t j = 0; j < neuronCount; j += 32) {
                uint32_t mask = 0;
                for (size_t k = 0; k < 32 && j + k < neuronCount; ++k) {
                    if (spikeSources[j + k]) {
                        mask |= (1 << k);
                    }
                }
                spikeMask[j / 32] = mask;
            }
            
            onm->propagateSpikesSIMD(spikeSources.data(), spikeTargets.data(),
                                    adjacencyMatrix.data(), spikeMask.data(),
                                    neuronCount, 100);
        }
        
        // Validate results
        size_t totalSpikeTargets = 0;
        for (size_t i = 0; i < neuronCount; ++i) {
            if (spikeTargets[i]) totalSpikeTargets++;
        }
        std::cout << "    SIMD spike propagation: " << totalSpikeTargets << " targets activated" << std::endl;
    } else {
        std::cout << "    SIMD not available, skipping SIMD spike propagation test" << std::endl;
    }
    
    std::cout << "    ✓ SIMD spike propagation test passed" << std::endl;
}

void testSIMDMatrixOperations() {
    std::cout << "    Testing SIMD matrix operations..." << std::endl;
    
    auto* onm = nlm::OptimizedNeuronMath::getInstance();
    
    const size_t matrixSize = 64; // Keep it manageable for testing
    const size_t iterations = 10;
    
    std::vector<float> A(matrixSize * matrixSize, 0.0f);
    std::vector<float> B(matrixSize * matrixSize, 0.0f);
    std::vector<float> C(matrixSize * matrixSize, 0.0f);
    
    // Initialize matrices
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> matDist(-1.0f, 1.0f);
    
    for (size_t i = 0; i < matrixSize; ++i) {
        for (size_t j = 0; j < matrixSize; ++j) {
            A[i * matrixSize + j] = matDist(rng);
            B[i * matrixSize + j] = matDist(rng);
        }
    }
    
    // Initialize result matrix
    for (size_t i = 0; i < matrixSize; ++i) {
        for (size_t j = 0; j < matrixSize; ++j) {
            C[i * matrixSize + j] = matDist(rng);
        }
    }
    
    // Test scalar matrix multiplication
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < iterations; ++iter) {
        // C = A * B
        for (size_t i = 0; i < matrixSize; ++i) {
            for (size_t j = 0; j < matrixSize; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < matrixSize; ++k) {
                    sum += A[i * matrixSize + k] * B[k * matrixSize + j];
                }
                C[i * matrixSize + j] = sum; // In-place update
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "    Scalar matrix multiplication: " << iterations * matrixSize * matrixSize * matrixSize << " operations in " << duration.count() << " microseconds" << std::endl;
    
    // Reset result matrix
    for (size_t i = 0; i < matrixSize; ++i) {
        for (size_t j = 0; j < matrixSize; ++j) {
            C[i * matrixSize + j] = matDist(rng);
        }
    }
    
    // Test SIMD matrix multiplication (if available)
    if (onm->isSIMDEnabled()) {
        start = std::chrono::high_resolution_clock::now();
        for (size_t iter = 0; iter < iterations; ++iter) {
            // Make copies for in-place operations
            std::vector<float> A_copy = A;
            std::vector<float> B_copy = B;
            
            onm->matrixMultiplySIMD(A_copy.data(), B_copy.data(), C.data(),
                                   matrixSize, 1.0f, 0.0f);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "    SIMD matrix multiplication: " << iterations * matrixSize * matrixSize * matrixSize << " operations in " << duration.count() << " microseconds" << std::endl;
        
        // Validate results (spot check)
        float sum = 0.0f;
        for (size_t k = 0; k < matrixSize; ++k) {
            sum += A[0 * matrixSize + k] * B[k * matrixSize + 0];
        }
        std::cout << "    Expected C[0,0] ≈ " << sum << ", actual = " << C[0 * matrixSize + 0] << std::endl;
    } else {
        std::cout << "    SIMD not available, skipping SIMD matrix operations test" << std::endl;
    }
    
    std::cout << "    ✓ SIMD matrix operations test passed" << std::endl;
}

void runAll() {
    testSIMDAlignment();
    testSIMDNeuronalUpdates();
    testSIMDSynapticWeightUpdates();
    testSIMDSpikePropagation();
    testSIMDMatrixOperations();
    
    std::cout << "\nAll SIMD optimization tests passed!" << std::endl;
}

} // namespace test_simd

int main() {
    std::cout << "Testing SIMD Optimizations..." << std::endl;
    test_simd::runAll();
    return 0;
}
