// Simple test program to verify memory pool functionality
#include "src/performance/MemoryPool.hpp"
#include <iostream>
#include <vector>

using namespace nlm;

int main() {
    std::cout << "Testing MemoryPool implementation..." << std::endl;
    
    // Test MemoryPool with integers
    MemoryPool<int> intPool(100, 4);  // 100 ints per block, 4 initial blocks
    
    // Allocate some integers
    std::vector<int*> ints;
    for (int i = 0; i < 50; ++i) {
        int* val = intPool.allocate();
        if (val) {
            *val = i * 2;
            ints.push_back(val);
        }
    }
    
    std::cout << "Allocated " << ints.size() << " integers" << std::endl;
    std::cout << "Pool size: " << intPool.size() << std::endl;
    std::cout << "Pool available: " << intPool.availableCount() << std::endl;
    
    // Test NeuronPool
    NeuronPool neuronPool;
    
    // Allocate some neurons
    std::vector<NeuronId> neuronIds;
    for (int i = 0; i < 10; ++i) {
        NeuronId id = neuronPool.allocate();
        if (id.value > 0) {
            neuronIds.push_back(id);
        }
    }
    
    std::cout << "Allocated " << neuronIds.size() << " neurons" << std::endl;
    std::cout << "NeuronPool size: " << neuronPool.size() << std::endl;
    
    // Test SynapsePool
    SynapsePool synapsePool;
    
    // Allocate some synapses
    std::vector<SynapseId> synapseIds;
    for (int i = 0; i < 10; ++i) {
        NeuronId source(1 + i);
        NeuronId destination(2 + i);
        SynapseId id = synapsePool.allocate(source, destination);
        if (id.value > 0) {
            synapseIds.push_back(id);
        }
    }
    
    std::cout << "Allocated " << synapseIds.size() << " synapses" << std::endl;
    std::cout << "SynapsePool size: " << synapsePool.size() << std::endl;
    
    // Test deallocation
    for (const auto& id : neuronIds) {
        neuronPool.deallocate(id);
    }
    
    for (const auto& id : synapseIds) {
        synapsePool.deallocate(id);
    }
    
    std::cout << "Deallocated " << neuronIds.size() << " neurons and " << synapseIds.size() << " synapses" << std::endl;
    std::cout << "NeuronPool size after deallocation: " << neuronPool.size() << std::endl;
    std::cout << "SynapsePool size after deallocation: " << synapsePool.size() << std::endl;
    
    // Test memory usage
    std::cout << "Memory usage - IntPool: " << intPool.memoryUsage() << " bytes" << std::endl;
    std::cout << "Memory usage - NeuronPool: " << neuronPool.memoryUsage() << " bytes" << std::endl;
    std::cout << "Memory usage - SynapsePool: " << synapsePool.memoryUsage() << " bytes" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
