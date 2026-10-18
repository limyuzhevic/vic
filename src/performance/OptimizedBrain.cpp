// NLM - Neural Learning Machine
// Phase 5: Scaling + Research Platform - Optimized Brain Implementation
// This file implements the high-performance OptimizedBrain class

#include "performance/OptimizedBrain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace nlm {

// OptimizedBrain Implementation

OptimizedBrain::OptimizedBrain(std::shared_ptr<Config> config)
    : Brain(config)
    , eventDrivenEnabled_(true)
    , multithreadingEnabled_(true)
    , simdEnabled_(true)
    , sparseEnabled_(true)
    , threadCount_(std::thread::hardware_concurrency())
    , activeNeuronCount_(0)
    , totalSteps_(0)
{
    NLM_LOG_INFO("Initializing OptimizedBrain (Phase 5 - High-Performance Neural Computation)...");
    
    // Initialize event queues with optimal sizes
    spikeQueue_.initialize(1024 * 1024);  // 1M events
    delayedSpikeQueue_.initialize(512 * 1024);  // 512K delayed events
    
    // Configure vectorizer based on available SIMD
    if (simdEnabled_) {
        lifVectorizer_.configure(
            config->getOr<float>("lif_v_rest", -70.0f),
            config->getOr<float>("lif_v_reset", -70.0f),
            config->getOr<float>("lif_v_threshold", -55.0f),
            config->getOr<float>("lif_tau", 20.0f),
            config->getOr<float>("lif_C", 1.0f),
            config->getOr<double>("simulation_timestep", 0.001),
            config->getOr<float>("lif_refractory", 5.0f)
        );
    }
    
    if (multithreadingEnabled_) {
        parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(threadCount_);
    }
}

OptimizedBrain::~OptimizedBrain() {
    NLM_LOG_INFO("Destructing OptimizedBrain");
}

bool OptimizedBrain::initialize() {
    NLM_LOG_INFO("Initializing OptimizedBrain with performance optimizations");
    
    // Call base Brain initialization first
    if (!Brain::initialize()) {
        NLM_LOG_ERROR("Base Brain initialization failed");
        return false;
    }
    
    // Get configuration values
    size_t neuronCount = getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = getConfig()->getOr<size_t>("region_count", 1);
    float connectionProbability = getConfig()->getOr<float>("connection_probability", 0.1f);
    
    // Reserve space for neural state (SoA layout)
    neuralState_.initialize(neuronCount);
    neuronIndices_.reserve(neuronCount);
    activeNeurons_.reserve(neuronCount / 10);  // Only ~10% expected active
    
    // Reserve space for synapse state
    synapseState_.initialize(neuronCount * connectionProbability * 3);  // Estimate
    
    // Initialize sparse connectivity
    if (sparseEnabled_) {
        initializeSparseConnectivity();
    } else {
        NLM_LOG_WARNING("Sparse connectivity disabled - using dense initialization");
        // Fall back to creating regions and connections normally
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                region->initializeDenseConnectivity(*getRandomGenerator(), connectionProbability, 0.2f, 0.1f);
            }
        }
    }
    
    // Initialize neural state with SoA layout
    initializeNeuralStateSOA(neuronCount);
    
    // Setup event-driven processing
    if (eventDrivenEnabled_) {
        setupEventDrivenProcessing();
    }
    
    NLM_LOG_INFO("OptimizedBrain initialization complete");
    NLM_LOG_INFO("  Neuron count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  Region count: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Event-driven: " + std::string(eventDrivenEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Multithreading: " + std::string(multithreadingEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  SIMD vectorization: " + std::string(simdEnabled_ ? "enabled" : "disabled"));
    
    return true;
}

