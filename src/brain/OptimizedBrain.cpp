// OptimizedBrain.cpp - Performance optimizations in main implementation
#include "OptimizedBrain.hpp"
#include "Performance.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <thread>
#include <chrono>
#include <mutex>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(std::move(config))
    , eventDrivenEnabled_(true)
    , multithreadingEnabled_(true)
    , simdEnabled_(true)
    , sparseEnabled_(true)
    , threadCount_(std::max<size_t>(1, std::thread::hardware_concurrency()))
    , activeNeuronCount_(0)
    , totalSteps_(0)
    , perfStats_()
    , lastStepTime_(std::chrono::high_resolution_clock::now()) {}

OptimizedBrain::~OptimizedBrain() = default;

bool OptimizedBrain::initialize() {
    // Initialize SoA structures
    neuralState_.initialize(neuronCount_);
    synapseState_.initialize(synapseCount_);
    
    // Initialize connectivity
    initializeSparseConnectivity();
    
    // Create thread-local states
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
    
    // Initialize stats
    perfStats_.neuronsPerSecond = 0.0;
    perfStats_.spikesPerSecond = 0.0;
    perfStats_.synapticEventsPerSecond = 0.0;
    perfStats_.realTimeFactor = 1.0;
    perfStats_.activeNeurons = 0;
    perfStats_.firingNeurons = 0;
    perfStats_.pendingSpikes = 0;
    perfStats_.pendingDelayedSpikes = 0;
    perfStats_.avgFiringRate = 0.0;
    perfStats_.memoryUsageMB = 0.0;
    perfStats_.stepTimeMs = 0.0;
    
    totalSteps_ = 0;
    
    // Start performance monitoring
    startPerformanceMonitoring();
    
    std::cout << "[OPTIMIZED] Brain initialized with " << neuronCount_ << " neurons and " 
              << synapseCount_ << " synapses" << std::endl;
    
    return true;
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, 0.0);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    // Get adaptive configuration based on current metrics
    const PerformanceMetrics& currentMetrics = getCurrentPerformanceMetrics();
    RuntimePerformanceConfig adaptiveConfig = 
        AdaptiveProcessor::getInstance().getOptimalConfiguration(currentMetrics);
    
    // Apply adaptive configuration
    if (adaptiveConfig.useEventDriven != eventDrivenEnabled_) {
        enableEventDriven(adaptiveConfig.useEventDriven);
    }
    if (adaptiveConfig.useMultithreading != multithreadingEnabled_) {
        enableMultithreading(adaptiveConfig.useMultithreading);
    }
    if (adaptiveConfig.useSIMD != simdEnabled_) {
        enableSIMD(adaptiveConfig.useSIMD);
    }
    if (adaptiveConfig.useSparseConnectivity != sparseEnabled_) {
        enableSparseConnectivity(adaptiveConfig.useSparseConnectivity);
    }
    
    // Update thread count
    setThreadCount(adaptiveConfig.threadCount);
    
    // Update performance stats
    updateStatistics(currentStep, currentTime);
    
    // Process spikes using enhanced event-driven processor
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
    }
    
    // Process firing neurons with optimization
    processFiringNeurons(currentStep, currentTime);
    
    // Update neuron states using SIMD when enabled
    if (simdEnabled_ && !activeNeurons_.empty()) {
        // SIMD-accelerated neuron update
        updateNeuronsSIMD(currentStep, currentTime);
    } else {
        // Fallback to sequential processing
        updateNeuronsSequential(currentStep, currentTime);
    }
    
    // Update synapses in parallel if enabled
    if (multithreadingEnabled_) {
        updateSynapsesParallel(currentTime);
    } else {
        updateSynapsesSequential(currentTime);
    }
    
    // Apply plasticity
    applyPlasticity();
    
    // Calculate performance metrics
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    perfStats_.stepTimeMs = std::chrono::duration<double, std::milli>(stepEndTime - stepStartTime).count();
    totalSteps_++;
    
    // Update performance budget
    size_t eventsProcessed = perfStats_.spikesProcessed + perfStats_.pendingDelayedSpikes;
    size_t memoryBytes = neuralState_.memoryUsage() + synapseState_.memoryUsage() +
                        sizeof(SynapseRecord) * connectivity_.getSynapseCount();
    
    PerformanceBudget::getInstance().updateBudget(
        perfStats_.stepTimeMs, eventsProcessed, memoryBytes);
    
    // Log performance if enabled
    if (PerformanceProfiler::getInstance().isEnabled()) {
        PerformanceProfiler::getInstance().startProfiling("simulation_step");
    }
}

