// AsyncTransferManager.cpp: Asynchronous transfer manager implementation
#include "AsyncTransferManager.hpp"
#include "CudaConfig.hpp"
#include <iostream>

namespace nlm {
namespace cuda {

AsyncTransferManager::AsyncTransferManager(size_t maxPendingTransfers, size_t maxStreams)
    : enabled_(false)
    , maxPendingTransfers_(maxPendingTransfers)
    , maxStreams_(maxStreams)
    , threadRunning_(false) {
    initializeStreams();
}

AsyncTransferManager::~AsyncTransferManager() {
    shutdown();
}

bool AsyncTransferManager::initialize() {
    if (!CudaDeviceQuery::isCudaAvailable()) {
        std::cout << "CUDA not available, AsyncTransferManager disabled" << std::endl;
        return false;
    }
    
    enabled_ = true;
    threadRunning_ = true;
    
    // Start transfer management thread
    transferThread_ = std::thread(&AsyncTransferManager::transferManagerThread, this);
    
    std::cout << "AsyncTransferManager initialized" << std::endl;
    return true;
}

void AsyncTransferManager::initializeStreams() {
    for (size_t i = 0; i < maxStreams_; ++i) {
        cudaStream_t stream;
        cudaError_t error = cudaStreamCreate(&stream);
        if (error == cudaSuccess) {
            transferStreams_.push_back(stream);
        }
    }
}

void AsyncTransferManager::cleanupStreams() {
    for (cudaStream_t stream : transferStreams_) {
        if (stream != nullptr) {
            cudaStreamDestroy(stream);
        }
    }
    transferStreams_.clear();
}

void AsyncTransferManager::transferManagerThread() {
    while (threadRunning_) {
        std::unique_lock<std::mutex> lock(transferMutex_);
        
        // Wait for pending transfers or shutdown
        transferCV_.wait_for(lock, std::chrono::milliseconds(100),
                           [this]() { return !pendingTransfers_.empty() || !threadRunning_; });
        
        if (!threadRunning_) {
            break;
        }
        
        if (!pendingTransfers_.empty()) {
            processPendingTransfers();
        }
    }
}

void AsyncTransferManager::processPendingTransfers() {
    while (!pendingTransfers_.empty()) {
        std::lock_guard<std::mutex> lock(transferMutex_);
        
        if (pendingTransfers_.empty()) {
            break;
        }
        
        TransferRequest request = pendingTransfers_.front();
        pendingTransfers_.pop();
        
        // Submit transfer
        submitTransfer(request);
    }
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
    
    // Submit transfer to the stream
    cudaError_t error;
    switch (request.type) {
        case TransferRequest::TransferType::HostToDevice:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyHostToDevice, stream);
            break;
        case TransferRequest::TransferType::DeviceToHost:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyDeviceToHost, stream);
            break;
        case TransferRequest::TransferType::DeviceToDevice:
            error = cudaMemcpyAsync(request.dst, request.src, request.size,
                                   cudaMemcpyDeviceToDevice, stream);
            break;
        default:
            std::cerr << "Unknown transfer type" << std::endl;
            return false;
    }
    
    if (error == cudaSuccess) {
        // Store the request in pending queue
        TransferRequest pendingRequest = request;
        pendingRequest.stream = stream;
        
        std::lock_guard<std::mutex> lock(transferMutex_);
        pendingTransfers_.push(pendingRequest);
    } else {
        std::cerr << "Transfer submission failed: " << CudaError::getErrorString(error) << std::endl;
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
        
        req.endTime = std::chrono::high_resolution_clock::now();
    }
    
    return req;
}

std::vector<TransferRequest> AsyncTransferManager::waitForAllTransfers() {
    std::vector<TransferRequest> completedRequests;
    
    // Wait for all streams to complete
    for (auto& stream : transferStreams_) {
        if (stream != nullptr) {
            cudaError_t error = cudaStreamSynchronize(stream);
            // In a real implementation, we would track individual transfer completions
        }
    }
    
    // Clear pending transfers
    {
        std::lock_guard<std::mutex> lock(transferMutex_);
        pendingTransfers_ = std::queue<TransferRequest>();
    }
    
    return completedRequests;
}

void AsyncTransferManager::cancelAllTransfers() {
    std::lock_guard<std::mutex> lock(transferMutex_);
    pendingTransfers_ = std::queue<TransferRequest>();
}

void AsyncTransferManager::shutdown() {
    // Stop transfer thread
    {
        std::lock_guard<std::mutex> lock(transferMutex_);
        threadRunning_ = false;
    }
    
    transferCV_.notify_all();
    
    if (transferThread_.joinable()) {
        transferThread_.join();
    }
    
    // Wait for all transfers to complete
    waitForAllTransfers();
    
    // Clean up streams
    cleanupStreams();
    
    enabled_ = false;
    std::cout << "AsyncTransferManager shutdown" << std::endl;
}

void AsyncTransferManager::printStatistics() const {
    std::cout << "=== AsyncTransferManager Statistics ===" << std::endl;
    std::cout << "Enabled: " << (enabled_ ? "Yes" : "No") << std::endl;
    std::cout << "Number of Streams: " << transferStreams_.size() << std::endl;
    std::cout << "Max Pending Transfers: " << maxPendingTransfers_ << std::endl;
    std::cout << "=================================" << std::endl;
}

} // namespace cuda
} // namespace nlm