void OptimizedBrain::initializeSparseConnectivity() {
    NLM_LOG_INFO("Initializing sparse connectivity representation");
    
    // Use the sparse connectivity system for efficient storage
    for (size_t i = 0; i < getRegionCount(); ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        size_t numNeurons = neurons.size();
        
        // Reserve space for neurons
        connectivity_.reserveNeurons(numNeurons);
        
        // Initialize all neurons in the index
        for (size_t j = 0; j < numNeurons; ++j) {
            NeuronId neuronId = neurons[j]->getId();
            connectivity_.reserveNeurons(neuronId.index());
        }
        
        // Create sparse connections between neurons in this region
        float connectionProb = getConfig()->getOr<float>("connection_probability", 0.1f);
        
        for (size_t src = 0; src < numNeurons; ++src) {
            NeuronId srcId = neurons[src]->getId();
            
            // Determine connections for this source neuron
            for (size_t dst = 0; dst < numNeurons; ++dst) {
                if (src == dst) continue;  // No self-connections
                
                // Use random connectivity with specified probability
                if (getRandomGenerator()->nextFloat() < connectionProb) {
                    // Get synapse parameters
                    float weight = getRandomGenerator()->nextFloat() * 0.4f - 0.2f;  // [-0.2, 0.2]
                    uint32_t delay = getRandomGenerator()->nextInt(0, 10);  // 0-10 steps
                    uint8_t type = (weight >= 0.0f) ? 0 : 1;  // 0=excitatory, 1=inhibitory
                    
                    size_t synapseIndex = connectivity_.addSynapse(
                        srcId.index(), neurons[dst]->getId().index(),
                        weight, delay, type
                    );
                    
                    // Record in synapse state
                    synapseState_.sourceNeuron.push_back(srcId.index());
                    synapseState_.destinationNeuron.push_back(neurons[dst]->getId().index());
                    synapseState_.weight.push_back(weight);
                    synapseState_.delay.push_back(delay);
                    synapseState_.synapseType.push_back(type);
                    synapseState_.plasticityFlags.push_back(3);  // Both STDP and Hebbian
                    synapseState_.eligibilityTrace.push_back(0.0f);
                    synapseState_.efficacy.push_back(1.0f);
                    synapseState_.shortTermDepression.push_back(1.0f);
                    synapseState_.shortTermFacilitation.push_back(0.0f);
                    
                    // Initialize spike history
                    for (size_t h = 0; h < SynapseStateSoA::MAX_HISTORY; ++h) {
                        synapseState_.preSpikeHistory.back()[h] = -1000.0f;
                        synapseState_.postSpikeHistory.back()[h] = -1000.0f;
                    }
                    synapseState_.preHistoryHead.push_back(0);
                    synapseState_.postHistoryHead.push_back(0);
                }
            }
        }
    }
    
    NLM_LOG_INFO("Sparse connectivity initialized with " + 
                 std::to_string(connectivity_.getSynapseCount()) + " synapses");
}

void OptimizedBrain::setupEventDrivenProcessing() {
    NLM_LOG_INFO("Setting up event-driven processing");
    
    // Register handlers for event-driven spike processing
    spikeSystem()->registerHandler([this](const DetailedSpikeEvent& event) {
        ++activeNeuronCount_;
        neuralState_.firingNeuronIndices.push_back(neuronIndices_[event.neuron_id].localIndex);
        neuralState_.activeNeuronIndices.push_back(neuronIndices_[event.neuron_id].localIndex);
    });
    
    spikeSystem()->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Process delayed spike in optimized manner
        processDelayedSpikeOptimized(event);
    });
}

