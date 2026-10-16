// CUDA accelerator abstraction
// Provides unified interface for GPU-accelerated neural operations

#pragma once

#include "CudaConfig.hpp"
#include "MemoryManager.hpp"
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace nlm {
namespace cuda {

// Forward declarations
class NeuralAccelerator;

// CUDA accelerator abstraction layer
// Provides high-level interface for GPU-accelerated operations
class CudaAccelerator {
public:
    CudaAccelerator();
    ~CudaAccelerator();
    
    // Initialize CUDA accelerator
    bool initialize();
    
    // Check if accelerator is ready
    bool isReady() const { return ready_; }
    
    // Get device information
    const CudaDeviceInfo& getDeviceInfo() const { return deviceInfo_; }
    
    // Get available memory
    size_t getAvailableMemory() const;
    
    // Get total memory
    size_t getTotalMemory() const;
    
    // Synchronize with GPU
    void synchronize();
    
    // Check CUDA runtime status
    cudaError_t getLastCudaError() const { return lastCudaError_; }
    
    // Get CUDA error string
    const char* getCudaErrorString(cudaError_t error) const {
        return CudaError::getErrorString(error);
    }
    
    // Check if operation succeeded
    bool isSuccess(cudaError_t error) const { return error == cudaSuccess; }
    
    // Print device information
    void printDeviceInfo() const;
    
    // Print memory usage
    void printMemoryUsage() const;
    
    // Reset statistics
    void resetStatistics() {
        totalTransfers_ = 0;
        transferBytes_ = 0;
        kernelExecutions_ = 0;
        lastError_ = "";
    }
    
    // Get statistics
    struct AcceleratorStats {
        uint64_t totalTransfers;     // Total number of memory transfers
        uint64_t transferBytes;      // Total bytes transferred
        uint64_t kernelExecutions;   // Total kernel executions
        std::string lastError;       // Last error message
        double averageTransferTime;  // Average transfer time
    };
    
    const AcceleratorStats& getStats() const { return stats_; }
    
    // Create CUDA stream
    cudaError_t createStream(cudaStream_t& stream);
    
    // Destroy CUDA stream
    cudaError_t destroyStream(cudaStream_t& stream);
    
    // Record event
    cudaError_t recordEvent(cudaEvent_t& event, cudaStream_t stream = nullptr);
    
    // Wait for event
    cudaError_t waitForEvent(cudaEvent_t event);
    
    // Destroy event
    cudaError_t destroyEvent(cudaEvent_t& event);
    
    // Execute kernel with timing
    template<typename KernelFunc>
    cudaError_t executeKernelWithTiming(KernelFunc kernel, dim3 grid, dim3 block,
                                        cudaStream_t stream = nullptr) {
        cudaError_t error = kernel(grid, block, 0, stream, kernel);
        if (error == cudaSuccess) {
            stats_.kernelExecutions++;
        }
        return error;
    }
    
    // Memory allocation with alignment
    template<typename T>
    cudaError_t allocateAligned(T** ptr, size_t count, size_t alignment = 256) {
        return cuda::CudaMemoryManager::allocateDevice(
            reinterpret_cast<void**>(ptr), count * sizeof(T));
    }
    
    // Memory copy with statistics
    template<typename T>
    cudaError_t copyHostToDevice(const T* host, T* device, size_t count,
                                cudaStream_t stream = nullptr) {
        cudaError_t error = cudaMemcpyAsync(device, host, count * sizeof(T),
                                           cudaMemcpyHostToDevice, stream);
        if (error == cudaSuccess) {
            stats_.totalTransfers++;
            stats_.transferBytes += count * sizeof(T);
        }
        return error;
    }
    
    template<typename T>
    cudaError_t copyDeviceToHost(const T* device, T* host, size_t count,
                                cudaStream_t stream = nullptr) {
        cudaError_t error = cudaMemcpyAsync(host, device, count * sizeof(T),
                                           cudaMemcpyDeviceToHost, stream);
        if (error == cudaSuccess) {
            stats_.totalTransfers++;
            stats_.transferBytes += count * sizeof(T);
        }
        return error;
    }
    
    // Get last error string
    std::string getLastError() const { return stats_.lastError; }
    
    // Set last error
    void setLastError(const std::string& error) { stats_.lastError = error; }
    
    // Check CUDA initialization
    static bool isCudaAvailable() {
        int deviceCount = 0;
        cudaError_t error = cudaGetDeviceCount(&deviceCount);
        return (error == cudaSuccess && deviceCount > 0);
    }
    
    // Get device count
    static int getDeviceCount() {
        int count = 0;
        cudaGetDeviceCount(&count);
        return count;
    }
    
private:
    bool ready_;                         // Whether accelerator is ready
    CudaDeviceInfo deviceInfo_;          // Device information
    AcceleratorStats stats_;            // Performance statistics
    cudaError_t lastCudaError_;          // Last CUDA error
    
    // CUDA resources
    std::vector<cudaStream_t> streams_;  // CUDA streams
    std::vector<cudaEvent_t> events_;    // CUDA events
    
    // Initialize CUDA context
    bool initializeCudaContext();
    
    // Clean up CUDA resources
    void cleanupCudaResources();
    
    // Create default streams and events
    void createDefaultResources();
    
    // Destroy all resources
    void destroyAllResources();
};

} // namespace cuda
} // namespace nlm
