// MemcpyManager: Efficient data transfers between CPU and GPU
// Manages memory transfers with optimization for neural network workloads

#pragma once

#include "MemcpyManager.hpp"
#include "CudaConfig.hpp"
#include "../core/Types/Types.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>
#include <algorithm>

namespace nlm {
namespace cuda {

// MemcpyManager implementation
MemcpyManager::MemcpyManager()
    : enabled_(false)
    , transferCount_(0)
    , totalBytesTransferred_(0)
    , lastTransferTime_(0.0)
    , bandwidth_(0.0) {
    // Initialize stream pool
    for (int i = 0; i < STREAM_POOL_SIZE; ++i) {
        cudaStream_t stream;
        if (cudaStreamCreate(&stream) == cudaSuccess) {
            streamPool_.push_back(stream);
        }
    }
}

MemcpyManager::~MemcpyManager() {
    // Clean up streams
    for (cudaStream_t stream : streamPool_) {
        cudaStreamDestroy(stream);
    }
    streamPool_.clear();
}

bool MemcpyManager::initialize() {
    if (!isCudaAvailable()) {
        std::cout << "CUDA not available, MemcpyManager disabled" << std::endl;
        return false;
    }
    
    enabled_ = true;
    std::cout << "MemcpyManager initialized" << std::endl;
    return true;
}

bool MemcpyManager::transferToDevice(const void* hostPtr, void* devicePtr,
                                    size_t size, cudaStream_t stream) {
    if (!enabled_ || !hostPtr || !devicePtr || size == 0) {
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    cudaError_t error = cudaMemcpyAsync(devicePtr, hostPtr, size,
                                       cudaMemcpyHostToDevice, stream);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (error == cudaSuccess) {
        transferCount_++;
        totalBytesTransferred_ += size;
        lastTransferTime_ = duration.count() / 1000.0;  // Convert to ms
        
        // Update bandwidth (bytes per second)
        if (lastTransferTime_ > 0.0) {
            bandwidth_ = (size * 1000.0) / lastTransferTime_;  // Convert to bytes/ms then to bytes/s
        }
        
        return true;
    } else {
        std::cerr << "MemcpyManager transfer to device failed: "
                  << CudaError::getErrorString(error) << std::endl;
        return false;
    }
}

bool MemcpyManager::transferFromDevice(void* hostPtr, const void* devicePtr,
                                      size_t size, cudaStream_t stream) {
    if (!enabled_ || !hostPtr || !devicePtr || size == 0) {
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    cudaError_t error = cudaMemcpyAsync(hostPtr, devicePtr, size,
                                       cudaMemcpyDeviceToHost, stream);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (error == cudaSuccess) {
        transferCount_++;
        totalBytesTransferred_ += size;
        lastTransferTime_ = duration.count() / 1000.0;  // Convert to ms
        
        // Update bandwidth (bytes per second)
        if (lastTransferTime_ > 0.0) {
            bandwidth_ = (size * 1000.0) / lastTransferTime_;  // Convert to bytes/ms then to bytes/s
        }
        
        return true;
    } else {
        std::cerr << "MemcpyManager transfer from device failed: "
                  << CudaError::getErrorString(error) << std::endl;
        return false;
    }
}

bool MemcpyManager::transferDeviceToDevice(void* dstPtr, const void* srcPtr,
                                          size_t size, cudaStream_t stream) {
    if (!enabled_ || !dstPtr || !srcPtr || size == 0) {
        return false;
    }
    
    cudaError_t error = cudaMemcpyAsync(dstPtr, srcPtr, size,
                                       cudaMemcpyDeviceToDevice, stream);
    
    if (error == cudaSuccess) {
        transferCount_++;
        totalBytesTransferred_ += size;
        return true;
    } else {
        std::cerr << "MemcpyManager device-to-device transfer failed: "
                  << CudaError::getErrorString(error) << std::endl;
        return false;
    }
}

cudaStream_t MemcpyManager::getStream() {
    if (streamPool_.empty()) {
        cudaStream_t stream;
        if (cudaStreamCreate(&stream) == cudaSuccess) {
            return stream;
        }
        return nullptr;
    }
    
    cudaStream_t stream = streamPool_.back();
    streamPool_.pop_back();
    return stream;
}

void MemcpyManager::returnStream(cudaStream_t stream) {
    if (stream != nullptr) {
        streamPool_.push_back(stream);
    }
}

void MemcpyManager::synchronizeStream(cudaStream_t stream) {
    if (stream != nullptr) {
        cudaStreamSynchronize(stream);
    }
}

void MemcpyManager::synchronizeAll() {
    for (cudaStream_t stream : streamPool_) {
        if (stream != nullptr) {
            cudaStreamSynchronize(stream);
        }
    }
}

void MemcpyManager::shutdown() {
    synchronizeAll();
    
    for (cudaStream_t stream : streamPool_) {
        if (stream != nullptr) {
            cudaStreamDestroy(stream);
        }
    }
    streamPool_.clear();
    
    enabled_ = false;
    std::cout << "MemcpyManager shutdown" << std::endl;
}

void MemcpyManager::printStatistics() const {
    std::cout << "=== MemcpyManager Statistics ===" << std::endl;
    std::cout << "Enabled: " << (enabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Transfer Count: " << transferCount_ << std::endl;
    std::cout << "Total Bytes Transferred: " << totalBytesTransferred_ << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Last Transfer Time: " << lastTransferTime_ << " ms" << std::endl;
    std::cout << "Current Bandwidth: " << bandwidth_ << " bytes/s" << std::endl;
    
    if (transferCount_ > 0) {
        std::cout << "Average Bandwidth: " 
                  << (totalBytesTransferred_ * 1000.0 / transferCount_) << " bytes/s" << std::endl;
    }
    
    std::cout << "===============================" << std::endl;
}

// AsyncTransferManager implementation
AsyncTransferManager::AsyncTransferManager()
    : enabled_(false)
    , maxPendingTransfers_(8)
    , maxStreams_(4) {
    initializeStreams();
}

AsyncTransferManager::~AsyncTransferManager() {
    shutdown();
}

bool AsyncTransferManager::initialize() {
    if (!isCudaAvailable()) {
        std::cout << "CUDA not available, AsyncTransferManager disabled" << std::endl;
        return false;
    }
    
    enabled_ = true;
    initializeStreams();
    std::cout << "AsyncTransferManager initialized" << std::endl;
    return true;
}

void AsyncTransferManager::initializeStreams() {
    for (int i = 0; i < maxStreams_; ++i) {
        cudaStream_t stream;
        cudaError_t error = cudaStreamCreate(&stream);
        if (error == cudaSuccess) {
            transferStreams_.push_back(stream);
        }
    }
}

AsyncTransferManager::TransferRequest AsyncTransferManager::createTransferRequest(
    TransferType type, void* dst, const void* src, size_t size,
    cudaStream_t stream) {
    
    TransferRequest request;
    request.type = type;
    request.dst = dst;
    request.src = src;
    request.size = size;
    request.stream = stream;
    request.completed = false;
    request.error = cudaSuccess;
    request.startTime = std::chrono::high_resolution_clock::now();
    
    return request;
}

bool AsyncTransferManager::submitTransfer(const TransferRequest& request) {
    if (!enabled_) return false;
    
    // Find available stream
    cudaStream_t stream = nullptr;
    for (cudaStream_t s : transferStreams_) {
        // Check if stream is available (simplified check)
        stream = s;
        break;
    }
    
    if (stream == nullptr) {
        std::cerr << "No available transfer streams" << std::endl;
        return false;
    }
    
    // Create and submit transfer
    cudaError_t error;
    switch (request.type) {
        case TransferType::HostToDevice:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyHostToDevice, stream);
            break;
        case TransferType::DeviceToHost:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyDeviceToHost, stream);
            break;
        case TransferType::DeviceToDevice:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyDeviceToDevice, stream);
            break;
        default:
            std::cerr << "Unknown transfer type" << std::endl;
            return false;
    }
    
    if (error != cudaSuccess) {
        std::cerr << "Transfer submission failed: "
                  << CudaError::getErrorString(error) << std::endl;
        return false;
    }
    
    return true;
}

TransferRequest AsyncTransferManager::waitForTransfer(const TransferRequest& request) {
    TransferRequest req = request;
    
    if (req.stream != nullptr) {
        cudaError_t error = cudaStreamSynchronize(req.stream);
        req.error = error;
        req.completed = (error == cudaSuccess);
    }
    
    req.endTime = std::chrono::high_resolution_clock::now();
    return req;
}

std::vector<TransferRequest> AsyncTransferManager::waitForAllTransfers() {
    std::vector<TransferRequest> completedRequests;
    
    for (auto& stream : transferStreams_) {
        if (stream != nullptr) {
            cudaError_t error = cudaStreamSynchronize(stream);
            // In a real implementation, we would track individual transfer completions
        }
    }
    
    return completedRequests;
}

void AsyncTransferManager::shutdown() {
    // Wait for all transfers to complete
    for (auto& stream : transferStreams_) {
        if (stream != nullptr) {
            cudaStreamSynchronize(stream);
            cudaStreamDestroy(stream);
        }
    }
    
    transferStreams_.clear();
    enabled_ = false;
    std::cout << "AsyncTransferManager shutdown" << std::endl;
}

void AsyncTransferManager::printStatistics() const {
    std::cout << "=== AsyncTransferManager Statistics ===" << std::endl;
    std::cout << "Enabled: " << (enabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Number of Streams: " << transferStreams_.size() << std::endl;
    std::cout << "Max Pending Transfers: " << maxPendingTransfers_ << std::endl;
    std::cout << "================================" << std::endl;
}

} // namespace cuda
} // namespace nlm
