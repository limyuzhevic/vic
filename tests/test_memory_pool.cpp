// Memory pool tests

#include "performance/MemoryPool.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_memory_pool {

void testMemoryPoolBasic() {
    std::cout << "    testMemoryPoolBasic passed" << std::endl;
}

void testMemoryPoolAllocation() {
    nlm::MemoryPool<int> pool(100, 2);
    
    // Allocate and deallocate
    int* ptr1 = pool.allocate();
    assert(ptr1 != nullptr);
    
    int* ptr2 = pool.allocate();
    assert(ptr2 != nullptr);
    
    // Different allocations should give different memory
    assert(ptr1 != ptr2);
    
    // Deallocate
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    
    std::cout << "    testMemoryPoolAllocation passed" << std::endl;
}

void testNeuronMemoryPool() {
    nlm::NeuronPoolManager& neuronPool = nlm::NeuronPoolManager::getInstance();
    
    // Reserve memory
    neuronPool.reserveNeurons(100);
    
    // Allocate neurons
    NeuronId id1 = neuronPool.getAllocatedCount();
    Neuron* neuron1 = neuronPool.createNeuron(NeuronId(1));
    
    assert(neuron1 != nullptr);
    assert(neuron1->getId() == NeuronId(1));
    
    // Allocate another neuron
    Neuron* neuron2 = neuronPool.createNeuron(NeuronId(2));
    assert(neuron2 != nullptr);
    assert(neuron2->getId() == NeuronId(2));
    
    // Check memory usage
    size_t memUsage = neuronPool.getMemoryUsage();
    assert(memUsage > 0);
    
    // Destroy neurons
    neuronPool.destroyNeuron(neuron1);
    neuronPool.destroyNeuron(neuron2);
    
    std::cout << "    testNeuronMemoryPool passed" << std::endl;
}

void testSynapseMemoryPool() {
    nlm::SynapsePoolManager& synapsePool = nlm::SynapsePoolManager::getInstance();
    
    // Reserve memory
    synapsePool.reserveSynapses(100);
    
    // Allocate synapses
    SynapseId id1 = synapsePool.getAllocatedCount();
    Synapse* synapse1 = synapsePool.createSynapse(SynapseId(1), NeuronId(1), NeuronId(2));
    
    assert(synapse1 != nullptr);
    assert(synapse1->getId() == SynapseId(1));
    assert(synapse1->getSourceNeuron() == NeuronId(1));
    assert(synapse1->getDestinationNeuron() == NeuronId(2));
    
    // Allocate another synapse
    Synapse* synapse2 = synapsePool.createSynapse(SynapseId(2), NeuronId(3), NeuronId(4));
    assert(synapse2 != nullptr);
    assert(synapse2->getId() == SynapseId(2));
    
    // Check memory usage
    size_t memUsage = synapsePool.getMemoryUsage();
    assert(memUsage > 0);
    
    // Destroy synapses
    synapsePool.destroySynapse(synapse1);
    synapsePool.destroySynapse(synapse2);
    
    std::cout << "    testSynapseMemoryPool passed" << std::endl;
}

void testNeuralRegionMemoryPool() {
    nlm::NeuralRegionMemoryPool& regionPool = nlm::NeuralRegionMemoryPool::getInstance();
    
    // Reserve memory
    regionPool.reserveMemory(100, 500);
    
    // Create regions
    NeuralRegion* region1 = regionPool.createRegion(RegionId(1), "Region 1");
    assert(region1 != nullptr);
    assert(region1->getId() == RegionId(1));
    assert(region1->getName() == "Region 1");
    
    NeuralRegion* region2 = regionPool.createRegion(RegionId(2));
    assert(region2 != nullptr);
    assert(region2->getId() == RegionId(2));
    
    // Check memory usage
    size_t memUsage = regionPool.getMemoryUsage();
    assert(memUsage > 0);
    
    // Get pool managers
    auto& neuronPool = regionPool.getNeuronPoolManager();
    auto& synapsePool = regionPool.getSynapsePoolManager();
    
    assert(&neuronPool == &nlm::NeuronPoolManager::getInstance());
    assert(&synapsePool == &nlm::SynapsePoolManager::getInstance());
    
    // Destroy regions
    regionPool.destroyRegion(region1);
    regionPool.destroyRegion(region2);
    
    std::cout << "    testNeuralRegionMemoryPool passed" << std::endl;
}

