// MemoryManager.cu: Device memory management implementation
#include "MemoryManager.hpp"
#include "CudaConfig.hpp"
#include <iostream>

namespace nlm {
namespace cuda {

MemoryManager::MemoryManager() {
    std::cout << "MemoryManager CUDA initialized" << std::endl;
}

MemoryManager::~MemoryManager() {
    std::cout << "MemoryManager CUDA destroyed" << std::endl;
    shutdown();
}

bool MemoryManager::initialize(const CudaConfig& config) {
    std::cout << "Initializing memory manager" << std::endl;
    
    // Check CUDA availability
    if (!CudaDeviceQuery::getDeviceCount() || CudaDeviceQuery::getDeviceInfo(0, deviceInfo_) != cudaSuccess) {
        std::cout << "CUDA device not available" << std::endl;
        return false;
    }
    
    // Store configuration
    this->config_ = config;
    
    // Initialize memory pools
    if (!neuronPool_.initialize(config.maxNeuronMemory)) {
        std::cerr << "Failed to initialize neuron memory pool" << std::endl;
        return false;
    }
    
    if (!synapsePool_.initialize(config.maxSynapseMemory)) {
        std::cerr << "Failed to initialize synapse memory pool" << std::endl;
        return false;
    }
    
    if (!eventQueuePool_.initialize(config.maxEventQueueMemory)) {
        std::cerr << "Failed to initialize event queue memory pool" << std::endl;
        return false;
    }
    
    if (!plasticityPool_.initialize(config.maxPlasticityMemory)) {
        std::cerr << "Failed to initialize plasticity memory pool" << std::endl;
        return false;
    }
    
    std::cout << "Memory manager initialized successfully" << std::endl;
    return true;
}

void* MemoryManager::allocateNeuronMemory(size_t numNeurons) {
    return neuronPool_.allocate(numNeurons * sizeof(NeuronState));
}

void* MemoryManager::allocateSynapseMemory(size_t numSynapses) {
    return synapsePool_.allocate(numSynapses * sizeof(Synapse));
}

void* MemoryManager::allocateEventQueueMemory(size_t numEvents) {
    return eventQueuePool_.allocate(numEvents * sizeof(QueuedSpikeEvent));
}

void* MemoryManager::allocatePlasticityMemory(size_t numUpdates) {
    return plasticityPool_.allocate(numUpdates * sizeof(PlasticityUpdate));
}

void MemoryManager::freeMemory(void* ptr) {
    if (ptr) {
        neuronPool_.free(ptr);
        synapsePool_.free(ptr);
        eventQueuePool_.free(ptr);
        plasticityPool_.free(ptr);
    }
}

bool MemoryManager::transferToDevice(const void* hostData, void* deviceData, 
                                   size_t size, cudaStream_t stream) {
    cudaError_t error = cudaMemcpyAsync(deviceData, hostData, size, 
                                       cudaMemcpyHostToDevice, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to transfer data to device: " << CudaError::getErrorString(error) << std::endl;
        return false;
    }
    return true;
}

bool MemoryManager::transferFromDevice(void* hostData, const void* deviceData,
                                      size_t size, cudaStream_t stream) {
    cudaError_t error = cudaMemcpyAsync(hostData, deviceData, size,
                                       cudaMemcpyDeviceToHost, stream);
    if (error != cudaSuccess) {
        std::cerr << "Failed to transfer data from device: " << CudaError::getErrorString(error) << std::endl;
        return false;
    }
    return true;
}

void MemoryManager::synchronize() {
    cudaDeviceSynchronize();
}

void MemoryManager::printMemoryInfo() const {
    std::cout << "=== CUDA Memory Manager Statistics ===" << std::endl;
    std::cout << "Neuron Pool:" << std::endl;
    neuronPool_.printStats();
    std::cout << "Synapse Pool:" << std::endl;
    synapsePool_.printStats();
    std::cout << "Event Queue Pool:" << std::endl;
    eventQueuePool_.printStats();
    std::cout << "Plasticity Pool:" << std::endl;
    plasticityPool_.printStats();
    std::cout << "=======================================" << std::endl;
}

void MemoryManager::shutdown() {
    neuronPool_.shutdown();
    synapsePool_.shutdown();
    eventQueuePool_.shutdown();
    plasticityPool_.shutdown();
    std::cout << "Memory manager shutdown" << std::endl;
}

// MemoryPool implementation
MemoryPool::MemoryPool() : totalAllocated_(0), peakAllocated_(0), allocationCount_(0) {}

MemoryPool::~MemoryPool() {
    shutdown();
}

bool MemoryPool::initialize(size_t initialSize) {
    if (!initialized_) {
        allocatedPtr_ = nullptr;
        allocatedSize_ = 0;
        currentPos_ = 0;
        initialized_ = true;
    }
    
    // Allocate initial memory
    if (initialSize > 0) {
        cudaError_t error = cudaMalloc(&allocatedPtr_, initialSize);
        if (error != cudaSuccess) {
            std::cerr << "Failed to allocate memory pool: " << CudaError::getErrorString(error) << std::endl;
            return false;
        }
        allocatedSize_ = initialSize;
        currentPos_ = 0;
    }
    
    return true;
}

void* MemoryPool::allocate(size_t size) {
    if (!initialized_) {
        std::cerr << "Memory pool not initialized" << std::endl;
        return nullptr;
    }
    
    // Check if we need to grow the pool
    if (currentPos_ + size > allocatedSize_) {
        // Grow the pool (double size)
        size_t newSize = allocatedSize_ * 2;
        if (newSize < allocatedSize_ + size) {
            newSize = allocatedSize_ + size;
        }
        
        void* newPtr = nullptr;
        cudaError_t error = cudaMalloc(&newPtr, newSize);
        if (error != cudaSuccess) {
            std::cerr << "Failed to grow memory pool: " << CudaError::getErrorString(error) << std::endl;
            return nullptr;
        }
        
        // Copy existing data
        if (allocatedPtr_) {
            cudaMemcpy(newPtr, allocatedPtr_, allocatedSize_, cudaMemcpyDeviceToDevice);
            cudaFree(allocatedPtr_);
        }
        
        allocatedPtr_ = newPtr;
        allocatedSize_ = newSize;
    }
    
    // Allocate from pool
    void* ptr = static_cast<char*>(allocatedPtr_) + currentPos_;
    currentPos_ += size;
    
    totalAllocated_ += size;
    if (totalAllocated_ > peakAllocated_) {
        peakAllocated_ = totalAllocated_;
    }
    allocationCount_++;
    
    return ptr;
}

void MemoryPool::free(void* ptr) {
    // MemoryPool doesn't actually free individual allocations
    // It just tracks usage
    // In a real implementation, we would need to track allocated blocks
}

void MemoryPool::printStats() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Total Allocated: " << (totalAllocated_ / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "  Peak Allocated: " << (peakAllocated_ / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "  Allocation Count: " << allocationCount_ << std::endl;
    std::cout << "  Current Pool Size: " << (allocatedSize_ / (1024.0 * 1024.0)) << " MB" << std::endl;
}

void MemoryPool::shutdown() {
    if (allocatedPtr_) {
        cudaFree(allocatedPtr_);
        allocatedPtr_ = nullptr;
        allocatedSize_ = 0;
        currentPos_ = 0;
    }
    totalAllocated_ = 0;
    peakAllocated_ = 0;
    allocationCount_ = 0;
    initialized_ = false;
}

// MemoryManager singleton implementation
MemoryManager& getMemoryManager() {
    static MemoryManager manager;
    return manager;
}

} // namespace cuda
} // namespace nlm