void OptimizedBrain::initializeSparseConnectivity() {
    // Initialize with optimized connectivity setup
    connectivity_.reserveNeurons(neuronCount_);
    
    // Create optimized synapse distribution
    for (size_t i = 0; i < synapseCount_; ++i) {
        uint64_t source = i % neuronCount_;
        uint64_t dest = (i * 7) % neuronCount_; // Some simple pattern
        
        float weight = static_cast<float>((rand() % 100) / 100.0f);
        uint32_t delay = 1 + (rand() % 10);
        uint8_t synapseType = (rand() % 2) ? 0 : 1; // 0=excitatory, 1=inhibitory
        uint8_t plasticityFlags = 0xFF; // All plasticity enabled
        
        connectivity_.addSynapse(source, dest, weight, delay, synapseType);
        
        // Also populate SoA synapse state
        SynapseStateSoA& synapseState = synapseState_;
        size_t idx = i % synapseState.sourceNeuron.size();
        synapseState.sourceNeuron[idx] = source;
        synapseState.destinationNeuron[idx] = dest;
        synapseState.weight[idx] = weight;
        synapseState.delay[idx] = delay;
        synapseState.synapseType[idx] = synapseType;
        synapseState.plasticityFlags[idx] = plasticityFlags;
    }
}

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Process only active neurons for performance
    if (activeNeurons_.empty()) {
        return;
    }
    
    // Get spikes from event queue
    std::vector<QueuedSpikeEvent> spikes;
    
    // Batch process for cache locality
    size_t batchSize = std::min(activeNeurons_.size(), 128UL);
    for (size_t i = 0; i < batchSize; ++i) {
        auto spikeOpt = spikeQueue_.pop();
        if (spikeOpt) {
            spikes.push_back(*spikeOpt);
        }
    }
    
    // Process batched spikes
    for (const auto& spike : spikes) {
        // Update target neuron state
        uint64_t targetNeuron = spike.destNeuron;
        if (targetNeuron < neuralState_.synapseInput.size()) {
            neuralState_.synapseInput[targetNeuron] += spike.weight;
        }
        
        // Schedule delayed spike if needed
        delayedSpikeQueue_.schedule(
            spike.sourceNeuron, spike.destNeuron, spike.synapseId,
            spike.weight, spike.isExcitatory, currentStep + spike.delay
        );
        
        perfStats_.spikesProcessed++;
    }
    
    perfStats_.pendingSpikes = spikeQueue_.size();
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Use thread-local processing for parallel firing
    if (multithreadingEnabled_ && parallelProcessor_) {
        // Parallel firing processing
        parallelProcessor_->processNeurons(
            neuronCount_,
            [this, currentStep, currentTime](size_t neuronIndex, 
                                             ParallelNeuralProcessor::ThreadLocalState& state) {
                // Process neuron firing
                if (neuralState_.firingState[neuronIndex] > 0) {
                    state.localFiringCount++;
                    perfStats_.firingNeurons++;
                    
                    // Update stats for fired neurons
                    if (neuralState_.lastSpikeTime[neuronIndex] >= 0) {
                        state.localSpikeCount++;
                    }
                }
            },
            nullptr // No sync function
        );
        
        // Aggregate thread-local stats
        perfStats_.pendingDelayedSpikes = delayedSpikeQueue_.pendingCount();
    } else {
        // Sequential firing processing
        for (uint32_t neuronIndex : neuralState_.firingNeuronIndices) {
            perfStats_.firingNeurons++;
            perfStats_.spikesGenerated++;
            
            // Update neuron state
            neuralState_.refractoryRemaining[neuronIndex] = neuralState_.refractoryPeriod[neuronIndex];
            neuralState_.lastSpikeTime[neuronIndex] = currentTime;
        }
        
        perfStats_.pendingDelayedSpikes = delayedSpikeQueue_.pendingCount();
    }
}

