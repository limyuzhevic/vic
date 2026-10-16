// NeuronKernels.hpp: CUDA headers for neuron dynamics
// Kernel declarations and type definitions for GPU-accelerated neuron computation

#pragma once

#include "../core/Types/Types.hpp"
#include "CudaConfig.hpp"
#include <cstddef>

namespace nlm {
namespace cuda {

// Kernel configuration constants
static constexpr size_t NEURON_KERNEL_THREADS_PER_BLOCK = 256;
static constexpr size_t NEURON_MAX_BLOCKS_PER_GRID = 65535;
static constexpr size_t NEURON_SHARED_MEMORY_SIZE = 48 * 1024;  // 48 KB

// Forward declarations
class NeuronDynamics;

// CUDA kernel functions for neuron dynamics
namespace NeuronKernels {
    // LIF neuron update kernel
    __global__ void kernelNeuronLIFUpdate(CudaNeuronState* neuronStates,
                                        const float* synapticInputs,
                                        const float* externalInputs,
                                        const float* leakConductances,
                                        const float* thresholds,
                                        const float* resetPotentials,
                                        const float* adaptationVariables,
                                        const float dt,
                                        const float currentTime,
                                        uint32_t* firingStates,
                                        uint32_t* refractoryCounters,
                                        float* membranePotentials,
                                        unsigned int numNeurons);
    
    // Neuron threshold check kernel
    __global__ void kernelNeuronThresholdCheck(const float* membranePotentials,
                                              const float* thresholds,
                                              uint32_t* firingStates,
                                              unsigned int* spikeCount,
                                              unsigned int numNeurons);
    
    // Neuron reset kernel
    __global__ void kernelNeuronReset(float* membranePotentials,
                                      const uint32_t* firingStates,
                                      const float* resetPotentials,
                                      unsigned int numNeurons);
    
    // Neuron state conversion kernels
    __global__ void kernelNeuronStateToHost(const CudaNeuronState* deviceStates,
                                           NeuronState* hostStates,
                                           unsigned int numNeurons);
    
    __global__ void kernelNeuronStateFromHost(const NeuronState* hostStates,
                                             CudaNeuronState* deviceStates,
                                             unsigned int numNeurons);
    
    // Batch neuron processing kernel
    __global__ void kernelNeuronBatchProcess(CudaNeuronState* neuronStates,
                                            const float* synapticInputs,
                                            const float* externalInputs,
                                            const float dt,
                                            const float currentTime,
                                            uint32_t* firingStates,
                                            uint32_t* refractoryCounters,
                                            float* membranePotentials,
                                            unsigned int startIdx,
                                            unsigned int endIdx,
                                            unsigned int batchSize);
}

// Neuron state structure for CUDA
struct CudaNeuronState {
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapseConductance;
    float firingRate;
    uint32_t firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // Constructors
    __host__ __device__ CudaNeuronState();
    __host__ __device__ CudaNeuronState(const NeuronState& state);
    __host__ __device__ NeuronState toNeuronState() const;
};

// Neuron dynamics class for CUDA operations
class NeuronDynamics {
public:
    // Update neuron states on GPU
    static void update(CudaNeuronState* deviceStates,
                      const float* synapticInputs,
                      const float* externalInputs,
                      const float* leakConductances,
                      const float* thresholds,
                      const float* resetPotentials,
                      const float* adaptationVariables,
                      float dt,
                      float currentTime,
                      uint32_t* firingStates,
                      uint32_t* refractoryCounters,
                      float* membranePotentials,
                      size_t numNeurons,
                      cudaStream_t stream = nullptr);
    
    // Batch process neurons
    static void batchProcess(CudaNeuronState* deviceStates,
                            const float* synapticInputs,
                            const float* externalInputs,
                            float dt,
                            float currentTime,
                            uint32_t* firingStates,
                            uint32_t* refractoryCounters,
                            float* membranePotentials,
                            size_t startIdx,
                            size_t endIdx,
                            cudaStream_t stream = nullptr);
    
    // Copy neuron states between host and device
    static void copyToDevice(const NeuronState* hostStates,
                            CudaNeuronState* deviceStates,
                            size_t count,
                            cudaStream_t stream = nullptr);
    
    static void copyFromDevice(CudaNeuronState* deviceStates,
                              NeuronState* hostStates,
                              size_t count,
                              cudaStream_t stream = nullptr);
    
    // Get performance statistics
    static void getPerformanceStats(float& kernelTime, float& memoryBandwidth);
    
private:
    // Private constructor (neuron dynamics is a namespace)
    NeuronDynamics() = default;
    ~NeuronDynamics() = default;
};

} // namespace cuda
} // namespace nlm
