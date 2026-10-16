// SynapseKernels.cu: CUDA kernels for synaptic operations
// High-performance GPU implementation of synaptic current calculation and spike propagation

#include "SynapseKernels.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {
namespace cuda {

// Synapse data structure for CUDA kernel
struct CudaSynapse {
    uint32_t sourceNeuron;
    uint32_t destinationNeuron;
    float weight;
    float delay;
    uint32_t type;
    uint32_t plasticityFlags;
    float efficacy;
    float eligibilityTrace;
    
    // Default constructor
    __host__ __device__ CudaSynapse() 
        : sourceNeuron(0), destinationNeuron(0), weight(0.0f), delay(0.0f),
          type(0), plasticityFlags(0), efficacy(0.0f), eligibilityTrace(0.0f) {}
    
    // Constructor from Synapse
    __host__ __device__ CudaSynapse(const Synapse* synapse) {
        if (synapse) {
            sourceNeuron = static_cast<uint32_t>(synapse->getSourceNeuron().index());
            destinationNeuron = static_cast<uint32_t>(synapse->getDestinationNeuron().index());
            weight = synapse->getWeight();
            delay = static_cast<float>(synapse->getDelay());
            type = static_cast<uint32_t>(synapse->getType());
            plasticityFlags = (synapse->getPlasticityFlags().hebbian ? 1 : 0) |
                            (synapse->getPlasticityFlags().stdp ? 2 : 0) |
                            (synapse->getPlasticityFlags().reward_modulated ? 4 : 0);
            efficacy = synapse->getEfficacy();
            eligibilityTrace = synapse->getEligibilityTrace();
        } else {
            sourceNeuron = destinationNeuron = 0;
            weight = delay = 0.0f;
            type = plasticityFlags = 0;
            efficacy = eligibilityTrace = 0.0f;
        }
    }
    
