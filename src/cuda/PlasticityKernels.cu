// PlasticityKernels.cu: CUDA kernels for plasticity calculations
// High-performance GPU implementation of STDP and Hebbian learning

#include "PlasticityKernels.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {
namespace cuda {

// Plasticity update data structure for CUDA kernel
struct CudaPlasticityUpdate {
    float weightDelta;           // Weight change from plasticity
    float eligibilityTrace;      // Eligibility trace for reward-modulated learning
    float efficacyUpdate;        // Efficacy update for short-term plasticity
    float stdpUpdate;            // STDP contribution
    float hebbianUpdate;         // Hebbian contribution
    float rewardModulated;       // Reward-modulated learning contribution
    uint32_t synapseId;           // Synapse identifier
    uint32_t sourceNeuronId;      // Source neuron identifier
    uint32_t destinationNeuronId; // Destination neuron identifier
    uint32_t plasticityFlags;     // Plasticity rule flags
    
    // Default constructor
    __host__ __device__ CudaPlasticityUpdate() 
        : weightDelta(0.0f), eligibilityTrace(0.0f), efficacyUpdate(0.0f),
          stdpUpdate(0.0f), hebbianUpdate(0.0f), rewardModulated(0.0f),
          synapseId(0), sourceNeuronId(0), destinationNeuronId(0),
          plasticityFlags(0) {}
    
    // Constructor from PlasticityUpdate
    __host__ __device__ CudaPlasticityUpdate(const PlasticityUpdate& update) {
        weightDelta = update.weightDelta;
        eligibilityTrace = update.eligibilityTrace;
        efficacyUpdate = update.efficacyUpdate;
        stdpUpdate = update.stdpUpdate;
        hebbianUpdate = update.hebbianUpdate;
        rewardModulated = update.rewardModulated;
        synapseId = update.synapseId;
        sourceNeuronId = update.sourceNeuronId;
        destinationNeuronId = update.destinationNeuronId;
        plasticityFlags = update.plasticityFlags;
    }
    
    // Convert to PlasticityUpdate
    __host__ __device__ PlasticityUpdate toPlasticityUpdate() const {
        PlasticityUpdate update;
        update.weightDelta = weightDelta;
        update.eligibilityTrace = eligibilityTrace;
        update.efficacyUpdate = efficacyUpdate;
        update.stdpUpdate = stdpUpdate;
        update.hebbianUpdate = hebbianUpdate;
        update.rewardModulated = rewardModulated;
        update.synapseId = synapseId;
        update.sourceNeuronId = sourceNeuronId;
        update.destinationNeuronId = destinationNeuronId;
        update.plasticityFlags = plasticityFlags;
        return update;
    }
};

// STDP (Spike-Timing-Dependent Plasticity) kernel
// Implements Hebbian learning based on spike timing
__global__ void kernelSTDPUpdate(const CudaNeuronState* neuronStates,
                                const float* preSpikeTimes,
                                const float* postSpikeTimes,
                                float* weightDeltas,
                                float* eligibilityTraces,
                                float currentTime,
                                float tauPlus,    // LTP time constant
                                float tauMinus,   // LTD time constant
                                float A_plus,     // LTP amplitude
                                float A_minus,    // LTD amplitude
                                unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    const CudaNeuronState& state = neuronStates[neuronIdx];
    
    // Get last spike times
    float preTime = preSpikeTimes[neuronIdx];
    float postTime = postSpikeTimes[neuronIdx];
    
    // Calculate STDP weight change
    float weightChange = 0.0f;
    
    // Check for pre-before-post spikes (LTP)
    if (preTime > 0.0f && postTime > 0.0f) {
        float dt = postTime - preTime;  // Positive = pre before post
        
        if (dt > 0.0f) {
            // Long-term potentiation
            weightChange += A_plus * exp(-dt / tauPlus);
        } else if (dt < 0.0f) {
            // Long-term depression
            weightChange += -A_minus * exp(dt / tauMinus);  // dt is negative
        }
    }
    
    // Update weight delta
    weightDeltas[neuronIdx] = weightChange;
    
    // Update eligibility trace
    eligibilityTraces[neuronIdx] = eligibilityTraces[neuronIdx] * 0.95f + weightChange;
}

// Hebbian learning kernel
// Implements Hebbian learning based on correlated activity
__global__ void kernelHebbianUpdate(const CudaNeuronState* neuronStates,
                                   const uint32_t* preFiringStates,
                                   const uint32_t* postFiringStates,
                                   float* weightDeltas,
                                   float* hebbianTraces,
                                   float learningRate,
                                   float currentTime,
                                   unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    const CudaNeuronState& state = neuronStates[neuronIdx];
    
    // Check if both pre- and post-synaptic neurons fired
    if (preFiringStates[neuronIdx] == 1 && postFiringStates[neuronIdx] == 1) {
        // Hebbian weight change: increase weight for correlated activity
        float hebbianChange = learningRate * 0.01f;  // Learning rate scaled
        
        // Update weight delta
        weightDeltas[neuronIdx] += hebbianChange;
        
        // Update Hebbian trace
        hebbianTraces[neuronIdx] = hebbianTraces[neuronIdx] * 0.9f + hebbianChange;
    }
}

// Reward-modulated plasticity kernel
// Implements learning based on reward signals
__global__ void kernelRewardModulatedPlasticity(const CudaNeuronState* neuronStates,
                                               const float* rewardSignal,
                                               float* weightDeltas,
                                               float* eligibilityTraces,
                                               float* rewardModulatedUpdates,
                                               float decayRate,
                                               float currentTime,
                                               unsigned int numNeurons) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    // Update eligibility trace with decay
    eligibilityTraces[neuronIdx] *= (1.0f - decayRate);
    