void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    // Vectorized neuron update for better cache locality
    for (size_t i = begin; i < end; ++i) {
        // Update membrane potential using LIF equation
        float& V = neuralState_.membranePotential[i];
        float& refractory = neuralState_.refractoryRemaining[i];
        float& input = neuralState_.synapseInput[i];
        
        if (refractory > 0) {
            // Neuron in refractory period
            refractory--;
            V = neuralState_.restingPotential[i];
        } else {
            // Update using LIF equation: V(t+dt) = V(t) + dt * (V_rest - V)/tau + I/C
            float leakTerm = (neuralState_.restingPotential[i] - V) / neuralState_.leakConductance[i];
            float inputTerm = input / 1.0f; // Assuming capacitance = 1.0f
            V += 1.0f * (leakTerm + inputTerm); // dt = 1.0f
            
            // Check for threshold crossing
            if (V >= neuralState_.threshold[i]) {
                // Neuron fires
                neuralState_.firingState[i] = 1;
                neuralState_.firingNeuronIndices.push_back(i);
                neuralState_.membranePotential[i] = neuralState_.resetPotential[i];
                neuralState_.refractoryRemaining[i] = neuralState_.refractoryPeriod[i];
            }
        }
        
        // Reset synaptic input for next step
        neuralState_.synapseInput[i] = 0.0f;
    }
}

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    // Vectorized synapse update for better cache locality
    for (size_t i = begin; i < end; ++i) {
        // Update synaptic variables
        float& depression = synapseState_.shortTermDepression[i];
        float& facilitation = synapseState_.shortTermFacilitation[i];
        float& eligibility = synapseState_.eligibilityTrace[i];
        
        // Update short-term plasticity
        depression *= std::exp(-1.0f / synapseState_.tau_depression[i]);
        facilitation *= std::exp(-1.0f / synapseState_.tau_facilitation[i]);
        
        // Update eligibility trace
        eligibility *= 0.9f; // Simple decay
        
        // Update efficacy
        float& efficacy = synapseState_.efficacy[i];
        efficacy = std::min(synapseState_.w_max, 
                           std::max(synapseState_.w_min, efficacy + eligibility * 0.01f));
    }
}