    // Convert to Synapse (simplified version)
    __host__ __device__ Synapse toSynapse() const {
        Synapse syn(0, 0);  // IDs would need to be restored
        syn.setWeight(weight);
        syn.setDelay(static_cast<Delay>(delay));
        // Note: Cannot restore neuron IDs from this structure
        return syn;
    }
};

// Synaptic current calculation kernel
// Computes synaptic currents for multiple synapses in parallel
__global__ void kernelSynapseCurrentCalculation(CudaSynapse* synapses,
                                                const float* neuronMembranePotentials,
                                                const uint32_t* neuronFiringStates,
                                                float* synapticInputs,
                                                float* synapticConductances,
                                                float currentTime,
                                                unsigned int numSynapses) {
    unsigned int synIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (synIdx >= numSynapses) return;
    
    CudaSynapse& syn = synapses[synIdx];
    
    // Calculate synaptic current based on synapse type and weight
    float synapticCurrent = 0.0f;
    float conductance = 0.0f;
    
    // Excitatory synapse (simple AMPA-like dynamics)
    if (syn.type == 0) {  // Excitatory
        // Simple exponential decay model
        float weight = syn.weight;
        conductance = weight * 0.1f;  // Scaling factor
        synapticCurrent = conductance * (neuronMembranePotentials[syn.destinationNeuron] - (-70.0f));
        
        // Add short-term plasticity
        if (syn.efficacy > 0.0f) {
            synapticCurrent *= syn.efficacy;
        }
    }
    
    // Inhibitory synapse (GABA-like dynamics)
    else if (syn.type == 1) {  // Inhibitory
        float weight = syn.weight;
        conductance = weight * 0.1f;
        synapticCurrent = conductance * (neuronMembranePotentials[syn.destinationNeuron] - (-55.0f));
        synapticCurrent *= -1.0f;  // Make it inhibitory
    }
    
    // Store results
    synapticInputs[syn.destinationNeuron] += synapticCurrent;
    synapticConductances[synIdx] = conductance;
}

// Batch synaptic current computation kernel
// Optimized for processing many synapses simultaneously
__global__ void kernelSynapseBatchCurrent(const CudaSynapse* synapses,
                                         const float* neuronMembranePotentials,
                                         const uint32_t* neuronFiringStates,
                                         float* synapticInputs,
                                         size_t numSynapses,
                                         size_t batchStart,
                                         size_t batchEnd) {
    // Process synapses in this batch
    for (size_t synIdx = batchStart + threadIdx.x;
         synIdx < batchEnd; synIdx += blockDim.x) {
        
        if (synIdx >= numSynapses) break;
        
        const CudaSynapse& syn = synapses[synIdx];
        
        // Calculate synaptic current
        float synapticCurrent = 0.0f;
        
        if (syn.type == 0) {  // Excitatory
            float weight = syn.weight * 0.1f;
            synapticCurrent = weight * (neuronMembranePotentials[syn.destinationNeuron] - (-70.0f));
            synapticCurrent *= syn.efficacy;
        }
        else if (syn.type == 1) {  // Inhibitory
            float weight = syn.weight * 0.1f;
            synapticCurrent = weight * (neuronMembranePotentials[syn.destinationNeuron] - (-55.0f));
            synapticCurrent *= -1.0f;
        }
        
        // Atomically add to destination neuron's synaptic input
        atomicAdd(&synapticInputs[syn.destinationNeuron], synapticCurrent);
    }
}

// Spike propagation kernel
// Delivers spike events from source to destination neurons
__global__ void kernelSynapseSpikePropagation(const CudaSynapse* synapses,
                                             const uint32_t* sourceFiringStates,
                                             float* synapticInputs,
                                             uint64_t* spikeDeliveryTimes,
                                             float currentTime,
                                             unsigned int numSynapses) {
    unsigned int synIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (synIdx >= numSynapses) return;
    
    const CudaSynapse& syn = synapses[synIdx];
    
    // Check if source neuron spiked
    if (sourceFiringStates[syn.sourceNeuron] == 1) {
        // Calculate spike delivery time (with delay)
        uint64_t deliveryTime = currentTime + static_cast<uint64_t>(syn.delay * 1000.0f);
        spikeDeliveryTimes[synIdx] = deliveryTime;
        
        // Calculate synaptic current for spike
        float spikeCurrent = 0.0f;
        
        if (syn.type == 0) {  // Excitatory
            spikeCurrent = syn.weight * 10.0f;  // Spike amplitude
            spikeCurrent *= syn.efficacy;
        }
        else if (syn.type == 1) {  // Inhibitory
            spikeCurrent = -syn.weight * 10.0f;  // Spike amplitude (inhibitory)
        }
        
        // Add to destination neuron's synaptic input
        atomicAdd(&synapticInputs[syn.destinationNeuron], spikeCurrent);
    }
}

// Delayed spike delivery kernel
// Delivers spikes that have been delayed
__global__ void kernelDelayedSpikeDelivery(const CudaSynapse* synapses,
                                          const uint64_t* spikeDeliveryTimes,
                                          const float* neuronMembranePotentials,
                                          float* synapticInputs,
                                          float currentTime,
                                          unsigned int numSynapses,
                                          uint32_t* deliveryStatus) {
    unsigned int synIdx = blockIdx.x * blockDim.x + threadIdx.x;
    if (synIdx >= numSynapses) return;
    
    const CudaSynapse& syn = synapses[synIdx];
    uint64_t deliveryTime = spikeDeliveryTimes[synIdx];
    
    // Check if spike is due for delivery
    if (deliveryTime > 0 && deliveryTime <= currentTime) {
        // Deliver spike to destination neuron
        float spikeCurrent = 0.0f;
        
        if (syn.type == 0) {  // Excitatory
            spikeCurrent = syn.weight * 10.0f;
            spikeCurrent *= syn.efficacy;
        }
        else if (syn.type == 1) {  // Inhibitory
            spikeCurrent = -syn.weight * 10.0f;
        }
        
        // Atomically add to destination neuron's synaptic input
        atomicAdd(&synapticInputs[syn.destinationNeuron], spikeCurrent);
        
        // Mark spike as delivered
        deliveryStatus[synIdx] = 1;
        
        // Reset delivery time
        spikeDeliveryTimes[synIdx] = 0;
    }
}

// Synapse processing implementation using CUDA
void SynapseProcessing::processCurrents(CudaSynapse* deviceSynapses,
                                       const float* neuronMembranePotentials,
                                       const uint32_t* neuronFiringStates,
                                       float* synapticInputs,
                                       float* synapticConductances,
                                       float currentTime,
                                       size_t numSynapses,
                                       cudaStream_t stream) {
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numSynapses + threadsPerBlock - 1) / threadsPerBlock;
    
    // Limit grid size for large populations
    blocksPerGrid = min(blocksPerGrid, SYNAPSE_MAX_BLOCKS_PER_GRID);
    
    // Launch synaptic current calculation kernel
    kernelSynapseCurrentCalculation<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceSynapses,
        neuronMembranePotentials,
        neuronFiringStates,
        synapticInputs,
        synapticConductances,
        currentTime,
        static_cast<unsigned int>(numSynapses)
    );
}

