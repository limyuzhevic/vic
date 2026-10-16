// NeuronDynamics.hpp: CUDA neuron dynamics interface
#pragma once

#include "../core/Types/Types.hpp"
#include "DeviceMemoryManager.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Performance metrics for neuron dynamics
enum class PerformanceMetric {
    SPIKES_PER_SECOND,
    NEURONS_PROCESSED,
    GPU_TIME,
    BANDWIDTH
};

// Configuration for neuron dynamics
struct NeuronDynamicsConfig {
    size_t numNeurons;
    bool useMemoryPools;
    bool enableAsynchronousTransfers;
    size_t maxThreadsPerBlock;
    size_t maxBlocksPerGrid;
    size_t sharedMemorySize;
    bool useVectorizedOperations;
    bool enableWarpSynchronous;
    float computeCapabilityMajor;
    float computeCapabilityMinor;
};

// Neuron dynamics class for GPU acceleration
class NeuronDynamics {
public:
    NeuronDynamics();
    ~NeuronDynamics();
    
    // Setup neuron dynamics on GPU
    void setup(DeviceMemoryManager& memoryManager,
              const NeuronDynamicsConfig& config);
    
    // Update neuron states on GPU
    void updateStates(DeviceMemoryManager& memoryManager,
                     float dt,
                     float currentTime,
                     cudaStream_t stream = nullptr);
    
    // Process spike events on GPU
    void processSpikes(DeviceMemoryManager& memoryManager,
                      const SpikeHistory& spikeHistory,
                      cudaStream_t stream = nullptr);
    
    // Reset neuron states on GPU
    void resetStates(DeviceMemoryManager& memoryManager,
                    cudaStream_t stream = nullptr);
    
    // Get performance metrics
    float getPerformanceMetric(PerformanceMetric metric) const;
    
    // Print statistics
    void printStatistics() const;
    
private:
    NeuronDynamicsConfig config_;
    float spikesPerSecond_;
    size_t neuronsProcessed_;
    float gpuTime_;
    float memoryBandwidth_;
};

// Global neuron dynamics instance
extern NeuronDynamics neuronDynamics;

} // namespace cuda
} // namespace nlm
