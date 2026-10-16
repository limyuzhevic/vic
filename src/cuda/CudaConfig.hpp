#pragma once

// CUDA acceleration configuration
// GPU acceleration support for NLM neural network simulation

#ifndef NLM_CUDA_CONFIG_H
#define NLM_CUDA_CONFIG_H

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cublas_v2.h>
#include <curand_kernel.h>

// CUDA version requirements
#define CUDA_MIN_VERSION 11000

// CUDA device properties
struct CudaDeviceInfo {
    int deviceId;
    char name[256];
    size_t totalGlobalMem;
    size_t sharedMemPerBlock;
    int regsPerBlock;
    int warpSize;
    int maxThreadsPerBlock;
    int maxThreadsPerMultiProcessor;
    int memoryClockRate;
    int memoryBusWidth;
    float computeCapabilityMajor;
    float computeCapabilityMinor;
    int multiProcessorCount;
};

// CUDA configuration options
struct CudaConfig {
    bool enableGpuAcceleration;      // Enable GPU acceleration
    bool useTensorCores;              // Use tensor cores for matrix operations
    bool enablePeerToPeer;            // Enable peer-to-peer memory access
    bool enableUnifiedMemory;         // Use unified memory
    int deviceId;                     // GPU device ID
    size_t maxMemoryAllocation;       // Maximum GPU memory allocation
    int cudaThreadsPerBlock;          // Threads per CUDA block
    int cudaMaxGridSizeX;             // Maximum grid size in X dimension
    int cudaMaxGridSizeY;             // Maximum grid size in Y dimension
    int cudaMaxGridSizeZ;             // Maximum grid size in Z dimension
    
    CudaConfig()
        : enableGpuAcceleration(true)
        , useTensorCores(false)
        , enablePeerToPeer(false)
        , enableUnifiedMemory(false)
        , deviceId(0)
        , maxMemoryAllocation(0)  // Auto-detect
        , cudaThreadsPerBlock(256)
        , cudaMaxGridSizeX(65535)
        , cudaMaxGridSizeY(65535)
        , cudaMaxGridSizeZ(65535) {}
};

// CUDA error handling
class CudaError {
public:
    static const char* getErrorString(cudaError_t error) {
        switch (error) {
            case cudaSuccess: return "cudaSuccess";
            case cudaErrorMissingConfiguration: return "cudaErrorMissingConfiguration";
            case cudaErrorMemoryAllocation: return "cudaErrorMemoryAllocation";
            case cudaErrorInitializationError: return "cudaErrorInitializationError";
            case cudaErrorLaunchOutOfResources: return "cudaErrorLaunchOutOfResources";
            case cudaErrorInvalidDevice: return "cudaErrorInvalidDevice";
            case cudaErrorInvalidValue: return "cudaErrorInvalidValue";
            case cudaErrorInvalidPitchOrPitchAlign: return "cudaErrorInvalidPitchOrPitchAlign";
            case cudaErrorInvalidResourceHandle: return "cudaErrorInvalidResourceHandle";
            case cudaErrorInvalidContext: return "cudaErrorInvalidContext";
            case cudaErrorNotReady: return "cudaErrorNotReady";
            case cudaErrorIllegalAddress: return "cudaErrorIllegalAddress";
            default: return "Unknown CUDA error";
        }
    }
    
    static void check(cudaError_t error, const char* operation) {
        if (error != cudaSuccess) {
            // In production, this would log an error message
            // For now, just output to stderr
            fprintf(stderr, "CUDA Error in %s: %s\n", operation, getErrorString(error));
        }
    }
};

// CUDA memory management utilities
class CudaMemoryManager {
public:
    static cudaError_t allocateHost(void** ptr, size_t size) {
        return cudaMallocHost(ptr, size);
    }
    
    static cudaError_t freeHost(void* ptr) {
        return cudaFreeHost(ptr);
    }
    
    static cudaError_t allocateDevice(void** ptr, size_t size) {
        return cudaMalloc(ptr, size);
    }
    
    static cudaError_t freeDevice(void* ptr) {
        return cudaFree(ptr);
    }
    
    static cudaError_t memcpyHostToDevice(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyHostToDevice);
    }
    
    static cudaError_t memcpyDeviceToHost(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyDeviceToHost);
    }
    
    static cudaError_t memcpyDeviceToDevice(void* dst, const void* src, size_t size) {
        return cudaMemcpy(dst, src, size, cudaMemcpyDeviceToDevice);
    }
};

// CUDA event management
class CudaEventManager {
public:
    static cudaError_t recordEvent(cudaEvent_t& event) {
        return cudaEventRecord(event);
    }
    
    static cudaError_t waitForEvent(cudaEvent_t event) {
        return cudaEventSynchronize(event);
    }
    
    static cudaError_t destroyEvent(cudaEvent_t& event) {
        cudaError_t error = cudaEventDestroy(event);
        if (error == cudaSuccess) event = nullptr;
        return error;
    }
};

// CUDA stream management
class CudaStreamManager {
public:
    static cudaError_t createStream(cudaStream_t& stream) {
        return cudaStreamCreate(&stream);
    }
    
    static cudaError_t destroyStream(cudaStream_t& stream) {
        cudaError_t error = cudaStreamDestroy(stream);
        if (error == cudaSuccess) stream = nullptr;
        return error;
    }
    
    static cudaError_t memcpyAsync(void* dst, const void* src, size_t size,
                                   cudaStream_t stream, cudaMemcpyKind kind) {
        return cudaMemcpyAsync(dst, src, size, kind, stream);
    }
};

// CUDA device query utilities
class CudaDeviceQuery {
public:
    static cudaError_t getDeviceInfo(int deviceId, CudaDeviceInfo& info) {
        cudaDeviceProp prop;
        cudaError_t error = cudaGetDeviceProperties(&prop, deviceId);
        if (error != cudaSuccess) return error;
        
        info.deviceId = deviceId;
        strncpy(info.name, prop.name, sizeof(info.name) - 1);
        info.name[sizeof(info.name) - 1] = '\0';
        info.totalGlobalMem = prop.totalGlobalMem;
        info.sharedMemPerBlock = prop.sharedMemPerBlock;
        info.regsPerBlock = prop.regsPerBlock;
        info.warpSize = prop.warpSize;
        info.maxThreadsPerBlock = prop.maxThreadsPerBlock;
        info.maxThreadsPerMultiProcessor = prop.maxThreadsPerMultiProcessor;
        info.memoryClockRate = prop.memoryClockRate;
        info.memoryBusWidth = prop.memoryBusWidth;
        info.computeCapabilityMajor = prop.major;
        info.computeCapabilityMinor = prop.minor;
        info.multiProcessorCount = prop.multiProcessorCount;
        
        return cudaSuccess;
    }
    
    static cudaError_t getCurrentDeviceId(int& deviceId) {
        return cudaGetDevice(&deviceId);
    }
    
    static cudaError_t setDevice(int deviceId) {
        return cudaSetDevice(deviceId);
    }
    
    static cudaError_t getDeviceCount(int& count) {
        return cudaGetDeviceCount(&count);
    }
};

#endif // NLM_CUDA_CONFIG_H
