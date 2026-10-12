#include "OptimizedBrain.hpp"
#include "../brain/Brain.hpp"
#include "../brain/MemoryPoolManager.hpp"
#include "../performance/EventSystem.hpp"
#include "../performance/ConnectivityManager.hpp"
#include "../performance/ParallelProcessor.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>

namespace nlm {

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(config)
    , eventDrivenEnabled_(config->getOr<bool>("enable_event_driven", true))
    , multithreadingEnabled_(config->getOr<bool>("enable_multithreading", true))
    , simdEnabled_(config->getOr<bool>("enable_simd", true))
    , sparseEnabled_(config->getOr<bool>("enable_sparse_connectivity", true))
    , threadCount_(config->getOr<size_t>("thread_count", std::thread::hardware_concurrency()))
    , currentStep_(0)
    , performanceMonitoringEnabled_(config->getOr<bool>("enable_performance_monitoring", true))
    , lastStepTime_(std::chrono::high_resolution_clock::now())
    , totalSteps_(0)
    , simdInitialized_(false)
{
    // Initialize neural state with SoA layout
    neuralState_.initialize(config->getOr<size_t>("neuron_count", 1000));
    
    // Initialize synapse state with SoA layout
    synapseState_.initialize(config->getOr<size_t>("synapse_count", 10000));
    
    // Initialize neuron indices
    neuronIndices_.reserve(neuralState_.membranePotential.size());
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        NeuronIndex idx;
        idx.globalId = i;
        idx.localIndex = i;
        idx.regionIndex = 0; // Simplified - all neurons in region 0
        idx.populationIndex = (i % 2 == 0) ? 0 : 1; // Simplified population assignment
        neuronIndices_.push_back(idx);
    }
    
    // Initialize event system
    eventSystem_ = std::make_unique<EventSystem>();
    
    // Initialize connectivity manager
    if (sparseEnabled_) {
        connectivity_ = std::make_unique<SparseConnectivity>();
        connectivity_->reserveNeurons(neuralState_.membranePotential.size());
    }
    
    // Initialize parallel processor
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelProcessor>(threadCount_);
    }
    
    // Initialize SIMD vectorizer
    if (simdEnabled_) {
        lifVectorizer_ = std::make_unique<VectorizedLIF>();
        simdInitialized_ = true;
    }
    
    // Initialize statistics
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
    
    // Setup connectivity if enabled
    if (sparseEnabled_) {
        initializeSparseConnectivity();
    }
}

OptimizedBrain::~OptimizedBrain() {
    // Cleanup is handled by smart pointers
}

bool OptimizedBrain::initialize() {
    // Call parent initialize
    if (!Brain::initialize()) {
        return false;
    }
    
    // Setup performance optimizations based on configuration
    if (eventDrivenEnabled_) {
        eventSystem_->setParallelProcessing(true);
    }
    
    if (multithreadingEnabled_) {
        parallelProcessor_->setWorkStealing(true);
        parallelProcessor_->setThreadAffinity(true);
    }
    
    if (simdEnabled_) {
        lifVectorizer_->enableSIMD(true);
    }
    
    NLM_LOG_INFO("OptimizedBrain initialized with performance optimizations");
    return true;
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep_);
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    currentStep_ = currentStep;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update current time in event system
    if (eventDrivenEnabled_) {
        eventSystem_->setCurrentTime(currentTime);
    }
    
    // Process events in event-driven mode
    if (eventDrivenEnabled_) {
        processActiveNeurons(currentStep, currentTime);
        processFiringNeurons(currentStep, currentTime);
    } else {
        // Fall back to original Brain processing
        processNeuronsSequential(currentStep, currentTime);
    }
    
    // Apply plasticity
    applyPlasticity();
    
    // Apply structural plasticity periodically
    if (currentStep_ % 100 == 0) {
        applyStructuralPlasticity(currentStep, currentTime);
    }
    
    // Update statistics
    updateStatistics();
    
    // Update performance monitoring
    if (performanceMonitoringEnabled_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        perfStats_.stepTimeMs = duration;
        
        // Update real-time factor
        perfStats_.realTimeFactor = duration / timestep_;
        
        // Log performance if monitoring enabled
        if (currentStep_ % 100 == 0) {
            reportPerformance();
        }
    }
    
    lastStepTime_ = std::chrono::high_resolution_clock::now();
    totalSteps_++;
}