    // Apply reward-modulated learning
    float reward = rewardSignal[0];  // Single reward signal for all neurons
    if (reward != 0.0f) {
        float rewardUpdate = eligibilityTraces[neuronIdx] * reward * 0.1f;
        rewardModulatedUpdates[neuronIdx] = rewardUpdate;
        weightDeltas[neuronIdx] += rewardUpdate;
    }
}

// Batch plasticity update kernel
// Processes multiple synapses in parallel
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
                                          float decayRate) {
    unsigned int neuronIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (neuronIdx >= numNeurons) return;
    
    CudaPlasticityUpdate& update = plasticityUpdates[neuronIdx];
    
    // Initialize update
    update.weightDelta = 0.0f;
    update.stdpUpdate = 0.0f;
    update.hebbianUpdate = 0.0f;
    update.rewardModulated = 0.0f;
    
    // STDP calculation
    float preTime = preSpikeTimes[neuronIdx];
    float postTime = postSpikeTimes[neuronIdx];
    
    if (preTime > 0.0f && postTime > 0.0f) {
        float dt = postTime - preTime;
        if (dt > 0.0f) {
            update.stdpUpdate = A_plus * exp(-dt / tauPlus);
        } else if (dt < 0.0f) {
            update.stdpUpdate = -A_minus * exp(dt / tauMinus);
        }
    }
    
    // Hebbian calculation
    if (preFiringStates[neuronIdx] == 1 && postFiringStates[neuronIdx] == 1) {
        update.hebbianUpdate = learningRate * 0.01f;
    }
    
    // Reward-modulated calculation
    if (rewardSignal != 0.0f) {
        update.eligibilityTrace = update.eligibilityTrace * (1.0f - decayRate);
        update.rewardModulated = update.eligibilityTrace * rewardSignal * 0.1f;
    }
    
    // Total weight change
    update.weightDelta = update.stdpUpdate + update.hebbianUpdate + update.rewardModulated;
    
    // Apply plasticity flags
    if (!(update.plasticityFlags & 1)) {  // No STDP
        update.stdpUpdate = 0.0f;
    }
    if (!(update.plasticityFlags & 2)) {  // No Hebbian
        update.hebbianUpdate = 0.0f;
    }
    if (!(update.plasticityFlags & 4)) {  // No reward-modulated
        update.rewardModulated = 0.0f;
    }
}

// Plasticity calculation implementation using CUDA
void PlasticityCalculations::updateSTDP(CudaNeuronState* deviceNeuronStates,
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
                                       cudaStream_t stream) {
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numNeurons + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelSTDPUpdate<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceNeuronStates,
        preSpikeTimes,
        postSpikeTimes,
        weightDeltas,
        eligibilityTraces,
        currentTime,
        tauPlus,
        tauMinus,
        A_plus,
        A_minus,
        static_cast<unsigned int>(numNeurons)
    );
}

