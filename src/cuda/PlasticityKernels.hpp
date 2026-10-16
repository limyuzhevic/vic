// PlasticityKernels.hpp: CUDA headers for plasticity calculations
// Kernel declarations and type definitions for GPU-accelerated plasticity computation

#pragma once

#include "../core/Types/Types.hpp"
#include "CudaConfig.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Kernel configuration constants
static constexpr size_t PLASTICITY_KERNEL_THREADS_PER_BLOCK = 256;
static constexpr size_t PLASTICITY_MAX_BLOCKS_PER_GRID = 65535;
static constexpr size_t PLASTICITY_SHARED_MEMORY_SIZE = 16 * 1024;  // 16 KB

// Forward declarations
class PlasticityCalculations;

// CUDA kernel functions for plasticity calculations
namespace PlasticityKernels {
    // STDP (Spike-Timing-Dependent Plasticity) kernel
    __global__ void kernelSTDPUpdate(const CudaNeuronState* neuronStates,
                                    const float* preSpikeTimes,
                                    const float* postSpikeTimes,
                                    float* weightDeltas,
                                    float* eligibilityTraces,
                                    float currentTime,
                                    float tauPlus,
                                    float tauMinus,
                                    float A_plus,
                                    float A_minus,
                                    unsigned int numNeurons);
    
    // Hebbian learning kernel
    __global__ void kernelHebbianUpdate(const CudaNeuronState* neuronStates,
                                       const uint32_t* preFiringStates,
                                       const uint32_t* postFiringStates,
                                       float* weightDeltas,
                                       float* hebbianTraces,
                                       float learningRate,
                                       float currentTime,
                                       unsigned int numNeurons);
    
    // Reward-modulated plasticity kernel
    __global__ void kernelRewardModulatedPlasticity(const CudaNeuronState* neuronStates,
                                                   const float* rewardSignal,
                                                   float* weightDeltas,
                                                   float* eligibilityTraces,
                                                   float* rewardModulatedUpdates,
                                                   float decayRate,
                                                   float currentTime,
                                                   unsigned int numNeurons);
    
    // Batch plasticity update kernel
    __global__ void kernelPlasticityBatchUpdate(const CudaNeuronState* neuronStates,
                                              const uint32_t* preFiringStates,
                                              const uint32_t* postFiringStates,
                                              const float* preSpikeTimes,
                                              const float* postSpikeTimes,
                                              CudaPlasticityUpdate* plasticityUpdates,
                                              float currentTime,
                                              unsigned int numNeurons,
                                              float tauPlus,
                                              float tauMinus,
                                              float A_plus,
                                              float A_minus,
                                              float learningRate,
                                              float rewardSignal,
                                              float decayRate);
}

// Plasticity update data structure for CUDA
struct CudaPlasticityUpdate {
    float weightDelta;
    float eligibilityTrace;
    float efficacyUpdate;
    float stdpUpdate;
    float hebbianUpdate;
    float rewardModulated;
    uint32_t synapseId;
    uint32_t sourceNeuronId;
    uint32_t destinationNeuronId;
    uint32_t plasticityFlags;
    
    // Constructors
    __host__ __device__ CudaPlasticityUpdate();
    __host__ __device__ CudaPlasticityUpdate(const PlasticityUpdate& update);
    __host__ __device__ PlasticityUpdate toPlasticityUpdate() const;
};

// Plasticity calculations class for CUDA operations
class PlasticityCalculations {
public:
    // Update STDP on GPU
    static void updateSTDP(CudaNeuronState* deviceNeuronStates,
                          const float* preSpikeTimes,
                          const float* postSpikeTimes,
                          float* weightDeltas,
                          float* eligibilityTraces,
                          float currentTime,
                          size_t numNeurons,
                          float tauPlus,
                          float tauMinus,
                          float A_plus,
                          float A_minus,
                          cudaStream_t stream = nullptr);
    
    // Update Hebbian learning on GPU
    static void updateHebbian(CudaNeuronState* deviceNeuronStates,
                             const uint32_t* preFiringStates,
                             const uint32_t* postFiringStates,
                             float* weightDeltas,
                             float* hebbianTraces,
                             float learningRate,
                             float currentTime,
                             size_t numNeurons,
                             cudaStream_t stream = nullptr);
    
    // Update reward-modulated plasticity on GPU
    static void updateRewardModulated(CudaNeuronState* deviceNeuronStates,
                                      const float* rewardSignal,
                                      float* weightDeltas,
                                      float* eligibilityTraces,
                                      float* rewardModulatedUpdates,
                                      float decayRate,
                                      float currentTime,
                                      size_t numNeurons,
                                      cudaStream_t stream = nullptr);
    
    // Batch update plasticity
    static void batchUpdate(CudaNeuronState* deviceNeuronStates,
                           const uint32_t* preFiringStates,
                           const uint32_t* postFiringStates,
                           const float* preSpikeTimes,
                           const float* postSpikeTimes,
                           CudaPlasticityUpdate* deviceUpdates,
                           float currentTime,
                           size_t numNeurons,
                           float tauPlus,
                           float tauMinus,
                           float A_plus,
                           float A_minus,
                           float learningRate,
                           float rewardSignal,
                           float decayRate,
                           cudaStream_t stream = nullptr);
    
    // Copy plasticity data between host and device
    static void copyToDevice(const PlasticityUpdate* hostUpdates,
                            CudaPlasticityUpdate* deviceUpdates,
                            size_t count,
                            cudaStream_t stream = nullptr);
    
    static void copyFromDevice(CudaPlasticityUpdate* deviceUpdates,
                              PlasticityUpdate* hostUpdates,
                              size_t count,
                              cudaStream_t stream = nullptr);
    
    // Process event queue for plasticity
    static void processEventQueue(const QueuedSpikeEvent* events,
                                 size_t numEvents,
                                 CudaPlasticityUpdate* deviceUpdates,
                                 float currentTime,
                                 cudaStream_t stream = nullptr);
    
    // Get performance statistics
    static void getPerformanceStats(float& kernelTime, float& memoryBandwidth);
    
private:
    // Private constructor (plasticity calculations is a namespace)
    PlasticityCalculations() = default;
    ~PlasticityCalculations() = default;
};

} // namespace cuda
} // namespace nlm