void OptimizedBrain::processActiveNeurons(uint64_t currentStep, double currentTime) {
    // Event-driven processing of active neurons only
    size_t activeCount = neuralState_.activeNeuronIndices.size();
    
    if (activeCount > 0) {
        // Process active neurons in batch
        if (multithreadingEnabled_ && activeCount > BATCH_SIZE_THRESHOLD) {
            // Use parallel processing for large batches
            processNeuronsBatchParallel(activeCount, currentStep, currentTime);
        } else {
            // Process sequentially for small batches
            processNeuronsBatchSequential(activeCount, currentStep, currentTime);
        }
        
        // Update firing statistics
        perfStats_.firingNeurons = neuralState_.firingNeuronIndices.size();
        perfStats_.activeNeurons = activeCount;
    }
}

void OptimizedBrain::processFiringNeurons(uint64_t currentStep, double currentTime) {
    // Process neurons that fired this step
    for (size_t idx : neuralState_.firingNeuronIndices) {
        if (idx >= neuralState_.membranePotential.size()) continue;
        
        // Deliver synaptic input to post-synaptic neurons
        auto connections = connectivity_ ? connectivity_->getConnectionsFrom(idx) : std::vector<SynapseConnection>{};
        
        for (const auto& conn : connections) {
            // Find destination neuron index
            size_t destIdx = conn.targetNeuron;
            if (destIdx < neuralState_.membranePotential.size()) {
                // Deliver synaptic input
                float synapticInput = neuralState_.synapseInput[destIdx] + conn.weight;
                neuralState_.synapseInput[destIdx] = synapticInput;
                
                // Record for event system
                if (eventDrivenEnabled_) {
                    EventSystem::Event event;
                    event.type = EventSystem::EventType::Spike;
                    event.step = currentStep;
                    event.time = currentTime;
                    event.neuronId = idx;
                    event.synapseId = conn.synapseId;
                    event.weightChange = conn.weight;
                    
                    eventSystem_->scheduleEvent(event);
                }
            }
        }
    }
}

void OptimizedBrain::updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    // Optimized neuron update using vectorized operations
    if (!simdInitialized_ || !enableVectorizedLIF_) {
        // Fallback to sequential processing
        updateNeuronsSequential(begin, end, currentStep, currentTime);
        return;
    }
    
    // Vectorized LIF update
    float vRest = -70.0f;
    float vReset = -70.0f;
    float vThreshold = -55.0f;
    float tau = 20.0f;
    float dt = 0.001f;
    
    if (simdFeatures_.avx512) {
        lifVectorizer_->updateNeuronBatchAVX512(
            &neuralState_.membranePotential[begin],
            &neuralState_.adaptationVariable[begin],
            nullptr, // I_ext
            &neuralState_.synapseInput[begin],
            nullptr, // I_total
            vRest, vReset, vThreshold, tau, dt,
            end - begin
        );
    } else if (simdFeatures_.avx2) {
        lifVectorizer_->updateNeuronBatchAVX2(
            &neuralState_.membranePotential[begin],
            &neuralState_.adaptationVariable[begin],
            nullptr, // I_ext
            &neuralState_.synapseInput[begin],
            nullptr, // I_total
            vRest, vReset, vThreshold, tau, dt,
            end - begin
        );
    } else if (simdFeatures_.sse) {
        lifVectorizer_->updateNeuronBatchSSE(
            &neuralState_.membranePotential[begin],
            &neuralState_.adaptationVariable[begin],
            nullptr, // I_ext
            &neuralState_.synapseInput[begin],
            nullptr, // I_total
            vRest, vReset, vThreshold, tau, dt,
            end - begin
        );
    }
}

