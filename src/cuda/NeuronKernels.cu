// NeuronKernels.cu: CUDA kernels for neuron dynamics and LIF integration
// High-performance GPU implementation of neuron state updates

#include "NeuronKernels.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {
namespace cuda {

// Neuron state structure for CUDA kernel
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
    
    // Default constructor
    __host__ __device__ CudaNeuronState() 
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(0)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
    
    // Constructor from NeuronState
    __host__ __device__ CudaNeuronState(const NeuronState& state) {
        membranePotential = state.membranePotential;
        restingPotential = state.restingPotential;
        threshold = state.threshold;
        resetPotential = state.resetPotential;
        leakConductance = state.leakConductance;
        synapseConductance = state.synapseConductance;
        firingRate = state.firingRate;
        firingState = static_cast<uint32_t>(state.firingState);
        refractoryRemaining = state.refractoryRemaining;
        refractoryPeriod = state.refractoryPeriod;
        adaptationVariable = state.adaptationVariable;
        lastSpikeTime = state.lastSpikeTime;
    }
    
    // Convert to NeuronState
    __host__ __device__ NeuronState toNeuronState() const {
        NeuronState state;
        state.membranePotential = membranePotential;
        state.restingPotential = restingPotential;
        state.threshold = threshold;
        state.resetPotential = resetPotential;
        state.leakConductance = leakConductance;
        state.synapseConductance = synapseConductance;
        state.firingRate = firingRate;
        state.firingState = static_cast<FiringState>(firingState);
        state.refractoryRemaining = refractoryRemaining;
        state.refractoryPeriod = refractoryPeriod;
        state.adaptationVariable = adaptationVariable;
        state.lastSpikeTime = lastSpikeTime;
        return state;
    }
};

// Vectorized LIF neuron update kernel
// Processes multiple neurons in parallel using CUDA threads
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
                                    unsigned int numNeurons) {
    
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    CudaNeuronState& state = neuronStates[neuronIdx];
    
    // Check if neuron is in refractory period
    if (refractoryCounters[neuronIdx] > 0) {
        // Keep at resting potential during refractory period
        membranePotentials[neuronIdx] = state.restingPotential;
        refractoryCounters[neuronIdx]--;
        firingStates[neuronIdx] = 0;  // Not firing
        return;
    }
    
    // Calculate synaptic input
    float synapticInput = synapticInputs[neuronIdx];
    if (externalInputs) {
        synapticInput += externalInputs[neuronIdx];
    }
    
    // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
    float leakTerm = (state.restingPotential - state.membranePotential) / 
                     (state.leakConductance * 1000.0f);  // Convert nS to ms
    float inputTerm = synapticInput / 1.0f;  // Simplified capacitance
    
    // Update membrane potential
    state.membranePotential += dt * 1000.0f * (leakTerm + inputTerm);
    
    // Update adaptation variable (simple spike frequency adaptation)
    if (state.adaptationVariable > 0.0f) {
        state.adaptationVariable *= 0.95f;  // Decay adaptation
        state.membranePotential -= state.adaptationVariable * 0.1f;
    }
    
    // Check for threshold crossing and spike generation
    if (state.membranePotential >= state.threshold) {
        // Spike detected
        firingStates[neuronIdx] = 1;
        state.membranePotential = state.resetPotential;
        refractoryCounters[neuronIdx] = state.refractoryPeriod;
        
        // Record spike time
        state.lastSpikeTime = currentTime;
        
        // Update adaptation variable on spike
        state.adaptationVariable += 0.1f;  // Increase adaptation
    }
}

// Vectorized neuron threshold check kernel
// Efficiently checks which neurons have crossed threshold
__global__ void kernelNeuronThresholdCheck(const float* membranePotentials,
                                          const float* thresholds,
                                          uint32_t* firingStates,
                                          unsigned int* spikeCount,
                                          unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    // Reset spike count first (atomic)
    if (threadIdx.x == 0) {
        atomicExch(spikeCount, 0);
    }
    __syncthreads();
    
    // Check threshold
    if (membranePotentials[neuronIdx] >= thresholds[neuronIdx]) {
        // Atomically increment spike count
        atomicAdd(spikeCount, 1);
        firingStates[neuronIdx] = 1;
    }
}

// Vectorized neuron reset kernel
// Resets membrane potential of spiked neurons
__global__ void kernelNeuronReset(float* membranePotentials,
                                 const uint32_t* firingStates,
                                 const float* resetPotentials,
                                 unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    if (firingStates[neuronIdx] == 1) {
        membranePotentials[neuronIdx] = resetPotentials[neuronIdx];
    }
}

// Vectorized neuron state conversion kernel
// Converts between host and device neuron state representations
__global__ void kernelNeuronStateToHost(const CudaNeuronState* deviceStates,
                                       NeuronState* hostStates,
                                       unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    hostStates[neuronIdx] = deviceStates[neuronIdx].toNeuronState();
}

__global__ void kernelNeuronStateFromHost(const NeuronState* hostStates,
                                         CudaNeuronState* deviceStates,
                                         unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    deviceStates[neuronIdx] = CudaNeuronState(hostStates[neuronIdx]);
}