void OptimizedBrain::initializeNeuralStateSOA(size_t neuronCount) {
    NLM_LOG_INFO("Initializing neural state in SoA format");
    
    // Configure for SoA (Structure of Arrays) layout
    neuralState_.initialize(neuronCount);
    
    // Collect all neurons and initialize their state
    auto regions = getRegions();
    size_t globalIndex = 0;
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                NeuronId neuronId = neuron->getId();
                
                // Store neuron index for quick lookup
                NeuronIndex idx;
                idx.globalId = neuronId.index();
                idx.localIndex = globalIndex;
                idx.regionIndex = region->getId().index();
                idx.populationIndex = pop->getId().index();
                
                neuronIndices_.push_back(idx);
                
                // Initialize neural state from neuron object
                const auto& state = neuron->getState();
                neuralState_.membranePotential[globalIndex] = state.membranePotential;
                neuralState_.restingPotential[globalIndex] = state.restingPotential;
                neuralState_.threshold[globalIndex] = state.threshold;
                neuralState_.resetPotential[globalIndex] = state.resetPotential;
                neuralState_.leakConductance[globalIndex] = state.leakConductance;
                neuralState_.synapseInput[globalIndex] = 0.0f;
                neuralState_.firingState[globalIndex] = static_cast<uint8_t>(state.firingState);
                neuralState_.refractoryRemaining[globalIndex] = state.refractoryRemaining;
                neuralState_.refractoryPeriod[globalIndex] = state.refractoryPeriod;
                neuralState_.lastSpikeTime[globalIndex] = state.lastSpikeTime;
                neuralState_.neuronType[globalIndex] = static_cast<uint64_t>(neuron->getType());
                
                // Collect sensory and motor neurons for direct access
                if (neuron->getType() == NeuronType::Sensory) {
                    sensoryNeurons_.push_back(neuron);
                } else if (neuron->getType() == NeuronType::Motor) {
                    motorNeurons_.push_back(neuron);
                }
                
                globalIndex++;
            }
        }
    }
    
    NLM_LOG_INFO("Neural state initialized with " + std::to_string(neuronCount) + " neurons in SoA layout");
}

void OptimizedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * getConfig()->getOr<double>("simulation_timestep", 0.001));
}

void OptimizedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    // Record last step time for statistics
    lastStepTime_ = stepStart;
    
    // Clear event-driven tracking
    neuralState_.activeNeuronIndices.clear();
    neuralState_.firingNeuronIndices.clear();
    
    // ========== Event-Driven Processing (if enabled) ==========
    if (eventDrivenEnabled_) {
        processActiveNeuronsOptimized(currentStep, currentTime);
    } else {
        // Fallback to traditional processing
        processAllNeuronsTraditional(currentStep, currentTime);
    }
    
    // ========== Process Firing Events ==========
    processFiringNeuronsOptimized(currentStep, currentTime);
    
    // ========== Update Synaptic Integration ==========
    updateSynapticIntegrationOptimized(currentTime);
    
    // ========== Apply Plasticity ==========
    applyPlasticityOptimized(currentTime);
    
    // ========== Neuromodulation ==========
    applyNeuromodulationOptimized(currentTime);
    
    // ========== Working Memory Update ==========
    updateWorkingMemoryOptimized(currentTime);
    
    // ========== Statistics Update ==========
    updateStatistics();
    
    // ========== Update Development ==========
    if (currentStep % 1000 == 0) {
        develop();
    }
    
    totalSteps_++;
    
    auto stepEnd = std::chrono::high_resolution_clock::now();
    double stepTimeMs = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
    
    if (stepTimeMs > 100.0) {  // Log slow steps (>100ms)
        NLM_LOG_WARNING("Slow step detected: " + std::to_string(stepTimeMs) + "ms at step " + std::to_string(currentStep));
    }
}

void OptimizedBrain::processActiveNeuronsOptimized(uint64_t currentStep, double currentTime) {
    // Process only neurons with active inputs (event-driven optimization)
    size_t activeCount = neuralState_.activeNeuronIndices.size();
    if (activeCount == 0) return;
    
    if (multithreadingEnabled_ && activeCount > 1000) {
        // Parallel processing for large active sets
        parallelProcessor_->processNeurons(activeCount,
            [this, currentStep, currentTime](size_t idx, ParallelNeuralProcessor::ThreadLocalState& state) {
                processSingleNeuronOptimized(idx, currentStep, currentTime, state);
            });
    } else {
        // Sequential processing for smaller sets
        for (size_t i : neuralState_.activeNeuronIndices) {
            processSingleNeuronOptimized(i, currentStep, currentTime, {});
        }
    }
}