void OptimizedBrain::updateSynapsesBatch(size_t begin, size_t end, double currentTime) {
    // Optimized synapse update using vectorized operations
    if (!simdInitialized_) {
        // Fallback to sequential processing
        updateSynapsesSequential(begin, end, currentTime);
        return;
    }
    
    // Vectorized synapse processing
    for (size_t i = begin; i < end; ++i) {
        if (i >= synapseState_.weight.size()) continue;
        
        // Update short-term plasticity
        float depression = synapseState_.shortTermDepression[i];
        float facilitation = synapseState_.shortTermFacilitation[i];
        
        // Update based on spike history
        size_t preHistoryIdx = synapseState_.preHistoryHead[i];
        size_t postHistoryIdx = synapseState_.postHistoryHead[i];
        
        if (preHistoryIdx < synapseState_.MAX_HISTORY) {
            float preSpikeTime = synapseState_.preSpikeHistory[i][preHistoryIdx];
            float postSpikeTime = synapseState_.postSpikeHistory[i][postHistoryIdx];
            
            // Update synaptic efficacy
            if (std::abs(postSpikeTime - preSpikeTime) < 5.0f) {  // Within 5ms
                facilitation = std::min(facilitation * 1.5f, 2.0f);
                depression = std::max(depression / 1.5f, 0.2f);
            }
            
            synapseState_.shortTermFacilitation[i] = facilitation;
            synapseState_.shortTermDepression[i] = depression;
            
            // Update eligibility trace
            synapseState_.eligibilityTrace[i] += (v_post[i] - v_pre[i]) * 0.01f;
        }
    }
}

void OptimizedBrain::applyPlasticity() {
    // Optimized plasticity application using vectorized operations
    size_t synapseCount = synapseState_.weight.size();
    
    if (synapseCount > 0) {
        if (multithreadingEnabled_ && synapseCount > PLASTICITY_BATCH_THRESHOLD) {
            // Use parallel processing for plasticity
            parallelProcessor_->processPlasticityBatch(
                synapseState_.weight, synapseCount
            );
        } else {
            // Process sequentially
            for (size_t i = 0; i < synapseCount; ++i) {
                applyPlasticityAtIndex(i);
            }
        }
    }
}

void OptimizedBrain::applyStructuralPlasticity(uint64_t currentStep, double currentTime) {
    // Optimized structural plasticity using event system
    if (!eventDrivenEnabled_) return;
    
    // Schedule structural plasticity events
    EventSystem::Event event;
    event.type = EventSystem::EventType::Structural;
    event.step = currentStep;
    event.time = currentTime;
    event.neuronId = -1;
    event.synapseId = -1;
    
    eventSystem_->scheduleEvent(event);
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    size_t activeNeurons = neuralState_.activeNeuronIndices.size();
    size_t firingNeurons = neuralState_.firingNeuronIndices.size();
    
    perfStats_.activeNeurons = activeNeurons;
    perfStats_.firingNeurons = firingNeurons;
    
    // Calculate average firing rate
    if (totalSteps_ > 0) {
        perfStats_.avgFiringRate = static_cast<float>(firingNeurons) / neuralState_.membranePotential.size();
    }
    
    // Calculate memory usage
    perfStats_.memoryUsageMB = (neuralState_.memoryUsage() + synapseState_.memoryUsage()) / (1024 * 1024);
    
    // Update events per second
    if (eventSystem_) {
        perfStats_.pendingSpikes = eventSystem_->getEventCount();
    }
}

void OptimizedBrain::reportPerformance() {
    NLM_LOG_INFO(std::string("OptimizedBrain Performance - Step ") + std::to_string(currentStep_) +
                 std::string(": ") + std::to_string(perfStats_.activeNeurons) +
                 std::string(" active, ") + std::to_string(perfStats_.firingNeurons) +
                 std::string(" firing, ") + std::to_string(perfStats_.avgFiringRate) +
                 std::string(" avg firing rate, ") + std::to_string(perfStats_.stepTimeMs) +
                 std::string(" ms/step, ") + std::to_string(perfStats_.realTimeFactor) +
                 std::string(" real-time factor"));
    
    // Report component performance
    if (parallelProcessor_) {
        parallelProcessor_->reportPerformance();
    }
    
    if (eventSystem_) {
        NLM_LOG_INFO(std::string("Event System: ") + std::to_string(eventSystem_->getEventCount()) +
                     std::string(" events, ") + std::to_string(eventSystem_->getAverageProcessingTime()) +
                     std::string(" ms average processing time"));
    }
}

