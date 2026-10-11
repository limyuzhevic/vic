#include "Allocator.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <algorithm>
#include <limits>
#include <thread>

namespace nlm {

// Static member definitions
NeuronPool NeuronAllocator::neuronPool_;
std::mutex NeuronAllocator::mutex_;

SynapsePool SynapseAllocator::synapsePool_;
std::mutex SynapseAllocator::mutex_;

MemoryPool<NeuralRegion> NeuralRegionAllocator::regionPool_;
std::mutex NeuralRegionAllocator::mutex_;

MemoryPool<float> VectorFloatAllocator::vectorPool_;
std::mutex VectorFloatAllocator::mutex_;

MemoryPool<NeuronId> NeuronIdVectorAllocator::neuronIdPool_;
std::mutex NeuronIdVectorAllocator::mutex_;

// AllocatorRegistry implementation
AllocatorRegistry::AllocatorRegistry() {
    // Initialize default allocators
    registerAllocator(typeid(NeuronAllocator).name(), &NeuronAllocator::allocate);
    registerAllocator(typeid(SynapseAllocator).name(), &SynapseAllocator::allocate);
    registerAllocator(typeid(NeuralRegionAllocator).name(), &NeuralRegionAllocator::allocate);
    registerAllocator(typeid(VectorFloatAllocator).name(), &VectorFloatAllocator::allocate);
    registerAllocator(typeid(NeuronIdVectorAllocator).name(), &NeuronIdVectorAllocator::allocate);
}

AllocatorRegistry::~AllocatorRegistry() {
    clearAll();
}

AllocatorRegistry& AllocatorRegistry::getInstance() {
    static AllocatorRegistry instance;
    return instance;
}

void AllocatorRegistry::registerAllocator(const std::string& typeName, void* allocator) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    allocators_[typeName] = allocator;
    allocatedCounts_[typeName] = 0;
    availableCounts_[typeName] = 0;
}

void AllocatorRegistry::unregisterAllocator(const std::string& typeName) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    allocators_.erase(typeName);
    allocatedCounts_.erase(typeName);
    availableCounts_.erase(typeName);
}

void* AllocatorRegistry::getAllocatorImpl(const std::string& typeName) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = allocators_.find(typeName);
    if (it != allocators_.end()) {
        return it->second;
    }
    return nullptr;
}

void* AllocatorRegistry::allocate(const std::string& typeName, size_t size) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = allocators_.find(typeName);
    if (it == allocators_.end()) {
        return nullptr;
    }
    
    // For now, we'll use a simple approach - allocate memory directly
    // In a real implementation, you'd call the actual allocator function
    void* ptr = std::malloc(size);
    if (ptr) {
        allocatedCounts_[typeName].fetch_add(1, std::memory_order_relaxed);
    }
    return ptr;
}

void AllocatorRegistry::deallocate(const std::string& typeName, void* ptr, size_t size) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    if (ptr) {
        std::free(ptr);
        allocatedCounts_[typeName].fetch_sub(1, std::memory_order_relaxed);
    }
}

size_t AllocatorRegistry::getTotalAllocated(const std::string& typeName) {
    auto it = allocatedCounts_.find(typeName);
    if (it != allocatedCounts_.end()) {
        return it->second.load(std::memory_order_relaxed);
    }
    return 0;
}

size_t AllocatorRegistry::getAvailable(const std::string& typeName) {
    auto it = availableCounts_.find(typeName);
    if (it != availableCounts_.end()) {
        return it->second.load(std::memory_order_relaxed);
    }
    return 0;
}

size_t AllocatorRegistry::getMemoryUsage(const std::string& typeName) {
    // Simplified memory usage calculation
    // In a real implementation, this would track actual memory usage
    return getTotalAllocated(typeName) * sizeof(float); // Approximate
}