void PlasticityCalculations::updateHebbian(CudaNeuronState* deviceNeuronStates,
                                          const uint32_t* preFiringStates,
                                          const uint32_t* postFiringStates,
                                          float* weightDeltas,
                                          float* hebbianTraces,
                                          float learningRate,
                                          float currentTime,
                                          size_t numNeurons,
                                          cudaStream_t stream) {
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numNeurons + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelHebbianUpdate<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceNeuronStates,
        preFiringStates,
        postFiringStates,
        weightDeltas,
        hebbianTraces,
        learningRate,
        currentTime,
        static_cast<unsigned int>(numNeurons)
    );
}

void PlasticityCalculations::updateRewardModulated(CudaNeuronState* deviceNeuronStates,
                                                  const float* rewardSignal,
                                                  float* weightDeltas,
                                                  float* eligibilityTraces,
                                                  float* rewardModulatedUpdates,
                                                  float decayRate,
                                                  float currentTime,
                                                  size_t numNeurons,
                                                  cudaStream_t stream) {
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numNeurons + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelRewardModulatedPlasticity<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceNeuronStates,
        rewardSignal,
        weightDeltas,
        eligibilityTraces,
        rewardModulatedUpdates,
        decayRate,
        currentTime,
        static_cast<unsigned int>(numNeurons)
    );
}

void PlasticityCalculations::batchUpdate(CudaNeuronState* deviceNeuronStates,
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
                                         cudaStream_t stream) {
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numNeurons + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelPlasticityBatchUpdate<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceNeuronStates,
        preFiringStates,
        postFiringStates,
        preSpikeTimes,
        postSpikeTimes,
        deviceUpdates,
        currentTime,
        static_cast<unsigned int>(numNeurons),
        tauPlus,
        tauMinus,
        A_plus,
        A_minus,
        learningRate,
        rewardSignal,
        decayRate
    );
}

void PlasticityCalculations::copyToDevice(const PlasticityUpdate* hostUpdates,
                                          CudaPlasticityUpdate* deviceUpdates,
                                          size_t count,
                                          cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    __global__ void kernelPlasticityCopyToDevice(const PlasticityUpdate* hostUpdates,
                                                 CudaPlasticityUpdate* deviceUpdates,
                                                 size_t count) {
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx < count) {
            deviceUpdates[idx] = CudaPlasticityUpdate(hostUpdates[idx]);
        }
    }
    
    kernelPlasticityCopyToDevice<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        hostUpdates,
        deviceUpdates,
        static_cast<size_t>(count)
    );
}

void PlasticityCalculations::copyFromDevice(CudaPlasticityUpdate* deviceUpdates,
                                            PlasticityUpdate* hostUpdates,
                                            size_t count,
                                            cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = PLASTICITY_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    __global__ void kernelPlasticityCopyFromDevice(const CudaPlasticityUpdate* deviceUpdates,
                                                   PlasticityUpdate* hostUpdates,
                                                   size_t count) {
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx < count) {
            hostUpdates[idx] = deviceUpdates[idx].toPlasticityUpdate();
        }
    }
    
    kernelPlasticityCopyFromDevice<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceUpdates,
        hostUpdates,
        static_cast<size_t>(count)
    );
}

// Statistics and benchmarking
void PlasticityCalculations::getPerformanceStats(float& kernelTime, float& memoryBandwidth) {
    kernelTime = 0.0f;
    memoryBandwidth = 0.0f;
    
    std::cout << "Plasticity Calculations CUDA Stats:" << std::endl;
    std::cout << "  Kernel execution time: " << kernelTime << " ms" << std::endl;
    std::cout << "  Memory bandwidth: " << memoryBandwidth << " GB/s" << std::endl;
}

// Event queue processing for plasticity
void PlasticityCalculations::processEventQueue(const QueuedSpikeEvent* events,
                                              size_t numEvents,
                                              CudaPlasticityUpdate* deviceUpdates,
                                              float currentTime,
                                              cudaStream_t stream) {
    // Process plasticity events from event queue
    // This would typically involve checking spike times and updating plasticity
    
    std::cout << "Plasticity event queue processing (CUDA)" << std::endl;
}

// Exponential function for STDP
__device__ float exp(float x) {
    return ::expf(x);
}

} // namespace cuda
} // namespace nlm
