// PlasticityCalculations.hpp: CUDA plasticity calculations interface
#pragma once

#include "../core/Types/Types.hpp"
#include "DeviceMemoryManager.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Performance metrics for plasticity calculations
enum class PerformanceMetric {
    PLASTICITY_UPDATES_PER_SECOND,
    WEIGHT_CHANGES_APPLIED,
    GPU_TIME,
    BANDWIDTH
};

// Configuration for plasticity calculations
struct PlasticityCalculationsConfig {
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
    bool stdpEnabled;
    bool hebbianEnabled;
    bool rewardModulatedEnabled;
    float tauPlus;
    float tauMinus;
    float A_plus;
    float A_minus;
    float learningRate;
    float rewardModulationStrength;
    float decayRate;
};

// Plasticity calculations class for GPU acceleration
class PlasticityCalculations {
public:
    PlasticityCalculations();
    ~PlasticityCalculations();
    
    // Setup plasticity calculations on GPU
    void setup(DeviceMemoryManager& memoryManager,
              const PlasticityCalculationsConfig& config);
    
    // Update STDP on GPU
    void updateSTDP(DeviceMemoryManager& memoryManager,
                   const NeuronState* neuronStates,
                   const Synapse* synapses,
                   float currentTime,
                   float* weightDeltas,
                   cudaStream_t stream = nullptr);
    
    // Update Hebbian learning on GPU
    void updateHebbian(DeviceMemoryManager& memoryManager,
                      const NeuronState* neuronStates,
                      const Synapse* synapses,
                      float currentTime,
                      float* weightDeltas,
                      cudaStream_t stream = nullptr);
    
    // Update reward-modulated plasticity on GPU
    void updateRewardModulated(DeviceMemoryManager& memoryManager,
                              const NeuronState* neuronStates,
                              const Synapse* synapses,
                              const float* rewardSignals,
                              float currentTime,
                              float* weightDeltas,
                              cudaStream_t stream = nullptr);
    
    // Process all plasticity rules on GPU
    void processPlasticity(DeviceMemoryManager& memoryManager,
                          Synapse* synapses,
                          float currentTime,
                          float plasticityMod,
                          cudaStream_t stream = nullptr);
    
    // Get performance metrics
    float getPerformanceMetric(PerformanceMetric metric) const;
    
    // Print statistics
    void printStatistics() const;
    
private:
    PlasticityCalculationsConfig config_;
    float plasticityUpdatesPerSecond_;
    size_t weightChangesApplied_;
    float gpuTime_;
    float memoryBandwidth_;
};

// Global plasticity calculations instance
extern PlasticityCalculations plasticityCalculations;

} // namespace cuda
} // namespace nlm
