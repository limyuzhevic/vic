// Memory usage benchmarks
#include "../src/performance/MemoryPool.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>
#include <cstdlib>
#include <ctime>

namespace benchmark_memory {

struct MemoryBenchmarkResults {
    size_t memoryUsed;
    size_t allocations;
    double timeMs;
    double allocationTimePerOp;
    std::string description;
    
    MemoryBenchmarkResults(size_t memory, size_t allocs, double time, const std::string& desc)
        : memoryUsed(memory), allocations(allocs), timeMs(time), 
          allocationTimePerOp(time / allocs), description(desc) {}
    
    void print() const {
        std::cout << std::setw(70) << std::left << description
                  << std::setw(20) << std::right << memoryUsed << " bytes"
                  << std::setw(15) << std::right << allocations << " allocs"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(6) << allocationTimePerOp << " μs/op" << std::endl;
    }
};

MemoryBenchmarkResults benchmarkMemoryPoolNeurons(size_t poolSize, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create memory pool for neurons
    nlm::MemoryPool<Neuron> neuronPool(poolSize, poolSize / 10);
    
    size_t allocations = 0;
    size_t deallocations = 0;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate and deallocate neurons
        std::vector<Neuron*> allocatedNeurons;
        
        for (size_t i = 0; i < poolSize; ++i) {
            Neuron* neuron = neuronPool.allocate();
            if (neuron) {
                allocatedNeurons.push_back(neuron);
                allocations++;
                
                // Initialize neuron (simulate typical usage)
                neuron->setId(i);
                neuron->setType(nlm::NeuronType::Excitatory);
                neuron->setPotential(-70.0f);
                neuron->setRefractoryTime(0.0f);
                neuron->setFiringState(nlm::FiringState::Resting);
            }
        }
        
        // Deallocate all neurons
        for (Neuron* neuron : allocatedNeurons) {
            neuronPool.deallocate(neuron);
            deallocations++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    size_t totalAllocations = allocations + deallocations;
    
    return MemoryBenchmarkResults(neuronPool.memoryUsage(), totalAllocations, timeMs,
                                 "Neuron Memory Pool: " + std::to_string(poolSize) + " size, " + std::to_string(iterations) + " iterations");
}

MemoryBenchmarkResults benchmarkMemoryPoolSynapses(size_t poolSize, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create memory pool for synapses
    nlm::MemoryPool<Synapse> synapsePool(poolSize, poolSize / 10);
    
    size_t allocations = 0;
    size_t deallocations = 0;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate and deallocate synapses
        std::vector<Synapse*> allocatedSynapses;
        
        for (size_t i = 0; i < poolSize; ++i) {
            Synapse* synapse = synapsePool.allocate();
            if (synapse) {
                allocatedSynapses.push_back(synapse);
                allocations++;
                
                // Initialize synapse (simulate typical usage)
                synapse->setId(i);
                synapse->setSourceId(i);
                synapse->setDestinationId(i + 1);
                synapse->setWeight(0.5f);
                synapse->setDelay(10);
                synapse->setType(nlm::SynapseType::Excitatory);
            }
        }
        
        // Deallocate all synapses
        for (Synapse* synapse : allocatedSynapses) {
            synapsePool.deallocate(synapse);
            deallocations++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    size_t totalAllocations = allocations + deallocations;
    
    return MemoryBenchmarkResults(synapsePool.memoryUsage(), totalAllocations, timeMs,
                                 "Synapse Memory Pool: " + std::to_string(poolSize) + " size, " + std::to_string(iterations) + " iterations");
}

MemoryBenchmarkResults benchmarkDynamicAllocation(size_t objectCount, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t totalMemory = 0;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Dynamic allocation using std::vector
        std::vector<nlm::NeuronState> neurons;
        neurons.reserve(objectCount);
        
        // Allocate objects
        for (size_t i = 0; i < objectCount; ++i) {
            neurons.emplace_back();
            allocations++;
            totalMemory += sizeof(nlm::NeuronState);
            
            // Initialize
            neurons.back().potential = -70.0f + static_cast<float>(i) * 0.01f;
            neurons.back().refractoryTime = 0.0f;
            neurons.back().isSpiking = false;
        }
        
        // Simulate computation
        float sum = 0.0f;
        for (const auto& neuron : neurons) {
            sum += neuron.potential;
        }
        
        // Deallocation happens automatically when vector goes out of scope
        deallocations += objectCount;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    size_t totalAllocations = allocations + deallocations;
    
    return MemoryBenchmarkResults(totalMemory, totalAllocations, timeMs,
                                 "Dynamic Allocation: " + std::to_string(objectCount) + " objects, " + std::to_string(iterations) + " iterations");
}

MemoryBenchmarkResults benchmarkMemoryPoolFloat(size_t poolSize, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create memory pool for floats (useful for SIMD)
    nlm::MemoryPool<float> floatPool(poolSize, poolSize / 10);
    
    size_t allocations = 0;
    size_t deallocations = 0;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate and deallocate floats
        std::vector<float*> allocatedFloats;
        
        for (size_t i = 0; i < poolSize; ++i) {
            float* f = floatPool.allocate();
            if (f) {
                allocatedFloats.push_back(f);
                allocations++;
                
                // Initialize with random values
                *f = static_cast<float>(std::rand()) / RAND_MAX;
            }
        }
        
        // Deallocate all floats
        for (float* f : allocatedFloats) {
            floatPool.deallocate(f);
            deallocations++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    size_t totalAllocations = allocations + deallocations;
    
    return MemoryBenchmarkResults(floatPool.memoryUsage(), totalAllocations, timeMs,
                                 "Float Memory Pool: " + std::to_string(poolSize) + " size, " + std::to_string(iterations) + " iterations");
}

MemoryBenchmarkResults benchmarkMemoryPoolVector(size_t poolSize, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create memory pool for vectors (complex objects)
    nlm::MemoryPool<std::vector<float>> vectorPool(poolSize, poolSize / 10);
    
    size_t allocations = 0;
    size_t deallocations = 0;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate and deallocate vectors
        std::vector<std::vector<float>*> allocatedVectors;
        
        for (size_t i = 0; i < poolSize; ++i) {
            std::vector<float>* vec = vectorPool.allocate();
            if (vec) {
                allocatedVectors.push_back(vec);
                allocations++;
                
                // Initialize vector
                vec->resize(100);
                for (size_t j = 0; j < 100; ++j) {
                    (*vec)[j] = static_cast<float>(std::rand()) / RAND_MAX;
                }
            }
        }
        
        // Deallocate all vectors
        for (std::vector<float>* vec : allocatedVectors) {
            vectorPool.deallocate(vec);
            deallocations++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    size_t totalAllocations = allocations + deallocations;
    
    return MemoryBenchmarkResults(vectorPool.memoryUsage(), totalAllocations, timeMs,
                                 "Vector Memory Pool: " + std::to_string(poolSize) + " size, " + std::to_string(iterations) + " iterations");
}

void runMemoryBenchmarks() {
    std::cout << "========================================" << std::endl;
    std::cout << "Memory Usage Benchmarks" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Benchmark memory pool for neurons
    std::cout << "Neuron Memory Pool Benchmarks:" << std::endl;
    std::cout << std::setw(70) << std::left << "Description"
              << std::setw(20) << std::right << "Memory (bytes)"
              << std::setw(15) << std::right << "Allocations"
              << std::setw(15) << std::right << "Time/Op (μs)" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    
    auto memResults1 = benchmarkMemoryPoolNeurons(1000, 50);
    memResults1.print();
    
    auto memResults2 = benchmarkMemoryPoolNeurons(5000, 20);
    memResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark memory pool for synapses
    std::cout << "Synapse Memory Pool Benchmarks:" << std::endl;
    std::cout << std::setw(70) << std::left << "Description"
              << std::setw(20) << std::right << "Memory (bytes)"
              << std::setw(15) << std::right << "Allocations"
              << std::setw(15) << std::right << "Time/Op (μs)" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    
    auto memResults3 = benchmarkMemoryPoolSynapses(1000, 50);
    memResults3.print();
    
    auto memResults4 = benchmarkMemoryPoolSynapses(5000, 20);
    memResults4.print();
    
    std::cout << std::endl;
    
    // Benchmark dynamic allocation
    std::cout << "Dynamic Allocation Benchmarks:" << std::endl;
    std::cout << std::setw(70) << std::left << "Description"
              << std::setw(20) << std::right << "Memory (bytes)"
              << std::setw(15) << std::right << "Allocations"
              << std::setw(15) << std::right << "Time/Op (μs)" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    
    auto memResults5 = benchmarkDynamicAllocation(1000, 50);
    memResults5.print();
    
    auto memResults6 = benchmarkDynamicAllocation(5000, 20);
    memResults6.print();
    
    std::cout << std::endl;
    
    // Benchmark memory pool for floats
    std::cout << "Float Memory Pool Benchmarks:" << std::endl;
    std::cout << std::setw(70) << std::left << "Description"
              << std::setw(20) << std::right << "Memory (bytes)"
              << std::setw(15) << std::right << "Allocations"
              << std::setw(15) << std::right << "Time/Op (μs)" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    
    auto memResults7 = benchmarkMemoryPoolFloat(10000, 100);
    memResults7.print();
    
    auto memResults8 = benchmarkMemoryPoolFloat(50000, 50);
    memResults8.print();
    
    std::cout << std::endl;
    
    // Benchmark memory pool for vectors
    std::cout << "Vector Memory Pool Benchmarks:" << std::endl;
    std::cout << std::setw(70) << std::left << "Description"
              << std::setw(20) << std::right << "Memory (bytes)"
              << std::setw(15) << std::right << "Allocations"
              << std::setw(15) << std::right << "Time/Op (μs)" << std::endl;
    std::cout << std::string(120, '-') << std::endl;
    
    auto memResults9 = benchmarkMemoryPoolVector(100, 50);
    memResults9.print();
    
    auto memResults10 = benchmarkMemoryPoolVector(500, 20);
    memResults10.print();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Memory benchmark completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace benchmark_memory

int main() {
    std::cout << "Running Memory Usage Benchmarks..." << std::endl;
    benchmark_memory::runMemoryBenchmarks();
    return 0;
}