void AllocatorRegistry::reserve(const std::string& typeName, size_t count) {
    // Reserve memory for the allocator
    // In a real implementation, you'd call allocator-specific reserve methods
    // For now, we'll just increment the allocated count
    allocatedCounts_[typeName].fetch_add(count, std::memory_order_relaxed);
}

void AllocatorRegistry::clearAll() {
    std::lock_guard<std::mutex> lock(registryMutex_);
    allocators_.clear();
    allocatedCounts_.clear();
    availableCounts_.clear();
}

// NeuronAllocator implementation
Neuron* NeuronAllocator::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    return neuronPool_.allocate();
}

void NeuronAllocator::deallocate(Neuron* ptr) {
    if (ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        neuronPool_.deallocate(ptr);
    }
}

void NeuronAllocator::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuronPool_.reserve(count);
}

size_t NeuronAllocator::allocatedCount() {
    return neuronPool_.allocatedCount();
}

size_t NeuronAllocator::availableCount() {
    return neuronPool_.availableCount();
}

// SynapseAllocator implementation
Synapse* SynapseAllocator::allocate(NeuronId source, NeuronId destination) {
    std::lock_guard<std::mutex> lock(mutex_);
    // For Synapse, we need to handle the additional initialization
    // with source and destination
    Synapse* synapse = synapsePool_.allocate();
    if (synapse) {
        // Initialize the synapse with the given source and destination
        // Note: In a real implementation, Synapse would need to support
        // construction from NeuronId parameters
    }
    return synapse;
}

void SynapseAllocator::deallocate(Synapse* ptr) {
    if (ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        synapsePool_.deallocate(ptr);
    }
}

void SynapseAllocator::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    synapsePool_.reserve(count);
}

size_t SynapseAllocator::allocatedCount() {
    return synapsePool_.allocatedCount();
}

size_t SynapseAllocator::availableCount() {
    return synapsePool_.availableCount();
}

// NeuralRegionAllocator implementation
NeuralRegion* NeuralRegionAllocator::allocate(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    NeuralRegion* region = regionPool_.allocate();
    if (region) {
        // Initialize the neural region with the given name
        new (region) NeuralRegion(name);
    }
    return region;
}

void NeuralRegionAllocator::deallocate(NeuralRegion* ptr) {
    if (ptr) {
        // Call destructor before deallocation
        ptr->~NeuralRegion();
        std::lock_guard<std::mutex> lock(mutex_);
        regionPool_.deallocate(ptr);
    }
}

void NeuralRegionAllocator::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    regionPool_.reserve(count);
}

size_t NeuralRegionAllocator::allocatedCount() {
    return regionPool_.allocatedCount();
}

size_t NeuralRegionAllocator::availableCount() {
    return regionPool_.availableCount();
}

// VectorFloatAllocator implementation
float* VectorFloatAllocator::allocate(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    return vectorPool_.allocate();
}

void VectorFloatAllocator::deallocate(float* ptr, size_t count) {
    if (ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        vectorPool_.deallocate(ptr);
    }
}

void VectorFloatAllocator::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    vectorPool_.reserve(count);
}

size_t VectorFloatAllocator::allocatedCount() {
    return vectorPool_.allocatedCount();
}

size_t VectorFloatAllocator::availableCount() {
    return vectorPool_.availableCount();
}

// NeuronIdVectorAllocator implementation
NeuronId* NeuronIdVectorAllocator::allocate(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    return neuronIdPool_.allocate();
}

void NeuronIdVectorAllocator::deallocate(NeuronId* ptr, size_t count) {
    if (ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        neuronIdPool_.deallocate(ptr);
    }
}

void NeuronIdVectorAllocator::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuronIdPool_.reserve(count);
}

size_t NeuronIdVectorAllocator::allocatedCount() {
    return neuronIdPool_.allocatedCount();
}

size_t NeuronIdVectorAllocator::availableCount() {
    return neuronIdPool_.availableCount();
}

} // namespace nlm