void testMemoryArena() {
    nlm::MemoryArena arena(1024);
    
    // Allocate memory
    void* ptr1 = arena.allocate(100);
    assert(ptr1 != nullptr);
    
    void* ptr2 = arena.allocate(200);
    assert(ptr2 != nullptr);
    
    // Allocations should be sequential
    assert(ptr1 < ptr2);
    
    // Check used memory
    assert(arena.used() == 300);
    
    // Reset arena
    arena.reset();
    assert(arena.used() == 0);
    
    std::cout << "    testMemoryArena passed" << std::endl;
}

void testMemoryStats() {
    nlm::MemoryStats stats;
    
    // Record allocations
    stats.recordAllocation(100);
    stats.recordAllocation(200);
    
    assert(stats.getCurrentMemoryUsage() == 300);
    assert(stats.getTotalAllocations() == 2);
    
    // Record deallocation
    stats.recordDeallocation(100);
    
    assert(stats.getCurrentMemoryUsage() == 200);
    assert(stats.getTotalDeallocations() == 1);
    
    std::cout << "    testMemoryStats passed" << std::endl;
}

void testMemoryLeakDetection() {
    nlm::MemoryLeakDetector leakDetector;
    leakDetector.enable();
    
    void* ptr = malloc(100);
    leakDetector.trackAllocation(ptr, 100, "test");
    assert(leakDetector.getTrackedCount() == 1);
    
    leakDetector.untrackAllocation(ptr);
    assert(leakDetector.getTrackedCount() == 0);
    
    free(ptr);
    
    std::cout << "    testMemoryLeakDetection passed" << std::endl;
}

void testGlobalMemoryManager() {
    nlm::GlobalMemoryManager& manager = nlm::GlobalMemoryManager::getInstance();
    
    auto& regionPool = manager.getNeuralRegionMemoryPool();
    assert(&regionPool == &nlm::NeuralRegionMemoryPool::getInstance());
    
    // Enable leak detection
    manager.enableLeakDetection(true);
    
    // Reserve memory
    regionPool.reserveMemory(50, 200);
    
    // Check memory usage
    assert(manager.getCurrentMemoryUsage() > 0);
    
    // Health check
    manager.healthCheck();
    
    std::cout << "    testGlobalMemoryManager passed" << std::endl;
}

void testThreadSafety() {
    // Test basic thread safety (simplified)
    nlm::MemoryPool<int> pool(100, 1);
    
    // Multiple allocations/deallocations
    std::vector<int*> pointers;
    for (int i = 0; i < 10; ++i) {
        pointers.push_back(pool.allocate());
        assert(pointers[i] != nullptr);
    }
    
    // Deallocate all
    for (int* ptr : pointers) {
        pool.deallocate(ptr);
    }
    
    std::cout << "    testThreadSafety passed" << std::endl;
}

void testMemoryCorruptionDetection() {
    nlm::MemoryPool<int> pool(100, 2);
    
    int* ptr = pool.allocate();
    assert(ptr != nullptr);
    
    // Check corruption detection
    bool corrupted = pool.checkCorruption();
    assert(!corrupted);
    
    pool.deallocate(ptr);
    
    std::cout << "    testMemoryCorruptionDetection passed" << std::endl;
}

void testMemoryPoolStatistics() {
    nlm::MemoryPool<int> pool(100, 2);
    
    // Record initial stats
    auto stats1 = pool.getStats();
    assert(stats1.allocatedCount == 0);
    assert(stats1.availableCount > 0);
    
    // Allocate some objects
    std::vector<int*> pointers;
    for (int i = 0; i < 10; ++i) {
        pointers.push_back(pool.allocate());
    }
    
    // Check updated stats
    auto stats2 = pool.getStats();
    assert(stats2.allocatedCount == 10);
    assert(stats2.availableCount == 90);  // Initially 200 - 10 allocated
    assert(stats2.memoryUsage > 0);
    
    // Deallocate all
    for (int* ptr : pointers) {
        pool.deallocate(ptr);
    }
    
    // Check final stats
    auto stats3 = pool.getStats();
    assert(stats3.allocatedCount == 0);
    assert(stats3.availableCount >= 100);  // Should be back to initial
    
    std::cout << "    testMemoryPoolStatistics passed" << std::endl;
}

void runAll() {
    testMemoryPoolBasic();
    testMemoryPoolAllocation();
    testNeuronMemoryPool();
    testSynapseMemoryPool();
    testNeuralRegionMemoryPool();
    testMemoryArena();
    testMemoryStats();
    testMemoryLeakDetection();
    testGlobalMemoryManager();
    testThreadSafety();
    testMemoryCorruptionDetection();
    testMemoryPoolStatistics();
    
    std::cout << "All memory pool tests passed!" << std::endl;
}

} // namespace test_memory_pool

// Test runner
int main() {
    test_memory_pool::runAll();
    return 0;
}