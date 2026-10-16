#include "OptimizedBrain.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

// OptimizedBrain implementation

// Initialize the OptimizedBrain
OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config) 
    : Brain(config)
    , eventDrivenEnabled_(true)
    , multithreadingEnabled_(true)
    , simdEnabled_(true)
    , sparseEnabled_(true)
    , threadCount_(std::max(1, static_cast<int>(std::thread::hardware_concurrency())))
    , activeNeuronCount_(0)
    , totalSpikesTotal(0)
    , currentStep(0)
    , currentTime(0.0)
    , totalSteps(0)
    , lifVectorizer_()
{
    // Initialize parallel processor
    parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
}

OptimizedBrain::~OptimizedBrain() = default;

// Initialize with configuration
bool OptimizedBrain::initialize() {
    // Initialize base Brain
    Brain::initialize();
    
    // Initialize performance monitoring
    if (auto* profiler = getPerformanceProfiler()) {
        profiler->startProfiling();
    }
    
    // Initialize neural state
    neuralState_.initialize(1000);  // Will be resized based on neuron count
    
    return true;
}

// Optimized simulation step
void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, currentTime);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Start performance timing
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    // Record performance snapshot
    if (auto* profiler = getPerformanceProfiler()) {
        profiler->recordPerformanceSnapshot();
    }
    
    // Optimized neural computation
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
        processFiringNeurons(currentStep, currentTime);
    } else {
        updateNeuronsBatch(0, neuralState_.size(), currentStep, currentTime);
    }
    
    // Update synapses
    if (multithreadingEnabled_) {
        updateSynapsesBatch(0, synapseState_.size(), currentTime);
    }
    
    // Apply plasticity
    applyPlasticity();
    
    // Update statistics
    updateStatistics();
    
    // Record step time
    auto stepEnd = std::chrono::high_resolution_clock::now();
    double stepTimeMs = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
    
    // Update performance stats
    perfStats_.stepTimeMs = stepTimeMs;
    perfStats_.neuronsPerSecond = (currentStep > 0) ? 
        (static_cast<double>(getTotalNeuronCount()) / stepTimeMs) * 1000.0 : 0.0;
}

// Performance analysis methods
size_t OptimizedBrain::getCurrentMemoryUsage() const {
    size_t total = neuralState_.memoryUsage();
    total += synapseState_.memoryUsage();
    total += connectivity_.getMemoryUsage();
    return total;
}

size_t OptimizedBrain::getTotalNeuronCount() const {
    return neuralState_.size();
}

size_t OptimizedBrain::getActiveNeuronCount() const {
    return activeNeuronCount_.load();
}

size_t OptimizedBrain::getFiringNeuronCount() const {
    return neuralState_.firingNeuronIndices.size();
}

size_t OptimizedBrain::getPendingSpikeCount() const {
    return spikeQueue_.size() + delayedSpikeQueue_.size();
}

size_t OptimizedBrain::getTotalSpikeCount() const {
    return totalSpikesTotal.load();
}

size_t OptimizedBrain::getCurrentStep() const {
    return currentStep.load();
}

double OptimizedBrain::getCurrentSimulationTime() const {
    return currentTime;
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

// Event-driven processing
void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Mark neurons with synaptic input as active
    for (uint32_t activeIndex : neuralState_.activeNeuronIndices) {
        if (activeIndex < neuralState_.size()) {
            activeNeuronCount_++;
        }
    }
    
    // Process each active neuron
    for (uint32_t activeIndex : neuralState_.activeNeuronIndices) {
        if (activeIndex < neuralState_.size()) {
            // Apply LIF dynamics
            float& v = neuralState_.membranePotential[activeIndex];
            float I_syn = neuralState_.synapseInput[activeIndex];
            
            // Vectorized LIF computation
            if (simdEnabled_) {
                // SIMD implementation
                v += (I_syn - v + neuralState_.restingPotential[activeIndex]) * 
                     (1.0f / neuralState_.leakConductance[activeIndex]) * timestep;
            } else {
                // Scalar implementation
                v += (I_syn - v + neuralState_.restingPotential[activeIndex]) * 
                     (1.0f / neuralState_.leakConductance[activeIndex]) * timestep;
            }
            
            // Check for firing
            if (v >= neuralState_.threshold[activeIndex]) {
                // Fire neuron
                neuralState_.firingNeuronIndices.push_back(activeIndex);
                v = neuralState_.resetPotential[activeIndex];
                
                // Record spike
                recordSpike(currentStep, activeIndex, v, true);
            }
        }
    }
    
    // Clear active neuron indices
    neuralState_.activeNeuronIndices.clear();
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Process firing neurons
    for (uint32_t firingIndex : neuralState_.firingNeuronIndices) {
        if (firingIndex < neuralState_.size()) {
            // Update refractory period
            neuralState_.refractoryRemaining[firingIndex] = neuralState_.refractoryPeriod[firingIndex];
            
            // Apply adaptation
            neuralState_.adaptationVariable[firingIndex] += 0.1f;
            
            // Generate spike event
            recordSpike(currentStep, firingIndex, neuralState_.membranePotential[firingIndex], false);
        }
    }
    
    neuralState_.firingNeuronIndices.clear();
}