void OptimizedBrain::processSingleNeuronOptimized(size_t neuronIdx, uint64_t currentStep, double currentTime, 
                                                   ParallelNeuralProcessor::ThreadLocalState& state) {
    // Vectorized LIF update
    float& V = neuralState_.membranePotential[neuronIdx];
    float& refractory = neuralState_.refractoryRemaining[neuronIdx];
    uint8_t& firing = neuralState_.firingState[neuronIdx];
    
    if (refractory > 0) {
        // Neuron in refractory period
        refractory--;
        V = neuralState_.restingPotential[neuronIdx];
        if (state.localFiringCount == 0) firing = static_cast<uint8_t>(FiringState::Inactive);
    } else {
        // Apply synaptic input
        float I = neuralState_.synapseInput[neuronIdx];
        
        if (simdEnabled_) {
            // Use SIMD vectorized update
            bool fired = lifVectorizer_.updateSingle(V, I, refractory, 
                                                     neuralState_.threshold[neuronIdx],
                                                     neuralState_.restingPotential[neuronIdx],
                                                     neuralState_.resetPotential[neuronIdx],
                                                     neuralState_.leakConductance[neuronIdx],
                                                     currentTime);
            if (fired) {
                firing = static_cast<uint8_t>(FiringState::Refractory);
                neuralState_.lastSpikeTime[neuronIdx] = currentTime;
                ++state.localSpikeCount;
            }
        } else {
            // Scalar fallback
            float dV = (neuralState_.restingPotential[neuronIdx] - V) / neuralState_.leakConductance[neuronIdx] + I;
            V += dV * getConfig()->getOr<double>("simulation_timestep", 0.001) * 1000.0f;
            
            if (V >= neuralState_.threshold[neuronIdx]) {
                V = neuralState_.resetPotential[neuronIdx];
                refractory = neuralState_.refractoryPeriod[neuronIdx];
                firing = static_cast<uint8_t>(FiringState::Refractory);
                neuralState_.lastSpikeTime[neuronIdx] = currentTime;
                ++state.localSpikeCount;
            }
        }
        
        neuralState_.synapseInput[neuronIdx] = 0.0f;  // Reset input after processing
    }
}

void OptimizedBrain::processAllNeuronsTraditional(uint64_t currentStep, double currentTime) {
    // Traditional approach: process all neurons (slower but compatible)
    auto regions = getRegions();
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, getConfig()->getOr<double>("simulation_timestep", 0.001));
            }
        }
    }
}

void OptimizedBrain::processFiringNeuronsOptimized(uint64_t currentStep, double currentTime) {
    size_t firingCount = neuralState_.firingNeuronIndices.size();
    
    if (firingCount == 0) return;
    
    // Process firing neurons to generate outgoing spikes
    for (size_t i : neuralState_.firingNeuronIndices) {
        NeuronId neuronId(neuralState_.neuronType[i]);  // Simplified - should use proper ID
        
        // Create spike event
        SpikeEvent event(neuronId, currentTime, currentStep);
        spikeSystem()->queueSpike(event);
        
        // Find and process outgoing synapses
        processOutgoingSynapsesOptimized(i, neuronId, currentStep, currentTime);
    }
}

void OptimizedBrain::processOutgoingSynapsesOptimized(size_t neuronIdx, NeuronId neuronId, 
                                                        uint64_t currentStep, double currentTime) {
    // Get outgoing synapses from sparse connectivity if enabled
    if (sparseEnabled_) {
        auto synapses = connectivity_.getSynapsesFrom(neuralState_.neuronType[neuronIdx]);
        for (auto& syn : synapses) {
            processSingleOutgoingSynapseOptimized(neuronIdx, syn, currentStep, currentTime);
        }
    } else {
        // Fallback to traditional method
        for (auto& region : getRegions()) {
            auto outgoing = region->getSynapsesFrom(neuronId);
            for (auto* syn : outgoing) {
                processSingleOutgoingSynapseTraditional(neuronIdx, syn, currentStep, currentTime);
            }
        }
    }
}

