// CudaBrain implementation
#include "CudaBrain.hpp"
#include "CudaConfig.hpp"
#include "../core/Types/Types.hpp"
#include "../performance/EventQueue.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace cuda {

// Implementation details
struct CudaBrain::Impl {
    // Event processing timestamps
    Timestamp lastProcessedStep = 0;
    Timestamp nextPlasticityUpdate = 0;
    
    // Processing flags
    bool plasticityEnabled = false;
    bool neuromodulationEnabled = false;
    bool developmentEnabled = false;
    
    // Performance tracking
    size_t neuronsProcessed = 0;
    size_t synapsesProcessed = 0;
    float lastGpuStepTime = 0.0f;
    float lastCpuStepTime = 0.0f;
    
    // Memory flags
    bool neuronsDirty = false;
    bool synapsesDirty = false;
    bool eventsDirty = false;
    bool plasticityDirty = false;
    
    // Configuration from config
    bool enableGpuAcceleration = true;
    bool automaticFallback = true;
    float gpuMinSpeedupThreshold = 1.5f;
    size_t gpuMinNeuronsForAcceleration = 1000;
};

// Constructor
CudaBrain::CudaBrain(std::shared_ptr<const Config> config)
    : config_(config)
    , gpuAccelerated_(false)
    , cpuBrain_(nullptr)
    , accelerator_(nullptr)
    , cudaAccelerator_(nullptr)
    , memcpyManager_(nullptr)
    , asyncTransferManager_(nullptr)
    , spikeQueue_(nullptr)
    , delayedSpikeQueue_(nullptr) {
    
    pImpl = std::make_unique<Impl>();
    
    // Configure from settings
    configureFromConfig();
    
    // Initialize event queues
    setupEventQueues();
    
    std::cout << "CudaBrain initialized" << std::endl;
}

// Move constructor
CudaBrain::CudaBrain(CudaBrain&& other) noexcept
    : config_(std::move(other.config_))
    , gpuConfig_(std::move(other.gpuConfig_))
    , gpuAccelerated_(other.gpuAccelerated_)
    , cpuBrain_(std::move(other.cpuBrain_))
    , accelerator_(std::move(other.accelerator_))
    , cudaAccelerator_(std::move(other.cudaAccelerator_))
    , memcpyManager_(std::move(other.memcpyManager_))
    , asyncTransferManager_(std::move(other.asyncTransferManager_))
    , spikeQueue_(std::move(other.spikeQueue_))
    , delayedSpikeQueue_(std::move(other.delayedSpikeQueue_))
    , acceleratorStats_(std::move(other.acceleratorStats_)) {
    
    pImpl = std::move(other.pImpl);
}

// Move assignment operator
CudaBrain& CudaBrain::operator=(CudaBrain&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        gpuConfig_ = std::move(other.gpuConfig_);
        gpuAccelerated_ = other.gpuAccelerated_;
        cpuBrain_ = std::move(other.cpuBrain_);
        accelerator_ = std::move(other.accelerator_);
        cudaAccelerator_ = std::move(other.cudaAccelerator_);
        memcpyManager_ = std::move(other.memcpyManager_);
        asyncTransferManager_ = std::move(other.asyncTransferManager_);
        spikeQueue_ = std::move(other.spikeQueue_);
        delayedSpikeQueue_ = std::move(other.delayedSpikeQueue_);
        acceleratorStats_ = std::move(other.acceleratorStats_);
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Destructor
CudaBrain::~CudaBrain() {
    cleanupGpuComponents();
    std::cout << "CudaBrain destroyed" << std::endl;
}

// Initialize brain with GPU acceleration
bool CudaBrain::initialize() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    std::cout << "Initializing CudaBrain with GPU acceleration..." << std::endl;
    
    // Create underlying CPU brain
    cpuBrain_ = std::make_unique<Brain>(config_);
    if (!cpuBrain_->initialize()) {
        std::cerr << "Failed to initialize CPU brain" << std::endl;
        return false;
    }
    
    // Initialize GPU components
    if (!initializeGpuComponents()) {
        std::cout << "GPU acceleration initialization failed, falling back to CPU-only mode" << std::endl;
        gpuAccelerated_ = false;
        
        // Try to continue without GPU acceleration
        if (pImpl->automaticFallback) {
            std::cout << "Continuing with CPU-only implementation" << std::endl;
        } else {
            std::cerr << "GPU acceleration is required but not available!" << std::endl;
            return false;
        }
    } else {
        std::cout << "GPU acceleration successfully initialized" << std::endl;
        gpuAccelerated_ = true;
    }
    
    return true;
}

