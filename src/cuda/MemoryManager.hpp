// CUDA memory management implementation
#include "CudaMemoryManager.hpp"
#include "CudaConfig.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {
namespace cuda {

// Device memory allocation for neural states
class DeviceMemoryManager {
public:
    static void* allocateNeuronStates(size_t numNeurons) {
        size_t size = numNeurons * sizeof(NeuronState);
        void* ptr;
        cudaError_t error = cudaMalloc(&ptr, size);
        if (error != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for neuron states: %s\n", 
                   CudaError::getErrorString(error));
            return nullptr;
        }
        return ptr;
    }
    
    static void* allocateSynapseData(size_t numSynapses) {
        size_t size = numSynapses * sizeof(SynapseData);
        void* ptr;
        cudaError_t error = cudaMalloc(&ptr, size);
        if (error != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for synapse data: %s\n", 
                   CudaError::getErrorString(error));
            return nullptr;
        }
        return ptr;
    }
    
    static void* allocateSpikeEvents(size_t numEvents) {
        size_t size = numEvents * sizeof(QueuedSpikeEvent);
        void* ptr;
        cudaError_t error = cudaMalloc(&ptr, size);
        if (error != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for spike events: %s\n", 
                   CudaError::getErrorString(error));
            return nullptr;
        }
        return ptr;
    }
    
    static void* allocatePlasticityData(size_t numSynapses) {
        size_t size = numSynapses * sizeof(PlasticityUpdate);
        void* ptr;
        cudaError_t error = cudaMalloc(&ptr, size);
        if (error != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for plasticity data: %s\n", 
                   CudaError::getErrorString(error));
            return nullptr;
        }
        return ptr;
    }
    
    static void free(void* ptr) {
        if (ptr) {
            cudaError_t error = cudaFree(ptr);
            if (error != cudaSuccess) {
                fprintf(stderr, "Failed to free device memory: %s\n", 
                       CudaError::getErrorString(error));
            }
        }
    }
    
    static cudaError_t copyToDevice(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyHostToDevice);
    }
    
    static cudaError_t copyToHost(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyDeviceToHost);
    }
    
    static cudaError_t copyDeviceToDevice(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyDeviceToDevice);
    }
    
    // Asynchronous memory transfers
    static cudaError_t copyAsyncToDevice(void* dst, const void* src, size_t size, cudaStream_t stream) {
        return cudaMemcpyAsync(dst, src, size, cudaMemcpyHostToDevice, stream);
    }
    
    static cudaError_t copyAsyncToHost(void* dst, const void* src, size_t size, cudaStream_t stream) {
        return cudaMemcpyAsync(dst, src, size, cudaMemcpyDeviceToHost, stream);
    }
    
    // Memory statistics
    static cudaError_t getMemoryInfo(size_t& freeMem, size_t& totalMem) {
        return cudaMemGetInfo(&freeMem, &totalMem);
    }
    
    static void printMemoryInfo() {
        size_t freeMem, totalMem;
        if (cudaMemGetInfo(&freeMem, &totalMem) == cudaSuccess) {
            printf("GPU Memory: %.2f MB free, %.2f MB total\n", 
                   freeMem / (1024.0f * 1024.0f), totalMem / (1024.0f * 1024.0f));
        }
    }
};

// Host-Direct memory (for better performance on some systems)
class HostDirectMemoryManager {
public:
    static cudaError_t allocatePinnedHost(void** ptr, size_t size) {
        return cudaMallocHost(ptr, size);
    }
    
    static cudaError_t freePinnedHost(void* ptr) {
        return cudaFreeHost(ptr);
    }
    
    // For unified memory support
    static cudaError_t allocateUnified(void** ptr, size_t size) {
        return cudaMallocManaged(ptr, size, cudaMemAttachGlobal);
    }
};

// CUDA stream for asynchronous operations
class CudaStreamManager {
public:
    static cudaError_t createComputeStream(cudaStream_t& stream) {
        return cudaStreamCreate(&stream);
    }
    
    static cudaError_t createMemcpyStream(cudaStream_t& stream) {
        cudaStream_t s;
        cudaError_t error = cudaStreamCreate(&s);
        if (error == cudaSuccess) {
            // Set lower priority for memory transfers
            cudaStreamSetPriority(s, cudaStreamLegacy);
            stream = s;
        }
        return error;
    }
    