// Batch processing
void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    if (multithreadingEnabled_) {
        // Parallel processing
        if (auto* parallelProcessor = parallelProcessor_.get()) {
            parallelProcessor->processNeurons(end - begin, 
                [this, begin, currentStep, currentTime](size_t i, auto& state) {
                    updateSingleNeuron(begin + i, currentStep, currentTime, state);
                });
        }
    } else {
        // Sequential processing
        for (size_t i = begin; i < end; ++i) {
            updateSingleNeuron(i, currentStep, currentTime, neuralState_);
        }
    }
}

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    for (size_t i = begin; i < end; ++i) {
        updateSingleSynapse(i, currentTime);
    }
}

void OptimizedBrain::updateSingleNeuron(size_t index, uint64_t currentStep, double currentTime, auto& state) {
    // Update single neuron
    float& v = state.membranePotential[index];
    float I_syn = state.synapseInput[index];
    
    v += (I_syn - v + state.restingPotential[index]) * 
         (1.0f / state.leakConductance[index]) * timestep;
    
    // Check for firing
    if (v >= state.threshold[index]) {
        state.firingNeuronIndices.push_back(index);
        v = state.resetPotential[index];
        recordSpike(currentStep, index, v, true);
    }
    
    // Update refractory period
    if (state.refractoryRemaining[index] > 0) {
        state.refractoryRemaining[index]--;
    }
}

void OptimizedBrain::updateSingleSynapse(size_t index, double currentTime) {
    // Update synaptic variables
    float& depression = synapseState_.shortTermDepression[index];
    float& facilitation = synapseState_.shortTermFacilitation[index];
    
    // Update short-term plasticity
    depression *= 0.95f;
    facilitation *= 0.95f;
    
    // Update eligibility trace
    synapseState_.eligibilityTrace[index] *= 0.99f;
}

void OptimizedBrain::applyPlasticity() {
    // Apply synaptic plasticity
    stdp->applyPlasticity();
    hebbian->applyPlasticity();
    structuralPlasticity->applyPlasticity();
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    perfStats_.activeNeurons = getActiveNeuronCount();
    perfStats_.firingNeurons = getFiringNeuronCount();
    perfStats_.pendingSpikes = getPendingSpikeCount();
    
    // Calculate average firing rate
    double totalFiringRate = 0.0;
    size_t count = neuralState_.firingNeuronIndices.size();
    for (uint32_t index : neuralState_.firingNeuronIndices) {
        if (index < neuralState_.size()) {
            totalFiringRate += neuralState_.firingRate[index];
        }
    }
    perfStats_.avgFiringRate = (count > 0) ? totalFiringRate / count : 0.0f;
}

// Initialize sparse connectivity
void OptimizedBrain::initializeSparseConnectivity() {
    connectivity_.initialize(neuralState_.size(), *getRandomGenerator(), 0.1f);
}

// Record spike
void OptimizedBrain::recordSpike(uint64_t currentStep, uint32_t neuronIndex, float voltage, bool isExcitatory) {
    totalSpikesTotal++;
    
    // Record in spike queue
    spikeQueue_.push(currentStep, neuronIndex, voltage, isExcitatory);
    
    // Record in spike history for STDP
    uint32_t historyIndex = neuronIndex % MAX_HISTORY;
    uint8_t& head = preSpikeHistory[neuronIndex].head;
    preSpikeHistory[neuronIndex][head][historyIndex] = currentTime;
    head = (head + 1) % MAX_HISTORY;
}

// Get current step
size_t OptimizedBrain::getCurrentStep() const {
    return currentStep.load();
}

// Get current simulation time
double OptimizedBrain::getCurrentSimulationTime() const {
    return currentTime;
}

// Get total neuron count
size_t OptimizedBrain::getTotalNeuronCount() const {
    return neuralState_.size();
}

// Get active neuron count
size_t OptimizedBrain::getActiveNeuronCount() const {
    return activeNeuronCount_.load();
}

// Get firing neuron count
size_t OptimizedBrain::getFiringNeuronCount() const {
    return neuralState_.firingNeuronIndices.size();
}

// Get pending spike count
size_t OptimizedBrain::getPendingSpikeCount() const {
    return spikeQueue_.size() + delayedSpikeQueue_.size();
}

// Get total spike count
size_t OptimizedBrain::getTotalSpikeCount() const {
    return totalSpikesTotal.load();
}

// Get performance statistics
OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

// Enable/disable optimizations
void OptimizedBrain::enableEventDriven(bool enable) {
    eventDrivenEnabled_ = enable;
}

void OptimizedBrain::enableMultithreading(bool enable) {
    multithreadingEnabled_ = enable;
}

void OptimizedBrain::enableSIMD(bool enable) {
    simdEnabled_ = enable;
}

void OptimizedBrain::enableSparseConnectivity(bool enable) {
    sparseEnabled_ = enable;
}

bool OptimizedBrain::isEventDrivenEnabled() const {
    return eventDrivenEnabled_;
}

bool OptimizedBrain::isMultithreadingEnabled() const {
    return multithreadingEnabled_;
}

bool OptimizedBrain::isSIMDEnabled() const {
    return simdEnabled_;
}

bool OptimizedBrain::isSparseEnabled() const {
    return sparseEnabled_;
}

// Set thread count
void OptimizedBrain::setThreadCount(size_t count) {
    threadCount_ = count;
    if (parallelProcessor_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
}

size_t OptimizedBrain::getThreadCount() const {
    return threadCount_;
}

} // namespace nlm