void SynapseProcessing::batchProcessCurrents(const CudaSynapse* deviceSynapses,
                                           const float* neuronMembranePotentials,
                                           const uint32_t* neuronFiringStates,
                                           float* synapticInputs,
                                           size_t numSynapses,
                                           size_t batchStart,
                                           size_t batchEnd,
                                           cudaStream_t stream) {
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (batchEnd - batchStart + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelSynapseBatchCurrent<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceSynapses,
        neuronMembranePotentials,
        neuronFiringStates,
        synapticInputs,
        numSynapses,
        static_cast<size_t>(batchStart),
        static_cast<size_t>(batchEnd)
    );
}

void SynapseProcessing::processSpikePropagation(const CudaSynapse* deviceSynapses,
                                               const uint32_t* sourceFiringStates,
                                               float* synapticInputs,
                                               uint64_t* spikeDeliveryTimes,
                                               float currentTime,
                                               size_t numSynapses,
                                               cudaStream_t stream) {
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numSynapses + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelSynapseSpikePropagation<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceSynapses,
        sourceFiringStates,
        synapticInputs,
        spikeDeliveryTimes,
        currentTime,
        static_cast<unsigned int>(numSynapses)
    );
}

void SynapseProcessing::processDelayedSpikeDelivery(const CudaSynapse* deviceSynapses,
                                                   const uint64_t* spikeDeliveryTimes,
                                                   const float* neuronMembranePotentials,
                                                   float* synapticInputs,
                                                   float currentTime,
                                                   size_t numSynapses,
                                                   uint32_t* deliveryStatus,
                                                   cudaStream_t stream) {
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (numSynapses + threadsPerBlock - 1) / threadsPerBlock;
    
    kernelDelayedSpikeDelivery<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceSynapses,
        spikeDeliveryTimes,
        neuronMembranePotentials,
        synapticInputs,
        currentTime,
        static_cast<unsigned int>(numSynapses),
        deliveryStatus
    );
}

// Synapse state conversion utilities
void SynapseProcessing::copyToDevice(const Synapse* hostSynapses,
                                     CudaSynapse* deviceSynapses,
                                     size_t count,
                                     cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    // Create a wrapper kernel for copy
    __global__ void kernelSynapseCopyToDevice(const Synapse* hostSynapses,
                                             CudaSynapse* deviceSynapses,
                                             size_t count) {
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx < count) {
            deviceSynapses[idx] = CudaSynapse(&hostSynapses[idx]);
        }
    }
    
    kernelSynapseCopyToDevice<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        hostSynapses,
        deviceSynapses,
        static_cast<size_t>(count)
    );
}

void SynapseProcessing::copyFromDevice(CudaSynapse* deviceSynapses,
                                       Synapse* hostSynapses,
                                       size_t count,
                                       cudaStream_t stream) {
    if (count == 0) return;
    
    size_t threadsPerBlock = SYNAPSE_KERNEL_THREADS_PER_BLOCK;
    size_t blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;
    
    __global__ void kernelSynapseCopyFromDevice(const CudaSynapse* deviceSynapses,
                                               Synapse* hostSynapses,
                                               size_t count) {
        unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx < count) {
            hostSynapses[idx] = deviceSynapses[idx].toSynapse();
        }
    }
    
    kernelSynapseCopyFromDevice<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
        deviceSynapses,
        hostSynapses,
        static_cast<size_t>(count)
    );
}

// Statistics and benchmarking
void SynapseProcessing::getPerformanceStats(float& kernelTime, float& memoryBandwidth) {
    kernelTime = 0.0f;
    memoryBandwidth = 0.0f;
    
    std::cout << "Synapse Processing CUDA Stats:" << std::endl;
    std::cout << "  Kernel execution time: " << kernelTime << " ms" << std::endl;
    std::cout << "  Memory bandwidth: " << memoryBandwidth << " GB/s" << std::endl;
}

// Event queue processing utilities
void SynapseProcessing::processEventQueue(const QueuedSpikeEvent* events,
                                         size_t numEvents,
                                         CudaSynapse* deviceSynapses,
                                         float* synapticInputs,
                                         cudaStream_t stream) {
    // Convert events to device format if needed
    // Process events using synapse kernels
    // This would typically use the EventQueueKernels
    
    std::cout << "Event queue processing (CUDA)" << std::endl;
}

} // namespace cuda
} // namespace nlm