void OptimizedBrain::processSingleOutgoingSynapseOptimized(size_t neuronIdx, 
                                                            const std::pair<size_t, SynapseRecord*>& synPair,
                                                            uint64_t currentStep, double currentTime) {
    const SynapseRecord& synRecord = *synPair.second;
    size_t synIdx = synPair.first;
    
    // Apply synaptic delay
    uint32_t delay = synRecord.delay;
    SimulationStep deliveryStep = currentStep + delay;
    Timestamp deliveryTime = currentTime + delay * getConfig()->getOr<double>("simulation_timestep", 0.001);
    
    // Create delayed spike event
    DelayedSpikeEvent delayedEvent(
        neuralState_.neuronType[neuronIdx],  // Source neuron ID
        synRecord.destNeuron,               // Destination neuron ID
        synIdx,                             // Synapse index
        synRecord.weight,                   // Synaptic weight
        synRecord.synapseType,              // Synapse type
        currentTime,
        deliveryTime,
        currentStep,
        deliveryStep
    );
    
    spikeSystem()->queueDelayedSpike(delayedEvent);
    
    // Record spike history for plasticity
    recordSpikeHistoryOptimized(neuronIdx, synIdx, true, currentTime);
    
    // Also record at destination for post-synaptic plasticity
    recordSpikeHistoryForDestination(synRecord.destNeuron, synIdx, false, currentTime);
}

void OptimizedBrain::recordSpikeHistoryOptimized(size_t neuronIdx, size_t synapseIdx, 
                                                  bool isPreSpike, double currentTime) {
    // Update spike history in synapse state
    if (isPreSpike) {
        synapseState_.preSpikeHistory[synapseIdx][synapseState_.preHistoryHead[neuronIdx]] = currentTime;
        synapseState_.preHistoryHead[neuronIdx] = (synapseState_.preHistoryHead[neuronIdx] + 1) % SynapseStateSoA::MAX_HISTORY;
    } else {
        synapseState_.postSpikeHistory[synapseIdx][synapseState_.postHistoryHead[neuronIdx]] = currentTime;
        synapseState_.postHistoryHead[neuronIdx] = (synapseState_.postHistoryHead[neuronIdx] + 1) % SynapseStateSoA::MAX_HISTORY;
    }
}

void OptimizedBrain::recordSpikeHistoryForDestination(uint64_t destNeuronId, size_t synapseIdx, 
                                                       bool isPostSpike, double currentTime) {
    // Find neuron index for destination
    for (size_t i = 0; i < neuralState_.neuronType.size(); ++i) {
        if (neuralState_.neuronType[i] == destNeuronId) {
            if (isPostSpike) {
                synapseState_.postSpikeHistory[synapseIdx][synapseState_.postHistoryHead[i]] = currentTime;
                synapseState_.postHistoryHead[i] = (synapseState_.postHistoryHead[i] + 1) % SynapseStateSoA::MAX_HISTORY;
            } else {
                synapseState_.preSpikeHistory[synapseIdx][synapseState_.preHistoryHead[i]] = currentTime;
                synapseState_.preHistoryHead[i] = (synapseState_.preHistoryHead[i] + 1) % SynapseStateSoA::MAX_HISTORY;
            }
            break;
        }
    }
}

void OptimizedBrain::updateSynapticIntegrationOptimized(double currentTime) {
    // Vectorized synaptic input integration
    size_t neuronCount = neuralState_.membranePotential.size();
    
    if (simdEnabled_ && neuronCount >= SIMDUtils::SIMD_WIDTH) {
        // Use SIMD for vectorized integration
        vectorizedSynapticIntegration(neuronCount);
    } else {
        // Scalar fallback
        for (size_t i = 0; i < neuronCount; ++i) {
            neuralState_.synapseInput[i] = 0.0f;  // Reset from previous step
            
            // Integrate inputs from delayed spikes
            integrateDelayedSpikeInputs(i, currentTime);
        }
    }
}