// NeuronDynamics::update implementation using CUDA
void NeuronDynamics::update(CudaNeuronState* deviceStates,
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
                           cudaStream_t stream) {
    
    // Calculate grid and block dimensions
    size_t threadsPerBlock = NEURON_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numNeurons + threadsPerBlock - 1) / threadsPerBlock;
    
    // Limit grid size for large populations
    blocksPerGrid = min(blocksPerGrid, NEURON_MAX_BLOCKS_PER_GRID);
    
    // Launch LIF update kernel
    kernelNeuronLIFUpdate<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceStates,
        synapticInputs,
        externalInputs,
        leakConductances,
        thresholds,
        resetPotentials,
        adaptationVariables,
        dt,
        currentTime,
        firingStates,
        refractoryCounters,
        membranePotentials,
        static_cast<unsigned int>(numNeurons)
    );
    
    // Launch threshold check kernel
    kernelNeuronThresholdCheck<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        membranePotentials,
        thresholds,
        firingStates,
        &spikeCount,
        static_cast<unsigned int>(numNeurons)
    );
    
    // Launch neuron reset kernel for spiked neurons
    kernelNeuronReset<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        membranePotentials,
        firingStates,
        resetPotentials,
        static_cast<unsigned int>(numNeurons)
    );
}

// Batch neuron processing kernel
// Efficiently processes batches of neurons for better cache utilization
__global__ void kernelNeuronBatchProcess(CudaNeuronState* neuronStates,
                                        const float* synapticInputs,
                                        const float* externalInputs,
                                        const float dt,
                                        const float currentTime,
                                        uint32_t* firingStates,
                                        uint32_t* refractoryCounters,
                                        float* membranePotentials,
                                        unsigned int startIdx,
                                        unsigned int endIdx) {
    unsigned int neuronIdx = startIdx + threadIdx.x;
    if (neuronIdx >= endIdx) return;
    
    CudaNeuronState& state = neuronStates[neuronIdx - startIdx];
    
    // Process neuron (simplified version)
    if (refractoryCounters[neuronIdx - startIdx] > 0) {
        membranePotentials[neuronIdx - startIdx] = state.restingPotential;
        refractoryCounters[neuronIdx - startIdx]--;
        firingStates[neuronIdx - startIdx] = 0;
        return;
    }
    
    float synapticInput = synapticInputs[neuronIdx - startIdx];
    if (externalInputs) {
        synapticInput += externalInputs[neuronIdx - startIdx];
    }
    
    float leakTerm = (state.restingPotential - state.membranePotential) / 
                     (state.leakConductance * 1000.0f);
    float inputTerm = synapticInput / 1.0f;
    
    state.membranePotential += dt * 1000.0f * (leakTerm + inputTerm);
    
    // Check for spike
    if (state.membranePotential >= state.threshold) {
        firingStates[neuronIdx - startIdx] = 1;
        state.membranePotential = state.resetPotential;
        refractoryCounters[neuronIdx - startIdx] = state.refractoryPeriod;
        state.lastSpikeTime = currentTime;
    }
}

void NeuronDynamics::batchProcess(CudaNeuronState* deviceStates,
                                  const float* synapticInputs,
                                  const float* externalInputs,
                                  float dt,
                                  float currentTime,
                                  uint32_t* firingStates,
                                  uint32_t* refractoryCounters,
                                  float* membranePotentials,
                                  size_t startIdx,
                                  size_t endIdx,
                                  cudaStream_t stream) {
    
    size_t batchSize = endIdx - startIdx;
    if (batchSize == 0) return;
    
    size_t threadsPerBlock = NEURON_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (batchSize + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelNeuronBatchProcess<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceStates,
        synapticInputs,
        externalInputs,
        dt,
        currentTime,
        firingStates,
        refractoryCounters,
        membranePotentials,
        static_cast<unsigned int>(startIdx),
        static_cast<unsigned int>(endIdx),
        static_cast<unsigned int>(batchSize)
    );
}

// Neuron state conversion utilities
void NeuronDynamics::copyToDevice(const NeuronState* hostStates,
                                  CudaNeuronState* deviceStates,
                                  size_t count,
                                  cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = NEURON_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelNeuronStateFromHost<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        hostStates,
        deviceStates,
        static_cast<unsigned int>(count)
    );
}

void NeuronDynamics::copyFromDevice(CudaNeuronState* deviceStates,
                                    NeuronState* hostStates,
                                    size_t count,
                                    cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = NEURON_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelNeuronStateToHost<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceStates,
        hostStates,
        static_cast<unsigned int>(count)
    );
}

// Statistics and benchmarking
void NeuronDynamics::getPerformanceStats(float& kernelTime, float& memoryBandwidth) {
    // In a real implementation, this would track kernel execution times
    // and calculate memory bandwidth usage
    kernelTime = 0.0f;
    memoryBandwidth = 0.0f;
    
    std::cout << "Neuron Dynamics CUDA Stats:" << std::endl;
    std::cout << "  Kernel execution time: " << kernelTime << " ms" << std::endl;
    std::cout << "  Memory bandwidth: " << memoryBandwidth << " GB/s" << std::endl;
}

} // namespace cuda
} // namespace nlm
