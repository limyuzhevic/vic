// CUDA Kernel Headers
#include "NeuronKernels.hpp"
#include "SynapseKernels.hpp"
#include "PlasticityKernels.hpp"
#include "EventQueueKernels.hpp"
#include "PerformanceKernels.hpp"

// CUDA accelerator abstraction
#include "CudaAccelerator.hpp"

// Main CPU-GPU integration interface
#include "CudaBrain.hpp"

// Data transfer manager
#include "MemcpyManager.hpp"

// Asynchronous transfer manager
#include "AsyncTransferManager.hpp"

// CUDA acceleration utilities
namespace nlm {
namespace cuda {

// Main acceleration interface for neural simulation
class NeuralAccelerator {
public:
    NeuralAccelerator();
    ~NeuralAccelerator();
    
    // Initialize accelerator
    bool initialize(const CudaConfig& config);
    
    // Check if GPU acceleration is available
    bool isEnabled() const { return enabled_; }
    
    // Check if CUDA is available on system
    static bool isCudaAvailable();
    
    // Get current device information
    const CudaDeviceInfo& getDeviceInfo() const { return deviceInfo_; }
    
    // Get acceleration statistics
    struct AccelerationStats {
        size_t neuronsProcessed;      // Number of neurons processed by GPU
        size_t synapsesProcessed;     // Number of synapses processed by GPU
        float gpuTimeMs;              // Time spent on GPU
        float cpuTimeMs;              // Time spent on CPU
        float transferTimeMs;         // Time spent on memory transfers
        size_t memoryAllocated;       // Memory allocated on GPU
        double speedup;               // Overall speedup vs CPU
    };
    
    const AccelerationStats& getStats() const { return stats_; }
    
    // Reset statistics
    void resetStats() {
        stats_.neuronsProcessed = 0;
        stats_.synapsesProcessed = 0;
        stats_.gpuTimeMs = 0.0f;
        stats_.cpuTimeMs = 0.0f;
        stats_.transferTimeMs = 0.0f;
        stats_.memoryAllocated = 0;
        stats_.speedup = 1.0;
    }
    
    // Process neuron states on GPU
    bool processNeurons(NeuronState* hostNeuronStates, size_t count,
                       Timestamp currentTime, TimestepDuration dt);
    
    // Process synapses on GPU
    bool processSynapses(Synapse* hostSynapses, NeuronState* hostNeuronStates,
                        size_t count, Timestamp currentTime);
    
    // Process plasticity on GPU
    bool processPlasticity(Synapse* hostSynapses, NeuronState* hostNeuronStates,
                          const SpikeHistory& spikeHistory, size_t count,
                          Timestamp currentTime, float plasticityMod);
    
    // Process spike events on GPU
    bool processSpikeEvents(const QueuedSpikeEvent* hostEvents, size_t count,
                           NeuronState* hostNeuronStates, size_t numNeurons);
    
    // Execute plasticity kernel
    bool executePlasticityKernel(SynapticWeight* hostWeights, const float* preSpikes,
                                const float* postSpikes, size_t synapseCount,
                                float learningRate, float currentTime);
    
    // Execute neuron integration kernel
    bool executeNeuronIntegrationKernel(float* hostMembranePotentials,
                                        const float* hostSynapticInputs,
                                        const float* hostRefractoryCounters,
                                        uint32_t* hostFiringStates,
                                        size_t count, float dt);
    
    // Transfer data to GPU
    bool transferNeuronsToDevice(const NeuronState* hostData, size_t count);
    bool transferSynapsesToDevice(const Synapse* hostData, size_t count);
    bool transferSpikeEventsToDevice(const QueuedSpikeEvent* hostData, size_t count);
    bool transferPlasticityDataToDevice(const PlasticityUpdate* hostData, size_t count);
    
    // Transfer data from GPU to CPU
    bool transferNeuronsFromDevice(NeuronState* hostData, size_t count);
    bool transferSynapsesFromDevice(Synapse* hostData, size_t count);
    
    // Synchronize with GPU
    void synchronize();
    
    // Shutdown accelerator
    void shutdown();
    
    // Get configuration
    const CudaConfig& getConfig() const { return config_; }
    
    // Set configuration
    void setConfig(const CudaConfig& config) { config_ = config; }
    
private:
    bool enabled_;                           // Whether GPU acceleration is enabled
    CudaConfig config_;                      // Configuration
    CudaDeviceInfo deviceInfo_;              // Device information
    AccelerationStats stats_;                // Performance statistics
    
    // Device pointers
    void* deviceNeuronStates_;               // Device memory for neuron states
    void* deviceSynapses_;                   // Device memory for synapses
    void* deviceSpikeEvents_;                // Device memory for spike events
    void* devicePlasticityData_;              // Device memory for plasticity data
    
    // CUDA streams
    cudaStream_t computeStream_;             // Stream for compute operations
    cudaStream_t memcpyStream_;              // Stream for memory transfers
    
    // CUDA events
    cudaEvent_t startEvent_;                 // Event for timing
    cudaEvent_t stopEvent_;                  // Event for timing
    
    // Initialize CUDA context
    bool initializeCudaContext();
    
    // Clean up CUDA context
    void cleanupCudaContext();
    
    // Transfer data with statistics
    template<typename T>
    bool transferDataToDevice(const T* hostData, size_t count, void*& devicePtr, cudaStream_t stream = nullptr);
    
    template<typename T>
    bool transferDataFromDevice(T* hostData, const void* devicePtr, size_t count, cudaStream_t stream = nullptr);
    
    // Record timing statistics
    void recordGpuTime(float ms) { stats_.gpuTimeMs += ms; }
    void recordCpuTime(float ms) { stats_.cpuTimeMs += ms; }
    void recordTransferTime(float ms) { stats_.transferTimeMs += ms; }
    
    // Calculate speedup
    void updateSpeedup() {
        if (stats_.cpuTimeMs > 0.0f) {
            stats_.speedup = (stats_.cpuTimeMs + stats_.gpuTimeMs) / stats_.gpuTimeMs;
        }
    }
};

} // namespace cuda
} // namespace nlm