// Initialize GPU components
bool CudaBrain::initializeGpuComponents() {
    // Check if CUDA is available
    if (!NeuralAccelerator::isCudaAvailable()) {
        std::cout << "CUDA is not available on this system" << std::endl;
        return false;
    }
    
    // Create accelerator
    accelerator_ = std::make_unique<NeuralAccelerator>();
    if (!accelerator_->initialize(gpuConfig_)) {
        std::cerr << "Failed to initialize neural accelerator" << std::endl;
        return false;
    }
    
    // Create CUDA accelerator abstraction
    cudaAccelerator_ = std::make_unique<CudaAccelerator>();
    if (!cudaAccelerator_->initialize()) {
        std::cerr << "Failed to initialize CUDA accelerator" << std::endl;
        return false;
    }
    
    // Create memory managers
    memcpyManager_ = std::make_unique<MemcpyManager>();
    asyncTransferManager_ = std::make_unique<AsyncTransferManager>();
    
    // Initialize memory pools
    if (!cuda::initializeMemoryPool()) {
        std::cerr << "Failed to initialize CUDA memory pool" << std::endl;
        return false;
    }
    
    return true;
}

// Clean up GPU components
void CudaBrain::cleanupGpuComponents() {
    if (accelerator_) {
        accelerator_->shutdown();
        accelerator_.reset();
    }
    
    if (cudaAccelerator_) {
        cudaAccelerator_.reset();
    }
    
    if (memcpyManager_) {
        memcpyManager_.reset();
    }
    
    if (asyncTransferManager_) {
        asyncTransferManager_.reset();
    }
    
    cuda::shutdownMemoryPool();
    
    gpuAccelerated_ = false;
}

// Configure from settings
bool CudaBrain::configureFromConfig() {
    if (!config_) {
        return false;
    }
    
    // Try to get GPU configuration from config file
    bool enableGpu = pImpl->enableGpuAcceleration;
    
    // Check config for GPU settings
    auto gpuEnable = config_->getBool("gpu.enable");
    if (gpuEnable.has_value()) {
        enableGpu = gpuEnable.value();
    }
    
    gpuConfig_.enableGpuAcceleration = enableGpu;
    
    // Device ID
    auto deviceId = config_->getInt("gpu.deviceId");
    if (deviceId.has_value()) {
        gpuConfig_.deviceId = deviceId.value();
    }
    
    // Threads per block
    auto threadsPerBlock = config_->getInt("gpu.threadsPerBlock");
    if (threadsPerBlock.has_value()) {
        gpuConfig_.cudaThreadsPerBlock = threadsPerBlock.value();
    }
    
    // Max speedup threshold for automatic fallback
    auto minSpeedup = config_->getFloat("gpu.minSpeedupThreshold");
    if (minSpeedup.has_value()) {
        pImpl->gpuMinSpeedupThreshold = minSpeedup.value();
    }
    
    return true;
}

// Setup event queues
void CudaBrain::setupEventQueues() {
    // Create spike event queues
    spikeQueue_ = std::make_unique<SpikeRingBuffer>();
    delayedSpikeQueue_ = std::make_unique<DelayedSpikeQueue>();
}

// Set GPU configuration
void CudaBrain::setGpuConfig(const CudaConfig& config) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    gpuConfig_ = config;
    
    if (accelerator_) {
        accelerator_->setConfig(config);
    }
}

// Main simulation step with GPU acceleration
void CudaBrain::step(SimulationStep currentStep) {
    Timestamp currentTime = static_cast<Timestamp>(currentStep);
    step(currentStep, currentTime);
}

// Simulation step with explicit timestamp
void CudaBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process GPU-accelerated step if available
    if (gpuAccelerated_ && accelerator_) {
        processGpuAcceleratedStep(currentStep, currentTime);
    } else {
        // Process CPU-only step
        processCpuFallbackStep(currentStep, currentTime);
    }
    
    // Update accelerator statistics
    updateAcceleratorStats();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Log step processing time
    if (duration.count() > 1000) {  // Log if step takes > 1ms
        std::cout << "Step " << currentStep << " completed in " << (duration.count() / 1000.0f) << " ms" << std::endl;
    }
}