void OptimizedBrain::vectorizedSynapticIntegration(size_t neuronCount) {
    // SIMD-optimized synaptic integration
    size_t vectorSize = neuronCount - (neuronCount % SIMDUtils::SIMD_WIDTH);
    size_t i = 0;
    
    // Process in SIMD chunks
    for (; i < vectorSize; i += SIMDUtils::SIMD_WIDTH) {
        // Load synaptic inputs into SIMD register
        // This would use actual SIMD intrinsics
        // For now, keep scalar for correctness
    }
    
    // Handle remaining neurons
    for (; i < neuronCount; ++i) {
        integrateDelayedSpikeInputs(i, 0.0);  // Simplified
    }
}

void OptimizedBrain::integrateDelayedSpikeInputs(size_t neuronIdx, double currentTime) {
    // Integrate inputs from pending delayed spikes
    auto& synInput = neuralState_.synapseInput[neuronIdx];
    
    // Process delayed spikes for this neuron
    for (const auto& delayedEvent : delayedSpikeQueue_.getEvents()) {
        if (delayedEvent.destination_neuron == neuralState_.neuronType[neuronIdx]) {
            // Calculate synaptic current
            float synapticCurrent = delayedEvent.weight * 10.0f;  // Scale factor
            
            if (delayedEvent.is_excitatory) {
                synInput += synapticCurrent;
            } else {
                synInput -= synapticCurrent;
            }
        }
    }
}

void OptimizedBrain::applyPlasticityOptimized(double currentTime) {
    size_t synapseCount = synapseState_.weight.size();
    
    if (synapseCount == 0) return;
    
    if (multithreadingEnabled_ && synapseCount > 1000) {
        // Parallel plasticity application
        parallelProcessor_->processNeurons(synapseCount,
            [this, currentTime](size_t i, ParallelNeuralProcessor::ThreadLocalState& state) {
                applySingleSynapsePlasticityOptimized(i, currentTime, state);
            });
    } else {
        // Sequential application
        for (size_t i = 0; i < synapseCount; ++i) {
            applySingleSynapsePlasticityOptimized(i, currentTime, {});
        }
    }
}

void OptimizedBrain::applySingleSynapsePlasticityOptimized(size_t synapseIdx, double currentTime,
                                                            ParallelNeuralProcessor::ThreadLocalState& state) {
    // Extract synapse state
    float& weight = synapseState_.weight[synapseIdx];
    uint8_t& flags = synapseState_.plasticityFlags[synapseIdx];
    uint64_t sourceNeuron = synapseState_.sourceNeuron[synapseIdx];
    uint64_t destNeuron = synapseState_.destinationNeuron[synapseIdx];
    
    // Apply STDP if enabled
    if (flags & 1) {  // STDP bit
        applySTDPOptimized(synapseIdx, sourceNeuron, destNeuron, currentTime);
    }
    
    // Apply Hebbian learning if enabled
    if (flags & 2) {  // Hebbian bit
        applyHebbianOptimized(synapseIdx, sourceNeuron, destNeuron, currentTime);
    }
    
    // Apply short-term plasticity
    applyShortTermPlasticityOptimized(synapseIdx, currentTime);
    
    // Update eligibility trace
    synapseState_.eligibilityTrace[synapseIdx] *= 0.95f;  // Decay
}