void OptimizedBrain::initializeSparseConnectivity() {
    if (!connectivity_) return;
    
    size_t neuronCount = neuralState_.membranePotential.size();
    float connectionProbability = 0.1f; // Default value
    
    // Initialize connectivity with sparse connections
    connectivity_->reserveNeurons(neuronCount);
    
    // Generate random connectivity (excitatory and inhibitory)
    for (size_t i = 0; i < neuronCount; ++i) {
        // Calculate number of outgoing connections
        size_t numOutgoing = static_cast<size_t>(neuronCount * connectionProbability);
        
        // Add excitatory connections
        for (size_t j = 0; j < numOutgoing / 2; ++j) {
            size_t target = (i + j + 1) % neuronCount; // Avoid self-connections
            connectivity_->addConnection(i, target, SynapseType::Excitatory);
        }
        
        // Add inhibitory connections
        for (size_t j = 0; j < numOutgoing / 4; ++j) {
            size_t target = (i + j + neuronCount / 2) % neuronCount;
            connectivity_->addConnection(i, target, SynapseType::Inhibitory);
        }
    }
    
    NLM_LOG_INFO(std::string("Initialized sparse connectivity with ") + 
                 std::to_string(connectivity_->getConnectionCount()) + std::string(" connections"));
}

void OptimizedBrain::processNeuronsBatchParallel(size_t activeCount, uint64_t currentStep, double currentTime) {
    // Process active neurons in parallel
    if (parallelProcessor_) {
        parallelProcessor_->processNeuronBatch(
            neuralState_.activeNeuronIndices.data(), activeCount,
            currentStep, currentTime
        );
    }
}