// Process GPU-accelerated step
void CudaBrain::processGpuAcceleratedStep(SimulationStep currentStep, Timestamp currentTime) {
    // Transfer neuron states to GPU
    transferNeuronsToGpu();
    
    // Transfer synapses to GPU
    transferSynapsesToGpu();
    
    // Transfer events to GPU
    transferEventsToGpu();
    
    // Transfer plasticity data to GPU
    transferPlasticityDataToGpu();
    
    // Execute GPU-accelerated computations
    // This would call the various CUDA kernels
    
    // Transfer results back from GPU
    transferNeuronsFromGpu();
    transferSynapsesFromGpu();
    
    // Synchronize GPU operations
    accelerator_->synchronize();
    
    // Update CPU brain's internal state with GPU results
    // (This would involve copying data back to CPU structures)
}

// Process CPU fallback step
void CudaBrain::processCpuFallbackStep(SimulationStep currentStep, Timestamp currentTime) {
    // Use underlying CPU brain for processing
    cpuBrain_->step(currentStep, currentTime);
}

// Transfer neurons to GPU
void CudaBrain::transferNeuronsToGpu() {
    if (!accelerator_ || !pImpl->neuronsDirty) return;
    
    // Transfer neuron states from CPU to GPU
    // This would use the accelerator's transferToDevice method
    pImpl->neuronsDirty = false;
}

// Transfer synapses to GPU
void CudaBrain::transferSynapsesToGpu() {
    if (!accelerator_ || !pImpl->synapsesDirty) return;
    
    // Transfer synapse data from CPU to GPU
    // This would use the accelerator's transferToDevice method
    pImpl->synapsesDirty = false;
}

// Transfer events to GPU
void CudaBrain::transferEventsToGpu() {
    if (!accelerator_ || !pImpl->eventsDirty) return;
    
    // Transfer event queue data from CPU to GPU
    // This would use the accelerator's transferToDevice method
    pImpl->eventsDirty = false;
}

// Transfer plasticity data to GPU
void CudaBrain::transferPlasticityDataToGpu() {
    if (!accelerator_ || !pImpl->plasticityDirty) return;
    
    // Transfer plasticity data from CPU to GPU
    // This would use the accelerator's transferToDevice method
    pImpl->plasticityDirty = false;
}

// Transfer neurons from GPU
void CudaBrain::transferNeuronsFromGpu() {
    if (!accelerator_) return;
    
    // Transfer neuron states from GPU to CPU
    // This would use the accelerator's transferFromDevice method
}

// Transfer synapses from GPU
void CudaBrain::transferSynapsesFromGpu() {
    if (!accelerator_) return;
    
    // Transfer synapse data from GPU to CPU
    // This would use the accelerator's transferFromDevice method
}

// Update accelerator statistics
void CudaBrain::updateAcceleratorStats() {
    if (accelerator_) {
        acceleratorStats_ = accelerator_->getStats();
    }
}

// Receive sensory input (with GPU acceleration if available)
void CudaBrain::receiveSensoryInput(const SensoryInput& input) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    // Send to underlying CPU brain
    cpuBrain_->receiveSensoryInput(input);
    
    // If GPU accelerated, also transfer to GPU (for future steps)
    if (gpuAccelerated_) {
        // Transfer updated neuron states to GPU for next step
        transferNeuronsToGpu();
    }
}

// Inject current into neuron
void CudaBrain::injectCurrent(NeuronId neuron, MembranePotential current) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->injectCurrent(neuron, current);
}

// Inject current to neurons of specific type
void CudaBrain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->injectCurrentToNeurons(type, current);
}

// Get spike system
SpikeSystem* CudaBrain::getSpikeSystem() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getSpikeSystem();
}

const SpikeSystem* CudaBrain::getSpikeSystem() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getSpikeSystem();
}

// Get plasticity systems
STDP* CudaBrain::getSTDP() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getSTDP();
}

Hebbian* CudaBrain::getHebbian() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getHebbian();
}

StructuralPlasticity* CudaBrain::getStructuralPlasticity() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getStructuralPlasticity();
}

// Statistics
float CudaBrain::getExcitationInhibitionRatio() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getExcitationInhibitionRatio();
}

size_t CudaBrain::getTotalSpikeCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getTotalSpikeCount();
}

size_t CudaBrain::getPendingSpikeEventCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return spikeQueue_ ? spikeQueue_->size() : 0;
}

// Produce motor output
std::unique_ptr<Action> CudaBrain::produceAction() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->produceAction();
}

// Apply neuromodulation
void CudaBrain::applyNeuromodulation(const Neuromodulator& signal) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->applyNeuromodulation(signal);
}

// Update plasticity rules
void CudaBrain::updatePlasticity() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->updatePlasticity();
}

// Apply developmental changes
void CudaBrain::develop() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->develop();
}