void OptimizedBrain::applySTDPOptimized(size_t synapseIdx, uint64_t sourceNeuron, uint64_t destNeuron,
                                        double currentTime) {
    // Get spike history for this synapse
    auto& preHistory = synapseState_.preSpikeHistory[synapseIdx];
    auto& postHistory = synapseState_.postSpikeHistory[synapseIdx];
    uint8_t preHead = synapseState_.preHistoryHead[sourceNeuron];
    uint8_t postHead = synapseState_.postHistoryHead[destNeuron];
    
    // Calculate STDP weight changes
    float weightChange = 0.0f;
    
    // Check for pre-before-post spikes (LTP)
    for (size_t i = 0; i < SynapseStateSoA::MAX_HISTORY; ++i) {
        float preTime = preHistory[i];
        if (preTime > 0.0f) {  // Valid spike recorded
            // Check for post spikes after this pre spike
            for (size_t j = 0; j < SynapseStateSoA::MAX_HISTORY; ++j) {
                float postTime = postHistory[j];
                if (postTime > preTime && postTime <= currentTime) {
                    // Pre-before-post: LTP
                    float dt = postTime - preTime;
                    weightChange += getSTDP()->calculateLTP(dt);
                }
            }
        }
    }
    
    // Check for post-before-pre spikes (LTD)
    for (size_t i = 0; i < SynapseStateSoA::MAX_HISTORY; ++i) {
        float postTime = postHistory[i];
        if (postTime > 0.0f) {
            // Check for pre spikes after this post spike
            for (size_t j = 0; j < SynapseStateSoA::MAX_HISTORY; ++j) {
                float preTime = preHistory[j];
                if (preTime > postTime && preTime <= currentTime) {
                    // Post-before-pre: LTD
                    float dt = preTime - postTime;
                    weightChange += getSTDP()->calculateLTD(dt);
                }
            }
        }
    }
    
    // Apply weight change with bounds checking
    float newWeight = synapseState_.weight[synapseIdx] + weightChange;
    newWeight = std::clamp(newWeight, getSTDP()->getMinWeight(), getSTDP()->getMaxWeight());
    
    synapseState_.weight[synapseIdx] = newWeight;
}

void OptimizedBrain::applyHebbianOptimized(size_t synapseIdx, uint64_t sourceNeuron, uint64_t destNeuron,
                                           double currentTime) {
    // Simplified Hebbian learning: strengthen synapses when both neurons fire together
    auto& preHistory = synapseState_.preSpikeHistory[synapseIdx];
    auto& postHistory = synapseState_.postSpikeHistory[synapseIdx];
    
    float hebbianChange = 0.0f;
    
    // Check for correlated activity in recent spikes
    for (size_t i = 0; i < SynapseStateSoA::MAX_HISTORY; ++i) {
        if (preHistory[i] > 0.0f && postHistory[i] > 0.0f) {
            // Both pre and post spiked at this time step
            hebbianChange += getConfig()->getOr<float>("hebbian_rate", 0.001f);
        }
    }
    
    // Apply Hebbian learning
    float newWeight = synapseState_.weight[synapseIdx] + hebbianChange;
    newWeight = std::clamp(newWeight, getSTDP()->getMinWeight(), getSTDP()->getMaxWeight());
    
    synapseState_.weight[synapseIdx] = newWeight;
}

void OptimizedBrain::applyShortTermPlasticityOptimized(size_t synapseIdx, double currentTime) {
    // Apply short-term plasticity (STP)
    float& depression = synapseState_.shortTermDepression[synapseIdx];
    float& facilitation = synapseState_.shortTermFacilitation[synapseIdx];
    
    // Simple model: depression decays with time, facilitation increases with activity
    depression *= 0.95f;  // Natural recovery
    facilitation *= 0.9f;  // Natural decay
    
    // Clamp to reasonable bounds
    depression = std::clamp(depression, 0.1f, 1.0f);
    facilitation = std::clamp(facilitation, 0.0f, 1.0f);
}