void OptimizedBrain::processNeuronsBatchSequential(size_t activeCount, uint64_t currentStep, double currentTime) {
    // Process active neurons sequentially
    for (size_t i = 0; i < activeCount; ++i) {
        size_t neuronIdx = neuralState_.activeNeuronIndices[i];
        if (neuronIdx >= neuralState_.membranePotential.size()) continue;
        
        // Update neuron using optimized vectorized operations
        updateNeuronOptimized(neuronIdx, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronOptimized(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    if (neuronIdx >= neuralState_.membranePotential.size()) return;
    
    // Vectorized neuron update using SIMD
    if (simdInitialized_ && enableVectorizedLIF_) {
        // Use vectorized LIF update
        float v = neuralState_.membranePotential[neuronIdx];
        float u = neuralState_.adaptationVariable[neuronIdx];
        float I_ext = neuralState_.synapseInput[neuronIdx];
        float I_syn = 0.0f;
        float I_total = I_ext + I_syn;
        
        // Apply LIF dynamics using vectorized operations
        float v_rest = -70.0f;
        float v_threshold = -55.0f;
        float v_reset = -70.0f;
        float tau = 20.0f;
        float dt = 0.001f;
        
        // Calculate new membrane potential
        float dv = (I_total - (v - v_rest)) / tau;
        float new_v = v + dv * dt;
        
        // Check for spike
        if (new_v >= v_threshold) {
            // Neuron fired
            new_v = v_reset;
            neuralState_.firingNeuronIndices.push_back(neuronIdx);
            
            // Record spike for plasticity
            recordSpikeForPlasticity(neuronIdx, currentTime);
        }
        
        // Update neuron state
        neuralState_.membranePotential[neuronIdx] = new_v;
        neuralState_.adaptationVariable[neuronIdx] = u * 0.95f + 1.0f; // Add adaptation
        
        // Update refractory period
        if (neuralState_.refractoryRemaining[neuronIdx] > 0) {
            neuralState_.refractoryRemaining[neuronIdx]--;
        }
        
        // Mark neuron as active if it has input
        if (neuralState_.synapseInput[neuronIdx] != 0.0f) {
            neuralState_.activeNeuronIndices.push_back(neuronIdx);
        }
    } else {
        // Fallback to scalar processing
        updateNeuronScalar(neuronIdx, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronScalar(size_t neuronIdx, uint64_t currentStep, double currentTime) {
    if (neuronIdx >= neuralState_.membranePotential.size()) return;
    
    // Scalar neuron update (original Brain implementation)
    float& v = neuralState_.membranePotential[neuronIdx];
    float& u = neuralState_.adaptationVariable[neuronIdx];
    float I_ext = neuralState_.synapseInput[neuronIdx];
    float I_syn = 0.0f;
    float I_total = I_ext + I_syn;
    
    // LIF dynamics
    float v_rest = -70.0f;
    float v_threshold = -55.0f;
    float v_reset = -70.0f;
    float tau = 20.0f;
    float dt = 0.001f;
    
    float leak = (v_rest - v) / tau;
    float input = I_total / 1.0f;  // C = 1.0
    float dv = (leak + input) * 1000.0f;
    
    v += dv * dt;
    
    if (v >= v_threshold) {
        // Neuron fired
        v = v_reset;
        u += 1.0f;  // Adaptation
        neuralState_.firingNeuronIndices.push_back(neuronIdx);
        recordSpikeForPlasticity(neuronIdx, currentTime);
    }
}

void OptimizedBrain::recordSpikeForPlasticity(size_t neuronIdx, double currentTime) {
    // Record spike for STDP and Hebbian plasticity
    size_t historyIdx = neuralState_.preHistoryHead[neuronIdx];
    synapseState_.preSpikeHistory[neuronIdx][historyIdx] = static_cast<float>(currentTime);
    neuralState_.preHistoryHead[neuronIdx] = (historyIdx + 1) % synapseState_.MAX_HISTORY;
}

void OptimizedBrain::applyPlasticityAtIndex(size_t synapseIdx) {
    if (synapseIdx >= synapseState_.weight.size()) return;
    
    float& weight = synapseState_.weight[synapseIdx];
    float v_pre = synapseState_.preSpikeHistory[synapseIdx][0];  // Simplified
    float v_post = synapseState_.postSpikeHistory[synapseIdx][0]; // Simplified
    float t_pre = synapseState_.preSpikeHistory[synapseIdx][1];   // Simplified
    float t_post = synapseState_.postSpikeHistory[synapseIdx][1]; // Simplified
    
    // Simplified STDP update
    float dt = t_post - t_pre;
    float learningRate = 0.01f;
    
    if (dt > 0.0f) {
        // Pre before post: LTP
        weight += learningRate * std::exp(-dt / 20.0f);
    } else if (dt < 0.0f) {
        // Post before pre: LTD
        weight -= learningRate * std::exp(dt / 20.0f);
    }
}

void OptimizedBrain::NeuralStateSoA::initialize(size_t size) {
    membranePotential.resize(size, -70.0f);
    restingPotential.resize(size, -70.0f);
    threshold.resize(size, -55.0f);
    resetPotential.resize(size, -70.0f);
    leakConductance.resize(size, 10.0f);
    synapseInput.resize(size, 0.0f);
    firingState.resize(size, 0);
    refractoryRemaining.resize(size, 0);
    refractoryPeriod.resize(size, 5);
    lastSpikeTime.resize(size, -1.0f);
    neuronType.resize(size, 0);
    adaptationVariable.resize(size, 0.0f);
    
    activeNeuronIndices.reserve(size);
    firingNeuronIndices.reserve(size);
}

void OptimizedBrain::NeuralStateSoA::clear() {
    std::fill(synapseInput.begin(), synapseInput.end(), 0.0f);
    firingNeuronIndices.clear();
    activeNeuronIndices.clear();
}

size_t OptimizedBrain::NeuralStateSoA::memoryUsage() const {
    size_t total = 0;
    total += membranePotential.capacity() * sizeof(float);
    total += restingPotential.capacity() * sizeof(float);
    total += threshold.capacity() * sizeof(float);
    total += resetPotential.capacity() * sizeof(float);
    total += leakConductance.capacity() * sizeof(float);
    total += synapseInput.capacity() * sizeof(float);
    total += firingState.capacity() * sizeof(uint8_t);
    total += refractoryRemaining.capacity() * sizeof(uint32_t);
    total += refractoryPeriod.capacity() * sizeof(uint32_t);
    total += lastSpikeTime.capacity() * sizeof(float);
    total += neuronType.capacity() * sizeof(uint64_t);
    total += adaptationVariable.capacity() * sizeof(float);
    return total;
}

void OptimizedBrain::SynapseStateSoA::initialize(size_t size) {
    sourceNeuron.resize(size);
    destinationNeuron.resize(size);
    weight.resize(size);
    delay.resize(size);
    synapseType.resize(size);
    plasticityFlags.resize(size);
    eligibilityTrace.resize(size, 0.0f);
    efficacy.resize(size, 1.0f);
    shortTermDepression.resize(size, 1.0f);
    shortTermFacilitation.resize(size, 0.0f);
    
    // Spike history for STDP (circular buffers)
    preSpikeHistory.resize(size);
    postSpikeHistory.resize(size);
    preHistoryHead.resize(size, 0);
    postHistoryHead.resize(size, 0);
}

void OptimizedBrain::SynapseStateSoA::clear() {
    eligibilityTrace.assign(eligibilityTrace.size(), 0.0f);
    for (auto& history : preSpikeHistory) {
        for (auto& spikeTime : history) {
            spikeTime = -1.0f;
        }
    }
    for (auto& history : postSpikeHistory) {
        for (auto& spikeTime : history) {
            spikeTime = -1.0f;
        }
    }
}

size_t OptimizedBrain::SynapseStateSoA::memoryUsage() const {
    size_t total = 0;
    total += sourceNeuron.capacity() * sizeof(uint64_t);
    total += destinationNeuron.capacity() * sizeof(uint64_t);
    total += weight.capacity() * sizeof(float);
    total += delay.capacity() * sizeof(uint32_t);
    total += synapseType.capacity() * sizeof(uint8_t);
    total += plasticityFlags.capacity() * sizeof(uint8_t);
    total += eligibilityTrace.capacity() * sizeof(float);
    total += efficacy.capacity() * sizeof(float);
    total += shortTermDepression.capacity() * sizeof(float);
    total += shortTermFacilitation.capacity() * sizeof(float);
    return total;
}

void OptimizedBrain::setMemoryPool(MemoryPoolManager& poolManager) {
    memoryPoolManager_ = &poolManager;
}

void OptimizedBrain::setEventSystem(EventSystem& eventSystem) {
    eventSystem_ = std::make_unique<EventSystem>(eventSystem);
}

void OptimizedBrain::enablePerformanceMonitoring(bool enable) {
    performanceMonitoringEnabled_ = enable;
}

void OptimizedBrain::reportPerformance() {
    // Report performance statistics
    std::string report = "OptimizedBrain Performance Report:\n";
    report += std::string("Step: ") + std::to_string(currentStep_) + std::string("\n");
    report += std::string("Active neurons: ") + std::to_string(perfStats_.activeNeurons) + std::string("\n");
    report += std::string("Firing neurons: ") + std::to_string(perfStats_.firingNeurons) + std::string("\n");
    report += std::string("Average firing rate: ") + std::to_string(perfStats_.avgFiringRate) + std::string("\n");
    report += std::string("Memory usage: ") + std::to_string(perfStats_.memoryUsageMB) + std::string(" MB\n");
    report += std::string("Step time: ") + std::to_string(perfStats_.stepTimeMs) + std::string(" ms\n");
    report += std::string("Real-time factor: ") + std::to_string(perfStats_.realTimeFactor) + std::string("\n");
    
    NLM_LOG_INFO(report);
}

void OptimizedBrain::processNeuronsSequential(uint64_t currentStep, double currentTime) {
    // Sequential processing (original Brain implementation)
    // This is a simplified version - the full implementation would be much larger
    for (size_t i = 0; i < neuralState_.membranePotential.size(); ++i) {
        updateNeuronScalar(i, currentStep, currentTime);
    }
}

void OptimizedBrain::updateNeuronsSequential(size_t begin, size_t end, uint64_t currentStep, double currentTime) {
    for (size_t i = begin; i < end; ++i) {
        updateNeuronScalar(i, currentStep, currentTime);
    }
}

void OptimizedBrain::updateSynapsesSequential(size_t begin, size_t end, double currentTime) {
    for (size_t i = begin; i < end; ++i) {
        applyPlasticityAtIndex(i);
    }
}

} // namespace nlm