    static cudaError_t destroyStream(cudaStream_t& stream) {
        cudaError_t error = cudaStreamDestroy(stream);
        if (error == cudaSuccess) stream = nullptr;
        return error;
    }
    
    static cudaError_t synchronizeStream(cudaStream_t stream) {
        return cudaStreamSynchronize(stream);
    }
    
    static cudaError_t waitEvent(cudaStream_t stream, cudaEvent_t event) {
        return cudaStreamWaitEvent(stream, event, 0);
    }
};

// CUDA event for timing and synchronization
class CudaEventManager {
public:
    static cudaError_t createEvent(cudaEvent_t& event, bool blocking = false) {
        unsigned int flags = blocking ? cudaEventBlockingSync : cudaEventDefault;
        return cudaEventCreate(&event, flags);
    }
    
    static cudaError_t recordEvent(cudaEvent_t event, cudaStream_t stream = nullptr) {
        return cudaEventRecord(event, stream);
    }
    
    static cudaError_t waitForEvent(cudaEvent_t event) {
        return cudaEventSynchronize(event);
    }
    
    static cudaError_t destroyEvent(cudaEvent_t& event) {
        cudaError_t error = cudaEventDestroy(event);
        if (error == cudaSuccess) event = nullptr;
        return error;
    }
    
    static cudaError_t elapsedTime(float& ms, cudaEvent_t start, cudaEvent_t end) {
        return cudaEventElapsedTime(&ms, start, end);
    }
};

// Memory pool for efficient allocation/deallocation
class CudaMemoryPool {
public:
    struct Allocation {
        void* ptr;
        size_t size;
        cudaStream_t stream;
        cudaEvent_t event;
        bool pinned;
    };
    
    static bool initialize(size_t initialPoolSize = 1024 * 1024 * 1024) {  // 1GB initial pool
        // Simple memory pool implementation
        poolSize_ = initialPoolSize;
        allocatedSize_ = 0;
        return true;
    }
    
    static Allocation allocate(size_t size, bool pinned = false, cudaStream_t stream = nullptr) {
        Allocation alloc;
        alloc.size = size;
        alloc.pinned = pinned;
        alloc.stream = stream;
        alloc.event = nullptr;
        
        cudaError_t error;
        if (pinned) {
            error = cudaMallocHost(&alloc.ptr, size);
        } else {
            error = cudaMalloc(&alloc.ptr, size);
        }
        
        if (error == cudaSuccess) {
            allocatedSize_ += size;
        } else {
            alloc.ptr = nullptr;
        }
        
        return alloc;
    }
    
    static void deallocate(Allocation& alloc) {
        if (alloc.ptr) {
            cudaError_t error;
            if (alloc.pinned) {
                error = cudaFreeHost(alloc.ptr);
            } else {
                error = cudaFree(alloc.ptr);
            }
            
            if (error == cudaSuccess) {
                allocatedSize_ -= alloc.size;
            }
            
            alloc.ptr = nullptr;
            alloc.size = 0;
            
            if (alloc.event) {
                CudaEventManager::destroyEvent(alloc.event);
                alloc.event = nullptr;
            }
        }
    }
    
    static void printStats() {
        printf("CUDA Memory Pool: %.2f MB allocated\n", allocatedSize_ / (1024.0f * 1024.0f));
    }
    
    static void shutdown() {
        // Clean up any remaining allocations
        allocatedSize_ = 0;
    }
    
private:
    static size_t poolSize_;
    static size_t allocatedSize_;
};

// Global memory pool instance
extern CudaMemoryPool::Allocation globalMemoryPool;

// Initialize and shutdown memory pool
cudaError_t initializeMemoryPool(size_t poolSize = 1024 * 1024 * 1024);  // 1GB
cudaError_t shutdownMemoryPool();

} // namespace cuda
} // namespace nlm

// Global CUDA memory pool instances
cudaError_t nlm::cuda::initializeMemoryPool(size_t poolSize) {
    return cudaSuccess;  // Simplified for now
}

cudaError_t nlm::cuda::shutdownMemoryPool() {
    return cudaSuccess;  // Simplified for now
}

namespace nlm {
namespace cuda {
    
    size_t CudaMemoryPool::poolSize_ = 0;
    size_t CudaMemoryPool::allocatedSize_ = 0;
    
} // namespace cuda
} // namespace nlm