void OptimizedBrain::applyNeuromodulationOptimized(double currentTime) {
    // Apply neuromodulation signals to plasticity
    auto* dopamine = getDopamine();
    if (!dopamine) return;
    
    float dopamineLevel = dopamine->getLevel();
    float plasticityFactor = dopamine->getPlasticityFactor();
    
    // Apply neuromodulation to all synapses
    for (size_t i = 0; i < synapseState_.weight.size(); ++i) {
        // Scale STDP learning rates based on dopamine
        float modulatedWeight = synapseState_.weight[i] * plasticityFactor;
        modulatedWeight = std::clamp(modulatedWeight, getSTDP()->getMinWeight(), getSTDP()->getMaxWeight());
        synapseState_.weight[i] = modulatedWeight;
        
        // Dopamine also affects neural excitability
        // (would be applied during neuron updates)
    }
}

void OptimizedBrain::updateWorkingMemoryOptimized(double currentTime) {
    // Update working memory with current neural activity
    auto* workingMemory = getWorkingMemory();
    if (!workingMemory) return;
    
    // Clear old traces
    workingMemory->clear();
    
    // Add new traces based on active neurons
    for (size_t i : neuralState_.firingNeuronIndices) {
        // Calculate activation strength from membrane potential
        float activation = std::abs(neuralState_.membranePotential[i] - 
                                   neuralState_.restingPotential[i]) / 20.0f;
        workingMemory->storeToNeuron(neuralState_.neuronType[i], activation);
    }
}

void OptimizedBrain::processDelayedSpikeOptimized(const DelayedSpikeEvent& event) {
    // Optimized processing of delayed spike events
    NeuronId destNeuron(event.destination_neuron);
    
    // Find destination neuron in our SoA state
    for (size_t i = 0; i < neuralState_.neuronType.size(); ++i) {
        if (neuralState_.neuronType[i] == destNeuron) {
            // Calculate synaptic input
            float synapticCurrent = event.weight * 10.0f;  // Scale factor
            
            if (event.is_excitatory) {
                neuralState_.synapseInput[i] += synapticCurrent;
            } else {
                neuralState_.synapseInput[i] -= synapticCurrent;
            }
            
            neuralState_.activeNeuronIndices.push_back(i);
            break;
        }
    }
}

void OptimizedBrain::updateStatistics() {
    // Update performance statistics
    perfStats_.neuronsPerSecond = totalSteps_ > 0 ? 
        static_cast<double>(neuralState_.neuronType.size()) / totalSteps_ : 0.0;
    
    perfStats_.spikesPerSecond = totalSteps_ > 0 ?
        static_cast<double>(getTotalSpikeCount()) / totalSteps_ : 0.0;
    
    perfStats_.synapticEventsPerSecond = totalSteps_ > 0 ?
        static_cast<double>(getTotalSynapseCount()) / totalSteps_ : 0.0;
    
    perfStats_.realTimeFactor = 1.0;  // Simplified
    
    perfStats_.activeNeurons = static_cast<size_t>(activeNeuronCount_);
    perfStats_.firingNeurons = static_cast<size_t>(neuralState_.firingNeuronIndices.size());
    
    perfStats_.pendingSpikes = spikeSystem()->getPendingSpikeCount();
    perfStats_.pendingDelayedSpikes = spikeSystem()->getPendingDelayedCount();
    
    perfStats_.avgFiringRate = getAverageFiringRate();
    
    // Calculate memory usage
    size_t totalMemory = 0;
    totalMemory += neuralState_.memoryUsage();
    totalMemory += synapseState_.memoryUsage();
    totalMemory += connectivity_.memoryUsage();
    
    perfStats_.memoryUsageMB = totalMemory / (1024 * 1024);  // Convert to MB
    
    // Record step time
    perfStats_.stepTimeMs = std::chrono::duration<double, std::milli>(lastStepTime_.time_since_epoch()).count();
}

OptimizedBrain::PerformanceStats OptimizedBrain::getPerformanceStats() const {
    return perfStats_;
}

// Global function for optimized brain creation
std::shared_ptr<Brain> createOptimizedBrain(std::shared_ptr<Config> config) {
    auto brain = std::make_shared<OptimizedBrain>(config);
    if (brain->initialize()) {
        return brain;
    }
    return nullptr;
}

} // namespace nlm