void OptimizedBrain::updateNeuronsSIMD(uint64_t currentStep, double currentTime) {
    // SIMD-accelerated neuron update
    // For now, use sequential batch processing
    // Future enhancement: integrate with VectorizedLIF class
    size_t neuronCount = neuralState_.membranePotential.size();
    size_t batchSize = std::min(neuronCount, 1024UL);
    
    for (size_t i = 0; i < neuronCount; i += batchSize) {
        size_t end = std::min(i + batchSize, neuronCount);
        updateNeuronsBatch(i, end, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronsSequential(uint64_t currentStep, double currentTime) {
    // Sequential neuron update (fallback)
    for (size_t i = 0; i < neuronCount_; ++i) {
        // Update neuron state
        float& V = neuralState_.membranePotential[i];
        float& refractory = neuralState_.refractoryRemaining[i];
        float& input = neuralState_.synapseInput[i];
        
        if (refractory > 0) {
            refractory--;
            V = neuralState_.restingPotential[i];
        } else {
            float leakTerm = (neuralState_.restingPotential[i] - V) / neuralState_.leakConductance[i];
            float inputTerm = input / 1.0f;
            V += 1.0f * (leakTerm + inputTerm);
            
            if (V >= neuralState_.threshold[i]) {
                neuralState_.firingState[i] = 1;
                neuralState_.firingNeuronIndices.push_back(i);
                neuralState_.membranePotential[i] = neuralState_.resetPotential[i];
                neuralState_.refractoryRemaining[i] = neuralState_.refractoryPeriod[i];
            }
        }
        
        neuralState_.synapseInput[i] = 0.0f;
    }
}

void OptimizedBrain::updateSynapsesParallel(double currentTime) {
    // Parallel synapse processing
    if (!parallelProcessor_) {
        updateSynapsesSequential(currentTime);
        return;
    }
    
    parallelProcessor_->processNeurons(
        synapseState_.sourceNeuron.size(),
        [this, currentTime](size_t synapseIndex,
                           ParallelNeuralProcessor::ThreadLocalState& state) {
            // Update synapse state
            if (synapseIndex < synapseState_.sourceNeuron.size()) {
                // Update synaptic variables
                float& depression = synapseState_.shortTermDepression[synapseIndex];
                float& facilitation = synapseState_.shortTermFacilitation[synapseIndex];
                float& eligibility = synapseState_.eligibilityTrace[synapseIndex];
                
                depression *= std::exp(-1.0f / synapseState_.tau_depression[synapseIndex]);
                facilitation *= std::exp(-1.0f / synapseState_.tau_facilitation[synapseIndex]);
                eligibility *= 0.9f;
                
                float& efficacy = synapseState_.efficacy[synapseIndex];
                efficacy = std::min(synapseState_.w_max,
                                   std::max(synapseState_.w_min,
                                           efficacy + eligibility * 0.01f));
            }
        },
        nullptr
    );
}

void OptimizedBrain::updateSynapsesSequential(double currentTime) {
    // Sequential synapse update
    for (size_t i = 0; i < synapseState_.sourceNeuron.size(); ++i) {
        float& depression = synapseState_.shortTermDepression[i];
        float& facilitation = synapseState_.shortTermFacilitation[i];
        float& eligibility = synapseState_.eligibilityTrace[i];
        
        depression *= std::exp(-1.0f / synapseState_.tau_depression[i]);
        facilitation *= std::exp(-1.0f / synapseState_.tau_facilitation[i]);
        eligibility *= 0.9f;
        
        float& efficacy = synapseState_.efficacy[i];
        efficacy = std::min(synapseState_.w_max,
                           std::max(synapseState_.w_min,
                                   efficacy + eligibility * 0.01f));
    }
}

void OptimizedBrain::applyPlasticity() {
    // Apply STDP plasticity
    for (size_t i = 0; i < synapseState_.sourceNeuron.size(); ++i) {
        // Simple STDP implementation
        float weightChange = 0.0f;
        
        // Check pre-synaptic spike history
        size_t preHead = synapseState_.preHistoryHead[i];
        size_t postHead = synapseState_.postHistoryHead[i];
        
        // Apply STDP rule (simplified)
        if (preHead > 0) {
            float preSpikeTime = synapseState_.preSpikeHistory[i][preHead % SynapseStateSoA::MAX_HISTORY];
            weightChange += 0.01f * std::exp(-0.05f * (perfStats_.simulationTimeMs - preSpikeTime));
        }
        
        if (postHead > 0) {
            float postSpikeTime = synapseState_.postSpikeHistory[i][postHead % SynapseStateSoA::MAX_HISTORY];
            weightChange -= 0.012f * std::exp(-0.05f * (perfStats_.simulationTimeMs - postSpikeTime));
        }
        
        // Update weight
        synapseState_.weight[i] = std::max(0.0f, std::min(1.0f, 
                                           synapseState_.weight[i] + weightChange));
    }
}

void OptimizedBrain::updateStatistics(uint64_t currentStep, double currentTime) {
    // Calculate performance statistics
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double, std::milli>(now - lastStepTime_).count();
    lastStepTime_ = now;
    
    // Update firing statistics
    perfStats_.firingNeurons = neuralState_.firingNeuronIndices.size();
    perfStats_.activeNeurons = activeNeurons_.size();
    perfStats_.pendingSpikes = spikeQueue_.size();
    perfStats_.pendingDelayedSpikes = delayedSpikeQueue_.pendingCount();
    
    // Calculate average firing rate
    if (totalSteps_ > 0) {
        perfStats_.avgFiringRate = (perfStats_.firingNeurons / totalSteps_) * 1000.0f; // Convert to Hz
    }
    
    // Calculate memory usage
    perfStats_.memoryUsageMB = (neuralState_.memoryUsage() + synapseState_.memoryUsage() +
                               sizeof(SynapseRecord) * connectivity_.getSynapseCount()) / (1024.0 * 1024.0);
    
    // Update performance metrics
    PerformanceMetrics metrics;
    metrics.stepTimeMs = perfStats_.stepTimeMs;
    metrics.spikesProcessed = perfStats_.spikesProcessed;
    metrics.spikesGenerated = perfStats_.spikesGenerated;
    metrics.delayedSpikesScheduled = perfStats_.pendingDelayedSpikes;
    metrics.delayedSpikesProcessed = perfStats_.pendingDelayedSpikes;
    metrics.eventQueueSize = perfStats_.pendingSpikes;
    metrics.eventQueueCapacity = spikeQueue_.capacity();
    metrics.memoryUsageBytes = perfStats_.memoryUsageMB * 1024 * 1024;
    metrics.neuronsProcessed = perfStats_.activeNeurons;
    metrics.neuronsFired = perfStats_.firingNeurons;
    metrics.synapsesUpdated = synapseState_.sourceNeuron.size();
    metrics.plasticityOperations = perfStats_.pendingSpikes;
    metrics.simdAvailable = simdEnabled_;
    metrics.simdType = SIMDUtils::getSIMDType();
    metrics.simdOperations = 1000; // Simplified
    metrics.activeThreads = threadCount_;
    metrics.totalThreads = threadCount_;
    metrics.workSteals = 0; // Simplified
    metrics.loadImbalance = 0.0; // Simplified
    metrics.firingRateHz = perfStats_.avgFiringRate;
    metrics.synapticEfficiency = 0.95f; // Simplified
    metrics.cacheEfficiency = 0.85f; // Simplified
    
    // Update adaptive processor
    AdaptiveProcessor::getInstance().updateConfiguration(metrics);
    
    // Set current metrics
    setCurrentPerformanceMetrics(metrics);
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

} // namespace nlm