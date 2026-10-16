// SynapseProcessing.hpp: CUDA synapse processing interface
#pragma once

#include "../core/Types/Types.hpp"
#include "DeviceMemoryManager.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Performance metrics for synapse processing
enum class PerformanceMetric {
    SYNAPSE_UPDATES_PER_SECOND,
    CURRENTS_COMPUTED,
    SPIKE_PROPAGATIONS,
    BANDWIDTH
};

// Configuration for synapse processing
struct SynapseProcessingConfig {
    size_t numSynapses;
    bool useMemoryPools;
    bool enableAsynchronousTransfers;
    size_t maxThreadsPerBlock;
    size_t maxBlocksPerGrid;
    size_t sharedMemorySize;
    bool useVectorizedOperations;
    bool enableWarpSynchronous;
    float computeCapabilityMajor;
    float computeCapabilityMinor;
    bool enableSTDP;
    bool enableHebbian;
    bool enableRewardModulated;
};

// Synapse processing class for GPU acceleration
class SynapseProcessing {
public:
    SynapseProcessing();
    ~SynapseProcessing();
    
    // Setup synapse processing on GPU
    void setup(DeviceMemoryManager& memoryManager,
              const SynapseProcessingConfig& config);
    
    // Compute synaptic currents on GPU
    void computeCurrents(DeviceMemoryManager& memoryManager,
                        const NeuronState* neuronStates,
                        const Synapse* synapses,
                        float* synapticInputs,
                        float* synapticConductances,
                        float currentTime,
                        cudaStream_t stream = nullptr);
    
    // Propagate spikes on GPU
    void propagateSpikes(DeviceMemoryManager& memoryManager,
                        const SpikeHistory& spikeHistory,
                        const Synapse* synapses,
                        float* synapticInputs,
                        uint64_t currentTime,
                        cudaStream_t stream = nullptr);
    
    // Process synaptic delays on GPU
    void processDelays(DeviceMemoryManager& memoryManager,
                      Synapse* synapses,
                      uint64_t currentTime,
                      cudaStream_t stream = nullptr);
    
    // Get performance metrics
    float getPerformanceMetric(PerformanceMetric metric) const;
    
    // Print statistics
    void printStatistics() const;
    
private:
    SynapseProcessingConfig config_;
    float synapseUpdatesPerSecond_;
    size_t currentsComputed_;
    size_t spikePropagations_;
    float gpuTime_;
    float memoryBandwidth_;
};

// Global synapse processing instance
extern SynapseProcessing synapseProcessing;

} // namespace cuda
} // namespace nlm