// Reset brain state
void CudaBrain::reset() {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    if (cpuBrain_) {
        cpuBrain_->reset();
    }
    
    if (accelerator_) {
        accelerator_->synchronize();
    }
    
    // Reset event queues
    if (spikeQueue_) {
        spikeQueue_->clear();
    }
    
    if (delayedSpikeQueue_) {
        delayedSpikeQueue_->clear();
    }
    
    // Reset internal flags
    pImpl->neuronsDirty = false;
    pImpl->synapsesDirty = false;
    pImpl->eventsDirty = false;
    pImpl->plasticityDirty = false;
}

// Save brain state
bool CudaBrain::save(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    // Save CPU brain state
    bool cpuSaved = cpuBrain_->save(filepath + ".cpu");
    
    // Save GPU state if accelerated
    bool gpuSaved = true;
    if (gpuAccelerated_ && accelerator_) {
        // Save GPU accelerator state
        // This would involve saving device memory
        gpuSaved = false;  // Not implemented yet
    }
    
    return cpuSaved && gpuSaved;
}

// Load brain state
bool CudaBrain::load(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    // Load CPU brain state
    bool cpuLoaded = cpuBrain_->load(filepath + ".cpu");
    
    // Load GPU state if accelerated
    bool gpuLoaded = true;
    if (gpuAccelerated_ && accelerator_) {
        // Load GPU accelerator state
        // This would involve loading device memory
        gpuLoaded = false;  // Not implemented yet
    }
    
    // Update flags
    if (cpuLoaded) {
        pImpl->neuronsDirty = true;
        pImpl->synapsesDirty = true;
        pImpl->eventsDirty = true;
        pImpl->plasticityDirty = true;
    }
    
    return cpuLoaded && gpuLoaded;
}

// Region management
RegionId CudaBrain::addRegion(const std::string& name) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->addRegion(name);
}

NeuralRegion* CudaBrain::getRegion(RegionId id) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getRegion(id);
}

const NeuralRegion* CudaBrain::getRegion(RegionId id) const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getRegion(id);
}

size_t CudaBrain::getRegionCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getRegionCount();
}

std::vector<RegionId> CudaBrain::getRegionIds() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getRegionIds();
}

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& CudaBrain::getRegions() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getRegions();
}

// Inter-region connection management
void CudaBrain::addInterRegionConnection(RegionId source, RegionId target, 
                                         float weight, Delay delay) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->addInterRegionConnection(source, target, weight, delay);
}

void CudaBrain::removeInterRegionConnection(RegionId source, RegionId target) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    cpuBrain_->removeInterRegionConnection(source, target);
}

// Global statistics
size_t CudaBrain::getTotalNeuronCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getTotalNeuronCount();
}

size_t CudaBrain::getTotalSynapseCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getTotalSynapseCount();
}

size_t CudaBrain::getActiveNeuronCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getActiveNeuronCount();
}

size_t CudaBrain::getFiringNeuronCount() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getFiringNeuronCount();
}

float CudaBrain::getAverageFiringRate() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    return cpuBrain_->getAverageFiringRate();
}

// Logging
void CudaBrain::logStatus() const {
    std::lock_guard<std::mutex> lock(syncMutex_);
    
    std::cout << "=== CudaBrain Status ===" << std::endl;
    std::cout << "GPU Acceleration: " << (gpuAccelerated_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "CPU Brain Neuron Count: " << getTotalNeuronCount() << std::endl;
    std::cout << "CPU Brain Synapse Count: " << getTotalSynapseCount() << std::endl;
    std::cout << "Total Spike Count: " << getTotalSpikeCount() << std::endl;
    std::cout << "Pending Event Count: " << getPendingSpikeEventCount() << std::endl;
    
    if (accelerator_) {
        const auto& stats = accelerator_->getStats();
        std::cout << "GPU Accelerator Stats:" << std::endl;
        std::cout << "  Neurons Processed: " << stats.neuronsProcessed << std::endl;
        std::cout << "  Synapses Processed: " << stats.synapsesProcessed << std::endl;
        std::cout << "  GPU Time: " << stats.gpuTimeMs << " ms" << std::endl;
        std::cout << "  CPU Time: " << stats.cpuTimeMs << " ms" << std::endl;
        std::cout << "  Transfer Time: " << stats.transferTimeMs << " ms" << std::endl;
        std::cout << "  Speedup: " << stats.speedup << "x" << std::endl;
        std::cout << "  Memory Allocated: " << stats.memoryAllocated << " bytes" << std::endl;
    }
    
    std::cout << "=========================" << std::endl;
}

} // namespace cuda
} // namespace nlm
