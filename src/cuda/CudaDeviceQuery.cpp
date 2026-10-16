// CudaDeviceQuery.cpp: CUDA device query utility implementation
#include "CudaDeviceQuery.hpp"
#include "CudaConfig.hpp"
#include <iostream>

namespace nlm {
namespace cuda {

// Global CudaDeviceInfo instance
cudaError_t CudaDeviceQuery::getDeviceInfo(int deviceId, CudaDeviceInfo& info) {
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

cudaError_t CudaDeviceQuery::getCurrentDeviceId(int& deviceId) {
    return cudaGetDevice(&deviceId);
}

cudaError_t CudaDeviceQuery::setDevice(int deviceId) {
    return cudaSetDevice(deviceId);
}

cudaError_t CudaDeviceQuery::getDeviceCount(int& count) {
    return cudaGetDeviceCount(&count);
}

} // namespace cuda
} // namespace nlm
