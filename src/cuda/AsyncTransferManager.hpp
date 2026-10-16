// AsyncTransferManager: Asynchronous memory transfer management
// Efficient GPU-CPU data transfers with overlapping computation and memory operations

#pragma once

#include "MemcpyManager.hpp"
#include "CudaConfig.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <functional>

namespace nlm {
namespace cuda {

// Transfer request structure
struct TransferRequest {
    enum class TransferType {
        HostToDevice,
        DeviceToHost,
        DeviceToDevice
    };
    
    TransferType type;
    void* dst;
    const void* src;
    size_t size;
    cudaStream_t stream;
    bool completed;
    cudaError_t error;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    TransferRequest() : dst(nullptr), src(nullptr), size(0), 
                       stream(nullptr), completed(false), error(cudaSuccess) {}
    
    TransferRequest(TransferType t, void* d, const void* s, size_t sz,
                   cudaStream_t str = nullptr)
        : type(t), dst(d), src(s), size(sz), stream(str),
          completed(false), error(cudaSuccess) {
        startTime = std::chrono::high_resolution_clock::now();
    }
};

// Transfer completion callback
using TransferCallback = std::function<void(const TransferRequest&)>;

// Asynchronous transfer manager
// Manages a queue of memory transfers with overlapping operations
class AsyncTransferManager {
public:
    AsyncTransferManager(size_t maxPendingTransfers = 8, size_t maxStreams = 4);
    ~AsyncTransferManager();
    
    // Initialize the transfer manager
    bool initialize();
    
    // Check if manager is enabled
    bool isEnabled() const { return enabled_; }
    
    // Submit a transfer request
    bool submitTransfer(const TransferRequest& request);
    
    // Wait for a specific transfer to complete
    TransferRequest waitForTransfer(const TransferRequest& request);
    
    // Wait for all pending transfers to complete
    std::vector<TransferRequest> waitForAllTransfers();
    
    // Cancel all pending transfers
    void cancelAllTransfers();
    
    // Get current number of pending transfers
    size_t getPendingTransfersCount() const { return pendingTransfers_.size(); }
    
    // Set maximum pending transfers
    void setMaxPendingTransfers(size_t count) { maxPendingTransfers_ = count; }
    
    // Get maximum pending transfers
    size_t getMaxPendingTransfers() const { return maxPendingTransfers_; }
    
    // Shutdown the transfer manager
    void shutdown();
    
    // Print transfer statistics
    void printStatistics() const;
    
private:
    bool enabled_;                           // Whether manager is enabled
    size_t maxPendingTransfers_;             // Maximum number of pending transfers
    size_t maxStreams_;                       // Maximum number of transfer streams
    
    // Transfer state
    std::queue<TransferRequest> pendingTransfers_;  // Queue of pending transfers
    mutable std::mutex transferMutex_;          // Mutex for thread safety
    std::condition_variable transferCV_;        // Condition variable for transfer completion
    
    // Stream management
    std::vector<cudaStream_t> transferStreams_;  // Available transfer streams
    
    // Background thread for managing transfers
    std::thread transferThread_;
    bool threadRunning_;                         // Whether transfer thread is running
    
    // Transfer management thread function
    void transferManagerThread();
    
    // Initialize transfer streams
    void initializeStreams();
    
    // Cleanup transfer streams
    void cleanupStreams();
    
    // Process pending transfers
    void processPendingTransfers();
    
    // Get available stream
    cudaStream_t getAvailableStream();
    
    // Return stream to pool
    void returnStream(cudaStream_t stream);
    
    // Calculate transfer time for statistics
    double calculateTransferTime(const TransferRequest& request) const;
};

} // namespace cuda
} // namespace nlm
