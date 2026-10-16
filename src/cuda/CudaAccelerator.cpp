// CudaAccelerator implementation
// CUDA accelerator abstraction layer

#include "CudaAccelerator.hpp"
#include "CudaConfig.hpp"
#include "MemoryManager.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {
namespace cuda {

// Constructor
CudaAccelerator::CudaAccelerator()
    : ready_(false)
    , lastCudaError_(cudaSuccess)
    , stats_{0, 0, 0, "", 0.0} {
    // Initialize default resources
    createDefaultResources();
    
    // Try to initialize CUDA context
    ready_ = initializeCudaContext();
}

// Destructor
CudaAccelerator::~CudaAccelerator() {
    cleanupCudaResources();
}

// Initialize CUDA context
bool CudaAccelerator::initializeCudaContext() {
    if (!isCudaAvailable()) {
        std::cerr << "CUDA is not available on this system" << std::endl;
        return false;
    }
    
    // Set device ID from config
    int deviceCount = getDeviceCount();
    if (deviceCount == 0) {
        std::cerr << "No CUDA devices found" << std::endl;
        return false;
    }
    
    // Use device 0 by default (could be configured)
    cudaError_t error = cudaSetDevice(0);
    if (error != cudaSuccess) {
        std::cerr << "Failed to set CUDA device: " 
                  << getCudaErrorString(error) << std::endl;
        return false;
    }
    
    // Get device properties
    error = CudaDeviceQuery::getDeviceInfo(0, deviceInfo_);
    if (error != cudaSuccess) {
        std::cerr << "Failed to get device info: " 
                  << getCudaErrorString(error) << std::endl;
        return false;
    }
    
    // Check compute capability
    if (deviceInfo_.computeCapabilityMajor < 5.0) {
        std::cout << "Warning: GPU has compute capability " 
                  << deviceInfo_.computeCapabilityMajor << ".0, "
                  << "expect reduced performance" << std::endl;
    }
    
    std::cout << "CUDA accelerator initialized successfully" << std::endl;
    std::cout << "Device: " << deviceInfo_.name << std::endl;
    std::cout << "Compute capability: " 
              << deviceInfo_.computeCapabilityMajor << "." 
              << deviceInfo_.computeCapabilityMinor << std::endl;
    
    return true;
}

// Clean up CUDA resources
void CudaAccelerator::cleanupCudaResources() {
    destroyAllResources();
    ready_ = false;
    std::cout << "CUDA accelerator cleaned up" << std::endl;
}

// Create default streams and events
void CudaAccelerator::createDefaultResources() {
    // Create compute stream
    cudaError_t error = createStream(computeStream_);
    if (error != cudaSuccess) {
        std::cerr << "Failed to create compute stream: " 
                  << getCudaErrorString(error) << std::endl;
    }
    
    // Create memory transfer stream
    error = createStream(memcpyStream_);
    if (error != cudaSuccess) {
        std::cerr << "Failed to create memcpy stream: " 
                  << getCudaErrorString(error) << std::endl;
    }
    
    // Create events for timing
    error = recordEvent(startEvent_);
    if (error != cudaSuccess) {
        std::cerr << "Failed to create start event: " 
                  << getCudaErrorString(error) << std::endl;
    }
    
    error = recordEvent(stopEvent_);
    if (error != cudaSuccess) {
        std::cerr << "Failed to create stop event: " 
                  << getCudaErrorString(error) << std::endl;
    }
}

// Destroy all resources
void CudaAccelerator::destroyAllResources() {
    // Destroy events
    destroyEvent(startEvent_);
    destroyEvent(stopEvent_);
    
    // Destroy streams
    destroyStream(computeStream_);
    destroyStream(memcpyStream_);
    
    streams_.clear();
    events_.clear();
}

// Create CUDA stream
cudaError_t CudaAccelerator::createStream(cudaStream_t& stream) {
    cudaError_t error = cudaStreamCreate(&stream);
    if (error == cudaSuccess) {
        streams_.push_back(stream);
    }
    lastCudaError_ = error;
    return error;
}

// Destroy CUDA stream
cudaError_t CudaAccelerator::destroyStream(cudaStream_t& stream) {
    if (stream == nullptr) return cudaSuccess;
    
    // Remove from streams vector
    auto it = std::find(streams_.begin(), streams_.end(), stream);
    if (it != streams_.end()) {
        streams_.erase(it);
    }
    
    cudaError_t error = cudaStreamDestroy(stream);
    if (error == cudaSuccess) {
        stream = nullptr;
    }
    lastCudaError_ = error;
    return error;
}

// Record event
cudaError_t CudaAccelerator::recordEvent(cudaEvent_t& event, cudaStream_t stream) {
    cudaError_t error = cudaEventCreate(&event, cudaEventDisableTiming);
    if (error != cudaSuccess) {
        lastCudaError_ = error;
        return error;
    }
    
    error = cudaEventRecord(event, stream);
    if (error == cudaSuccess) {
        events_.push_back(event);
    }
    lastCudaError_ = error;
    return error;
}

// Wait for event
cudaError_t CudaAccelerator::waitForEvent(cudaEvent_t event) {
    cudaError_t error = cudaEventSynchronize(event);
    lastCudaError_ = error;
    return error;
}

// Destroy event
cudaError_t CudaAccelerator::destroyEvent(cudaEvent_t& event) {
    if (event == nullptr) return cudaSuccess;
    
    // Remove from events vector
    auto it = std::find(events_.begin(), events_.end(), event);
    if (it != events_.end()) {
        events_.erase(it);
    }
    
    cudaError_t error = cudaEventDestroy(event);
    if (error == cudaSuccess) {
        event = nullptr;
    }
    lastCudaError_ = error;
    return error;
}

// Get available memory
size_t CudaAccelerator::getAvailableMemory() const {
    size_t freeMem, totalMem;
    cudaError_t error = cudaMemGetInfo(&freeMem, &totalMem);
    if (error == cudaSuccess) {
        return freeMem;
    }
    return 0;
}

// Get total memory
size_t CudaAccelerator::getTotalMemory() const {
    size_t freeMem, totalMem;
    cudaError_t error = cudaMemGetInfo(&freeMem, &totalMem);
    if (error == cudaSuccess) {
        return totalMem;
    }
    return 0;
}

// Synchronize with GPU
void CudaAccelerator::synchronize() {
    cudaDeviceSynchronize();
    lastCudaError_ = cudaGetLastError();
}

// Print device information
void CudaAccelerator::printDeviceInfo() const {
    std::cout << "=== CUDA Device Information ===" << std::endl;
    std::cout << "Device ID: " << deviceInfo_.deviceId << std::endl;
    std::cout << "Device Name: " << deviceInfo_.name << std::endl;
    std::cout << "Total Global Memory: " 
              << (deviceInfo_.totalGlobalMem / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "Shared Memory per Block: " << deviceInfo_.sharedMemPerBlock << " bytes" << std::endl;
    std::cout << "Maximum Threads per Block: " << deviceInfo_.maxThreadsPerBlock << std::endl;
    std::cout << "Warp Size: " << deviceInfo_.warpSize << std::endl;
    std::cout << "Multiprocessor Count: " << deviceInfo_.multiProcessorCount << std::endl;
    std::cout << "Compute Capability: " 
              << deviceInfo_.computeCapabilityMajor << "." 
              << deviceInfo_.computeCapabilityMinor << std::endl;
    std::cout << "Available Memory: " 
              << (getAvailableMemory() / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "================================" << std::endl;
}

// Print memory usage
void CudaAccelerator::printMemoryUsage() const {
    size_t freeMem, totalMem;
    cudaError_t error = cudaMemGetInfo(&freeMem, &totalMem);
    if (error == cudaSuccess) {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "GPU Memory Usage:" << std::endl;
        std::cout << "  Used: " << ((totalMem - freeMem) / (1024.0 * 1024.0)) << " MB" << std::endl;
        std::cout << "  Free: " << (freeMem / (1024.0 * 1024.0)) << " MB" << std::endl;
        std::cout << "  Total: " << (totalMem / (1024.0 * 1024.0)) << " MB" << std::endl;
        std::cout << "  Usage: " << (100.0 * (1.0 - freeMem / (double)totalMem)) << "%" << std::endl;
    }
}

} // namespace cuda
} // namespace nlm
